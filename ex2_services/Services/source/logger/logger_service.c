/*
 * Copyright (C) 2015  University of Alberta
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
 * @file logger_service.c
 * @author Dustin Wagner
 * @date 2021-07-14
 */

#include "logger/logger_service.h"
#include "logger/logger.h"
#include "services.h"
#include <redposix.h>

#include "util/service_utilities.h" //for setting csp packet length
#include <csp/csp.h>

uint32_t max_string_length = 500; // limits based on csp packet size

/* @brief
 *      Check if file with given name exists
 * @param filename
 *      const char * to name of file to check
 * @return 0 if exists, 1 otherwise
 */
int file_exists(const char *filename) {
    int32_t file;
    if (file = red_open(filename, RED_O_CREAT | RED_O_EXCL | RED_O_RDWR) == -1) { // open file to read binary
        if (red_errno == RED_EEXIST) {
            red_close(file);
            return 0;
        }
    }
    red_close(file);
    red_unlink(filename);
    return 1;
}

/**
 * @brief
 *      generic file to open and read any file that would contain text data.
 *      primarily designed to handle either of the 2 log files that may be read
 *
 * @param filename
 *      the name of the file to be opened and read
 * @param packet
        the packet that holds the service subtype and will be filled with the log data
 * @return SAT_returnState
        state to define success of the operation
 */
SAT_returnState get_file(char *filename, csp_packet_t *packet) {
    uint32_t max_file_size;
    int8_t status;
    int32_t file;
    uint32_t data_size;
    char *log_data;
    get_logger_file_size(&max_file_size);
    log_data = (char *)pvPortMalloc(max_file_size);
    if (log_data == NULL) {
        status = -2;
    } else if (file_exists(filename) == 0) {
        file = red_open(filename, RED_O_RDONLY);
        if (file > -1) {
            data_size = red_read(file, log_data, max_file_size);
            if (data_size == 0) {
                status = -1;
                data_size = sprintf(log_data, "Log file %s is empty\n", filename);
            } else {
                status = 0;
            }
        } else {
            status = -1;
            data_size = sprintf(log_data, "Can't open log file. red_errno: %d\n", red_errno);
        }
    } else {
        status = -1;
        data_size = sprintf(log_data, "Log file %s does not exist\n", filename);
    }
    for (uint32_t i = data_size; i < max_file_size; i++) {
        log_data[i] = '\0';
    }
    memcpy(&packet->data[STATUS_BYTE], &status, 1);
    memcpy(&packet->data[OUT_DATA_BYTE], log_data, max_string_length);
    set_packet_length(packet, max_string_length + 2);
    vPortFree(log_data);
    return SATR_OK;
}

/**
 * @brief
 *      logger service app to perform operations based on the given service subtype

 * @param packet
        the packet that holds the service subtype and will be filled with the log data
 * @return SAT_returnState
        state to define success of the operation
 */
SAT_returnState logger_service_app(csp_packet_t *packet) {
    uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];
    int8_t status;
    uint32_t *data32;
    int32_t file_size;
    char *log_file;

    switch (ser_subtype) {
    case SET_FILE_SIZE:
        // pull param from packet
        data32 = (uint32_t *)(packet->data + 1);
        file_size = data32[0];
        status = set_logger_file_size(file_size);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1); // +1 for subservice
        break;
    case GET_FILE_SIZE:
        status = get_logger_file_size(&file_size);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &file_size, sizeof(file_size));
        set_packet_length(packet, sizeof(int8_t) + sizeof(file_size) + 1);
        break;
    case GET_FILE:
        log_file = get_logger_file();
        get_file(log_file, packet);
        break;
    case GET_OLD_FILE:
        log_file = get_logger_old_file();
        get_file(log_file, packet);
        break;
    default:
        ex2_log("No such subservice\n");
        return SATR_PKT_ILLEGAL_SUBSERVICE;
    }

    return SATR_OK;
}

SAT_returnState start_logger_service(void);

/**
 * @brief
 *      FreeRTOS logger server task
 * @details
 *      Accepts incoming logger service packets and executes the application
 * @param void* param
 * @return None
 */
void logger_service(void *param) {
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_RDPREQ);
    csp_bind(sock, TC_LOGGER_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN);

    for (;;) {
        csp_conn_t *conn;
        csp_packet_t *packet;
        if ((conn = csp_accept(sock, CSP_MAX_TIMEOUT)) == NULL) {
            /* timeout */
            continue;
        }
        while ((packet = csp_read(conn, 50)) != NULL) {
            if (logger_service_app(packet) != SATR_OK) {
                // something went wrong, this shouldn't happen
                csp_buffer_free(packet);
            } else {
                if (!csp_send(conn, packet, 50)) {
                    csp_buffer_free(packet);
                }
            }
        }
        csp_close(conn); // frees buffers used
    }
}

/**
 * @brief
 *      Start the logger server task
 * @details
 *      Starts the FreeRTOS task responsible for accepting incoming
 *      logger service requests
 * @param None
 * @return SAT_returnState
 *      success report
 */
SAT_returnState start_logger_service(void) {
    if (xTaskCreate((TaskFunction_t)logger_service, "start_logger_service", 1200, NULL, NORMAL_SERVICE_PRIO,
                    NULL) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK start_logger_service\n");
        return SATR_ERROR;
    }
    ex2_log("Service handlers started\n");
    return SATR_OK;
}
