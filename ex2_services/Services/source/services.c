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
 * @file services.c
 * @author Andrew Rooney
 * @date 2020-07-12
 */

#include "services.h"

#include <FreeRTOS.h>
#include <csp/csp.h>
#include <os_task.h>

#include "communication/communication_service.h"
#include "general.h"
#include "housekeeping/housekeeping_service.h"
#include "logger/logger_service.h"
#include "time_management/time_management_service.h"
#include "updater/updater.h"
#include "util/service_utilities.h"
#include "cli/cli.h"
#include "dfgm/dfgm_service.h"
#include "scheduler/scheduler.h"

void csp_server(void *parameters);
SAT_returnState start_service_server(void);

/**
 * @brief
 *      Start the services server, and response task
 * @details
 *      intitializes the FreeRTOS queue and task
 * @param void
 * @return SAT_returnState
 *      success or failure
 */
SAT_returnState start_service_server(void) {
    if (xTaskCreate((TaskFunction_t)csp_server, "csp_server THREAD", 256, NULL, NORMAL_SERVICE_PRIO, NULL) !=
        pdPASS) {
        return SATR_ERROR;
    }
    start_cli_service();
    if (//start_communication_service() != SATR_OK ||
        //start_time_management_service() != SATR_OK ||
        start_scheduler_service() != SATR_OK ||
        //start_housekeeping_service() != SATR_OK ||
        start_general_service() != SATR_OK ||
        start_updater_service() != SATR_OK ||
        start_logger_service() != SATR_OK) {
        //start_dfgm_service() != SATR_OK) {
        return SATR_ERROR;
    }
    return SATR_OK;
}

/**
 * @brief
 * 		CSP server loop to services pings, and such
 * @details
 * 		send incoming CSP packets to the appropriate service queues,
 * otherwise pass it to the CSP service handler
 * @param void *parameters
 * 		not used
 */
void csp_server(void *parameters) {
    csp_socket_t *sock;

    /* Create socket and listen for incoming connections */
    sock = csp_socket(CSP_SO_NONE);
    //  csp_bind(sock, CSP_CMP);
    csp_bind(sock, CSP_PING);
    //  csp_bind(sock, CSP_PS);
    //  csp_bind(sock, CSP_MEMFREE);
    //  csp_bind(sock, CSP_REBOOT);
    //  csp_bind(sock, CSP_BUF_FREE);
    //  csp_bind(sock, CSP_UPTIME);

    csp_listen(sock, SERVICE_BACKLOG_LEN);

    for (;;) {
        /* Process incoming packet */
        csp_conn_t *conn;
        csp_packet_t *packet;
        if ((conn = csp_accept(sock, CSP_MAX_TIMEOUT)) == NULL) {
            /* timeout */
            continue;
        }

        while ((packet = csp_read(conn, 50)) != NULL) {
            csp_service_handler(conn, packet);
        }
        csp_close(conn);
    }
}

// for testing only. do hex dump
// size is the number of bytes we want to print
void hex_dump(char *stuff, int size) {
    uint32_t current_packet_index = 0;
    printf("printing number of bytes: %u\n", size);
    int j = 0;
    for (j = 0; j < size; j += 1) {
        if (stuff[current_packet_index] < 0x10) {
            printf("0");
        }
        printf("%X ", stuff[current_packet_index]);
        current_packet_index += 1;
        if (current_packet_index % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}
