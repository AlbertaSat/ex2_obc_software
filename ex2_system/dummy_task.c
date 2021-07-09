/*
 * dummy_task.c
 *
 *  Created on: Jul. 6, 2021
 *      Author: robert
 */

#include <FreeRTOS.h>
#include "task_manager/task_manager.h"
#include "os_task.h"
#include "task_manager/task_manager.h"

uint32_t delay = 1000;

void dummy_task(void * pvParameters) {
    for(;;) {
        ex2_log("Dummy Task Running");
        TaskHandle_t myHandle = xTaskGetCurrentTaskHandle();
        ex2_set_task_delay(myHandle, delay+10);
        uint32_t myDelay = ex2_get_task_delay(myHandle);
        ex2_log("My delay is: %d", myDelay);
        vTaskDelay(pdMS_TO_TICKS(delay));
    }

}

uint32_t getDelay() {
    return delay;
}

void setDelay(uint32_t _delay) {
    delay = _delay;
}

void start_dummy_task() {
    TaskHandle_t hand = NULL;
    xTaskCreate(dummy_task, "Dummy", 200, NULL, 1, &hand);
    taskFunctions funcs;
    funcs.setDelayFunction = setDelay;
    funcs.getDelayFunction = getDelay;
    int i;
    for (i = 0; i <= 10; i++) {
        ex2_register(hand, funcs, true);
    }
}




