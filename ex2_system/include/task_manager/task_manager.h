/*
 * Copyright (C) 2015  University of Alberta
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
 * task_manager.h
 *
 *  Created on: Jun. 28, 2021
 *      Author: Robert Taylor
 */

#ifndef EX2_SYSTEM_INCLUDE_TASK_MANAGER_H_
#define EX2_SYSTEM_INCLUDE_TASK_MANAGER_H_

#include "logger/logger.h"
#include "os_task.h"
#include "system.h"
#include <FreeRTOS.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct taskFunctions {
    uint32_t (*getDelayFunction)(void);
    void (*setDelayFunction)(uint32_t delay);
    uint32_t (*getCounterFunction)(void);
} taskFunctions;

typedef struct {
    TaskHandle_t task;
    uint32_t prev_counter;
    taskFunctions funcs;
} task_info;

typedef struct {
    TaskHandle_t task;
    char *task_name;
} user_info;

typedef struct task_info_node {
    struct task_info_node *next;
    task_info info_list[10]; // each node will contain 10 tasks
} task_info_node;

TaskHandle_t ex2_get_task_handle_by_name(char *name);

char *ex2_get_task_name_by_handle(TaskHandle_t handle);

void ex2_get_task_list(user_info **task_lst, uint32_t *size);

void ex2_deregister(TaskHandle_t task);

void ex2_register(TaskHandle_t task, taskFunctions funcs);

bool ex2_set_task_delay(TaskHandle_t task, uint32_t delay);

uint32_t ex2_get_task_delay(TaskHandle_t task);

UBaseType_t dev_ex2_get_task_high_watermark(TaskHandle_t task);

bool ex2_task_exists(TaskHandle_t task);

void ex2_task_init_mutex();

SAT_returnState start_watchdog();

SAT_returnState start_task_manager(void);

#endif /* EX2_SYSTEM_INCLUDE_TASK_MANAGER_H_ */
