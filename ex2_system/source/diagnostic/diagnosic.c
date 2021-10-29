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
#include "uhf.h"
#include "eps.h"
#include "system.h"

static void uhf_watchdog_daemon(void *pvParameters);
SAT_returnState start_diagnostic_daemon(void);

/**
 * @brief Check that the UHF is responsive. If not, toggle power.
 * 
 * @param pvParameters Task parameters (not used)
 */
static void uhf_watchdog_daemon(void *pvParameters) {
    TickType_t delay = pdMS_TO_TICKS(3 * 60 * 1000); // 3 minutes
    for (;;) {
        // Get status word from UHF
        uint8_t status[32];
        const unsigned int retries = 3;
        UHF_return err;
        for (int i = 0; i < retries; i++) {
            err = UHF_get_status(status);
            if (err == U_GOOD_CONFIG) {
                break;
            }
        }

        if (err != U_GOOD_CONFIG) {
            ex2_log("UHF was not responsive - attempting to toggle power.");
            // Toggle the UHF.
            unsigned int timeout = pdMS_TO_TICKS(30 * 1000); // 30 seconds
            eps_set_pwr_chnl(UHF_PWR_CHNL, OFF);
            TickType_t start = xTaskGetTickCount();

            while (eps_get_pwr_chnl(UHF_PWR_CHNL) != OFF || xTaskGetTickCount() - start < timeout) {
                vTaskDelay(pdMS_TO_TICKS(1000));
            }

            if (eps_get_pwr_chnl(UHF_PWR_CHNL) != OFF) {
                ex2_log("UHF failed to power off.");
            }

            eps_set_pwr_chnl(UHF_PWR_CHNL, ON);
            start = xTaskGetTickCount();

            while (eps_get_pwr_chnl(UHF_PWR_CHNL) != ON || xTaskGetTickCount() - start < timeout) {
                vTaskDelay(pdMS_TO_TICKS(1000));
            }

            if (eps_get_pwr_chnl(UHF_PWR_CHNL) != ON) {
                ex2_log("UHF failed to power on.");
            } else {
                ex2_log("UHF powered back on.");
            }
        }

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
    if (xTaskCreate((TaskFunction_t)uhf_watchdog_daemon, "uhf_watchdog_daemon", 2048, NULL,
                    DIAGNOSTIC_TASK_PRIO, NULL) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK uhf_watchdog_daemon.\n");
        return SATR_ERROR;
    }
    ex2_log("UHF watchdog task started.\n");
    return SATR_OK;
}
