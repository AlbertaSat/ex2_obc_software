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
#include "general.h"
#include "HL_reg_system.h"
#include "privileged_functions.h"
#include "services.h"
#include "task_manager/task_manager.h"
#include "util/service_utilities.h"
#include <FreeRTOS.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <main/system.h>
#include <os_task.h>
#include "diagnostic.h"
#include "deployablescontrol.h"
#include "bl_eeprom.h"
#include "uhf_pipe_timer.h"
#include "beacon_task.h"

SAT_returnState general_app(csp_conn_t *conn, csp_packet_t *packet);
void general_service(void *param);

static uint32_t svc_wdt_counter = 0;

static uint32_t get_svc_wdt_counter() { return svc_wdt_counter; }
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
    TaskHandle_t svc_tsk;
    taskFunctions svc_funcs = {0};
    svc_funcs.getCounterFunction = get_svc_wdt_counter;

    if (xTaskCreate((TaskFunction_t)general_service, "general_service", 300, NULL, NORMAL_SERVICE_PRIO,
                    &svc_tsk) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK general_service\n");
        return SATR_ERROR;
    }
    ex2_register(svc_tsk, svc_funcs);
    ex2_log("General service started\n");
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
    sock = csp_socket(CSP_SO_HMACREQ); // require RDP connection
    csp_bind(sock, TC_GENERAL_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN);
    svc_wdt_counter++;

    for (;;) {
        svc_wdt_counter++;
        csp_conn_t *conn;
        csp_packet_t *packet;

        // wait for connection, timeout
        if ((conn = csp_accept(sock, DELAY_WAIT_TIMEOUT)) == NULL) {
            svc_wdt_counter++;
            /* timeout */
            continue;
        }
        svc_wdt_counter++;

        while ((packet = csp_read(conn, 50)) != NULL) {
            if (general_app(conn, packet) != SATR_OK) {
                csp_buffer_free(packet);
                ex2_log("Error responding to packet");
            } else {
                if (!csp_send(conn, packet, CSP_TIMEOUT)) {
                    csp_buffer_free(packet);
                }
            }
        }
        csp_close(conn); // frees buffers used
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
SAT_returnState general_app(csp_conn_t *conn, csp_packet_t *packet) {
    uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];
    int8_t status;
    char reboot_type;

    switch (ser_subtype) {
    case REBOOT: {

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
        csp_send(conn, packet, CSP_TIMEOUT);

        if (status == 0) {
            sw_reset(reboot_type, REQUESTED);
        }

        break;
    }

    case DEPLOY_DEPLOYABLES: {
        Deployable_t dep;
        memcpy(&dep, &packet->data[IN_DATA_BYTE], sizeof(uint8_t));
        status = (int8_t)deploy(dep, 1);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + sizeof(uint16_t) + 1); // +1 for subservice

        break;
    }

    case GET_SWITCH_STATUS: {
        uint8_t sw[8] = {0};
        for (Deployable_t i = DFGM; i <= Starboard; i++) {
            sw[i] = (uint8_t)switchstatus(i);
        }
        packet->data[STATUS_BYTE] = 0;
        memcpy(&packet->data[OUT_DATA_BYTE], sw, sizeof(sw));
        set_packet_length(packet, sizeof(sw) + 2); // +1 for subservice

        break;
    }

    case GET_UHF_WATCHDOG_TIMEOUT: {
        status = 0;
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        unsigned int timeout = get_uhf_watchdog_delay();
        memcpy(&packet->data[OUT_DATA_BYTE], &timeout, sizeof(unsigned int));
        set_packet_length(packet, sizeof(int8_t) + sizeof(unsigned int) + 1); // +1 for subservice

        break;
    }

    case SET_UHF_WATCHDOG_TIMEOUT: {
        unsigned int timeout_new = 0;
        memcpy(&timeout_new, &packet->data[IN_DATA_BYTE], sizeof(unsigned int));
        status = set_uhf_watchdog_delay(timeout_new);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1); // +1 for subservice

        break;
    }

    case GET_SBAND_WATCHDOG_TIMEOUT: {
        status = 0;
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        unsigned int timeout = get_sband_watchdog_delay();
        memcpy(&packet->data[OUT_DATA_BYTE], &timeout, sizeof(unsigned int));
        set_packet_length(packet, sizeof(int8_t) + sizeof(unsigned int) + 1); // +1 for subservice

        break;
    }

    case SET_SBAND_WATCHDOG_TIMEOUT: {
        unsigned int timeout_new = 0;
        memcpy(&timeout_new, &packet->data[IN_DATA_BYTE], sizeof(unsigned int));
        status = set_sband_watchdog_delay(timeout_new);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1); // +1 for subservice

        break;
    }

    case GET_CHARON_WATCHDOG_TIMEOUT: {
        status = 0;
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        unsigned int timeout = get_charon_watchdog_delay();
        memcpy(&packet->data[OUT_DATA_BYTE], &timeout, sizeof(unsigned int));
        set_packet_length(packet, sizeof(int8_t) + sizeof(unsigned int) + 1); // +1 for subservice

        break;
    }

    case SET_CHARON_WATCHDOG_TIMEOUT: {
        unsigned int timeout_new = 0;
        memcpy(&timeout_new, &packet->data[IN_DATA_BYTE], sizeof(unsigned int));
        status = set_charon_watchdog_delay(timeout_new);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1); // +1 for subservice

        break;
    }

    case GET_ADCS_WATCHDOG_TIMEOUT: {
        status = 0;
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        unsigned int timeout = get_adcs_watchdog_delay();
        memcpy(&packet->data[OUT_DATA_BYTE], &timeout, sizeof(unsigned int));
        set_packet_length(packet, sizeof(int8_t) + sizeof(unsigned int) + 1); // +1 for subservice

        break;
    }

    case SET_ADCS_WATCHDOG_TIMEOUT: {
        unsigned int timeout_new = 0;
        memcpy(&timeout_new, &packet->data[IN_DATA_BYTE], sizeof(unsigned int));
        status = set_adcs_watchdog_delay(timeout_new);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1); // +1 for subservice

        break;
    }

    case GET_NS_PAYLOAD_WATCHDOG_TIMEOUT: {
        status = 0;
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        unsigned int timeout = get_ns_watchdog_delay();
        memcpy(&packet->data[OUT_DATA_BYTE], &timeout, sizeof(unsigned int));
        set_packet_length(packet, sizeof(int8_t) + sizeof(unsigned int) + 1); // +1 for subservice

        break;
    }

    case SET_NS_PAYLOAD_WATCHDOG_TIMEOUT: {
        unsigned int timeout_new = 0;
        memcpy(&timeout_new, &packet->data[IN_DATA_BYTE], sizeof(unsigned int));
        status = set_ns_watchdog_delay(timeout_new);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1); // +1 for subservice

        break;
    }

    case ENABLE_BEACON_TASK: {
        status = enable_beacon_task();
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1); // +1 for subservice
        break;
    }

    case DISABLE_BEACON_TASK: {
        status = disable_beacon_task();
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1); // +1 for subservice
        break;
    }

    case BEACON_TASK_GET_STATE: {
        status = 0;
        bool state = beacon_task_get_state();
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &state, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + sizeof(bool) + 1); // +1 for subservice
        break;
    }


    default: {
        ex2_log("No such subservice\n");
        return SATR_PKT_ILLEGAL_SUBSERVICE;
    }
    }
    return SATR_OK;
}
