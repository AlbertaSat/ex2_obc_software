/*
 * dummy_task.c
 *
 *  Created on: Jul. 6, 2021
 *      Author: robert
 */

/**
 * This file serves as an example for how to create a task for ex-alta 2
 * A good task is defined as one that can be watched with the watchdog, uses
 * the logger properly, and uses FreeRTOS features
 */

#include <FreeRTOS.h>
#include "os_task.h"
#include "system.h"
#include "logger/logger.h"

uint32_t delay = 1000;
static uint32_t wdt_counter = 0;

void dummy_task(void *pvParameters) {
    uint32_t delayed_time;

    for (;;) {
        delayed_time = 0;
        ex2_log("Dummy Task Running");
        delayed_time += delay;
        vTaskDelay(delay);
    }
}

uint32_t getDelay() { return delay; }

void setDelay(uint32_t _delay) { delay = _delay; }

uint32_t getCounter() { return wdt_counter; }

SAT_returnState start_dummy_task() {
    TaskHandle_t hand = NULL;
    if (xTaskCreate(dummy_task, "Dummy", 200, NULL, 1, &hand) != pdPASS) {
        ex2_log("Could not start dummy task");
        return SATR_ERROR;
    }

    return SATR_OK;
}
