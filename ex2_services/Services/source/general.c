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

/*
 * general.c
 *
 *  Created on: May 14, 2021
 *      Author: Robert Taylor
 */
#include "HL_reg_system.h"
#include <FreeRTOS.h>
#include <os_task.h>

#include "general.h"
#include "privileged_functions.h"
#include "services.h"
#include "util/service_utilities.h"
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <main/system.h>

SAT_returnState general_app(csp_packet_t *packet);
void general_service(void *param);

csp_conn_t *conn;

/**
 * @brief
 *      Start the general server task
 * @details
 *      Starts the FreeRTOS task responsible for accepting incoming
 *      general packets
 * @param None
 * @return SAT_returnState
 *      success report
 */
SAT_returnState start_general_service(void) {
    if (xTaskCreate((TaskFunction_t)general_service, "general_service", 300, NULL, NORMAL_SERVICE_PRIO, NULL) !=
        pdPASS) {
        ex2_log("FAILED TO CREATE TASK general_service\n");
        return SATR_ERROR;
    }

    return SATR_OK;
}

/**
 * @brief
 *      FreeRTOS general server task
 * @details
 *      Accepts incoming csp connections to perform tasks not covered by other services
 * @param void* param
 * @return None
 */
void general_service(void *param) {
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_RDPREQ); // require RDP connection
    csp_bind(sock, TC_GENERAL_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN);

    for (;;) {
        csp_packet_t *packet;
        if ((conn = csp_accept(sock, CSP_MAX_TIMEOUT)) == NULL) {
            /* timeout */
            continue;
        }
        while ((packet = csp_read(conn, 50)) != NULL) {
            if (general_app(packet) != SATR_OK) {
                // something went wrong, this shouldn't happen
                csp_buffer_free(packet);
            } else {
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
 *      Handle incoming csp_packet_t
 * @details
 *      Takes a csp packet destined for the general service handler,
 *              and will handle the packet based on it's subservice type.
 * @param csp_packet_t *packet
 *              Incoming CSP packet - we can be sure that this packet is
 *              valid and destined for this service.
 * @return SAT_returnState
 *      success report
 */
SAT_returnState general_app(csp_packet_t *packet) {
    uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];
    int8_t status;
    char reboot_type;

    switch (ser_subtype) {
    case REBOOT:

        reboot_type = packet->data[IN_DATA_BYTE];

        switch (reboot_type) {
        case 'A':
        case 'B':
        case 'G':
            status = 0;
            break;
        default:
            status = -1;
            break;
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1); // +1 for subservice
        csp_send(conn, packet, 50);

        if (status == 0) {
            reboot_system(reboot_type);
        }

        break;

    default:
        ex2_log("No such subservice\n");
        return SATR_PKT_ILLEGAL_SUBSERVICE;
    }
    return SATR_OK;
}
