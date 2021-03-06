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
 * @file diagnosic.c
 * @author Andrew R. Rooney
 * @date Mar. 6, 2021
 */
#include "diagnostic/diagnostic.h"

#include <FreeRTOS.h>
#include <os_task.h>

static void * diagnostic_daemon(void *pvParameters);
SAT_returnState start_diagnostic_daemon(void);

/**
 * Run subsystem diagnositc periodically at a low priority. Any corrective
 * action should be handled in the hardware interface layer.
 *
 * @param pvParameters
 *    task parameters (not used)
 */
static void * diagnostic_daemon(void *pvParameters) {
    TickType_t delay = pdMS_TO_TICKS(1000);
    for ( ;; ) {
        // TODO Run subsystem diagnosics

        vTaskDelay(delay);
    }
}

/**
 * Start the diagnostics daemon
 *
 * @returns status
 *   error report of task creation
 */
SAT_returnState start_diagnostic_daemon(void) {
    if (xTaskCreate((TaskFunction_t)diagnostic_daemon,
                  "coordinate_management_daemon", 2048, NULL, DIAGNOSTIC_TASK_PRIO,
                  NULL) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK coordinate_management_daemon\n");
        return SATR_ERROR;
    }
    ex2_log("Coordinate management started\n");
    return SATR_OK;
}

