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

#ifndef LOGGER_H
#define LOGGER_H

#include "FreeRTOS.h"
#include "os_task.h"
#include "system.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define PRINT_BUF_LEN 65

#define STRING_MAX_LEN 201

typedef enum {
    PANIC,     // Emergency - system is going down
    ALERT,     // Should be corrected immediately
    CRITICAL,  // Primary system failure
    ERROR,     // Non-urgent failures
    WARN,      // Not a failure, but error could follow
    NOTICE,    // Unusual but not error condition
    INFO,      // Normal operational message
    DEBUG,
} SysLog_Level;

void sys_log(SysLog_Level level, const char *format, ...);

#define ex2_log(_args_...) sys_log(INFO, _args_)

int8_t store_logger_file_size(void);

int8_t load_logger_file_size(void);

int8_t set_logger_file_size(uint32_t file_size);

int8_t get_logger_file_size(uint32_t *file_size);

char *get_logger_file();

char *get_logger_old_file();

SAT_returnState start_logger_daemon(TaskHandle_t *handle);

void kill_logger_daemon();

bool init_logger_fs();

void stop_logger_fs();

// to be called before scheduler is started
void init_logger_queue();

#endif // LOGGER_H
