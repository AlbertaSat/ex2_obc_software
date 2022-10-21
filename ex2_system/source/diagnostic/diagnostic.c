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
#include "HL_gio.h"
#include "uhf.h"
#include "sband.h"
#include "sTransmitter.h"
#include "eps.h"
#include "skytraq_binary_types.h"
#include "skytraq_binary.h"
#include "skytraq_gps.h"
#include "system.h"
#include "adcs.h"
#include "logger/logger.h"
#include "ns_payload.h"
#include "iris.h"
#include "hal_athena.h"
#include "bl_eeprom.h"

static void uhf_watchdog_daemon(void *pvParameters);
static void sband_watchdog_daemon(void *pvParameters);
static void charon_watchdog_daemon(void *pvParameters);
static void adcs_watchdog_daemon(void *pvParameters);
static void payload_watchdog_daemon(void *pvParameters);
SAT_returnState start_diagnostic_daemon(void);

const unsigned int mutex_timeout = pdMS_TO_TICKS(100);
const unsigned int reset_wait_period = 5 * ONE_SECOND; // 5 seconds
const unsigned int watchdog_retries = 3;

static TickType_t uhf_prv_watchdog_delay = ONE_MINUTE;
static TickType_t sband_prv_watchdog_delay = ONE_MINUTE;
static TickType_t charon_prv_watchdog_delay = ONE_MINUTE;
static TickType_t adcs_prv_watchdog_delay = ONE_MINUTE;
static TickType_t payload_prv_watchdog_delay = ONE_MINUTE;

static void boot_counter_reset(void *pvParameters) {
    while (1) { // Syntactic sugar, freertos tasks should be a loop.
        vTaskDelay(BOOT_COUNTER_RESET_DELAY);
        boot_info b_inf = {0};
        Fapi_StatusType status = eeprom_get_boot_info(&b_inf);
        if (status != Fapi_Status_Success) {
            sys_log(WARN, "Could not get boot info");
            break;
        }
        b_inf.attempts = 0;
        status = eeprom_set_boot_info(&b_inf);
        if (status != Fapi_Status_Success) {
            sys_log(WARN, "Could not set boot info");
            break;
        }
        break;
    }
    sys_log(INFO, "Boot attempts reset to 0");
    vTaskDelete(0);
}

#if UHF_IS_STUBBED == 0
/**
 * @brief Check that the UHF is responsive. If not, toggle power.
 *
 * @param pvParameters Task parameters (not used)
 */
static void uhf_watchdog_daemon(void *pvParameters) {
    for (;;) {
        TickType_t delay = get_uhf_watchdog_delay();

#if FLIGHT_CONFIGURATION == 1
        /* In flight configuration, the watchdog will turn the UHF on if it is off */
        if (eps_get_pwr_chnl(UHF_5V0_PWR_CHNL) == OFF) {
            sys_log(ERROR, "UHF not on during watchdog check, power will be toggled\n");
        }
#else
        /* In non-flight configuration, the UHF watchdog avoids powering unexpected channels */
        if (eps_get_pwr_chnl(UHF_5V0_PWR_CHNL) == OFF) {
            sys_log(ERROR, "UHF not on during watchdog check, power not toggled\n");
            vTaskDelay(delay);
            continue;
        }
#endif
        UHF_return err;
        for (int i = 0; i < watchdog_retries; i++) {
            err = UHF_refresh_state();
            if (err == U_GOOD_CONFIG) {
                break;
            } else if (err == U_IN_PIPE) {
                break;
            }
            vTaskDelay(2 * ONE_SECOND);
        }

        if (err == U_IN_PIPE) {
            sys_log(WARN, "UHF in PIPE Mode - power not toggled.");
        } else if (err != U_GOOD_CONFIG) {
            sys_log(ERROR, "UHF was not responsive - attempting to toggle power.");

            // Turn off the UHF.
            eps_set_pwr_chnl(UHF_5V0_PWR_CHNL, OFF);

            // Allow the system to fully power off
            vTaskDelay(reset_wait_period);

            // Check that the UHF has been turned off.
            if (eps_get_pwr_chnl(UHF_5V0_PWR_CHNL) != OFF) {
                sys_log(ERROR, "UHF failed to power off.");
                vTaskDelay(delay);
                continue;
            }

            // Turn the UHF back on.
            eps_set_pwr_chnl(UHF_5V0_PWR_CHNL, ON);

            // Allow the system to fully power on
            vTaskDelay(reset_wait_period);

            // Check that the UHF has been turned on
            if (eps_get_pwr_chnl(UHF_5V0_PWR_CHNL) != ON) {
                sys_log(ERROR, "UHF failed to power on.");
            } else {
                sys_log(INFO, "UHF powered back on.");
            }
        }

        delay = get_uhf_watchdog_delay();
        vTaskDelay(delay);
    }
}
#endif

