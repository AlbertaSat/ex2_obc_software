/*
 * Copyright (C) 2021  University of Alberta
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
/**
 * @file ftp.c
 * @author Robert Taylor
 * @date
 */

#include <FreeRTOS.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <os_queue.h>
#include <redposix.h>

#include "ftp.h"
#include "services.h"
#include "task_manager/task_manager.h"
#include "util/service_utilities.h"
#include <string.h>

#define WORKER_QUEUE_LEN 10
#define WORKER_QUEUE_ITEM_SIZE sizeof(void *)

#define FTP_STACK_SIZE 256

typedef enum {
    GET_REQUEST = 0,
    POST_REQUEST = 1
} FTP_REQUESTTYPE;

static uint32_t svc_wdt_counter = 0;
static uint32_t get_svc_wdt_counter() { return svc_wdt_counter; }

typedef struct {
    uint32_t req_id;
    char fname[REDCONF_NAME_MAX];
    REDSTAT fstat;
    FTP_REQUESTTYPE type;
    uint32_t blocksize;
    uint32_t skip;
    uint32_t count;
} FTP_t;

SAT_returnState send_download_burst(csp_conn_t *conn, FTP_t *ftp) {
/*
 * #TODO: Maybe this should be a struct..
 * Download packets contain:
 * uint8_t subservice byte. Always FTP_DATA_PACKET
 * int8_t status_byte. Set to -1 if end of file
 * uint32_t request id
 * uint32_t data size
 * uint32_t blocknumber of this transfer
 */

    FTP_t *current = ftp;
    int fd = red_open(current->fname, RED_O_RDONLY);
    if (fd < 0) {
        sys_log(WARN, "Could not open file %s. Errno: %d", current->fname, red_errno);
        return SATR_ERROR;
    }
    if (red_lseek(fd, current->skip * current->blocksize, RED_SEEK_SET) < 0) {
        sys_log(WARN, "Could not seek file %s. Errno: %d", current->fname, red_errno);
        red_close(fd);
        return SATR_ERROR;
    }
    int8_t status = 0;
    uint16_t blocknumber = 0;
    while(current->count--) {
        csp_packet_t *packet = csp_buffer_get(current->blocksize);
        if (packet == NULL) {
            sys_log(WARN, "Could not allocate CSP buffer");
            red_close(fd);
            return SATR_ERROR;
        }
        packet->data[SUBSERVICE_BYTE] = (uint8_t) FTP_DATA_PACKET;
        memcpy(&packet->data[OUT_DATA_BYTE], &current->req_id, sizeof(current->req_id));

        int32_t bytes_read = red_read(fd, &(packet->data[OUT_DATA_BYTE]) + 10, current->blocksize);
        memcpy(&(packet->data[OUT_DATA_BYTE]) + 4, &bytes_read, sizeof(bytes_read));
        memcpy(&(packet->data[OUT_DATA_BYTE]) + 8, &blocknumber, sizeof(blocknumber));
        if (bytes_read == 0) {
            sys_log(INFO, "FTP is done reading file %s", current->fname);
            status = -1;
        }
        if ( bytes_read < 0) {
            sys_log(WARN, "Could not read file %s. Errno: %d", current->fname, red_errno);
            status = -1;
        }
        set_packet_length(packet, bytes_read + 2 * sizeof(int8_t) + 2 * sizeof(uint32_t) + sizeof(uint16_t));
        memcpy(&packet->data[STATUS_BYTE], &status,
                   sizeof(status)); // 0 for not done, -1 for done
        if (!csp_send(conn, packet, CSP_MAX_TIMEOUT)) {
            csp_buffer_free(packet);
            red_close(fd);
            sys_log(WARN, "Could not send packet");
            return SATR_ERROR;
        }
        if (status == -1) {
            break;
        }
        blocknumber++;
    }
    red_close(fd);

    return SATR_OK;
}

SAT_returnState FTP_app(csp_packet_t *packet, QueueHandle_t worker_rx_queue, csp_conn_t *conn);

/**
 * @brief
 *      FreeRTOS File Transferring (FT) server task
 * @details
 *      Accepts incoming FTP service packets and executes
 *      the application
 * @param void* param
 * @return None
 */
void FTP_service(void *param) {
    // socket initialization
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_NONE);
    csp_bind(sock, TC_FTP_COMMAND_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN);
    QueueHandle_t worker_rx_queue = (QueueHandle_t)param;
    svc_wdt_counter++;
    csp_packet_t *packet;
    csp_conn_t *conn;

    for (;;) {
        // establish a connection

        if ((conn = csp_accept(sock, DELAY_WAIT_TIMEOUT)) == NULL) {
            svc_wdt_counter++;
            /* timeout */
            continue;
        }
        svc_wdt_counter++;

        // read and process packets
        while ((packet = csp_read(conn, 50)) != NULL) {
            if (FTP_app(packet, worker_rx_queue, conn) != SATR_OK) {
                // something went wrong in the subservice
                csp_buffer_free(packet);
            } else {
                // subservice was successful
                if (!csp_send(conn, packet, 50)) {
                    csp_buffer_free(packet);
                }
            }
        }
        csp_close(conn);
    }
}

