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
#include "task_manager/task_manager.h"
#include "time_management/time_management_service.h"
#include "util/service_utilities.h"
#include "eps.h"
#include "adcs.h"
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <main/system.h>
#include <stdio.h>

#define TIME_MANAGEMENT_SIZE 300

SAT_returnState time_management_app(csp_packet_t *packet);

static uint32_t svc_wdt_counter = 0;

static uint32_t get_svc_wdt_counter() { return svc_wdt_counter; }

/**
 * @brief
 *      FreeRTOS time management server task
 * @details
 *      Accepts incoming time management service packets and executes the application
 * @param void* param
 * @return None
 */
void time_management_service(void *param) {
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_HMACREQ);
    csp_bind(sock, TC_TIME_MANAGEMENT_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN);
    svc_wdt_counter++;
    for (;;) {
        svc_wdt_counter++;
        csp_conn_t *conn;
        csp_packet_t *packet;
        if ((conn = csp_accept(sock, DELAY_WAIT_TIMEOUT)) == NULL) {
            svc_wdt_counter++;
            /* timeout */
            continue;
        }
        svc_wdt_counter++;
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
    TaskHandle_t svc_tsk;
    taskFunctions svc_funcs = {0};
    svc_funcs.getCounterFunction = get_svc_wdt_counter;
    if (xTaskCreate((TaskFunction_t)time_management_service, "time_management_service", TIME_MANAGEMENT_SIZE, NULL,
                    NORMAL_SERVICE_PRIO, &svc_tsk) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK time_management_service\n");
        return SATR_ERROR;
    }
    ex2_register(svc_tsk, svc_funcs);

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
SAT_returnState time_management_app(csp_packet_t *packet) {
    uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];
    int8_t status;
    uint32_t temp_time;

    switch (ser_subtype) {

    case GET_TIME: {
        // Step 1: get the data

        status = RTCMK_GetUnix(&temp_time);
        // Step 2: convert to network order
        temp_time = csp_hton32(temp_time);
        // step3: copy data & status byte into packet
        memcpy(&packet->data[STATUS_BYTE], &status,
               sizeof(int8_t)); // 0 for success
        memcpy(&packet->data[OUT_DATA_BYTE], &temp_time, sizeof(uint32_t));
        // Step 4: set packet length
        set_packet_length(packet, sizeof(int8_t) + sizeof(uint32_t) + 1); // plus one for sub-service
        break;
    }

    case SET_TIME: {
        cnv8_32(&packet->data[IN_DATA_BYTE], &temp_time);
        temp_time = csp_ntoh32(temp_time);

        if (!TIMESTAMP_ISOK(temp_time)) {
            status = -1;
            memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        } else {
            status = RTCMK_SetUnix(temp_time);
            status += synchronize_all_clocks(temp_time);

            memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        }

        set_packet_length(packet, sizeof(int8_t) + 1); // +1 for subservice

        break;
    }

    default:
        ex2_log("No such subservice\n");
        return SATR_PKT_ILLEGAL_SUBSERVICE;
    }
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
#if IS_EXALTA2 == 1 && PAYLOAD_IS_STUBBED == 0
    // Iris set time
#endif
    return status;
}
