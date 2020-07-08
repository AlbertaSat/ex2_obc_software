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

#include <csp/csp.h>
#include <stdint.h>

#include "services.h"

#define MIN_YEAR 1577836800  // 2020-01-01
#define MAX_YEAR 1893456000  // 2030-01-01

#define TIMESTAMP_ISOK(x) (x > MIN_YEAR && x < MAX_YEAR) ? 1 : 0

struct time_utc {
  uint32_t unix_timestamp;
};

void set_time_UTC(struct time_utc utc);

SAT_returnState time_management_app(csp_packet_t *pck);

#endif /* TIME_MANAGEMENT_H */
