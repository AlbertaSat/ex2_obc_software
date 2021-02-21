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
 * @file service_utilities.c
 * @author Andrew Rooney
 * @date 2020-07-23
 */

#include <FreeRTOS.h>
#include <queue.h>
#include <os_semphr.h>
#include <task.h>

#include "eps.h"

static void * housekeeping_daemon(void *pvParameters) {
    TickType_t hk_delay = pdMS_TO_TICKS(1000);
    for (; ; ) {
        // TODO periodically construct a HK element (data from each subsystem) to save to the SD card
        eps_refresh_instantaneous_telemetry();
        eps_instantaneous_telemetry_t eps = get_eps_instantaneous_telemetry();

        hk_delay = pdMS_TO_TICKS(3000);
        vTaskDelay(hk_delay);
    }
}

SAT_returnState start_housekeeping_daemon(void) {
    if (xTaskCreate((TaskFunction_t)housekeeping_daemon,
                  "housekeeping_daemon", 2048, NULL, NORMAL_SERVICE_PRIO,
                  NULL) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK housekeeping_daemon\n");
        return SATR_ERROR;
    }
    ex2_log("Service handlers started\n");
    return SATR_OK;
}



