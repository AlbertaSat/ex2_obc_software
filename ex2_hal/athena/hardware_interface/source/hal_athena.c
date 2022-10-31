/*
 * hal_athena.c
 *
 *  Created on: Jul 18, 2022
 *      Author: Liam Droog
 */
#include <hal_athena.h>
#include <ina209.h>
#include "FreeRTOS.h"
#include "HL_gio.h"
#include "logger.h"
#include "ina209.h"
#include "bl_eeprom.h"
#include "os_semphr.h"

#define MAX6374_WDI_DELAY_MS 4000

static SemaphoreHandle_t wdo_smphr;

uint32_t is_oc;

int initAthena(void) {
#if IS_ATHENA_V2 == 1

    test_currentsense();

    init_ina209(SOLAR_INA209_ADDR);

    // set pin high, ensuring FET is allowing current through
    gioSetBit(SOLAR_CURRENTSENSE_SHDN_PORT, SOLAR_CURRENTSENSE_SHDN_PIN, 1);

    // check pin to ensure there isn't an overcurrent event
    is_oc = gioGetBit(SOLAR_CURRENTSENSE_ALERT_PORT, SOLAR_CURRENTSENSE_ALERT_PIN);
    if (is_oc != 1) {
        // pull pin low to cut MOSFET
        gioSetBit(SOLAR_CURRENTSENSE_SHDN_PORT, SOLAR_CURRENTSENSE_SHDN_PIN, 0);
        sys_log(CRITICAL, "Solar panel overcurrent event occurred.");
    }
#endif
    return 0;
}

void is_SolarPanel_overcurrent(void *pvParameters) {
    while (1) {
        is_oc = gioGetBit(SOLAR_CURRENTSENSE_ALERT_PORT, SOLAR_CURRENTSENSE_ALERT_PIN);
        if (is_oc != 1) {
            // pull pin low to cut MOSFET
            gioSetBit(SOLAR_CURRENTSENSE_SHDN_PORT, SOLAR_CURRENTSENSE_SHDN_PIN, 0);
            sys_log(CRITICAL, "Solar panel overcurrent event occurred.");
            vTaskDelete(0);
        } else {
            vTaskDelay(300);
        }
    }
}

void obc_wdoInt_gioNotification(gioPORT_t *port, uint32 bit) {
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(wdo_smphr, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void athena_hw_wdi_daemon(void *pvParameters) {
    while (1) {
        gioToggleBit(gioPORTB, 4);
        vTaskDelay(MAX6374_WDI_DELAY_MS); // hardware watchdog max period configured to 10s
    }
}

void athena_hw_wdt_reset_daemon(void *pvParameters) {
    while (1) {
        if (xSemaphoreTake(wdo_smphr, portMAX_DELAY) == pdTRUE) {
            sw_reset('A', REQUESTED); // reboot application image
        }
    }
}

/**
 * @brief
 * Start the athena_hw_watchdog_daemon
 *
 * @returns status
 *   error report of task creation
 */
SAT_returnState start_athena_hw_wdi_daemon() {
#if IS_ATHENA_V2 == 1
    wdo_smphr = xSemaphoreCreateBinary();
    if (wdo_smphr == NULL) {
        ex2_log("FAILED TO CREATE TASK athena_hw_watchdog\n");
        return SATR_ERROR;
    }
    if (xTaskCreate((TaskFunction_t)athena_hw_wdi_daemon, "athena_hw_watchdog", ATH_WD_DM_SIZE, NULL,
                    ATH_WDT_DM_PRIO, NULL) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK athena_wdi_daemon\n");
        return SATR_ERROR;
    }
    if (xTaskCreate((TaskFunction_t)athena_hw_wdt_reset_daemon, "athena_hw_wdt_reset_daemon", ATH_WD_DM_SIZE, NULL,
                    ATH_WDT_DM_PRIO, NULL) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK athena_hw_wdt_reset_daemon\n");
        return SATR_ERROR;
    }
    ex2_log("Athena_hw_watchdog daemon Started");
#else
    return SATR_OK;
}
