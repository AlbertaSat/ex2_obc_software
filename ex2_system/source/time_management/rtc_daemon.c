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
 * rtc_daemon.c
 *
 *  Created on: May 2, 2022
 *      Author: Robert Taylor
 */

#include <FreeRTOS.h>
#include "skytraq_gps.h"
#include "rtcmk.h"
#include "system.h"
#include "task_manager/task_manager.h"
#include "time_management/rtc_daemon.h"
#include "HL_gio.h"

static uint32_t rtc_wdt_counter = 0;

#define DISCIPLINE_DELAY 10000 // every 10 seconds for testing purposes

uint32_t get_rtc_wdt_counter() { return rtc_wdt_counter; }

#define RTC_DAEMON_TASK_SIZE 256

static TickType_t last_second;

int RTCMK_GetMs() {
    return ( ( xTaskGetTickCount() * 1000 ) / configTICK_RATE_HZ ) - ( ( last_second * 1000 ) / configTICK_RATE_HZ );
}

/**
 * @brief
 *      FreeRTOS daemon for disciplining RTC
 * @details
 *      Disicplines rtc by asking the gps subsystem for the time and writing that time to the rtc
 * @param none
 * @return none. use FreeRTOS task features to poll
 */
void RTC_discipline_daemon(void) {

    sys_log(INFO, "GPS Task Started");

    time_t utc_time;

    uint16_t total_delay = 0;

    rtc_wdt_counter++;

    for (;;) {
        while (total_delay < DISCIPLINE_DELAY) {
            vTaskDelay(DELAY_WAIT_INTERVAL);
            total_delay += DELAY_WAIT_INTERVAL;
            rtc_wdt_counter++;
        }
        rtc_wdt_counter++;
        total_delay = 0;
        if (!(gps_get_utc_time(&utc_time))) {
            sys_log(NOTICE, "Couldn't get gps time");
            continue; // delay wait until gps signal acquired
        } else {
            RTCMK_SetUnix(utc_time);
            sys_log(INFO, "Current time: %d", utc_time);
        }
        rtc_wdt_counter++;
    }
}

/**
 * @brief
 *      Start the RTC discipline daemon
 * @details
 *      Starts the FreeRTOS task responsible for disciplining the RTC
 * @return SAT_returnState
 *      success report
 */
SAT_returnState start_RTC_daemon() {
    TaskHandle_t rtc_handle;
    if (xTaskCreate((TaskFunction_t)RTC_discipline_daemon, "RTC_daemon", RTC_DAEMON_TASK_SIZE, NULL, 1, &rtc_handle) !=
        pdPASS) {
        return SATR_ERROR;
    }
    taskFunctions rtc_funcs = {0};
    rtc_funcs.getCounterFunction = get_rtc_wdt_counter;
    ex2_register(rtc_handle, rtc_funcs);
    RTCMK_EnableInt(RTCMK_ADDR);
    gioEnableNotification(RTC_INT_PORT, RTC_INT_PIN);
    return SATR_OK;
}

void rtcInt_gioNotification(gioPORT_t *port, uint32 bit) {
    last_second = xTaskGetTickCountFromISR();
}
