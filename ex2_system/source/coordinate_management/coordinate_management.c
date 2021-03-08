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
 * @file coordinate_management.c
 * @author Andrew R. Rooney
 * @date Mar. 6, 2021
 */
#include "coordinate_management/coordinate_management.h"

#include <FreeRTOS.h>
#include <os_task.h>

static void * coordinate_management_daemon(void *pvParameters);
SAT_returnState start_coordinate_management_daemon(void);

/**
 * Coordinate management. Handle updates of current latitude, longitude, and time as
 * reported by the Global Positioning System.
 *
 * @param pvParameters
 *    task parameters (not used)
 */
static void * coordinate_management_daemon(void *pvParameters) {
    TickType_t delay = pdMS_TO_TICKS(1000);
    for ( ;; ) {
        // TODO Application level management of the GPS

        vTaskDelay(delay);
    }
}

/**
 * Start the coordinate management daemon
 *
 * @returns status
 *   error report of task creation
 */
SAT_returnState start_coordinate_management_daemon(void) {
    if (xTaskCreate((TaskFunction_t)coordinate_management_daemon,
                  "coordinate_management_daemon", 2048, NULL, COORDINATE_MANAGEMENT_TASK_PRIO,
                  NULL) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK coordinate_management_daemon\n");
        return SATR_ERROR;
    }
    ex2_log("Coordinate management started\n");
    return SATR_OK;
}

