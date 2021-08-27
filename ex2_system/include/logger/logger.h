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

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "os_task.h"
#include "system.h"

#define PRINT_BUF_LEN 65

#define STRING_MAX_LEN 201

static bool exists(const char *filename);

int8_t store_logger_file_size(void);

int8_t load_logger_file_size(void);

int8_t set_logger_file_size(uint32_t file_size);

int8_t get_logger_file_size(uint32_t* file_size);

char * get_logger_file();

char * get_logger_old_file();

void ex2_log(const char *format, ...);

SAT_returnState start_logger_daemon(TaskHandle_t *handle);

void kill_logger_daemon();

bool init_logger_fs();

void stop_logger_fs();

// to be called before scheduler is started
void init_logger_queue();

#endif // LOGGER_H
