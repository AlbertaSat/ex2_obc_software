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
 * @file    tempsense_athena.c
 * @author  Josh Lazaruk
 * @date    2020-08-16
 */
#include "FreeRTOS.h"
#include "os_task.h"
#include "tempsense_athena.h"
#include "tmp117.h"

#define CHANNEL_LOCAL 0
#define CHANNEL_REMOTE 1
#define ATHENA_TEMPSENSE_DELAY pdMS_TO_TICKS(10)
#define MCU_CORE_TEMP_ADD 0x4A
#define CONVERTER_TEMP_ADD 0x49

uint8_t tmp_addr[NUM_TEMP_SENSOR] = {TEMP_ADDRESS_1, TEMP_ADDRESS_2};

int get_Athena_Temperatures(int16_t *MCU_core_temp, int16_t *converter_temp) {
    tmp117_read(MCU_CORE_TEMP_ADD, MCU_core_temp); // assuming we want to read remote channel
    vTaskDelay(ATHENA_TEMPSENSE_DELAY);
    tmp117_read(CONVERTER_TEMP_ADD, converter_temp);

    return 0;
}
