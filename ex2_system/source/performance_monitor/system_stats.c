/*
 * Copyright (C) 2020  University of Alberta
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
 * @file communication_service.c
 * @author Andrew Rooney
 * @date 2020-09-25
 */
#include "performance_monitor/system_stats.h"

#include <FreeRTOS.h>
#include <os_task.h>
#include <csp/csp.h>
#include <main/system.h>

#include "services.h"

void task_stats(void * param) {
    const TickType_t xDelay = 10000 / portTICK_PERIOD_MS;
    for(;;) {
        vTaskDelay(xDelay);
        char buf[1024];
        vTaskGetRunTimeStats(buf);
//        fprintf(stderr, "%s\n", buf);
    }
}

SAT_returnState start_task_stats(void) {
  if (xTaskCreate((TaskFunction_t)task_stats,
                  "task_stats", 512, NULL, NORMAL_SERVICE_PRIO,
                  NULL) != pdPASS) {
    ex2_log("FAILED TO CREATE TASK start_communication_service\n");
    return SATR_ERROR;
  }
  ex2_log("Service handlers started\n");
  return SATR_OK;
}

