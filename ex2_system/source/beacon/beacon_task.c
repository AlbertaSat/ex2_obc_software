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
 * @file beacon_task.c
 * @author Andrew R. Rooney
 * @date Mar. 6, 2021
 */

#include <FreeRTOS.h>
#include <os_task.h>

#include "HL_gio.h"
#include "HL_het.h"
#include "beacon/beacon.h"
#include "eps.h"
#include "uhf.h"

#define SCW_BCN_FLAG 5
#define SCW_BCN_ON 1

typedef struct __attribute__((packed)) {
    uint32_t time;
    uint8_t packet_number;
    uint8_t eps_mode;
    uint16_t battery_voltage;
    uint16_t battery_input_current;
    uint16_t current_channels[10];
    uint16_t output_states;
    uint8_t output_faults[10];
    uint16_t EPS_boot_count;
    uint8_t eps_last_reset_reason;
    uint16_t gs_wdt;
    uint8_t obc_wdt;
    uint16_t gs_wdt_expr;
    uint16_t obc_wdt_expr;
    int8_t temps[18];
} beacon_common_t;
typedef struct __attribute__((packed)) {
    beacon_common_t common;
    int8_t angular_rate;
    int8_t adcs_control_mode;
    uint16_t uhf_uptime;
    uint8_t payload_software_version;
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
    uint32_t log1_timestamp;
    uint8_t log1_code;
} beacon_packet_1_t;
typedef struct __attribute__((packed)) {
    beacon_common_t common;
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

static void *beacon_daemon(void *pvParameters);
SAT_returnState start_beacon_daemon(void);
static TickType_t beacon_delay = pdMS_TO_TICKS(1000); //converts 1000 ms to number of ticks
/**
 * Construct and send out the system beacon at the required frequency.
 *
 * @param pvParameters
 *    task parameters (not used)
 */
static void *beacon_daemon(void *pvParameters) {
    for (;;) {
        int8_t uhf_status = -1;
        /* Constructing the system beacon content */
        // Refer to table 3 of MOP
        UHF_configStruct beacon_msg;
        beacon_packet_1_t beacon_packet_one = {
            //define each element as 1 for testing purposes
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,BLANK_SPACE};


        memcpy(&(beacon_msg.message), &beacon_packet_one, sizeof(beacon_packet_1_t));
        beacon_msg.len = sizeof(beacon_packet_1_t);
        //memcpy(&(beacon_msg.message), &beacon_packet_2, sizeof(beacon_packet_2));
        // TODO: call the appropriate HAL functions to get the most updated or
        // cached information of the components + state machine, RTC, etc.
        // Then uncomment the next line:
        uhf_status = HAL_UHF_setBeaconMsg(beacon_msg);
        /* Sending the beacon */
        // The beacon transmission period is configurable through comms service
        // by the operator or here through HAL_UHF_getBeaconT().
        uint8_t scw[SCW_LEN];
        scw[5] = 1;
        HAL_UHF_setSCW(scw);

#ifndef UHF_IS_STUBBED
    uhf_status = HAL_UHF_getSCW(scw);

    if (uhf_status == U_GOOD_CONFIG) {
      scw[SCW_BCN_FLAG] = SCW_BCN_ON;
      uhf_status = HAL_UHF_setSCW(scw);
    }
#endif
#ifndef EPS_IS_STUBBED
    if (uhf_status != U_GOOD_CONFIG) {

      if (eps_get_pwr_chnl(UHF_PWR_CHNL) == 1 &&
          gioGetBit(UHF_GIO_PORT, UHF_GIO_PIN) == 1) {
        printf("Beacon failed");
      } else
        printf("UHF is off.");
    }
#endif

    vTaskDelay(beacon_delay);
  }
}

/**
 * Start the beacon daemon
 *
 * @returns status
 *   error report of task creation
 */
SAT_returnState start_beacon_daemon(void) {
  if (xTaskCreate((TaskFunction_t)beacon_daemon, "beacon_daemon", 2048, NULL,
                  BEACON_TASK_PRIO, NULL) != pdPASS) {
    ex2_log("FAILED TO CREATE TASK coordinate_management_daemon\n");
    return SATR_ERROR;
  }
  ex2_log("Coordinate management started\n");
  return SATR_OK;
}
