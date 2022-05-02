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

static uint32_t rtc_wdt_counter = 0;

#define DISCIPLINE_DELAY 10000 // every 10 seconds for testing purposes

uint32_t get_rtc_wdt_counter() { return rtc_wdt_counter; }

#define RTC_DAEMON_TASK_SIZE configMINIMAL_STACK_SIZE
/**
 * @brief
 *      FreeRTOS daemon for disciplining RTC
 * @details
 *      Disicplines rtc by asking the gps subsystem for the time and writing that time to the rtc
 * @param none
 * @return none. use FreeRTOS task features to poll
 */
void RTC_discipline_daemon(void) {

    ex2_log("GPS Task Started");

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
            ex2_log("Couldn't get gps time");
            continue; // delay wait until gps signal acquired
        } else {
            RTCMK_SetUnix(utc_time);
            ex2_log("Current time: %d", utc_time);
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

    return SATR_OK;
}
