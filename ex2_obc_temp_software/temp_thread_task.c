/*
 * temp_thread_task.c
 *
 *  Created on: Aug 19, 2020
 *      Author: alexqi
 */
#include "obc.h"

#include <FreeRTOS.h>
#include <stdio.h>
#include <TempSensor.h>

#include "service_utilities.h"
#include "services.h"
#include "system.h"
#include "task.h"

extern Equipment_Queues_t equipment_queues;

SAT_returnState start_detection_server() {
  /**
   * Create the queues & tasks for each service implemented by this module
   */
  if (!(equipment_queues.temp_sensor_queue =
            xQueueCreate((unsigned portBASE_TYPE)EQUIPMENT_QUEUE_LEN,
                         (unsigned portBASE_TYPE)CSP_PKT_QUEUE_SIZE))) {
    ex2_log("FAILED TO CREATE time_management_app_queue\n");
    return SATR_ERROR;
  };


  if (xTaskCreate((TaskFunction_t)demo_get_temp,
                  "demo_get_temp", 500, NULL, NORMAL_SERVICE_PRIO,
                  NULL) != pdPASS) {
    ex2_log("FAILED TO CREATE TASK temp_detection\n");
    return SATR_ERROR;
  };

  ex2_log("Detection process started\n");
  return SATR_OK;
}



