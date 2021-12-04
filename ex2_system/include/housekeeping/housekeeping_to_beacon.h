/*
 * Copyright (C) 2020  University of Alberta
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
 * @date Dec. 03, 2021
 */
#ifndef EX2_SYSTEM_HOUSEKEEPING_TO_BEACON_H_
#define EX2_SYSTEM_HOUSEKEEPING_TO_BEACON_H_

#include "housekeeping_to_beacon.h"
#include "housekeeping_service.h" //contains all housekeeping data
#include "rtcmk.h" //to get time from RTC
#include "services.h"
#include "beacon_task.h" //contains beacon packets (ie. a summary of housekeeping data)


void update_beacon(All_systems_housekeeping* all_hk_data);

#endif /* EX2_SYSTEM_HOUSEKEEPING_TO_BEACON_H_ */
