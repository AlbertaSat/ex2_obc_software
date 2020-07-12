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
#include <task.h>
#include <csp/csp.h>

#include "service_utilities.h"
#include "system.h"

extern Service_Queues_t service_queues;

/**
 * @brief
 * 		main CSP server loop
 * @details
 * 		send incoming CSP packets to the appropriate service queues,
 * otherwise pass it to the CSP service handler
 * @param void *parameters
 * 		not used
 */
void service_server(void *parameters) {
  csp_socket_t *sock;
  csp_conn_t *conn;

  /* Create socket and listen for incoming connections */
  sock = csp_socket(CSP_SO_NONE);
  csp_bind(sock, CSP_ANY);
  csp_listen(sock, 5);
  portBASE_TYPE err;

  /* Super loop */
  ex2_log("Starting CSP server\n");
  for (;;) {
    /* Process incoming packet */
    if ((conn = csp_accept(sock, 10000)) == NULL) {
      /* timeout */
      continue;
    }

    csp_packet_t *packet;
    while ((packet = csp_read(conn, 50)) != NULL) {
      switch (csp_conn_dport(conn)) {
        case TC_HOUSEKEEPING_SERVICE:
          err = xQueueSendToBack(service_queues.hk_app_queue, (void *) &packet,
                                 NORMAL_TICKS_TO_WAIT);
          if (err != pdPASS) {
            ex2_log("FAILED TO QUEUE MESSAGE");
          }
          break;

        case TC_TIME_MANAGEMENT_SERVICE:
          err = xQueueSendToBack(service_queues.time_management_app_queue,
                                 (void *) &packet, NORMAL_TICKS_TO_WAIT);
          if (err != pdPASS) {
            ex2_log("FAILED TO QUEUE MESSAGE");
          }
          break;

        default:
          /* let CSP respond to requests */
          csp_service_handler(conn, packet);
          break;
      }
    }
    csp_close(conn);
  }

  return;
}

/**
 * @brief
 * 		Start the services server, and response task
 * @details
 * 		intitializes the FreeRTOS queue and task
 * @param void
 * @return SAT_returnState
 * 		success or failure
 */
SAT_returnState start_service_server(void) {
  if (xTaskCreate((TaskFunction_t)service_server, "SERVER THREAD", 2048, NULL, 1,
                    NULL) != pdPASS) {
    return SATR_ERROR;
  }
  return SATR_OK;
}
