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
 * @file    housekeeping_athena.h
 * @author  Dustin Wagner
 * @date    2021-05-13
 */

#ifndef HOUSEKEEPING_ATHENA_H
#define HOUSEKEEPING_ATHENA_H
#define ATHENA_TEMP_ARRAY_SIZE 2

#include <stdint.h>

/*Add includes for other athena files to get hk data from*/
#include "tempsense_athena.h"

/*Add housekeeping fields to this struct to be included in hk reports
ATTENTION:
  Basic data types and fixed arrays allowed
    e.g.
    uint16_t, char, float, char string[30]
  Structs and dynamic types not allowed. will break generic read/write to file system
    e.g.
    char *string, struct {} dataSet;
*/

typedef struct __attribute__((packed)) {
    uint16_t version_major;
    uint16_t version_minor;
    uint16_t version_patch;
    int16_t MCU_core_temp;
    int16_t converter_temp;
    uint32_t OBC_uptime;              // Seconds
    uint8_t vol0_usage_percent;       // Expected usage is 0-80%
    uint8_t vol1_usage_percent;       // Expected usage is 0-80%
    uint16_t boot_cnt;                // Total # of OBC boots
    uint16_t boot_src;                //
    uint8_t last_reset_reason;        //
    uint16_t solar_panel_supply_curr; //
    uint16_t commands_received;       // Number this boot
    uint32_t heap_free;               // Number of bytes left in the heap
    uint32_t lowest_heap_free;        // Lowest number of bytes free this boot
} athena_housekeeping;

int Athena_getHK(athena_housekeeping *athena_hk);
int Athena_hk_convert_endianness(athena_housekeeping *athena_hk);
uint32_t Athena_get_OBC_uptime();
uint16_t Athena_get_solar_supply_curr();

#endif /* HOUSEKEEPING_ATHENA_H */
