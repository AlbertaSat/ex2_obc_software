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
/**
 * @file system_stats.c
 * @author Andrew R. Rooney
 * @date 2020-09-25
 */
#include "performance_monitor/system_stats.h"

#include <FreeRTOS.h>
#include <os_task.h>
#include <stdio.h>

static void system_stats_daemon(void * param);
SAT_returnState start_system_stats_daemon(void);

/**
 * Periodically collect and display system statistics. For
 * performance monitoring.
 *
 * @param param
 *  any task parameters (not used)
 */
static void system_stats_daemon(void * param) {
  const TickType_t xDelay = 10000 / portTICK_PERIOD_MS;
  for(;;) {
    vTaskDelay(xDelay);
    char buf[1024];
    vTaskGetRunTimeStats(buf);
//    fprintf(stderr, "%s\n", buf);
  }
}

/**
 * Start the task statistics daemon.
 *
 * @return status
 *  error report
 */
SAT_returnState start_system_stats_daemon(void) {
  if (xTaskCreate((TaskFunction_t)system_stats_daemon,
                "task_stats", 512, NULL, SYSTEM_STATS_TASK_PRIO,
                NULL) != pdPASS) {
    ex2_log("FAILED TO CREATE TASK task_stats\n");
    return SATR_ERROR;
  }
  ex2_log("Stats task started\n");
  return SATR_OK;
}

