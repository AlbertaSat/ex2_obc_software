/*
 * external_watchdog.c
 *
 *  Created on: Sep 26, 2022
 *      Author: joshd
 */

#include "HL_het.h"
#include "HL_gio.h"
#include "FreeRTOS.h"
#include "os_timer.h"
#include "system.h"

#define WDI_ASSERT_LEN_TICKS 10

TimerHandle_t wdo_timer;

void irisExternalWatchdogCallback(TimerHandle_t xTimer){
    while((uint32_t)pvTimerGetTimerID(xTimer) < 1);
    gioSetBit(hetPORT1, IRIS_WDI_PIN, 0);
}

SAT_returnState start_ext_watchdog_handler(){
    wdo_timer = xTimerCreate("iris_ext_wdt", WDI_ASSERT_LEN_TICKS, 0, (void *) 0, irisExternalWatchdogCallback);
    if(wdo_timer == NULL){
        return SATR_ERROR;
    }
    return SATR_OK;
}

void irisWdoNotification(){
    if(wdo_timer){
        gioSetBit(hetPORT1, IRIS_WDI_PIN, 1);
        xTimerResetFromISR( wdo_timer, 0 );
    }
}
