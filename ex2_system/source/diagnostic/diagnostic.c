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
#include "sband.h"
#include "eps.h"
#include "skytraq_binary_types.h"
#include "skytraq_binary.h"
#include "system.h"

static void uhf_watchdog_daemon(void *pvParameters);
static void sband_watchdog_daemon(void *pvParameters);
static void charon_watchdog_daemon(void *pvParameters);
SAT_returnState start_diagnostic_daemon(void);

const unsigned int mutex_timeout = pdMS_TO_TICKS(100);
const unsigned int reset_wait_period = 5 * ONE_SECOND; // 5 seconds
const unsigned int watchdog_retries = 3;

static TickType_t uhf_prv_watchdog_delay = ONE_MINUTE;
static TickType_t sband_prv_watchdog_delay = ONE_MINUTE;
static TickType_t charon_prv_watchdog_delay = ONE_MINUTE;

static SemaphoreHandle_t uhf_watchdog_mtx = NULL;
static SemaphoreHandle_t sband_watchdog_mtx = NULL;
static SemaphoreHandle_t charon_watchdog_mtx = NULL;

/**
 * @brief Check that the UHF is responsive. If not, toggle power.
 *
 * @param pvParameters Task parameters (not used)
 */
static void uhf_watchdog_daemon(void *pvParameters) {
    for (;;) {
        TickType_t delay = get_uhf_watchdog_delay();
        if (eps_get_pwr_chnl(UHF_PWR_CHNL) == 0) {
            ex2_log("UHF not on - power not toggled");
            vTaskDelay(delay);
            continue;
        }
        // Get status word from UHF
        uint8_t scw[12];
        UHF_return err;
        for (int i = 0; i < watchdog_retries; i++) {
            err = HAL_UHF_getSCW(scw);
            if (err == U_ANS_SUCCESS) {
                break;
            } else if (err == U_I2C_IN_PIPE){
                break;
            }
        }

        if (err == U_I2C_IN_PIPE){
            ex2_log("UHF in PIPE Mode - power not toggled.");
        } else if (err != U_ANS_SUCCESS) {
            ex2_log("UHF was not responsive - attempting to toggle power.");

            // Turn off the UHF.
            eps_set_pwr_chnl(UHF_PWR_CHNL, OFF);

            // Allow the system to fully power off
            vTaskDelay(reset_wait_period);

            // Check that the UHF has been turned off.
            if (eps_get_pwr_chnl(UHF_PWR_CHNL) != OFF) {
                ex2_log("UHF failed to power off.");
                vTaskDelay(delay);
                continue;
            }

            // Turn the UHF back on.
            eps_set_pwr_chnl(UHF_PWR_CHNL, ON);

            // Allow the system to fully power on
            vTaskDelay(reset_wait_period);

            // Check that the UHF has been turned on
            if (eps_get_pwr_chnl(UHF_PWR_CHNL) != ON) {
                ex2_log("UHF failed to power on.");
            } else {
                ex2_log("UHF powered back on.");
            }
        }

        if (xSemaphoreTake(uhf_watchdog_mtx, mutex_timeout) == pdPASS) {
            delay = uhf_prv_watchdog_delay;
            xSemaphoreGive(uhf_watchdog_mtx);
        }
        vTaskDelay(delay);
    }
}

/**
 * @brief Check that the SBAND is responsive. If not, toggle power.
 *
 * @param pvParameters Task parameters (not used)
 */
static void sband_watchdog_daemon(void *pvParameters) {
    for (;;) {
        TickType_t delay = get_sband_watchdog_delay();
        if (gioGetBit(hetPORT2, 23) == 0) {
            ex2_log("SBAND not enabled - power not toggled");
            vTaskDelay(delay);
            continue;
        }
        // Get SBAND control values
        uint16_t SBAND_version = 0;
        STX_return err;
        for (int i = 0; i < watchdog_retries; i++) {
            STX_getFirmwareV(&SBAND_version);
            if (SBAND_version != 0) {
                break;
            }
        }
        if (SBAND_version == 0) {
            // TODO: Currently no way for power toggling to return fail
            ex2_log("SBAND was not responsive - attempting to toggle power.");

            // Reset the SBAND by toggling the reset pin
            gioSetBit(hetPORT2, 21, 0); // Het2 21 is the S-band nRESET pin
            vTaskDelay(2 * ONE_SECOND);
            gioSetBit(hetPORT2, 21, 1); // Het2 21 is the S-band nRESET pin
            vTaskDelay(2 * ONE_SECOND);

            // Disable the SBAND
            STX_Disable();
            vTaskDelay(10*ONE_SECOND);

            // Enable the S-band
            STX_Enable();
            vTaskDelay(ONE_SECOND);

            ex2_log("SBAND power toggled");

        }

        if (xSemaphoreTake(sband_watchdog_mtx, mutex_timeout) == pdPASS) {
            delay = sband_prv_watchdog_delay;
            xSemaphoreGive(sband_watchdog_mtx);
        }
        vTaskDelay(delay);
    }
}

/**
 * @brief Check that Charon is responsive. If not, toggle power.
 *
 * @param pvParameters Task parameters (not used)
 */

