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
 * @file    housekeeping_charon.h
 * @author  Thomas Ganley
 * @date    2021-12-29
 */

#include <stdint.h>
#include "skytraq_binary_types.h"
#include "ads7128.h"

typedef struct __attribute__((packed)) {
    uint16_t crc;
    int8_t temparray[8]; // Charon temperature array
} charon_housekeeping;

GPS_RETURNSTATE Charon_getHK(charon_housekeeping *hk);
