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
 * @file state_task.c
 * @author Andrew R. Rooney
 * @date Feb. 19, 2021
 */
#include "system_state/state_task.h"

#include <FreeRTOS.h>
#include <os_task.h>

#include "eps.h"

static void state_daemon(void *pvParam);
SAT_returnState start_state_daemon();

/**
 * Query state from NanoAvionics EPS and make required updates
 * as-per the modes of operations.
 *
 * @param param
 *  Task parameter (not used)
 */
static void state_daemon(void *pvParam) {
  TickType_t delay = pdMS_TO_TICKS(1000);
  for (;;) {
    /* TODO: Get the state from EPS system and make any required updates
     * to system operations.
     */
    vTaskDelay(delay);
  }
}

/**
 * start the state task daemon.
 *
 * @returns status
 *  error report of creation
 */
SAT_returnState start_state_daemon() {
  if (xTaskCreate((TaskFunction_t)state_daemon,
                "state_task", 512, NULL, STATE_TASK_PRIO,
                NULL) != pdPASS) {
    ex2_log("FAILED TO CREATE TASK state_task\n");
    return SATR_ERROR;
  }
  ex2_log("State task started\n");
  return SATR_OK;
}
