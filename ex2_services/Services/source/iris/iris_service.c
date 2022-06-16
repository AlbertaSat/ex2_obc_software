/*
 * Copyright (C) 2015  University of Alberta
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributeds in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <FreeRTOS.h>
#include <os_task.h>

#include "rtcmk.h"
#include "services.h"
#include "task_manager/task_manager.h"
#include "iris/iris_service.h"
#include "util/service_utilities.h"
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <main/system.h>
#include <stdio.h>

#define IRIS_SIZE 1000

SAT_returnState iris_service_app(csp_packet_t *packet);

static uint32_t svc_wdt_counter = 0;

static uint32_t get_svc_wdt_counter() { return svc_wdt_counter; }

/**
 * @brief
 *      Initialize and bind socket, and listen for packets in an infinite loop
 * @details
 *      Accepts incoming iris service packets and executes the application
 * @param void* param
 * @return None
 */
void iris_service(void *param) {
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_HMACREQ); // require RDP connection
    csp_bind(sock, TC_IRIS_SERVICE);
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
            if (iris_app(packet) != SATR_OK) {
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
 *      Start the iris server task
 * @details
 *      Starts the FreeRTOS task responsible for accepting incoming
 *      iris packets
 * @param None
 * @return SAT_returnState
 *      success report
 */
SAT_returnState start_iris_service(void) {
    TaskHandle_t svc_tsk;
    taskFunctions svc_funcs = {0};
    svc_funcs.getCounterFunction = get_svc_wdt_counter;
    if (xTaskCreate((TaskFunction_t)iris_service, "iris_service", IRIS_SIZE, NULL,
                    NORMAL_SERVICE_PRIO, &svc_tsk) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK iris_service\n");
        return SATR_ERROR;
    }
    ex2_register(svc_tsk, svc_funcs);

    return SATR_OK;
}

/**
 * @brief
 *      Handle incoming csp_packet_t
 * @details
 *      Takes a csp packet destined for the iris service,
 *              and will handle the packet based on it's subservice type.
 * @param csp_packet_t *packet
 *              Incoming CSP packet - we can be sure that this packet is
 *              valid and destined for this service.
 * @return SAT_returnState
 *      success report
 */
SAT_returnState iris_service_app(csp_packet_t *packet) {
    uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];
    uint8_t status;

    switch (ser_subtype) {
    case IRIS_POWER_ON:
        // TODO
    case IRIS_POWER_OFF:
        // TODO
    case IRIS_TAKE_IMAGE:
        // TODO
    case IRIS_DELIVER_IMAGE:
        // TODO
    case IRIS_PROGRAM_FLASH:
        // TODO
    case IRIS_GET_HK:
        // TODO
    default:
        ex2_log("No such subservice\n");
        return SATR_PKT_ILLEGAL_SUBSERVICE;
    }
    return SATR_OK;
}
