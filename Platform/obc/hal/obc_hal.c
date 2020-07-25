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
/**
 * @file demo_hal.c
 * @author Andrew Rooney
 * @date 2020-06-06
 */

#include <obc/hal/obc_hal.h>
#include "hal.h"

uint32_t current_time;

/**
 * These functions are WIP stubs to a non-existent RTC
 */

void HAL_sys_setTime(uint32_t unix_timestamp) {
  HAL_RTC_SetTime(unix_timestamp);
}

void HAL_sys_getTime(uint32_t *unix_timestamp) {
  HAL_RTC_GetTime(unix_timestamp);
}
