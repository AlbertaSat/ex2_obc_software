/*
 * task_manager.c
 *
 *  Created on: Jun. 29, 2021
 *      Author: Robert Taylor
 */
#include <FreeRTOS.h>
#include "task_manager/task_manager.h"
#include "privileged_functions.h"
#include "semphr.h"
#include "os_task.h"
#include <string.h>

task_info_node *tasks_start = NULL;
SemaphoreHandle_t task_mutex = NULL;

task_info *get_task_info(TaskHandle_t task) {
    task_info* ret = NULL;
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

task_info_node *get_new_task_node() {
    task_info_node *new_node = malloc(sizeof(task_info_node));
    memset(new_node, 0, sizeof(new_node));
    return new_node;
}

bool is_task_in_list(TaskHandle_t task) {
    if (get_task_info(task)) {
        return true;
    }
    return false;
}

void compress_list() {

}

// returns false if task already in list or if task list doesn't exist
// behavior on malloc() fail is undefined
// TODO: make this handle malloc failure
bool add_task_to_list(task_info *new_tsk) {
    if (is_task_in_list(new_tsk->task)) {
        return;
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
            while(curr->next) {
                curr = curr->next;
            }
            curr->next = get_new_task_node();
            tsk = &(curr->next->info_list[0]);
        }
    }
    memcpy(tsk, new_tsk, sizeof(task_info)); //copy from stack to heap
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

void start_task_manager() {
    ex2_task_init_mutex();
    //tasks_start = pvPortMalloc(sizeof(task_info_node));
}

TaskHandle_t ex2_get_task_handle_by_name(char *name) {
    return xTaskGetHandle(name);
}

char * ex2_get_task_name_by_handle(TaskHandle_t handle) {
    return pcTaskGetName(handle);
}

task_info_node *ex2_get_task_list() {
    return tasks_start;
}

void ex2_deregister(TaskHandle_t task) {
    remove_task_from_list(task);
}

void ex2_register(TaskHandle_t task, taskFunctions funcs, bool persistent) {
    task_info new_task;
    new_task.task = task;
    new_task.persistent = persistent;
    new_task.funcs = funcs;
    add_task_to_list(&new_task);
}

void ex2_set_task_delay(TaskHandle_t task, uint32_t delay) {
    task_info *tsk = get_task_info(task);
    if (!tsk) {
        return 0;
    }
    if (!tsk->funcs.getDelayFunction) {
        return 0;
    }
    return tsk->funcs.setDelayFunction(delay);
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

uint32_t dev_ex2_get_task_high_watermark(TaskHandle_t task);

void ex2_task_init_mutex() {
    if (task_mutex == NULL){
        task_mutex = xSemaphoreCreateRecursiveMutex();
    }
}
