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
/*
 * time_struct.h
 *
 *  Created on: Mar. 22, 2021
 *      Author: Robert Taylor
 */

#ifndef TIME_STRUCT_H_
#define TIME_STRUCT_H_

#include <stdint.h>

typedef struct ex2_time_t {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t ms;
} ex2_time_t;

typedef struct date_t {
    uint8_t day;
    uint8_t month;
    uint8_t year;
} date_t;

#endif /* TIME_STRUCT_H_ */

