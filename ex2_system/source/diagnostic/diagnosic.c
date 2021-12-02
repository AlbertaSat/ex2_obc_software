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
#include <os_semphr.h>
#include "uhf.h"
#include "eps.h"
#include "system.h"

static void uhf_watchdog_daemon(void *pvParameters);
SAT_returnState start_diagnostic_daemon(void);

const unsigned int mutex_timeout = pdMS_TO_TICKS(100);

static TickType_t prv_watchdog_delay = 3 * ONE_MINUTE; // 3 minutes
static SemaphoreHandle_t uhf_watchdog_mtx = NULL;
/**
 * @brief Check that the UHF is responsive. If not, toggle power.
 *
 * @param pvParameters Task parameters (not used)
 */
static void uhf_watchdog_daemon(void *pvParameters) {
    TickType_t delay = prv_watchdog_delay;
    for (;;) {
        // Get status word from UHF
        char status[32];
        const unsigned int retries = 3;
        UHF_return err;
        for (int i = 0; i < retries; i++) {
            err = HAL_UHF_getSCW(status);
            if (err == U_GOOD_CONFIG) {
                break;
            }
        }

        if (err != U_GOOD_CONFIG) {
            ex2_log("UHF was not responsive - attempting to toggle power.");
            // Toggle the UHF.
            const unsigned int timeout = 5 * ONE_SECOND; // 5 seconds
            eps_set_pwr_chnl(UHF_PWR_CHNL, OFF);         // Turn off the UHF.
            TickType_t start = xTaskGetTickCount();

            vTaskDelay(timeout); // Allow the system to fully power off. Wait 5 seconds.

            if (eps_get_pwr_chnl(UHF_PWR_CHNL) != OFF) { // Check to see that the UHF has been turned off.
                ex2_log("UHF failed to power off.");
                break;
            }

            eps_set_pwr_chnl(UHF_PWR_CHNL, ON); // Turn the UHF back on.
            start = xTaskGetTickCount();

            while ((eps_get_pwr_chnl(UHF_PWR_CHNL) != ON) && ((xTaskGetTickCount() - start) < timeout)) {
                vTaskDelay(ONE_SECOND);
            }

            if (eps_get_pwr_chnl(UHF_PWR_CHNL) != ON) {
                ex2_log("UHF failed to power on.");
            } else {
                ex2_log("UHF powered back on.");
            }
        }

        if (xSemaphoreTake(uhf_watchdog_mtx, mutex_timeout) == pdPASS) {
            delay = prv_watchdog_delay;
            xSemaphoreGive(uhf_watchdog_mtx);
        }
        vTaskDelay(delay);
    }
}

TickType_t get_uhf_watchdog_delay(void) {
    if (xSemaphoreTake(uhf_watchdog_mtx, mutex_timeout) == pdPASS) {
        TickType_t delay = prv_watchdog_delay;
        xSemaphoreGive(uhf_watchdog_mtx);
        return delay;
    } else {
        return 0;
    }
}

SAT_returnState set_uhf_watchdog_delay(const TickType_t delay) {
    if (xSemaphoreTake(uhf_watchdog_mtx, mutex_timeout) == pdPASS) {
        prv_watchdog_delay = delay;
        xSemaphoreGive(uhf_watchdog_mtx);
        return SATR_OK;
    }
    return SATR_ERROR;
}

/**
 * Start the diagnostics daemon
 *
 * @returns status
 *   error report of task creation
 */
SAT_returnState start_diagnostic_daemon(void) {
    if (xTaskCreate((TaskFunction_t)uhf_watchdog_daemon, "uhf_watchdog_daemon", 2048, NULL, DIAGNOSTIC_TASK_PRIO,
                    NULL) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK uhf_watchdog_daemon.\n");
        return SATR_ERROR;
    }
    ex2_log("UHF watchdog task started.\n");
    uhf_watchdog_mtx = xSemaphoreCreateMutex();
    if (uhf_watchdog_mtx == NULL) {
        ex2_log("FAILED TO CREATE MUTEX uhf_watchdog_mtx.\n");
        return SATR_ERROR;
    }
    return SATR_OK;
}
