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
 * @file leop.c
 * @author Andrew
 * @date Mar. 6, 2021
 */
#include <FreeRTOS.h>
#include <os_task.h>
#include <os_timer.h>

#include "system_tasks.h"
#include "services.h"

static TimerHandle_t leop_timer = NULL;

void init_leop(TickType_t wait_time);
static void leop_sequence_callback(TimerHandle_t pxTimer);

/**
 * Register the LEOP callback after the given time period. This time
 * period must comply with the NanoRacks specs.
 *
 * @alert This must take 30 Minutes for the flight-software to
 *  comply with the NanoRacks LEOP requirements
 *
 * @param wait_time (milliseconds)
 *  time to wait before commissioning subsystems
 */
void init_leop(TickType_t wait_time) {
  leop_timer = xTimerCreate((const char *) "LEOP Timer",
                            wait_time,
                            pdFALSE,
                            NULL,
                            leop_sequence_callback);
  configASSERT(leop_timer);
  xTimerStart(leop_timer, 0);
  return;
}

/**
 * Run LEOP sequence after timer has expired. Hardware should be commissioned
 * here in the prescribed order. System tasks will then be started.
 *
 * @param pxTimer
 *  timer handle (not used)
 */
static void leop_sequence_callback(TimerHandle_t pxTimer) {
  /**
   * Commission hardware and start system.
   */

  /* start system tasks and service listeners */
  if (start_service_server() != SATR_OK ||
      start_system_tasks() != SATR_OK) {
    ex2_log("Initialization error\n");
  }
}
