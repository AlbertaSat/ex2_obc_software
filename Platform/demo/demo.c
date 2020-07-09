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
 * @file demo.c
 * @author Andrew Rooney
 * @date 2020-06-06
 */

#include "demo.h"

#include <FreeRTOS.h>
#include <stdio.h>

#include "housekeeping_service.h"
#include "service_utilities.h"
#include "services.h"
#include "system.h"
#include "task.h"
#include "time_management_service.h"

extern Service_Queues_t service_queues;
/*create a variable to record # of packets sent to ground*/
unsigned int sent_count = 0;

/**
 * @brief
 * 		FreeRTOS task wakes up to service housekeeping request
 * @details
 * 		Will pass the incoming packet to the application code
 * @param void *param
 * 		not used
 */
static void housekeeping_app_route(void *parameters) {
  csp_packet_t packet;
  for (;;) {
    if (xQueueReceive(service_queues.hk_app_queue, &packet,
                      NORMAL_TICKS_TO_WAIT) == pdPASS) {
      hk_service_app(&packet);
    }
  }

  return;
}

/**
 * @brief
 * 		FreeRTOS task wakes up to service time_management request
 * @details
 * 		Will pass the incoming packet to the application code
 * @param void *param
 * 		not used
 */
static void time_management_app_route(void *parameters) {
  csp_packet_t packet;
  for (;;) {
    ex2_log("time_management_app Receive packet attempt\n");
    if (xQueueReceive(service_queues.time_management_app_queue, &packet,
                      NORMAL_TICKS_TO_WAIT) == pdPASS) {
      time_management_app(&packet);
    }
  }

  return;
}

/**
 * @brief
 * 		Initialize service handling tasks, and queues
 * @details
 * 		Starts the FreeRTOS queueues and the tasks that wait on them for
 * incoming CSP
 * @return SAT_returnState
 * 		success report
 */
SAT_returnState start_service_handlers() {
  /**
   * Create the queues & tasks for each service implemented by this module
   */
  if (!(service_queues.time_management_app_queue =
            xQueueCreate((unsigned portBASE_TYPE)SERVICE_QUEUE_LEN,
                         (unsigned portBASE_TYPE)CSP_PKT_QUEUE_SIZE))) {
    ex2_log("FAILED TO CREATE time_management_app_queue\n");
    return SATR_ERROR;
  };

  if (!(service_queues.hk_app_queue =
            xQueueCreate((unsigned portBASE_TYPE)SERVICE_QUEUE_LEN,
                         (unsigned portBASE_TYPE)CSP_PKT_QUEUE_SIZE))) {
    ex2_log("FAILED TO CREATE hk_app_queue\n");
    return SATR_ERROR;
  };

  if (xTaskCreate((TaskFunction_t)housekeeping_app_route,
                    "housekeeping_app_route", 2048, NULL, NORMAL_SERVICE_PRIO,
                    NULL) != pdPASS) {
    ex2_log("FAILED TO CREATE TASK housekeeping_app_route\n");
    return SATR_ERROR;
  };

  if (xTaskCreate((TaskFunction_t)time_management_app_route,
                    "time_management_app_route", 2048, NULL, NORMAL_SERVICE_PRIO,
                    NULL) != pdPASS) {
    ex2_log("FAILED TO CREATE TASK time_management_app_route\n");
    return SATR_ERROR;
  }
  ex2_log("Service handlers started\n");
  return SATR_OK;
}
