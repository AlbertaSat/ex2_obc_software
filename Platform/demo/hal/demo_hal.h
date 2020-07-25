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

#ifndef DEMO_HAL_H
#define DEMO_HAL_H

#include <inttypes.h>

#include "services.h"

void HAL_sys_getTime(uint32_t *unix_timestamp);
void HAL_sys_setTime(uint32_t unix_timestamp);
SAT_returnState HAL_hk_report(uint8_t sid, void *output);

#endif /* DEMO_HAL_H */
