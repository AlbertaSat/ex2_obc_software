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
 * @file beacon_task.h
 * @author Andrew R. Rooney, Grace Yi
 * @date Mar. 6, 2021
 */
#ifndef EX2_SYSTEM_INCLUDE_BEACON_TASK_H_
#define EX2_SYSTEM_INCLUDE_BEACON_TASK_H_

#include <FreeRTOS.h>
#include <os_task.h>

#include "HL_gio.h"
#include "HL_het.h"
#include "eps.h"
#include "uhf.h"

#include "system.h"
#include "uTransceiver.h"
#include "housekeeping_service.h"

typedef struct __attribute__((packed)) {
    // META
    uint32_t time;
    uint8_t packet_number;
    // Deployables
    uint8_t switch_stat;
    // EPS
    uint8_t eps_mode;
    uint16_t battery_voltage;
    uint16_t battery_input_current;
    uint16_t current_channels[10];
    uint16_t output_status;
    uint8_t output_faults[10];
    uint16_t EPS_boot_count;
    uint8_t eps_last_reset_reason;
    // EPS Watchdogs
    uint32_t gs_wdt_time;
    uint8_t gs_wdt_cnt;
    uint8_t obc_wdt_toggles;
    uint8_t obc_wdt_turnoffs;
} beacon_packet_1_t;

typedef struct __attribute__((packed)) {
    // META
    uint32_t time;
    uint8_t packet_number;
    // Temperatures
    int8_t temps[17];
    // ADCS
    int8_t angular_rate_X;
    int8_t angular_rate_Y;
    int8_t angular_rate_Z;
    int8_t adcs_control_mode;
    // UHF
    uint32_t uhf_uptime;
    // OBC
    uint16_t boot_cnt;
    uint8_t last_reset_reason;
    uint32_t OBC_uptime;
    uint8_t solar_panel_supply_curr;
    uint8_t OBC_software_ver;
    uint16_t cmds_received;
} beacon_packet_2_t;

/**
 * @brief
 *      Updates the beacon packet with the latest housekeeping data
 * @param all_hk_data
 *      The pointer to where housekeeping data is kept
 */

/* Updates Beacon Packet with the latest housekeeping data */
void update_beacon(All_systems_housekeeping *all_hk_data, beacon_packet_1_t *beacon_packet_one,
                   beacon_packet_2_t *beacon_packet_two);

SAT_returnState start_beacon_daemon(void);
bool enable_beacon_task(void);
bool disable_beacon_task(void);
bool beacon_task_get_state(void);

#endif /* EX2_SYSTEM_INCLUDE_BEACON_TASK_H_ */
