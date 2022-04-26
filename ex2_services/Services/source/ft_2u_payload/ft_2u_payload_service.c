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
 * @file ft_2u_payload_service.c
 * @author Daniel Sacro
 * @date
 */
#include "ft_2u_payload/ft_2u_payload_service.h"

#include <FreeRTOS.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <main/system.h>

#include "ft_2u_payload.h"
#include "services.h"
#include "task_manager/task_manager.h"
#include "util/service_utilities.h"

#include <stdio.h>
#include <string.h>

SAT_returnState FT_2U_payload_service_app(csp_packet_t *packet);

static uint32_t svc_wdt_counter = 0;
static uint32_t get_svc_wdt_counter() { return svc_wdt_counter; }

/**
 * @brief
 *      FreeRTOS 2U Payload File Transferring (FT) server task
 * @details
 *      Accepts incoming 2U Payload FT service packets and executes
 *      the application
 * @param void* param
 * @return None
 */
void FT_2U_payload_service(void *param) {
    // socket initialization
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_RDPREQ);
    csp_bind(sock, TC_2U_PAYLOAD_FT_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN);

    svc_wdt_counter++;

    for (;;) {
        // establish a connection
        csp_packet_t *packet;
        csp_conn_t *conn;
        if ((conn = csp_accept(sock, DELAY_WAIT_TIMEOUT)) == NULL) {
            svc_wdt_counter++;
            /* timeout */
            continue;
        }
        svc_wdt_counter++;

        // read and process packets
        while ((packet = csp_read(conn, 50)) != NULL) {
            if (FT_2U_payload_service_app(packet) != SATR_OK) {
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
 *      Starts the 2U Payload File Transferring (FT) server task
 * @details
 *      Starts the FreeRTOS task responsible for accepting incoming
 *      2U Payload FT service requests
 * @param None
 * @return SAT_returnState
 *      Success report
 */
SAT_returnState start_FT_2U_payload_service(void) {
    TaskHandle_t svc_tsk;
    taskFunctions svc_funcs = {0};
    svc_funcs.getCounterFunction = get_svc_wdt_counter;

    if (xTaskCreate((TaskFunction_t)FT_2U_payload_service, "FT_2U_payload_service", 1024, NULL,
                    NORMAL_SERVICE_PRIO, &svc_tsk) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK start_2U_payload_FT_service\n");
        return SATR_ERROR;
    }
    ex2_register(svc_tsk, svc_funcs);
    ex2_log("2U Payload FT service started\n");
    return SATR_OK;
}

/**
 * @brief
 *      Takes a CSP packet and switches based on the subservice command
 * @details
 *      Transfers files between the OBC and GS as well as the OBC and 2U
 *      Payload
 * @param *packet
 *      The CSP packet
 * @return SAT_returnState
 *      Success or failure
 */
SAT_returnState FT_2U_payload_service_app(csp_packet_t *packet) {
    uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];
    int8_t status;
    SAT_returnState return_state = SATR_OK; // OK until an error is encountered
    char * filename;

    switch (ser_subtype) {
    case FT_2U_PAYLOAD_DOWNLINK: {
        // Get filename
        filename = packet->data[IN_DATA_BYTE]

        // Prepare to send file data
        status = HAL_2U_PAYLOAD_getFile(filename);

        // Return file transfer mode and filename to GS
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], filename, sizeof(filename));
        set_packet_length(packet, sizeof(int8_t) + sizeof(filename) + 1);
        break;
    }

    case FT_2U_PAYLOAD_UPLINK: {
        // Get filename
        filename = packet->data[IN_DATA_BYTE]

        // Prepare to receive and process file data
        status = HAL_2U_PAYLOAD_putFile(filename);

        // Return file transfer mode and filename to GS
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], filename, sizeof(filename));
        set_packet_length(packet, sizeof(int8_t) + sizeof(filename) + 1);
        break;
    }

    case FT_2U_PAYLOAD_STOP_FT: {
        // Tell OBC to stop processing or sending file data
        status = HAL_2U_PAYLOAD_stopFileTransfer();

        // Return success report
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case FT_2U_PAYLOAD_SEND_BYTES: {
        // Tell OBC to send file data to GS
        FT_2U_PAYLOAD_filePacket outgoingPacket;
        status = HAL_2U_PAYLOAD_sendDataBytes(&outgoingPacket);

        // Return success report and N bytes of file data
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &outgoingPacket, sizeof(outgoingPacket));
        set_packet_length(packet, sizeof(int8_t) + sizeof(outgoingPacket) + 1);
        break;
    }

    case FT_2U_PAYLOAD_PROCESS_BYTES: {
        // Receive N bytes of file data from the GS
        FT_2U_PAYLOAD_filePacket incomingPacket = {0};

        // TODO - Requires testing, but either method below could work for reading bytes from CSP

//        cnv8_16(&packet->data[IN_DATA_BYTE], &incomingPacket.bytesToRead);
//        memcpy(&incomingPacket.byte, &packet->data[IN_DATA_BYTE + 2], incomingPacket.bytesToRead);

        memcpy(&incomingPacket, &packet->data[IN_DATA_BYTE], sizeof(incomingPacket));

        // Process file data
        status = HAL_2U_PAYLOAD_receiveDataBytes(&incomingPacket);

        // Return success report (saying ready to receive or abort FT)
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    default:
        ex2_log("No such subservice!\n");
        return_state = SATR_PKT_ILLEGAL_SUBSERVICE;
    }

    return return_state;
}
