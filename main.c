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
 * @file main.c
 * @author Andrew Rooney
 * @date 2020-06-06
 */

#include <FreeRTOS.h>
#include <csp/csp.h>
#include <csp/interfaces/csp_if_zmqhub.h>
#include <fcntl.h>
#include <service_utilities.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <task.h>
#include <unistd.h>

#include "demo.h"
#include "services.h"
#include "system.h"

// #ifndef CSP_HAVE_LIBZMQ
// #define CSP_HAVE_LIBZMQ 1
// #endif

/*Create service queues*/
Service_Queues_t service_queues;
/* A response queue to ground station for all service*/
xQueueHandle response_queue;

void server_loop(void *parameters);
void vAssertCalled(unsigned long ulLine, const char *const pcFileName);
static inline SAT_returnState init_zmq();

int main(int argc, char **argv) {
  ex2_log("-- starting command demo --\n");
  TC_TM_app_id my_address = DEMO_APP_ID;

  if (start_service_handlers() != SATR_OK) {
    ex2_log("COULD NOT START TELECOMMAND HANDLER\n");
    return -1;
  }

  /* Init CSP with address and default settings */
  csp_conf_t csp_conf;
  csp_conf_get_defaults(&csp_conf);
  csp_conf.address = my_address;
  int error = csp_init(&csp_conf);
  if (error != CSP_ERR_NONE) {
    ex2_log("csp_init() failed, error: %d\n", error);
    return -1;
  }
  ex2_log("Running at %d\n", my_address);
  /* Set default route and start router & server */
  // csp_route_set(CSP_DEFAULT_ROUTE, &this_interface, CSP_NODE_MAC);
  csp_route_start_task(500, 0);

  #ifdef USE_LOCALHOST
  init_zmq();
  #else
  // implement other interfaces perhaps..
  #endif

  xTaskCreate((TaskFunction_t)server_loop, "SERVER THREAD", 2048, NULL, 1,
              NULL);

  vTaskStartScheduler();

  for (;;);

  return 0;
}

/**
 * @brief
 * 		initialize zmq interface, and configure the routing table
 * @details
 * 		start the localhost zmq server and add it to the default route
 * with no VIA address
 */
static inline SAT_returnState init_zmq() {
  csp_iface_t *default_iface = NULL;
  int error =
      csp_zmqhub_init(csp_get_address(), "localhost", 0, &default_iface);
  if (error != CSP_ERR_NONE) {
    ex2_log("failed to add ZMQ interface [%s], error: %d", "localhost", error);
    return SATR_ERROR;
  }
  csp_rtable_set(CSP_DEFAULT_ROUTE, 0, default_iface, CSP_NO_VIA_ADDRESS);
  return SATR_OK;
}

void vAssertCalled(unsigned long ulLine, const char *const pcFileName) {
  ex2_log("error line: %lu in file: %s", ulLine, pcFileName);
}

/**
 * @brief
 * 		main CSP server loop
 * @details
 * 		send incoming CSP packets to the appropriate service queues,
 * otherwise pass it to the CSP service handler
 * @param void *parameters
 * 		not used
 */
void server_loop(void *parameters) {
  csp_socket_t *sock;
  csp_conn_t *conn;
  csp_packet_t *packet;

  /* Create socket and listen for incoming connections */
  sock = csp_socket(CSP_SO_NONE);
  csp_bind(sock, CSP_ANY);
  csp_listen(sock, 5);
  portBASE_TYPE err,err2;

  /* Super loop */
  ex2_log("Starting CSP server\n");
  for (;;) {
    /* Process incoming packet */
    if ((conn = csp_accept(sock, 10000)) == NULL) {
      /* timeout */
      continue;
    }
    while ((packet = csp_read(conn, 50)) != NULL) {
      switch (csp_conn_dport(conn)) {
        case TC_HOUSEKEEPING_SERVICE:
          err = xQueueSendToBack(service_queues.hk_app_queue, packet,
                                 NORMAL_TICKS_TO_WAIT);
          if (err != pdPASS) {
            ex2_log("FAILED TO QUEUE MESSAGE");
          }
          csp_buffer_free(packet);
          break;

        case TC_TIME_MANAGEMENT_SERVICE:
          err = xQueueSendToBack(service_queues.time_management_app_queue,
                                 packet, NORMAL_TICKS_TO_WAIT);
          if (err != pdPASS) {
            ex2_log("FAILED TO QUEUE MESSAGE");
          }
          csp_buffer_free(packet);
          break;

        default:
          /* let CSP respond to requests */
          csp_service_handler(conn, packet);
          break;
      }
    }
    csp_close(conn);
  }
}
