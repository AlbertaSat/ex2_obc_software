/*
 * hal_athena.c
 *
 *  Created on: Jul 18, 2022
 *      Author: joshd
 */
#include <hal_athena.h>
#include <ina209.h>
#include "HL_gio.h"
#include "logger.h"

uint32_t is_oc;

int initAthena(void) {
#if IS_ATHENA_V2 == 1
    // set pin high, ensuring FET is allowing current through
    gioSetBit(SOLAR_CURRENTSENSE_SHDN_PORT, SOLAR_CURRENTSENSE_SHDN_PIN, 1);
    init_ina209(SOLAR_INA209_ADDR);
    return 0;
#endif
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
