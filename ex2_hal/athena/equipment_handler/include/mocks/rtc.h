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

#ifndef HAL_H
#define HAL_H
#include <stdint.h>
#include "FreeRTOS.h"
#include "os_task.h"
#include "time_struct.h"

void mock_RTC_set_unix_time(uint32_t unix_timestamp);

void mock_RTC_get_unix_time(uint32_t *unix_timestamp);

#define HAL_RTC_GetTime mock_RTC_get_unix_time
#define HAL_RTC_SetTime mock_RTC_set_unix_time

void HAL_get_temperature(float *temp);

void HAL_get_current_1(float *current);

void HAL_get_current_2(float *current);

void HAL_get_voltage_1(float *voltage);

void HAL_get_voltage_2(float *voltage);

void mock_RTC_set_date(date_t new_date);

void mock_RTC_get_date(date_t *out_date);

void mock_RTC_get_time(ex2_time_t *out_time);

void mock_RTC_set_time(ex2_time_t new_time);

BaseType_t start_mock_rtc();

#endif
