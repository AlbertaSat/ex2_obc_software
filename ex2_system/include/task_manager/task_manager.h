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

#include <FreeRTOS.h>
#include "os_task.h"
#include <stdbool.h>
#include <stdint.h>
#include "logger/logger.h"
#include "system.h"

typedef struct taskFunctions {
    uint32_t (*getDelayFunction)(void);
    void (*setDelayFunction)(uint32_t delay);
} taskFunctions;

typedef struct task_info {
    TaskHandle_t task;
    bool persistent;
    taskFunctions funcs;
} task_info;

typedef struct task_info_node {
    struct task_info_node *next;
    struct task_info info_list[10]; // each node will contain 10 tasks
} task_info_node;

BaseType_t start_task_manager_watchdog();

BaseType_t start_task_manager_daemon();

TaskHandle_t ex2_get_task_handle_by_name(char *name);

char * ex2_get_task_name_by_handle(TaskHandle_t handle);

task_info_node * ex2_get_task_list();

void ex2_task_killall();

void ex2_task_kill(TaskHandle_t task);

void ex2_deregister(TaskHandle_t task);

void ex2_register(TaskHandle_t task, taskFunctions funcs, bool persistent);

void ex2_set_task_delay(TaskHandle_t task, uint32_t delay);

uint32_t ex2_get_task_delay(TaskHandle_t task);

uint32_t dev_ex2_get_task_high_watermark(TaskHandle_t task);

bool ex2_task_exists(TaskHandle_t task);

void ex2_task_init_mutex();

#endif /* EX2_SYSTEM_INCLUDE_TASK_MANAGER_H_ */
