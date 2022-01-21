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
#include "housekeeping_to_beacon.h"

typedef struct __attribute__((packed)) {
    //common
    uint32_t time;
    uint8_t packet_number;
    //EPS
    uint8_t eps_mode;
    uint16_t battery_voltage;
    uint16_t battery_input_current;
    uint16_t current_channels[10];
    uint16_t output_states;
    uint8_t output_faults[10];
    uint16_t EPS_boot_count;
    uint8_t eps_last_reset_reason;
    //Watchdogs
    uint16_t gs_wdt;
    uint8_t obc_wdt;
    uint16_t gs_wdt_expr;
    uint16_t obc_wdt_expr;
    //Temperatures
    int8_t temps[18];
    //ADCS
    int8_t angular_rate;
    int8_t adcs_control_mode;
    //UHF
    uint16_t uhf_uptime;
    //Payload
    uint8_t payload_software_version;
} beacon_packet_1_t;
typedef struct __attribute__((packed)) {
    //common
    uint32_t time;
    uint8_t packet_number;
    //OBC
    uint16_t obc_boot_count;
    uint8_t obc_last_reset_reason;
    uint8_t obc_mode;
    uint16_t obc_uptime;
    uint8_t solar_panel_current;
    uint16_t mcu_core_current;
    uint8_t obc_software_version;
    uint16_t commands_received;
    uint16_t fec_recovered_packets;
    uint8_t logged_items_count;
    //Logged Items
    uint32_t log1_timestamp;
    uint8_t log1_code;
    uint32_t log2_timestamp;
    uint8_t log2_code;
    uint32_t log3_timestamp;
    uint8_t log3_code;
    uint32_t log4_timestamp;
    uint8_t log4_code;
    uint32_t log5_timestamp;
    uint8_t log5_code;
    uint32_t log6_timestamp;
    uint8_t log6_code;
} beacon_packet_2_t;

//extern beacon_packet_1_t beacon_packet_one;
//extern beacon_packet_2_t beacon_packet_two;
beacon_packet_1_t beacon_packet_one;
beacon_packet_2_t beacon_packet_two;

SAT_returnState start_beacon_daemon(void);

#endif /* EX2_SYSTEM_INCLUDE_BEACON_TASK_H_ */
