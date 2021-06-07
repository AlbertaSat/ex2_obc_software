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

#include <stdint.h>

/*Add includes for other athena files to get hk data from*/
#ifndef ATHENA_IS_STUBBED
#include "tempsense_athena.h"
#endif

/*Add housekeeping fields to this struct to be included in hk reports
ATTENTION:
  Basic data types and fixed arrays allowed
    e.g.
    uint16_t, char, float, char string[30]
  Structs and dynamic types not allowed. will break generic read/write to file system
    e.g.
    char *string, struct {} dataSet;
*/
typedef struct __attribute__((packed)){
  long temparray[6];                     //Athena temperature array
} athena_housekeeping;

int Athena_getHK(athena_housekeeping* athena_hk);
int Athena_hk_convert_endianness(athena_housekeeping* athena_hk);

#endif
