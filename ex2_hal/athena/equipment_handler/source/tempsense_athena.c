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

#include "tempsense_athena.h"
#include "tmp421.h"
#include "tmp117.h"
#include "housekeeping_athena.h"
#include "system.h"

#define CHANNEL_LOCAL 0
#define CHANNEL_REMOTE 1
#define ATHENA_TEMPSENSE_DELAY 0.01 * ONE_SECOND
#define MCU_CORE_TEMP_ADD 0x4A
#define CONVERTER_TEMP_ADD 0x49

uint8_t tmp_addr[NUM_TEMP_SENSOR] = {TEMP_ADDRESS_1, TEMP_ADDRESS_2};

void inittemp_all(void) {
    tmp117_init(MCU_CORE_TEMP_ADD);
    tmp117_init(CONVERTER_TEMP_ADD);
}

int gettemp_all(long *MCU_core_temp_add, long *converter_temp_add) {
    tmp117_read(MCU_CORE_TEMP_ADD, MCU_core_temp_add); // assuming we want to read remote channel
    vTaskDelay(ATHENA_TEMPSENSE_DELAY);
    tmp117_read(CONVERTER_TEMP_ADD, converter_temp_add);

    return 0;
}
