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
 * @author Andrew Rooney, Haoran Qi
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

#include "service_response.h"
#include "services.h"
#include "system.h"  // platform definitions
#include <fcntl.h>

#include <pthread.h>
/**
 * The main function must:
 *  - Define the Service_Queues_t service_queues;
 *  - Start the services handlers
 *  - Initialize CSP and the desired interface
 *  - Start the service server, and response server
 *  - Start all platform specific tasks, structures
 *  - Start the FreeRTOS sceduler
 */

/*Create service queues*/
Service_Queues_t service_queues;

/* FIFO INTERFACE */
pthread_t rx_thread;
int rx_channel, tx_channel;
#define BUF_SIZE    250

int csp_fifo_tx(csp_iface_t *ifc, csp_packet_t *packet, uint32_t timeout);

csp_iface_t csp_if_fifo = {
    .name = "fifo",
    .nexthop = csp_fifo_tx,
    .mtu = BUF_SIZE,
};

int csp_fifo_tx(csp_iface_t *ifc, csp_packet_t *packet, uint32_t timeout) {
    /* Write packet to fifo */
    if (write(tx_channel, &packet->length, packet->length + sizeof(uint32_t) + sizeof(uint16_t)) < 0)
        printf("Failed to write frame\r\n");
    csp_buffer_free(packet);
    return CSP_ERR_NONE;
}

void * fifo_rx(void * parameters) {
    csp_packet_t *buf = csp_buffer_get(BUF_SIZE);
    /* Wait for packet on fifo */
    while (read(rx_channel, &buf->length, BUF_SIZE) > 0) {
        buf = csp_buffer_get(BUF_SIZE);
    }

    return NULL;
}
/** FIFO INTERFACE ENDS **/

void vAssertCalled(unsigned long ulLine, const char *const pcFileName);
static inline SAT_returnState init_zmq();

int main(int argc, char **argv) {
  ex2_log("-- starting command demo --\n");

  char *tx_channel_name, *rx_channel_name;
  tx_channel_name = "/datavolume1/sat_to_ground";
  rx_channel_name = "/datavolume1/ground_to_sat";

  TC_TM_app_id my_address = DEMO_APP_ID;
  csp_debug_level_t debug_level = CSP_INFO;

  /* Start platform-implemented service handlers & their queues */
  if (start_service_handlers() != SATR_OK) {
    ex2_log("COULD NOT START TELECOMMAND HANDLER\n");
    return -1;
  }

  for (csp_debug_level_t i = 0; i <= CSP_LOCK; ++i) {
    csp_debug_set_level(i, (i <= debug_level) ? true : false);
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

  tx_channel = open(tx_channel_name, O_RDWR);
  if (tx_channel < 0) {
      printf("Failed to open TX channel\r\n");
      return -1;
  }

  rx_channel = open(rx_channel_name, O_RDWR);
  if (rx_channel < 0) {
      printf("Failed to open RX channel\r\n");
      return -1;
  }
  pthread_create(&rx_thread, NULL, fifo_rx, NULL);
  ex2_log("Running at %d\n", my_address);
  /* Set default route and start router & server */
  csp_route_start_task(1024, 0);
  csp_iflist_add(&csp_if_fifo);
  csp_rtable_set(16, 0, &csp_if_fifo, CSP_NO_VIA_ADDRESS);
  csp_route_set(16, &csp_if_fifo, CSP_NODE_MAC);


  printf("Connection table\r\n");
  csp_conn_print_table();

  printf("Interfaces\r\n");
  csp_route_print_interfaces();

  printf("Route table\r\n");
  csp_route_print_table();

  /* Start service server, and response server */
  if (start_service_server() != SATR_OK ||
      start_service_response() != SATR_OK) {
    ex2_log("Initialization error\n");
    return -1;
  }

  /* Start FreeRTOS! */
  vTaskStartScheduler();

  for (;;)
    ;

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
