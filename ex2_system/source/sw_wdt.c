/*
 * Copyright (C) 2022  University of Alberta
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
/*
 * sw_wdt.c
 *
 *  Created on: Sep. 27, 2022
 *      Author: Robert Taylor
 */

#include <FreeRTOS.h>
#include "system.h"
#include "HL_reg_rti.h"
#include "privileged_functions.h"

void feed_dog() {
#if WATCHDOG_IS_STUBBED == 0
    RAISE_PRIVILEGE;
    portENTER_CRITICAL();
    rtiREG1->WDKEY = 0x0000E51AU;
    rtiREG1->WDKEY = 0x0000A35CU;
    portEXIT_CRITICAL();
    RESET_PRIVILEGE;
#endif
}

void start_dog() {
#if WATCHDOG_IS_STUBBED == 0
    RAISE_PRIVILEGE;
    rtiREG1->WDSTATUS = 0xFFU;
    rtiREG1->DWDPRLD = 0xFFFF;
    rtiREG1->DWDCTRL = 0xA98559DA;
    RESET_PRIVILEGE;
#endif
}

// the purpose of this software watchdog is to feed the dog indiscriminately. This prevents some latch-up scenarios
void sw_watchdog(void *pvParameters) {
    TickType_t last_wake_time = xTaskGetTickCount();
    start_dog();
    for (;;) {
        feed_dog();
        vTaskDelayUntil(&last_wake_time, WDT_DELAY);
    }
}

SAT_returnState start_sw_watchdog() {
    xTaskCreate(sw_watchdog, "WDT", 128, NULL, configMAX_PRIORITIES - 1, NULL);
    return SATR_OK;
}
