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

#include "system.h"
#include "task.h"
#include "time_management_service.h"

extern service_queues_t service_queues;

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
    if (xQueueReceive(service_queues.time_management_app_queue, &packet,
                      NORMAL_TICKS_TO_WAIT) == pdPASS) {
      printf("Time time_management_service SERVICE RX: %d, ID: %d\n",
             packet.data[0], packet.id);
      time_management_app(&packet);
    }
  }
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
            xQueueCreate((unsigned portBASE_TYPE)NORMAL_QUEUE_LEN,
                         (unsigned portBASE_TYPE)NORMAL_QUEUE_SIZE))) {
    printf("FAILED TO CREATE time_management_app_queue");
    return SATR_ERROR;
  };

  xTaskCreate((TaskFunction_t)time_management_app_route, "time_management_app",
              2048, NULL, NORMAL_SERVICE_PRIO, NULL);

  return SATR_OK;
}
