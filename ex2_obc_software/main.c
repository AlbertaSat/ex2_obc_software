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
#include <service_utilities.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <task.h>
#include <csp/drivers/usart.h>

#include "service_response.h"
#include "services.h"
#include "system.h" // platform definitions

/**
 * The main function must:
 *  - Define the Service_Queues_t service_queues;
 *  - Start the services handlers
 *  - Initialize CSP and the desired interface
 *  - Start the service server, and response server
 *  - Start all platform specific tasks, structures
 *  - Start the FreeRTOS scheduler
 */

/*Create service queues*/
Service_Queues_t service_queues;

void vAssertCalled(unsigned long ulLine, const char *const pcFileName);
static inline SAT_returnState init_interface();


int ex2_main(int argc, char **argv) {
  ex2_log("-- starting command demo --\n");
  TC_TM_app_id my_address = SYSTEM_APP_ID;

  /* Start platform-implemented service handlers & their queues */
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
  csp_route_start_task(500, 0);
  init_interface();

  /* Start service server, and response server */
  if (start_service_server() != SATR_OK || start_service_response() != SATR_OK) {
    ex2_log("Initialization error\n");
    return -1;
  }

  /* Start FreeRTOS! */
  vTaskStartScheduler();

  for (;;);

  return 0;
}

/**
 * @brief
 *      initialize zmq interface, and configure the routing table
 * @details
 *      start the localhost zmq server and add it to the default route
 * with no VIA address
 */
static inline SAT_returnState init_interface() {
  csp_iface_t *default_iface = NULL;
  csp_usart_conf_t conf = {
              .device = "yo",
              .baudrate = 9600, /* supported on all platforms */
              .databits = 8,
              .stopbits = 2,
              .paritysetting = 0,
              .checkparity = 0};
  int error = csp_usart_open_and_add_kiss_interface(&conf, CSP_IF_KISS_DEFAULT_NAME,  &default_iface);
  if(error != CSP_ERR_NONE) {
    return SATR_ERROR;
  }
  csp_rtable_set(CSP_DEFAULT_ROUTE, 0, default_iface, CSP_NO_VIA_ADDRESS);
  return SATR_OK;
}

void vAssertCalled(unsigned long ulLine, const char *const pcFileName) {
  ex2_log("error line: %lu in file: %s", ulLine, pcFileName);
}