#if SBAND_IS_STUBBED == 0
/**
 * @brief Check that the SBAND is responsive. If not, toggle power.
 *
 * @param pvParameters Task parameters (not used)
 */
static void sband_watchdog_daemon(void *pvParameters) {
    for (;;) {
        TickType_t delay = get_sband_watchdog_delay();
        if (gioGetBit(hetPORT2, 23) == 0) {
            sys_log(WARN, "SBAND not enabled - power not toggled");
            vTaskDelay(delay);
            continue;
        }
        // Get SBAND control values
        uint16_t SBAND_version = 0;
        STX_return err;
        for (int i = 0; i < watchdog_retries; i++) {
            STX_getFirmwareV(&SBAND_version);
            if (SBAND_version != 0)
                break;
            vTaskDelay(2 * ONE_SECOND);
        }
        if (SBAND_version == 0) {
            // TODO: Currently no way for power toggling to return fail
            sys_log(ERROR, "SBAND was not responsive - attempting to toggle power.");

            // Reset the SBAND by toggling the reset pin
            gioSetBit(hetPORT2, 21, 0); // Het2 21 is the S-band nRESET pin
            vTaskDelay(2 * ONE_SECOND);
            gioSetBit(hetPORT2, 21, 1); // Het2 21 is the S-band nRESET pin
            vTaskDelay(2 * ONE_SECOND);

            // Disable the SBAND
            STX_Disable();
            vTaskDelay(10 * ONE_SECOND);

            // Enable the S-band
            STX_Enable();
            vTaskDelay(ONE_SECOND);

            sys_log(INFO, "SBAND power toggled");
        }

        delay = get_sband_watchdog_delay();
        vTaskDelay(delay);
    }
}
#endif

#if CHARON_IS_STUBBED == 0 && IS_EXALTA2 == 1
/**
 * @brief Check that Charon is responsive. If not, toggle power.
 *
 * @param pvParameters Task parameters (not used)
 */

static void charon_watchdog_daemon(void *pvParameters) {
    for (;;) {
        TickType_t delay = get_charon_watchdog_delay();
        if (eps_get_pwr_chnl(CHARON_3V3_PWR_CHNL) == 0) {
            sys_log(WARN, "Charon not on - power not toggled");
            vTaskDelay(delay);
            continue;
        }

        // Get Charon gps firmware version
        uint32_t version = NULL;
        GPS_RETURNSTATE err;
        for (int i = 0; i < watchdog_retries; i++) {
            err = gps_skytraq_get_software_version(&version);
            if (err == GPS_SUCCESS)
                break;
            vTaskDelay(2 * ONE_SECOND);
        }

        if ((err != GPS_SUCCESS) || (version == NULL)) {
            sys_log(ERROR, "Charon was not responsive - attempting to toggle power.");

            // Turn Charon off
            eps_set_pwr_chnl(CHARON_3V3_PWR_CHNL, OFF);

            // Allow the system to fully power off
            vTaskDelay(reset_wait_period);

            // Check that Charon has been turned off.
            if (eps_get_pwr_chnl(CHARON_3V3_PWR_CHNL) != OFF) {
                sys_log(ERROR, "Charon failed to power off.");
                vTaskDelay(delay);
                continue;
            }

            // Turn Charon back on.
            eps_set_pwr_chnl(CHARON_3V3_PWR_CHNL, ON);

            // Allow the system to fully power on
            vTaskDelay(reset_wait_period);

            // Check that Charon has been turned on
            if (eps_get_pwr_chnl(CHARON_3V3_PWR_CHNL) != ON) {
                sys_log(ERROR, "Charon failed to power on.");
            } else {
                sys_log(INFO, "Charon powered back on.");
            }
        }

        delay = get_charon_watchdog_delay();
        vTaskDelay(delay);
    }
}
#endif

