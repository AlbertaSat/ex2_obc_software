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
 * @file time_management_service.c
 * @author Andrew Rooney, Robert Taylor
 * @date 2020-06-06
 */

#include <FreeRTOS.h>
#include <os_task.h>

#include "rtcmk.h"
#include "services.h"

#include "time_management/time_management_service.h"
#include "util/service_utilities.h"
#include "eps.h"
#include "adcs.h"
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <main/system.h>
#include <logger/logger.h>
#include <stdio.h>

static SAT_returnState time_management_app(csp_packet_t *packet);

/**
 * @brief
 *      FreeRTOS time management server task
 * @details
 *      Accepts incoming time management service packets and executes the application
 * @param void* param
 * @return None
 */
static void time_management_service(void *param) {
    increment_commands_recv();
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_HMACREQ);
    csp_bind(sock, TC_TIME_MANAGEMENT_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN);

    for (;;) {

        csp_conn_t *conn;
        csp_packet_t *packet;
        if ((conn = csp_accept(sock, DELAY_WAIT_TIMEOUT)) == NULL) {

            /* timeout */
            continue;
        }

        while ((packet = csp_read(conn, 50)) != NULL) {
            if (time_management_app(packet) != SATR_OK) {
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
 *      Start the time management server task
 * @details
 *      Starts the FreeRTOS task responsible for accepting incoming
 *      time management packets
 * @param None
 * @return SAT_returnState
 *      success report
 */
SAT_returnState start_time_management_service(void) {

    if (xTaskCreate((TaskFunction_t)time_management_service, "time_management_service", TIMEMANAGEMENT_SVC_SIZE,
                    NULL, NORMAL_SERVICE_PRIO, NULL) != pdPASS) {
        sys_log(WARN, "FAILED TO CREATE TASK time_management_service");
        return SATR_ERROR;
    }
    return SATR_OK;
}

/**
 * @brief
 * 		Handle incoming csp_packet_t
 * @details
 * 		Takes a csp packet destined for the time_management service,
 *              and will handle the packet based on it's subservice type.
 * @param csp_packet_t *packet
 *              Incoming CSP packet - we can be sure that this packet is
 *              valid and destined for this service.
 * @return SAT_returnState
 * 		success report
 */
static SAT_returnState time_management_app(csp_packet_t *packet) {
    uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];
    int8_t status = SATR_OK;
    uint32_t temp_time;

    switch (ser_subtype) {
    case GET_TIME: {
        // Step 1: get the data
        temp_time = RTCMK_Unix_Now();
        // Step 2: convert to network order
        sys_log(INFO, "get_time: %ld", temp_time);

        // step3: copy data into packet
        temp_time = csp_hton32(temp_time);
        memcpy(&packet->data[OUT_DATA_BYTE], &temp_time, sizeof(uint32_t));
        // Step 4: set packet length
        set_packet_length(packet, sizeof(int8_t) + sizeof(uint32_t) + 1); // plus one for sub-service
        break;
    }

    case SET_TIME: {
        memcpy((char *) &temp_time, &packet->data[IN_DATA_BYTE], sizeof(uint32_t));
        temp_time = csp_ntoh32(temp_time);

        if (!TIMESTAMP_ISOK(temp_time)) {
            sys_log(NOTICE, "set_time illegal time: %ld", temp_time);
            status = -1;
        } else {
            sys_log(INFO, "set_time: %ld", temp_time);
            status = RTCMK_SetUnix(temp_time);
            status += synchronize_all_clocks(temp_time);
        }
        set_packet_length(packet, 2); // +1 for subservice
        break;
    }

    default:
        sys_log(NOTICE, "No such subservice\n");
        status = SATR_PKT_ILLEGAL_SUBSERVICE;
        set_packet_length(packet, 2); // +1 for subservice
        break;
    }

    packet->data[STATUS_BYTE] = status;
    return SATR_OK;
}

SAT_returnState synchronize_all_clocks(uint32_t temp_time) {
    SAT_returnState status = SATR_OK;
#if ADCS_IS_STUBBED == 0
    status += (SAT_returnState)HAL_ADCS_set_unix_t(temp_time, 0);
#endif
#if EPS_IS_STUBBED == 0
    status += eps_set_unix_time(&temp_time);
#endif
#if IRIS_IS_STUBBED == 0
    // Iris set time
#endif
    return status;
}
