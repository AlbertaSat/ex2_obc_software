/*
 * Copyright (C) 2020  University of Alberta
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

//TODO: determine the required stack size
#define SCHEDULER_STACK_SIZE 12000
#define MAX_DATA_LEN 12     //TODO: determine if this is the best max length
#define SCHEDULE_HISTORY_SIZE 100 // number of most recent executed commands that will be stored in the history.
                                  // once this # is exceeded, commands in the beginning of the file will be overwritten.
                                  // eg. the 102th command will overwrite the 2nd command. 
#include "system.h"
#include <os_queue.h>
#include <FreeRTOS.h>
#include <os_semphr.h>
#include "scheduler/scheduler.h"
#include <redposix.h> //include for file system
#include "csp_types.h"

extern char* fileName2;
extern char* fileName3;

#endif /* SCHEDULER_TASK_H_ */
