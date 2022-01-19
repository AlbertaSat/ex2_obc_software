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
 * @file housekeeping_to_beacon.c
 * @author Grace Yi
 * @date 2021-10-27
 */
#include <beacon_task.h> //contains beacon packets (ie. a summary of housekeeping data)
#include "housekeeping_service.h" //contains all housekeeping data
#include "rtcmk.h"                //to get time from RTC
#include "services.h"
#include "adcs_handler.h"

/**
 * @brief
 *      Updates the beacon packet with the latest housekeeping data
 * @param all_hk_data
 *      The pointer to where housekeeping data is kept
 */

extern beacon_packet_1_t beacon_packet_one;
extern beacon_packet_2_t beacon_packet_two;

/* Updates Beacon Packet with the latest housekeeping data */
void update_beacon(All_systems_housekeeping *all_hk_data);
