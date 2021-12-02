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
#include "housekeeping_athena.h"

#define CHANNEL_LOCAL 0
#define CHANNEL_REMOTE 1

uint8_t tmp_addr[NUM_TEMP_SENSOR] = {TEMP_ADDRESS_1, TEMP_ADDRESS_2};

void inittemp_all(void) {
    int i;
    for (i = 0; i < NUM_TEMP_SENSOR; i++) {
        tmp421_init_client(tmp_addr[i]);
    }
}

int gettemp_all(long *temparray) {
    int i;
    for (i = 0; i < NUM_TEMP_SENSOR; i++) {
        tmp421_read(tmp_addr[i], CHANNEL_LOCAL, &temparray[i]); // assuming we want to read remote channel
    }
    return 0;
}
