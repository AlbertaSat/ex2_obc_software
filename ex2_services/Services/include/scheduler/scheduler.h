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
 * scheduler.h
 *
 *  Created on: Nov. 22, 2021
 *      Author: Grace Yi
 */

#ifndef SERVICE_INCLUDE_SCHEDULER_H_
#define SERVICE_INCLUDE_SCHEDULER_H_

#include <FreeRTOS.h>
#include "semphr.h"
#include "system.h"

#define SCHED_SEM_WAIT ((TickType_t) 8000)
#define MAX_NUM_CMDS 16
#define MAX_DATA_LEN 16   // TODO: determine if this is the best max length
#define MAX_CMD_LENGTH 16 // TODO: review max cmd length required w mission design/ gs

// custom scheduler error codes
typedef enum {
    SCHED_ERR_OK = 0,
    SCHED_ERR_NO_MEM,
    SCHED_ERR_IO,
    SCHED_ERR_SUBSERVICE, // from SAT_returnState
    SCHED_ERR_RTC,
    SCHED_ERR_LOCK,
} SchedulerError_t;

// The file that holds the schedule
extern const char *ScheduleFile;

extern SemaphoreHandle_t SchedLock;

// Structure inspired by: https://man7.org/linux/man-pages/man5/crontab.5.html
typedef struct __attribute__((packed)) {
    uint32_t next;   // next scheduled execution time
    uint32_t period; // frequency the cmd needs to be executed in seconds
    uint32_t last;   // stop repeating after the last time
    uint16_t msecs;
    uint16_t dst;
    uint16_t dport;
    uint16_t len;   // length of the cmd op plus args
    uint8_t cmd[MAX_CMD_LENGTH];
} ScheduledCmd_t;

typedef enum {
    SET_SCHEDULE = 0,
    GET_SCHEDULE = 1,
    REPLACE_SCHEDULE = 2,
    DELETE_SCHEDULE = 3,
    PING_SCHEDULE = 4
} Scheduler_Subtype;

void sort_cmds(ScheduledCmd_t **cmds, int num_cmds);

SAT_returnState start_scheduler_service(void);

#endif /* SERVICE_INCLUDE_SCHEDULER_H_ */
