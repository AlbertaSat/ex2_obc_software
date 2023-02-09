/*
 * Copyright (C) 2022-2023  University of Alberta
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
/**
 * @file scheduler_task.h
 * @author Grace Yi
 * @date Jan. 26, 2022
 */
#ifndef SCHEDULER_TASK_H_
#define SCHEDULER_TASK_H_

#include <FreeRTOS.h>
#include "os_queue.h"
#include "system.h"

extern QueueHandle_t SchedulerNotificationQueue;

#define SCHEDULER_STACK_SIZE 12000

SAT_returnState start_scheduler_task(void);

#endif /* SCHEDULER_TASK_H_ */
