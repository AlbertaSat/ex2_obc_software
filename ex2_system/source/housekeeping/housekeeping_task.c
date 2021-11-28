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

static void *housekeeping_daemon(void *pvParameters);
SAT_returnState start_housekeeping_daemon(void);

/**
 * Housekeeping task. Query subsystem housekeeping and stores to file.
 *
 * @param pvParameters
 *    task parameters (not used)
 */
static void *housekeeping_daemon(void *pvParameters) {
    TickType_t hk_delay = pdMS_TO_TICKS(1000);
    uint32_t seconds_delay = 5;
    for (;;) {
        // Call housekeeping and have them collect and store data to SD card

        int num_ticks_before_hk = xTaskGetTickCount();
        populate_and_store_hk_data();
        int num_ticks_after_hk = xTaskGetTickCount();
        int ticks_elapsed = num_ticks_after_hk - num_ticks_before_hk;
        printf("%x\n", ticks_elapsed);


        hk_delay = pdMS_TO_TICKS(seconds_delay * 1000);

        //portGET_RUN_TIME_COUNTER_VALUE();
        int num_ticks_before_runtime = xTaskGetTickCount();
        //printf("%d\n", num_ticks_before);

        static char cbuffer_hk[500];
        memset(cbuffer_hk, 0, 500);
        vTaskGetRunTimeStats(cbuffer_hk);

        for (int i = 0; i<500; i++) {
            if (cbuffer_hk[i] != 0) {
                printf("bad time @ %x\n", i);
                break;
            }
        }
        int num_ticks_after_runtime = xTaskGetTickCount();
        //printf("%d\n", num_ticks_after);

        printf("%s\n", cbuffer_hk);

        memset(cbuffer_hk, 0, 500);

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
    if (xTaskCreate((TaskFunction_t)housekeeping_daemon, "housekeeping_daemon", 2000, NULL, 4,
                    NULL) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK housekeeping_daemon\n");
        return SATR_ERROR;
    }
    ex2_log("Housekeeping task started\n");
    return SATR_OK;
}
