/*
 * Copyright (C) 2021  University of Alberta
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
 * @file northern_spirit_io.c
 * @author Thomas Ganley
 * @date 2021-10-26
 */

#include "northern_spirit_handler.h"
#include "FreeRTOS.h"
#include "HL_sci.h"
#include "i2c_io.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_task.h"
#include "system.h"
#include <stdbool.h>
#include <string.h>

#define NS_SEMAPHORE_TIMEOUT_MS 100
#define NS_UART_TIMEOUT_MS 10000

void init_ns_io();

NS_return send_NS_command(uint8_t* command, uint32_t command_length, uint8_t* answer, uint8_t answer_length);