#if ADCS_IS_STUBBED == 0
/**
 * @brief Check that the ADCS is responsive. If not, toggle power.
 *
 * @param pvParameters Task parameters (not used)
 */

static void adcs_watchdog_daemon(void *pvParameters) {
    for (;;) {
        TickType_t delay = get_adcs_watchdog_delay();

        if (eps_get_pwr_chnl(ADCS_3V3_PWR_CHNL) == 0) {
            sys_log(WARN, "ADCS not on - power not toggled");
            vTaskDelay(delay);
            continue;
        }

        ADCS_boot_program_stat test_stat;

        ADCS_returnState err;
        for (int i = 0; i < watchdog_retries; i++) {
            err =
                HAL_ADCS_get_boot_program_stat(&test_stat); // Chosen bc the ADCS will respond in boot and app mode
            if (err == ADCS_OK)
                break;
            vTaskDelay(2 * ONE_SECOND);
        }

        if ((err != ADCS_OK) && (err != ADCS_UART_BUSY)) {
            sys_log(ERROR, "ADCS was not responsive - attempting to toggle power.");

            // Turn the ADCS off
            eps_set_pwr_chnl(ADCS_3V3_PWR_CHNL, OFF);
            eps_set_pwr_chnl(ADCS_5V0_PWR_CHNL, OFF);

            // Allow the system to fully power off
            vTaskDelay(reset_wait_period);

            // Check that the ADCS has been turned off.
            if ((eps_get_pwr_chnl(ADCS_3V3_PWR_CHNL) != OFF) || (eps_get_pwr_chnl(ADCS_5V0_PWR_CHNL) != OFF)) {
                sys_log(ERROR, "ADCS failed to power off.");
                vTaskDelay(delay);
                continue;
            }

            // Turn the ADCS back on.
            eps_set_pwr_chnl(ADCS_3V3_PWR_CHNL, ON);
            eps_set_pwr_chnl(ADCS_5V0_PWR_CHNL, ON);

            // Allow the system to fully power on
            vTaskDelay(reset_wait_period);

            // Check that the ADCS has been turned on
            if ((eps_get_pwr_chnl(ADCS_3V3_PWR_CHNL) != ON) && (eps_get_pwr_chnl(ADCS_5V0_PWR_CHNL) != ON)) {
                sys_log(ERROR, "ADCS failed to power on.");
            } else {
                sys_log(INFO, "ADCS powered back on.");
            }
        }

        delay = get_adcs_watchdog_delay();
        vTaskDelay(delay);
    }
}
#endif

#if IRIS_IS_STUBBED == 0 || NS_IS_STUBBED == 0
/**
 * @brief Check that the payload is responsive. If not, toggle power.
 *
 * @param pvParameters Task parameters (not used)
 */

