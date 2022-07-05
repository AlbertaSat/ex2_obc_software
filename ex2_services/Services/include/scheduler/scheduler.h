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

#ifndef EX2_SYSTEM_INCLUDE_SCHEDULER_H_
#define EX2_SYSTEM_INCLUDE_SCHEDULER_H_

#include <FreeRTOS.h>
#include "system.h"
#include "os_task.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "rtcmk.h"
#include "csp_types.h"
#include "csp_buffer.h"
#include "ex2_time.h"
#include "semphr.h"
#include "services.h"
#include <string.h>
#include <csp/csp.h>
#include "housekeeping_service.h"
#include "task_manager.h"
#include <redposix.h> //include for file system
#include "logger.h"
#include "util/service_utilities.h"

#define SCHEDULER_SIZE 1000
#define EX2_SEMAPHORE_WAIT 8000
#define MAX_NUM_CMDS 5
#define MAX_DATA_LEN 12   // TODO: determine if this is the best max length
#define MAX_CMD_LENGTH 10 // TODO: review max cmd length required w mission design/ gs
#define MAX_BUFFER_LENGTH 500
#define ASTERISK 255 //'*' is binary 42 in ASCII.
// TODO: convert the asterisk from 42 to 255 on the python gs code
// Since seconds and minutes can both use the value 42, to avoid conflict, asterisk will be replaced with 255.

// custom scheduler error codes
#define NO_ERROR 0
#define CALLOC_ERROR 4
#define RTC_ERROR 6
#define MUTEX_ERROR 7
#define SSCANF_ERROR -2
#define DELETE_ERROR 8
#define ABORT_DELAY_ERROR 10
static char cmd_buff[MAX_BUFFER_LENGTH];

extern char *fileName1;
extern int delay_aborted;
extern int delete_task;

// Structure inspired by: https://man7.org/linux/man-pages/man5/crontab.5.html
typedef struct __attribute__((packed)) {
    // TODO: determine if second accuracy is needed
    tmElements_t scheduled_time;
    uint16_t milliseconds;
    uint16_t length;
    uint16_t dst;
    uint16_t dport;
    char data[MAX_CMD_LENGTH];
} scheduled_commands_t;

typedef struct __attribute__((packed)) {
    // TODO: determine if seconds accuracy is needed
    uint32_t unix_time;
    uint16_t milliseconds;
    uint32_t
        frequency; // frequency the cmd needs to be executed in seconds, value of 0 means the cmd is not repeated
    uint16_t length;
    uint16_t dst;
    uint16_t dport;
    char data[MAX_CMD_LENGTH];
} scheduled_commands_unix_t;

typedef struct __attribute__((packed)) {
    // TODO: determine if second accuracy is needed
    uint8 non_rep_cmds;
    uint8 rep_cmds;
} number_of_cmds_t;

static number_of_cmds_t num_of_cmds;

typedef enum {
    SET_SCHEDULE = 0,
    GET_SCHEDULE = 1,
    REPLACE_SCHEDULE = 2,
    DELETE_SCHEDULE = 3,
    PING_SCHEDULE = 4
} Scheduler_Subtype;

SAT_returnState scheduler_service_app(csp_packet_t *gs_cmds, SemaphoreHandle_t scheduleSemaphore);
// SAT_returnState scheduler_service_app(char *gs_cmds);
SAT_returnState scheduler_service(SemaphoreHandle_t scheduleSemaphore);
SAT_returnState start_scheduler_service(void);
int calc_cmd_frequency(scheduled_commands_t *cmds, int number_of_cmds, scheduled_commands_unix_t *sorted_cmds);
SAT_returnState sort_cmds(scheduled_commands_unix_t *sorted_cmds, int number_of_cmds);
static scheduled_commands_t *prv_get_cmds_scheduler();
SAT_returnState vSchedulerHandler(void *pvParameters);

#endif /* EX2_SYSTEM_INCLUDE_SCHEDULER_H_ */
