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
#include "housekeeping_to_beacon.h"

/**
 * @brief
 *      Updates the beacon packet with the latest housekeeping data
 * @param all_hk_data
 *      The pointer to where housekeeping data is kept
 */

/* Updates Beacon Packet with the latest housekeeping data */
// void update_beacon(All_systems_housekeeping *all_hk_data, beacon_packet_1_t *beacon_packet_one,
// beacon_packet_2_t *beacon_packet_two) {
void update_beacon(All_systems_housekeeping *all_hk_data, beacon_packet_1_t *beacon_packet_one,
                   beacon_packet_2_t *beacon_packet_two) {
    int i;
    // Populate the beacon packet by fetching relevant data from housekeeping_services.c

    // TODO: remove this after testing
    // get the unix time from RTC, and convert it to a struct using RTCMK_GetUnix
    // RTCMK_GetUnix(&(beacon_packet.time));

    beacon_packet_one->time = all_hk_data->hk_timeorder.UNIXtimestamp;
    beacon_packet_two->time = all_hk_data->hk_timeorder.UNIXtimestamp;
    beacon_packet_one->packet_number = 1;
    beacon_packet_two->packet_number = 2;

    /*-------EPS-------*/
    beacon_packet_one->eps_mode = all_hk_data->EPS_hk.battMode;
    beacon_packet_one->battery_voltage = all_hk_data->EPS_hk.vBatt;
    beacon_packet_one->battery_input_current = all_hk_data->EPS_hk.curBattIn;
    uint16_t current_channels_array_length[10];
    memcpy(&(beacon_packet_one->current_channels), &(all_hk_data->EPS_hk.curOutput),
           sizeof(current_channels_array_length));

    beacon_packet_one->output_states = all_hk_data->EPS_hk.outputStatus;

    uint16_t output_faults_array_length[10];
    memcpy(&(beacon_packet_one->output_faults), &(all_hk_data->EPS_hk.outputFaultCnt),
           sizeof(output_faults_array_length));

    beacon_packet_one->EPS_boot_count = all_hk_data->EPS_hk.bootCnt;

    // Last EPS reset reason
    typedef enum {
        Power_on = 0, // After power supply removal or hard reset
        IWDG = 1, // Internal watchdog reseted system due to software process hanging or MCU hardware malfunction
        NRST = 2, // MCU reset pin driven low
        Software = 3, // Software reset by command
    } EPS_reset_TypeDef;

    EPS_reset_TypeDef EPS_last_reset_reason;
    switch (all_hk_data->EPS_startup_hk.last_reset_reason_reg) {
    case 0x0C800000:
        EPS_last_reset_reason = Power_on;
        break;
    case 0x24000000:
        EPS_last_reset_reason = IWDG;
        break;
    case 0x04000000:
        EPS_last_reset_reason = NRST;
        break;
    case 0x14000000:
        EPS_last_reset_reason = Software;
        break;
    }
    // TEST: eps_last_reset_reason added, define the function based on EPS ICD manual
    beacon_packet_one->eps_last_reset_reason = EPS_last_reset_reason;

    /*-------Watchdog-------*/
    // TODO: More work to be done on watchdogs, using temporary placeholders from hk data
    beacon_packet_one->gs_wdt = all_hk_data->EPS_hk.vBatt;
    beacon_packet_one->obc_wdt = all_hk_data->EPS_hk.outputConverterState;
    beacon_packet_one->gs_wdt_expr = all_hk_data->EPS_hk.vBatt;
    beacon_packet_one->obc_wdt_expr = all_hk_data->EPS_hk.outputConverterState;

    /*-------Temperatures-------*/
    // TODO: more temp data was added to beacon from hk, review temp fields before finalizing beacon struct
    // Onboard battery pack temp location defined in ICD
    beacon_packet_one->temps[0] = all_hk_data->EPS_hk.temp[8];

    // TODO: Athena MCU temperature should not be stored as long data type since data size can change depending on
    // the computer
    //      should stick with int64_t format. Change the temp driver to fix this.
    long athena_MCU_temp_long[2];
    int8_t athena_MCU_temp[2];
    // Min/max operating temperature on the temp sensor is -55/127 Celsius,
    //      hence int8_t is sufficient, convert long into int8_t
    for (i = 0; i < 2; i++) {
        athena_MCU_temp_long[i] = all_hk_data->Athena_hk.temparray[i];
        if (athena_MCU_temp_long[i] > 127) {
            athena_MCU_temp_long[i] = 127;
        }
        if (athena_MCU_temp_long[i] < -128) {
            athena_MCU_temp_long[i] = -128;
        }
        if ((athena_MCU_temp_long[i] >> 63) == 1) { // negative
            athena_MCU_temp[i] = ((athena_MCU_temp_long[i] & 127) | 128);
        } else { // positive
            athena_MCU_temp[i] = (athena_MCU_temp_long[i] & 127);
        }
    }
    // use max temp out of the 2 Athena temps
    int8_t max_MCU_temp = max(athena_MCU_temp[0], athena_MCU_temp[1]);
    beacon_packet_one->temps[1] = max_MCU_temp;

    beacon_packet_one->temps[2] = all_hk_data->UHF_hk.temperature;

    // TODO: write a function/preprocesor to determine 2U or 3U, and then fetch payload temp with CAN protocol
    // memcpy(&(beacon_packet.temps[3]), &(all_hk_data->payload_hk.payload_temp), sizeof(int8_t));

    beacon_packet_one->temps[4] = all_hk_data->hyperion_hk.Nadir_Temp1;
    beacon_packet_one->temps[5] = all_hk_data->hyperion_hk.Zenith_Temp1;
    beacon_packet_one->temps[6] = all_hk_data->hyperion_hk.Port_Temp1;
    beacon_packet_one->temps[7] = all_hk_data->hyperion_hk.Star_Temp1;

    /*-------ADCS-------*/
    // Of the x, y, and z angular rates, use the maximum angular rate
    // TODO: convert angular rate from float to uint_8
    uint8_t max_rate =
        max(all_hk_data->adcs_hk.Estimated_Angular_Rate_X, all_hk_data->adcs_hk.Estimated_Angular_Rate_Y);
    max_rate = max(max_rate, all_hk_data->adcs_hk.Estimated_Angular_Rate_Z);
    beacon_packet_one->angular_rate = max_rate;

    adcs_state adcs_current_hk;
    ADCS_get_current_state(&adcs_current_hk);
    beacon_packet_one->adcs_control_mode = adcs_current_hk.att_ctrl_mode;

    /*-------UHF-------*/
    // Convert uint32_t to uint16_t, seconds = UHF_uptime*10. Max = 655350 seconds (7.6 days)
    uint16_t UHF_uptime = all_hk_data->UHF_hk.uptime;
    beacon_packet_one->uhf_uptime = UHF_uptime;

    /*-------Payload-------*/
    // TODO: Write payload driver to get payload software version and temp
    // memcpy(&(beacon_packet.payload_software_ver), &(all_hk_data->payload_hk.payload_software_ver),
    // sizeof(uint8_t));

    /*-------OBC-------*/
    // TODO: write missing drivers and functions for OBC/athena housekeeping
    beacon_packet_two->obc_boot_count = all_hk_data->Athena_hk.boot_cnt;
    beacon_packet_two->obc_last_reset_reason = all_hk_data->Athena_hk.last_reset_reason;
    beacon_packet_two->obc_mode = all_hk_data->Athena_hk.OBC_mode;

    // Unit will be deca-seconds due to size restraint, seconds = OBC_uptime*10
    beacon_packet_two->obc_uptime = all_hk_data->Athena_hk.OBC_uptime;
    beacon_packet_two->solar_panel_current = all_hk_data->Athena_hk.solar_panel_supply_curr;
    beacon_packet_two->obc_software_version = all_hk_data->Athena_hk.OBC_software_ver;
    beacon_packet_two->commands_received = all_hk_data->Athena_hk.cmds_received;
    beacon_packet_two->fec_recovered_packets = all_hk_data->Athena_hk.pckts_uncovered_by_FEC;

    /*-------Logged Items-------*/
    // TODO: write the function for logged items in beacon_task
    beacon_packet_two->log1_timestamp = all_hk_data->EPS_hk.outputConverterState;
    beacon_packet_two->log1_code = all_hk_data->EPS_hk.outputConverterState;
    beacon_packet_two->log2_timestamp = all_hk_data->EPS_hk.outputConverterState;
    beacon_packet_two->log2_code = all_hk_data->EPS_hk.outputConverterState;
    beacon_packet_two->log3_timestamp = all_hk_data->EPS_hk.outputConverterState;
    beacon_packet_two->log3_code = all_hk_data->EPS_hk.outputConverterState;
    beacon_packet_two->log4_timestamp = all_hk_data->EPS_hk.outputConverterState;
    beacon_packet_two->log4_code = all_hk_data->EPS_hk.outputConverterState;
    beacon_packet_two->log5_timestamp = all_hk_data->EPS_hk.outputConverterState;
    beacon_packet_two->log5_code = all_hk_data->EPS_hk.outputConverterState;
    beacon_packet_two->log6_timestamp = all_hk_data->EPS_hk.outputConverterState;
    beacon_packet_two->log6_code = all_hk_data->EPS_hk.outputConverterState;
}
