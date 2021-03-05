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

#define CHANNEL_LOCAL 0
#define CHANNEL_REMOTE 1

uint8_t tmp_addr[6] = {0x1D, 0x1E, 0x2A, 0x4C, 0x4F, 0x4E};

void inittemp_all(void){
    int i;
    int delay;
    for(i=0;i<6;i++){
        tmp421_init_client(tmp_addr[i]);
        for (delay = 0; delay < 0x1000; delay++);//temporary fix... don't want delay down the road
    }
}

int gettemp_all(long temparray[6]){
    int i;
    int delay;
    for(i=0;i<6;i++){
        tmp421_read(tmp_addr[i], CHANNEL_LOCAL, &temparray[i]);//assuming we want to read remote channel
        for (delay = 0; delay < 0x1000; delay++);//temporary fix... don't want delay down the road
    }
    return 0;
}