static void payload_watchdog_daemon(void *pvParameters) {
    for (;;) {
        TickType_t delay = get_payload_watchdog_delay();

#if (IS_EXALTA2 == 1) || (IS_AURORASAT == 1)
        uint8_t mcu_channel = PYLD_3V3_PWR_CHNL;
#else
        uint8_t mcu_channel = PYLD_5V0_PWR_CHNL;
#endif

        if (eps_get_pwr_chnl(mcu_channel) == 0) {
            sys_log(INFO, "Payload not on - power not toggled");
            vTaskDelay(delay);
            continue;
        }

#if IRIS_IS_STUBBED == 0
        Iris_HAL_return err;
        Iris_HAL_return expected_err = IRIS_HAL_OK;
#endif

#if NS_IS_STUBBED == 0
        NS_return err;
        NS_return expected_err = NS_OK;
        uint8_t heartbeat;
#endif

        for (int i = 0; i < watchdog_retries; i++) {
#if IRIS_IS_STUBBED == 0
            err = iris_wdt_ack();
#endif

#if NS_IS_STUBBED == 0
            err = HAL_NS_get_heartbeat(&heartbeat);
#endif
            if (err == expected_err) {
                break;
            }
            vTaskDelay(10 * ONE_SECOND);
        }

        if (err != expected_err) {
            sys_log(ERROR, "Payload was not responsive - attempting to toggle power.");

            // Turn the payload off
            eps_set_pwr_chnl(PYLD_3V3_PWR_CHNL, OFF);
            eps_set_pwr_chnl(PYLD_5V0_PWR_CHNL, OFF);

            // Allow the system to fully power off
            vTaskDelay(reset_wait_period);

            // Check that the payload has been turned off.
            if ((eps_get_pwr_chnl(PYLD_3V3_PWR_CHNL) != OFF) || (eps_get_pwr_chnl(PYLD_5V0_PWR_CHNL) != OFF)) {
                sys_log(ERROR, "Payload failed to power off.");
                vTaskDelay(delay);
                continue;
            }

            // Turn the payload back on.
            eps_set_pwr_chnl(PYLD_3V3_PWR_CHNL, ON);
            eps_set_pwr_chnl(PYLD_5V0_PWR_CHNL, ON);

            // Allow the system to fully power on
            vTaskDelay(reset_wait_period);

            // Check that the payload has been turned on
            if ((eps_get_pwr_chnl(PYLD_3V3_PWR_CHNL) != ON) && (eps_get_pwr_chnl(PYLD_5V0_PWR_CHNL) != ON)) {
                sys_log(ERROR, "Payload failed to power on.");
            } else {
                sys_log(INFO, "Payload powered back on.");
            }
        }

        delay = get_payload_watchdog_delay();
        vTaskDelay(delay);
    }
}
#endif

TickType_t get_uhf_watchdog_delay(void) {
#if UHF_IS_STUBBED == 1
    return STUBBED_WATCHDOG_DELAY;
#else
    return uhf_prv_watchdog_delay;
#endif
}

TickType_t get_sband_watchdog_delay(void) {
#if SBAND_IS_STUBBED == 1
    return STUBBED_WATCHDOG_DELAY;
#else
    return sband_prv_watchdog_delay;
#endif
}

TickType_t get_charon_watchdog_delay(void) {
#if CHARON_IS_STUBBED == 1
    return STUBBED_WATCHDOG_DELAY;
#else
    return charon_prv_watchdog_delay;
#endif
}

TickType_t get_adcs_watchdog_delay(void) {
#if ADCS_IS_STUBBED == 1
    return STUBBED_WATCHDOG_DELAY;
#else
    return adcs_prv_watchdog_delay;
#endif
}

TickType_t get_payload_watchdog_delay(void) {
#if IRIS_IS_STUBBED == 1 && NS_IS_STUBBED == 1
    return STUBBED_WATCHDOG_DELAY;
#else
    return payload_prv_watchdog_delay;
#endif
}

SAT_returnState set_uhf_watchdog_delay(const unsigned int ms_delay) {
#if UHF_IS_STUBBED == 1
    return SATR_OK;
#else
    if (ms_delay < WATCHDOG_MINIMUM_DELAY_MS) {
        return SATR_ERROR;
    }
    uhf_prv_watchdog_delay = pdMS_TO_TICKS(ms_delay);
    return SATR_OK;
#endif
}

SAT_returnState set_sband_watchdog_delay(const unsigned int ms_delay) {
#if SBAND_IS_STUBBED == 1
    return SATR_OK;
#else
    if (ms_delay < WATCHDOG_MINIMUM_DELAY_MS) {
        return SATR_ERROR;
    }
    sband_prv_watchdog_delay = pdMS_TO_TICKS(ms_delay);
    return SATR_OK;
#endif
}

SAT_returnState set_charon_watchdog_delay(const unsigned int ms_delay) {
#if CHARON_IS_STUBBED == 1
    return SATR_OK;
#else
    if (ms_delay < WATCHDOG_MINIMUM_DELAY_MS) {
        return SATR_ERROR;
    }
    charon_prv_watchdog_delay = pdMS_TO_TICKS(ms_delay);
    return SATR_OK;
#endif
}

SAT_returnState set_adcs_watchdog_delay(const unsigned int ms_delay) {
#if ADCS_IS_STUBBED == 1
    return SATR_OK;
#else
    if (ms_delay < WATCHDOG_MINIMUM_DELAY_MS) {
        return SATR_ERROR;
    }
    adcs_prv_watchdog_delay = pdMS_TO_TICKS(ms_delay);
    return SATR_OK;
#endif
}