static void charon_watchdog_daemon(void *pvParameters) {
    for (;;) {
        TickType_t delay = get_charon_watchdog_delay();
        if (eps_get_pwr_chnl(CHARON_PWR_CHNL) == 0) {
            ex2_log("Charon not on - power not toggled");
            vTaskDelay(delay);
            continue;
        }

        // Get Charon gps firmware version
        uint32_t version = NULL;
        GPS_RETURNSTATE err;
        for (int i = 0; i < watchdog_retries; i++) {
            err = gps_skytraq_get_software_version(&version);
            if(err == GPS_SUCCESS) break;
        }

        if ((err != GPS_SUCCESS) || (version == NULL)) {
            ex2_log("Charon was not responsive - attempting to toggle power.");\

            // Turn Charon off
            eps_set_pwr_chnl(CHARON_PWR_CHNL, OFF);

            // Allow the system to fully power off
            vTaskDelay(reset_wait_period);

            // Check that Charon has been turned off.
            if (eps_get_pwr_chnl(CHARON_PWR_CHNL) != OFF) {
                ex2_log("Charon failed to power off.");
                vTaskDelay(delay);
                continue;
            }

            // Turn Charon back on.
            eps_set_pwr_chnl(CHARON_PWR_CHNL, ON);

            // Allow the system to fully power on
            vTaskDelay(reset_wait_period);

            // Check that Charon has been turned on
            if (eps_get_pwr_chnl(CHARON_PWR_CHNL) != ON) {
                ex2_log("Charon failed to power on.");
            } else {
                ex2_log("Charon powered back on.");
            }
        }

        if (xSemaphoreTake(charon_watchdog_mtx, mutex_timeout) == pdPASS) {
            delay = charon_prv_watchdog_delay;
            xSemaphoreGive(charon_watchdog_mtx);
        }
        vTaskDelay(delay);
    }
}

TickType_t get_uhf_watchdog_delay(void) {
    if (xSemaphoreTake(uhf_watchdog_mtx, mutex_timeout) == pdPASS) {
        TickType_t delay = uhf_prv_watchdog_delay;
        xSemaphoreGive(uhf_watchdog_mtx);
        return delay;
    } else {
        return 0;
    }
}

TickType_t get_sband_watchdog_delay(void) {
    if (xSemaphoreTake(sband_watchdog_mtx, mutex_timeout) == pdPASS) {
        TickType_t delay = sband_prv_watchdog_delay;
        xSemaphoreGive(sband_watchdog_mtx);
        return delay;
    } else {
        return 0;
    }
}

TickType_t get_charon_watchdog_delay(void) {
    if (xSemaphoreTake(charon_watchdog_mtx, mutex_timeout) == pdPASS) {
        TickType_t delay = charon_prv_watchdog_delay;
        xSemaphoreGive(charon_watchdog_mtx);
        return delay;
    } else {
        return 0;
    }
}

SAT_returnState set_uhf_watchdog_delay(const TickType_t delay) {
    if (xSemaphoreTake(uhf_watchdog_mtx, mutex_timeout) == pdPASS) {
        uhf_prv_watchdog_delay = delay;
        xSemaphoreGive(uhf_watchdog_mtx);
        return SATR_OK;
    }
    return SATR_ERROR;
}

SAT_returnState set_sband_watchdog_delay(const TickType_t delay) {
    if (xSemaphoreTake(sband_watchdog_mtx, mutex_timeout) == pdPASS) {
        sband_prv_watchdog_delay = delay;
        xSemaphoreGive(sband_watchdog_mtx);
        return SATR_OK;
    }
    return SATR_ERROR;
}

SAT_returnState set_charon_watchdog_delay(const TickType_t delay) {
    if (xSemaphoreTake(charon_watchdog_mtx, mutex_timeout) == pdPASS) {
        charon_prv_watchdog_delay = delay;
        xSemaphoreGive(charon_watchdog_mtx);
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
#ifndef UHF_IS_STUBBED
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
#endif

#ifndef SBAND_IS_STUBBED
    if (xTaskCreate((TaskFunction_t)sband_watchdog_daemon, "sband_watchdog_daemon", 2048, NULL, DIAGNOSTIC_TASK_PRIO,
                    NULL) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK sband_watchdog_daemon.\n");
        return SATR_ERROR;
    }
    ex2_log("SBAND watchdog task started.\n");
    sband_watchdog_mtx = xSemaphoreCreateMutex();
    if (sband_watchdog_mtx == NULL) {
        ex2_log("FAILED TO CREATE MUTEX sband_watchdog_mtx.\n");
        return SATR_ERROR;
    }
#endif

#ifndef CHARON_IS_STUBBED
    if (xTaskCreate(charon_watchdog_daemon, "charon_watchdog_daemon", 2048, NULL, 3,
                    NULL) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK charon_watchdog_daemon.\n");
        return SATR_ERROR;
    }
    ex2_log("Charon watchdog task started.\n");
    charon_watchdog_mtx = xSemaphoreCreateMutex();
    if (charon_watchdog_mtx == NULL) {
        ex2_log("FAILED TO CREATE MUTEX charon_watchdog_mtx.\n");
        return SATR_ERROR;
    }
#endif

    return SATR_OK;
}
