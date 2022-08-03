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
 * @file dfgm_service.c
 * @author Daniel Sacro
 * @date 2022-02-08
 */

#include "dfgm/dfgm_service.h"

#include <FreeRTOS.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <main/system.h>

#include "dfgm.h"
#include "services.h"
#include "task_manager/task_manager.h"
#include "util/service_utilities.h"

#include <limits.h>
#include <stdint.h>

SAT_returnState dfgm_service_app(csp_packet_t *packet);

static uint32_t svc_wdt_counter = 0;
static uint32_t get_svc_wdt_counter() { return svc_wdt_counter; }

/**
 * @brief
 *      FreeRTOS DFGM server task
 * @details
 *      Accepts incoming DFGM service packets and executes
 *      the application
 * @param void* param
 * @return None
 */
void dfgm_service(void *param) {
    // socket initialization
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_HMACREQ);
    csp_bind(sock, TC_DFGM_SERVICE);
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
            if (dfgm_service_app(packet) != SATR_OK) {
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
 *      Starts the DFGM server task
 * @details
 *      Starts the FreeRTOS task responsible for accepting incoming
 *      DFGM service requests
 * @param None
 * @return SAT_returnState
 *      Success report
 */
SAT_returnState start_dfgm_service(void) {
    TaskHandle_t svc_tsk;
    taskFunctions svc_funcs = {0};
    svc_funcs.getCounterFunction = get_svc_wdt_counter;

    if (xTaskCreate((TaskFunction_t)dfgm_service, "dfgm_service", 1024, NULL, NORMAL_SERVICE_PRIO, &svc_tsk) !=
        pdPASS) {
        ex2_log("FAILED TO CREATE TASK start_dfgm_service\n");
        return SATR_ERROR;
    }
    ex2_register(svc_tsk, svc_funcs);
    ex2_log("DFGM service started\n");
    return SATR_OK;
}

/**
 * @brief
 *      Takes a CSP packet and switches based on the subservice command
 * @details
 *      Reads/Writes data from DFGM EHs using subservices
 * @param *packet
 *      The CSP packet
 * @return SAT_returnState
 *      Success or failure
 */
SAT_returnState dfgm_service_app(csp_packet_t *packet) {
    uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];
    int8_t status;
    SAT_returnState return_state = SATR_OK; // OK until an error is encountered
    uint32_t givenRuntime = 0;
    int32_t maxRuntime = INT_MAX; // INT_MAX = 2^31 - 1 seconds = ~68.05 yrs

    switch (ser_subtype) {
    case DFGM_RUN: {
        // Get runtime
        cnv8_32(&packet->data[IN_DATA_BYTE], &givenRuntime);

        // Give runtime (in seconds) to the DFGM Rx Task
        status = HAL_DFGM_run(givenRuntime);

        // Return success report
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case DFGM_START: {
        // Give the max runtime (in seconds) to the DFGM Rx Task
        status = HAL_DFGM_run(maxRuntime);

        // Return success report
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case DFGM_STOP: {
        // Tell the DFGM Rx Task to stop running
        status = HAL_DFGM_stop();

        // Return success report
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case DFGM_GET_HK: {
        // Get DFGM HK data
        DFGM_Housekeeping HK = {0};
        status = HAL_DFGM_get_HK(&HK);

        // Convert floats from host byte order to server byte order
        HK.coreVoltage = csp_hton16(HK.coreVoltage);
        HK.sensorTemp = csp_hton16(HK.sensorTemp);
        HK.refTemp = csp_hton16(HK.refTemp);
        HK.boardTemp = csp_hton16(HK.boardTemp);
        HK.posRailVoltage = csp_hton16(HK.posRailVoltage);
        HK.inputVoltage = csp_hton16(HK.inputVoltage);
        HK.refVoltage = csp_hton16(HK.refVoltage);
        HK.inputCurrent = csp_hton16(HK.inputCurrent);
        HK.reserved1 = csp_hton16(HK.reserved1);
        HK.reserved2 = csp_hton16(HK.reserved2);
        HK.reserved3 = csp_hton16(HK.reserved3);
        HK.reserved4 = csp_hton16(HK.reserved4);

        // Return success report and DFGM HK data
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &HK, sizeof(HK));
        set_packet_length(packet, sizeof(int8_t) + sizeof(HK) + 1);
        break;
    }

    default:
        ex2_log("No such subservice!\n");
        return_state = SATR_PKT_ILLEGAL_SUBSERVICE;
    }

    return return_state;
}
