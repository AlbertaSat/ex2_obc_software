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
 * @file housekeeping_task.c
 * @author Andrew R. Rooney
 * @date 2020-07-23
 */
#include "housekeeping/housekeeping_task.h"

#include <FreeRTOS.h>
#include <os_queue.h>
#include <os_semphr.h>
#include <os_task.h>

#include "housekeeping_service.h"

static void * housekeeping_daemon(void *pvParameters);
SAT_returnState start_housekeeping_daemon(void);

/**
 * Housekeeping task. Query subsystem housekeeping and stores to file.
 *
 * @param pvParameters
 *    task parameters (not used)
 */
static void * housekeeping_daemon(void *pvParameters) {
    TickType_t hk_delay = pdMS_TO_TICKS(1000);
    uint32_t seconds_delay = 30;
    for ( ;; ) {
        // Call housekeeping and have them collect and store data to SD card
        populate_and_store_hk_data();

        hk_delay = pdMS_TO_TICKS(seconds_delay * 1000);
        vTaskDelay(hk_delay);
    }
}

/**
 * Start the housekeeping daemon
 *
 * @returns status
 *   error report of task creation
 */
SAT_returnState start_housekeeping_daemon(void) {
    if (xTaskCreate((TaskFunction_t)housekeeping_daemon,
                  "housekeeping_daemon", 2048, NULL, HOUSEKEEPING_TASK_PRIO,
                  NULL) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK housekeeping_daemon\n");
        return SATR_ERROR;
    }
    ex2_log("Housekeeping task started\n");
    return SATR_OK;
}
