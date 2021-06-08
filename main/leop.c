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
 * Here lives the LEOP sequence. This should be an implementation of the
 * startup routines outlined in the Ex-Alta 2 Mission Operations Plan.
 *
 * Startup steps:
 * - Housekeeping and DFGM data collection begins
 * - Battery heaters turn on
 * - UHF is commissioned (i.e. powered on)
 * - OBC is powered on at T+30min
 * - DFGM deploys at T+32min
 * - UHF Antenna deploys at T+34min
 * - The remaining steps are done by ground user. See MOP document.
 *
 * @file leop.c
 * @author Andrew Rooney
 * @date Mar. 6, 2021
 */
#include <FreeRTOS.h>
#include <os_task.h>
#include <os_timer.h>

#include "system_tasks.h"
#include "services.h"
#include "deployablescontrol.h"

#define TWO_MINUTES pdMS_TO_TICKS(2 * 60 * 1000)
#define TWENTY_SECONDS pdMS_TO_TICKS(20 * 1000)

static TimerHandle_t leop_timer = NULL;


void init_leop(TickType_t wait_time);
static void leop_sequence_callback(TimerHandle_t pxTimer);

/**
 * This function has two responsibilities:
 * 1) Do the LEOP routine items that are to start at time T+30min (i.e., when OBC is powered on).
 * 2) Register the LEOP callback after the given time period. This time period must comply with the NanoRacks specs.
 *
 * @alert This must take 30 Minutes for the flight-software to
 *  comply with the NanoRacks LEOP requirements
 *
 * @param wait_time (milliseconds)
 *  time to wait before commissioning subsystems
 */
void init_leop(const TickType_t wait_time) {

  // TODO: start housekeeping
  // TODO: Ensure battery heaters are on

  leop_timer = xTimerCreate((const char *) "LEOP Timer",
                            TWO_MINUTES,
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
static void leop_sequence_callback(const TimerHandle_t pxTimer) {
  /**
   * Commission hardware and start system.
   */

  const int retries = 10, extraCheck = 2;
  int i, passes;

  passes = 0;
  for (i = 0; i <= retries; i++) {
    if (deploy(DFGM)) {
      passes++;
      if (passes == extraCheck) {
        break;
      }
    } else {
      if (i == retries) {
        // TODO: Log error and reboot perhaps?
      }
    }
    vTaskDelay(TWENTY_SECONDS);
  }

  vTaskDelay(TWO_MINUTES);

  passes = 0;
  for (i = 0; i <= retries; i++) {
    if (deploy(UHF_P) && deploy(UHF_Z) && deploy(UHF_S) && deploy(UHF_N)) {
      passes++;
      if (passes == extraCheck) {
        break;
      }
    } else {
      if (i == retries) {
        // TODO: Log error and reboot perhaps?
      }
    }
    vTaskDelay(TWENTY_SECONDS);
  }

  // TODO: commission UHF

  /* start system tasks and service listeners */
  if (start_service_server() != SATR_OK ||
      start_system_tasks() != SATR_OK) {
    ex2_log("Initialization error\n");
  }
}