/**
 * @brief
 *      Starts the File Transferring (FT) server task
 * @details
 *      Starts the FreeRTOS task responsible for accepting incoming
 * @param None
 * @return SAT_returnState
 *      Success report
 */
SAT_returnState start_FTP_service(void) {
    TaskHandle_t svc_tsk;
    taskFunctions svc_funcs = {0};
    svc_funcs.getCounterFunction = get_svc_wdt_counter;
    QueueHandle_t worker_rx_queue = xQueueCreate(WORKER_QUEUE_LEN, WORKER_QUEUE_ITEM_SIZE);

    if (xTaskCreate((TaskFunction_t)FTP_service, "FTP_service", FTP_STACK_SIZE, worker_rx_queue,
                    NORMAL_SERVICE_PRIO, &svc_tsk) != pdPASS) {
        sys_log(CRITICAL, "FAILED TO CREATE TASK FTP_service");
        return SATR_ERROR;
    }

    ex2_register(svc_tsk, svc_funcs);
    sys_log(INFO, "File Transfer service started\n");
    return SATR_OK;
}

/**
 * @brief
 *      Takes a CSP packet and switches based on the subservice command
 * @details
 *      Transfers files between the OBC and GS 
 * @param *packet
 *      The CSP packet
 * @return SAT_returnState
 *      Success or failure
 */
SAT_returnState FTP_app(csp_packet_t *packet, QueueHandle_t worker_rx_queue, csp_conn_t *conn) {
    uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];
    int8_t status = 0;
    SAT_returnState return_state = SATR_OK; // OK until an error is encountered
    uint32_t reply_len = 0;

    switch (ser_subtype) {
    case FTP_GET_FILE_SIZE: {
        /**
         * sends a null-terminated string of max size REDCONF_NAME_MAX
         * Replies with uint64_t in big endian
         */
        char *fname = (char *)&packet->data[IN_DATA_BYTE];
        uint32_t open_flags = RED_O_RDONLY;
        int fd = red_open(fname, open_flags);
        if (fd < 0) {
            sys_log(WARN, "FTP failed to open file %s, red_errno: %d", fname, red_errno);
            status = -1;
            break;
        }
        REDSTAT stat = {0};
        if (red_fstat(fd, &stat) < 0) {
            status = -1;
            red_close(fd);
            break;
        }
        memcpy(&packet->data[OUT_DATA_BYTE], &stat.st_size, sizeof(stat.st_size));
        reply_len = sizeof(stat.st_size);
        red_close(fd);
        break;
    }
    case FTP_REQUEST_BURST_DOWNLOAD: {
        /** Request
         * uint32_t req_id
         * uint32_t blocksize in bytes
         * uint32_t skip Represented in blocks
         * uint32_t count in blocks
         * char *filename
         * Response:
         * int8 status
         * uint32_t mtime
         * uint32_t ctime
         */
        uint32_t req_id;
        uint32_t blocksize;
        uint32_t skip;
        uint32_t count;
        cnv8_32(&packet->data[IN_DATA_BYTE ], &req_id);
        cnv8_32(&packet->data[IN_DATA_BYTE +4], &blocksize);
        cnv8_32(&packet->data[IN_DATA_BYTE +8], &skip);
        cnv8_32(&packet->data[IN_DATA_BYTE +12], &count);
        char *fname = (char *)&packet->data[IN_DATA_BYTE +16];

        uint32_t open_flags = RED_O_RDONLY;
        int fd = red_open(fname, open_flags);
        if (fd < 0) {
            status = -1;
            break;
        }
        REDSTAT stat = {0};
        if (red_fstat(fd, &stat) < 0) {
            status = -1;
            red_close(fd);
            break;
        }

        FTP_t ftp = {0};
        strncpy(&ftp.fname, fname, REDCONF_NAME_MAX);
        ftp.blocksize = blocksize;
        ftp.req_id = req_id;
        memcpy(&ftp.fstat, &stat, sizeof(REDSTAT));
        ftp.type = GET_REQUEST;
        ftp.skip = skip;
        ftp.count = count;

        red_close(fd);

        send_download_burst(conn, &ftp);

        memcpy(&packet->data[OUT_DATA_BYTE], &stat.st_mtime, sizeof(stat.st_mtime));
        memcpy(&packet->data[OUT_DATA_BYTE + 4], &stat.st_ctime, sizeof(stat.st_ctime));
        reply_len = sizeof(stat.st_mtime) + sizeof(stat.st_ctime);
        break;
    }
        default:
            ex2_log("No such subservice!\n");
            return_state = SATR_PKT_ILLEGAL_SUBSERVICE;
    }
    memcpy(&packet->data[STATUS_BYTE], &status,
               sizeof(int8_t)); // 0 for success
    set_packet_length(packet, reply_len + sizeof(int8_t) + 1); // +1 for subservice

    return return_state;
}
