/*
 *
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
 * @file housekeeping_to_beacon.h
 * @author Grace Yi
 * @date 2021-10-22
 */

#ifndef HOUSEKEEPING_TO_BEACON_H
#define HOUSEKEEPING_TO_BEACON_H

#include <csp/csp.h>
#include <FreeRTOS.h>
#include <os_task.h>
#include <os_semphr.h>
#include <os_projdefs.h>
#include <os_portmacro.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "skytraq_gps_driver.h"
#include "time_management_service.h"
#include "beacon_task.h"

// Beacon packet is split into two different packets, hence will be updated separately
void update_beacon(All_systems_housekeeping *all_hk_data);

#endif /* HOUSEKEEPING_TO_BEACON_H */
