/*
 * task_manager.c
 *
 *  Created on: Jun. 29, 2021
 *      Author: Robert Taylor
 */
#include "task_manager/task_manager.h"
#include "HL_reg_rti.h"
#include "os_task.h"
#include "privileged_functions.h"
#include "semphr.h"
#include "system.h"
#include <FreeRTOS.h>
#include <string.h>

task_info_node *tasks_start = NULL;
SemaphoreHandle_t task_mutex = NULL;

task_info *get_task_info(TaskHandle_t task) {
    task_info *ret = NULL;
    xSemaphoreTakeRecursive(task_mutex, portMAX_DELAY);
    task_info_node *curr = tasks_start;
    bool done = false;
    while (!done) {
        if (curr == NULL) {
            done = true;
            break;
        }
        int i;
        for (i = 0; i < 10; i++) {
            if (curr->info_list[i].task == task) {
                ret = &(curr->info_list[i]);
                done = true;
                break;
            }
        }
        curr = curr->next;
    }
    xSemaphoreGiveRecursive(task_mutex);
    return ret;
}

uint32_t get_task_count() {
    task_info_node *curr = tasks_start;
    uint32_t count = 0;
    while (curr) {
        int i;
        for (i = 0; i < 10; i++) {
            if (curr->info_list[i].task > 0) {
                count++;
            }
        }
        curr = curr->next;
    }
    return count;
}

task_info_node *get_new_task_node() {
    task_info_node *new_node = pvPortMalloc(sizeof(task_info_node));
    memset(new_node, 0, sizeof(new_node));
    return new_node;
}

bool is_task_in_list(TaskHandle_t task) {
    if (get_task_info(task)) {
        return true;
    }
    return false;
}

void compress_list() {}

// returns false if task already in list or if task list doesn't exist
// behavior on malloc() fail is undefined
// TODO: make this handle malloc failure
bool add_task_to_list(task_info *new_tsk) {
    if (is_task_in_list(new_tsk->task)) {
        return false;
    }
    xSemaphoreTakeRecursive(task_mutex, portMAX_DELAY);
    task_info *tsk = get_task_info(0);
    if (!tsk) { // no free space found. Make a new node
        task_info_node *curr = tasks_start;
        if (curr == NULL) {
            curr = get_new_task_node();
            tasks_start = curr;
            tsk = &(curr->info_list[0]);
        } else {
            while (curr->next) {
                curr = curr->next;
            }
            curr->next = get_new_task_node();
            tsk = &(curr->next->info_list[0]);
        }
    }
    memcpy(tsk, new_tsk, sizeof(task_info)); // copy from stack to heap
    xSemaphoreGiveRecursive(task_mutex);
    return true;
}

void remove_task_from_list(TaskHandle_t task) {
    xSemaphoreTakeRecursive(task_mutex, portMAX_DELAY);
    task_info *tsk;
    if (tsk) {
        memset(tsk, 0, sizeof(task_info));
    }
    xSemaphoreGiveRecursive(task_mutex);
}

SAT_returnState start_task_manager() {
    ex2_task_init_mutex();
    return SATR_OK;
}

TaskHandle_t ex2_get_task_handle_by_name(char *name) { return xTaskGetHandle(name); }

char *ex2_get_task_name_by_handle(TaskHandle_t handle) { return pcTaskGetName(handle); }

// Returns array of structs
// array is dynamically allocated and must be freed
void ex2_get_task_list(user_info **task_lst, uint32_t *size) {
    uint32_t count = get_task_count();
    *task_lst = (user_info *)pvPortMalloc(sizeof(user_info) * count);
    if (*task_lst == NULL) {
        *size = 0;
        return;
    }
    *size = count;
    task_info_node *curr = tasks_start;
    int task_lst_i = 0;
    while (curr) {
        int i;
        for (i = 0; i < 10; i++) {
            if (curr->info_list[i].task > 0) {
                (*task_lst)[task_lst_i].task = curr->info_list[i].task;
                (*task_lst)[task_lst_i].task_name = ex2_get_task_name_by_handle(curr->info_list[i].task);
                task_lst_i++;
            }
        }
        curr = curr->next;
    }
}

void ex2_deregister(TaskHandle_t task) { remove_task_from_list(task); }

void ex2_register(TaskHandle_t task, taskFunctions funcs) {
    task_info new_task = {0};
    new_task.task = task;
    new_task.funcs = funcs;
    add_task_to_list(&new_task);
}

bool ex2_set_task_delay(TaskHandle_t task, uint32_t delay) {
    task_info *tsk = get_task_info(task);
    if (!tsk) {
        return false;
    }
    if (!tsk->funcs.getDelayFunction) {
        return false;
    }
    tsk->funcs.setDelayFunction(delay);
    return true;
}

uint32_t ex2_get_task_delay(TaskHandle_t task) {
    task_info *tsk = get_task_info(task);
    if (!tsk) {
        return 0;
    }
    if (!tsk->funcs.getDelayFunction) {
        return 0;
    }
    return tsk->funcs.getDelayFunction();
}

UBaseType_t dev_ex2_get_task_high_watermark(TaskHandle_t task) { return uxTaskGetStackHighWaterMark(task); }

bool ex2_task_exists(TaskHandle_t task) { return is_task_in_list(task); }

void ex2_task_init_mutex() {
    if (task_mutex == NULL) {
        task_mutex = xSemaphoreCreateRecursiveMutex();
    }
}

bool check_tasks_health() {
    xSemaphoreTakeRecursive(task_mutex, portMAX_DELAY);
    task_info_node *curr = tasks_start;
    while (1) {
        if (curr == NULL) {
            break;
        }
        int i;
        for (i = 0; i < 10; i++) {
            task_info tsk = curr->info_list[i];
            if (tsk.task == 0) {
                continue;
            }
            if (tsk.funcs.getCounterFunction == 0) {
                continue;
            }
            uint32_t tsk_counter = tsk.funcs.getCounterFunction();
            if (tsk_counter == tsk.prev_counter) {
                return false;
            }
            tsk.prev_counter = tsk_counter;
        }
        curr = curr->next;
    }
    xSemaphoreGiveRecursive(task_mutex);
    return true;
}

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

// the purpose of this software watchdog is to feed the dog if everything is going well
void sw_watchdog(void *pvParameters) {
    TickType_t last_wake_time = xTaskGetTickCount();
    uint32_t delayed_time = 0;
    bool should_feed = true;
    vTaskDelay(5000);
    start_dog();
    for (;;) {
        if (check_tasks_health()) {
            should_feed = true;
        } else {

            should_feed = false;
#if WATCHDOG_IS_STUBBED == 1
            ex2_log("Watchdog would have reset");
#endif
        }
        if (should_feed) {
            while (delayed_time < 10000) {
                feed_dog();
                vTaskDelayUntil(&last_wake_time, WDT_DELAY);
                delayed_time += WDT_DELAY;
            }
            delayed_time = 0;
        } else {
            vTaskDelay(10000); // time to die
        }
    }
}

SAT_returnState start_watchdog() {
    ex2_task_init_mutex();
    xTaskCreate(sw_watchdog, "WDT", 500, NULL, configMAX_PRIORITIES - 1, NULL);
}
