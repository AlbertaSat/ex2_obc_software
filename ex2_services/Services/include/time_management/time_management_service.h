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

#ifndef TIME_MANAGEMENT_H
#define TIME_MANAGEMENT_H

#include <stdint.h>

#include "services.h"

#define MIN_YEAR 1577836800  // 2020-01-01
#define MAX_YEAR 1893456000  // 2030-01-01

#define TIMESTAMP_ISOK(x) (x > MIN_YEAR && x < MAX_YEAR) ? 1 : 0

// TIME MANAGEMENT SERVICE
#define TC_TIME_MANAGEMENT_SERVICE 8
typedef enum {
  GET_TIME = 0,
  SET_TIME = 1
} Time_Management_Subtype;  // shared with EPS!

struct time_utc {
  uint32_t unix_timestamp;
};

SAT_returnState start_time_management_service(void);

#endif /* TIME_MANAGEMENT_H */