SAT_returnState set_payload_watchdog_delay(const unsigned int ms_delay) {
#if IRIS_IS_STUBBED == 1 || NS_IS_STUBBED == 1
    return SATR_OK;
#else
    if (ms_delay < WATCHDOG_MINIMUM_DELAY_MS) {
        return SATR_ERROR;
    }
    payload_prv_watchdog_delay = pdMS_TO_TICKS(ms_delay);
    return SATR_OK;
#endif
}

/**
 * Start the diagnostics daemon
 *
 * @returns status
 *   error report of task creation
 */
SAT_returnState start_diagnostic_daemon(void) {
#if UHF_IS_STUBBED == 0
    if (xTaskCreate((TaskFunction_t)uhf_watchdog_daemon, "uhf_watchdog_daemon", DIAGNOSTIC_DM_SIZE, NULL,
                    DIAGNOSTIC_TASK_PRIO, NULL) != pdPASS) {
        sys_log(ERROR, "FAILED TO CREATE TASK uhf_watchdog_daemon.\n");
    } else {
        sys_log(INFO, "UHF watchdog task started.\n");
    }
#endif

#if SBAND_IS_STUBBED == 0
    if (xTaskCreate((TaskFunction_t)sband_watchdog_daemon, "sband_watchdog_daemon", DIAGNOSTIC_DM_SIZE, NULL,
                    DIAGNOSTIC_TASK_PRIO, NULL) != pdPASS) {
        sys_log(ERROR, "FAILED TO CREATE TASK sband_watchdog_daemon.\n");
    } else {
        sys_log(INFO, "SBAND watchdog task started.\n");
    }
#endif

#if CHARON_IS_STUBBED == 0 && IS_EXALTA2 == 1
    if (xTaskCreate(charon_watchdog_daemon, "charon_watchdog_daemon", DIAGNOSTIC_DM_SIZE, NULL,
                    DIAGNOSTIC_TASK_PRIO, NULL) != pdPASS) {
        sys_log(ERROR, "FAILED TO CREATE TASK charon_watchdog_daemon.\n");
    } else {
        sys_log(INFO, "Charon watchdog task started.\n");
    }
#endif

#if ADCS_IS_STUBBED == 0
    if (xTaskCreate(adcs_watchdog_daemon, "adcs_watchdog_daemon", DIAGNOSTIC_DM_SIZE, NULL, DIAGNOSTIC_TASK_PRIO,
                    NULL) != pdPASS) {
        sys_log(ERROR, "FAILED TO CREATE TASK adcs_watchdog_daemon.\n");
    } else {
        sys_log(INFO, "ADCS watchdog task started.\n");
    }
#endif

#if IRIS_IS_STUBBED == 0 || NS_IS_STUBBED == 0
    if (xTaskCreate(payload_watchdog_daemon, "payload_watchdog_daemon", DIAGNOSTIC_DM_SIZE, NULL,
                    DIAGNOSTIC_TASK_PRIO, NULL) != pdPASS) {
        sys_log(ERROR, "FAILED TO CREATE TASK payload_watchdog_daemon.\n");
    } else {
        sys_log(INFO, "Payload watchdog task started.\n");
    }
#endif

#if ATHENA_IS_STUBBED == 0
#if IS_ATHENA_V2 == 1
    if (xTaskCreate(is_SolarPanel_overcurrent, "Solar_Panel_Current_Monitor", DIAGNOSTIC_DM_SIZE, NULL,
                    DIAGNOSTIC_TASK_PRIO, NULL) != pdPASS) {
        sys_log(ERROR, "FAILED TO CREATE TASK: Solar Panel monitor failed to start!");
    } else {
        sys_log(INFO, "Created Task: Solar Panel Current Monitor.");
    }
#endif
#endif
    if (xTaskCreate(boot_counter_reset, "BootRst", 128, NULL, 1, NULL) != pdPASS) {
        sys_log(ERROR, "FAILED TO CREATE TASK BootRst.");
    }
    sys_log(INFO, "BootRst task started.\n");
    return SATR_OK;
}
