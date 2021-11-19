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
#include "housekeeping_service.h" //contains all housekeeping data
#include "rtcmk.h"                //to get time from RTC
#include "services.h"
#include "adcs_handler.h"
#include "beacon_task.h" //contains beacon packets (ie. a summary of housekeeping data)

/**
 * @brief
 *      Updates the beacon packet with the latest housekeeping data
 * @param all_hk_data
 *      The pointer to where housekeeping data is kept
 */

beacon_t beacon_packet;
int i;

/* Updates Beacon Packet with the latest housekeeping data */
void update_beacon(All_systems_housekeeping *all_hk_data) {
    // Populate the beacon packet by fetching relevant data from housekeeping_services.c
    // get the unix time from RTC, and convert it to a struct using RTCMK_GetUnix
    // RTCMK_GetUnix(&(beacon_packet.time));
    memcpy(&(beacon_packet.time), &(all_hk_data->hk_timeorder.UNIXtimestamp), sizeof(beacon_packet.time));

    /*-------EPS-------*/
    memcpy(&(beacon_packet.eps_mode), &(all_hk_data->EPS_hk.battMode), sizeof(beacon_packet.eps_mode));
    memcpy(&(beacon_packet.battery_voltage), &(all_hk_data->EPS_hk.vBatt), sizeof(beacon_packet.battery_voltage));
    memcpy(&(beacon_packet.battery_input_current), &(all_hk_data->EPS_hk.curBattIn),
           sizeof(beacon_packet.battery_input_current));

    uint16_t current_channels_array_length[10];
    memcpy(&(beacon_packet.current_channels), &(all_hk_data->EPS_hk.curOutput),
           sizeof(current_channels_array_length));

    memcpy(&(beacon_packet.output_states), &(all_hk_data->EPS_hk.outputStatus), sizeof(uint16_t));

    uint16_t output_faults_array_length[10];
    memcpy(&(beacon_packet.output_faults), &(all_hk_data->EPS_hk.outputFaultCnt),
           sizeof(output_faults_array_length));

    memcpy(&(beacon_packet.EPS_boot_count), &(all_hk_data->EPS_hk.bootCnt), sizeof(beacon_packet.EPS_boot_count));

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
    memcpy(&(beacon_packet.eps_last_reset_reason), &EPS_last_reset_reason, sizeof(uint8_t));

    /*-------Watchdog-------*/
    // TODO: More work to be done on watchdogs
    memcpy(&(beacon_packet.gs_wdt), &(all_hk_data->EPS_hk.outputConverterState), sizeof(uint8_t));
    memcpy(&(beacon_packet.obc_wdt), &(all_hk_data->EPS_hk.outputConverterState), sizeof(uint16_t));
    memcpy(&(beacon_packet.gs_wdt_expr), &(all_hk_data->EPS_hk.outputConverterState), sizeof(uint8_t));
    memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.outputConverterState), sizeof(uint16_t));

    /*-------Temperatures-------*/
    // Onboard battery pack temp location defined in ICD
    memcpy(&(beacon_packet.temps[0]), &(all_hk_data->EPS_hk.temp[8]), sizeof(int8_t));

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
    memcpy(&(beacon_packet.temps[1]), &max_MCU_temp, sizeof(int8_t));

    memcpy(&(beacon_packet.temps[2]), &(all_hk_data->UHF_hk.temperature), sizeof(int8_t));

    // TODO: write a function/preprocesor to determine 2U or 3U, and then fetch payload temp with CAN protocol
    // memcpy(&(beacon_packet.temps[3]), &(all_hk_data->payload_hk.payload_temp), sizeof(int8_t));

    memcpy(&(beacon_packet.temps[4]), &(all_hk_data->hyperion_hk.Nadir_Temp1), sizeof(int8_t));
    memcpy(&(beacon_packet.temps[5]), &(all_hk_data->hyperion_hk.Zenith_Temp1), sizeof(int8_t));
    memcpy(&(beacon_packet.temps[6]), &(all_hk_data->hyperion_hk.Port_Temp1), sizeof(int8_t));
    memcpy(&(beacon_packet.temps[7]), &(all_hk_data->hyperion_hk.Star_Temp1), sizeof(int8_t));

    /*-------ADCS-------*/
    // Of the x, y, and z angular rates, use the maximum angular rate
    // TODO: convert angular rate from float to uint_8
    uint8_t max_rate =
        max(all_hk_data->adcs_hk.Estimated_Angular_Rate_X, all_hk_data->adcs_hk.Estimated_Angular_Rate_Y);
    max_rate = max(max_rate, all_hk_data->adcs_hk.Estimated_Angular_Rate_Z);
    memcpy(&(beacon_packet.angular_rate), &max_rate, sizeof(int8_t));

    adcs_state adcs_current_hk;
    ADCS_get_current_state(&adcs_current_hk);
    memcpy(&(beacon_packet.adcs_control_mode), &(adcs_current_hk.att_ctrl_mode), sizeof(uint8_t));

    /*-------UHF-------*/
    // Convert uint32_t to uint16_t, seconds = UHF_uptime*10. Max = 655350 seconds (7.6 days)
    uint16_t UHF_uptime = all_hk_data->UHF_hk.uptime;
    memcpy(&(beacon_packet.uhf_uptime), &(UHF_uptime), sizeof(uint16_t));

    /*-------Payload-------*/
    // TODO: Write payload driver to get payload software version and temp
    // memcpy(&(beacon_packet.payload_software_ver), &(all_hk_data->payload_hk.payload_software_ver),
    // sizeof(uint8_t));

    /*-------OBC-------*/
    // TODO: write missing drivers and functions for OBC/athena housekeeping
    memcpy(&(beacon_packet.obc_boot_count), &(all_hk_data->Athena_hk.boot_cnt), sizeof(uint16_t));
    memcpy(&(beacon_packet.obc_last_reset_reason), &(all_hk_data->Athena_hk.last_reset_reason), sizeof(uint8_t));
    memcpy(&(beacon_packet.obc_mode), &(all_hk_data->Athena_hk.OBC_mode), sizeof(uint8_t));

    // Unit will be deca-seconds due to size restraint, seconds = OBC_uptime*10
    memcpy(&(beacon_packet.obc_uptime), &(all_hk_data->Athena_hk.OBC_uptime), sizeof(uint16_t));

    memcpy(&(beacon_packet.solar_panel_supply_curr), &(all_hk_data->Athena_hk.solar_panel_supply_curr),
           sizeof(uint8_t));
    memcpy(&(beacon_packet.obc_software_ver), &(all_hk_data->Athena_hk.OBC_software_ver), sizeof(uint8_t));
    memcpy(&(beacon_packet.obc_cmds_received), &(all_hk_data->Athena_hk.cmds_received), sizeof(uint16_t));
    memcpy(&(beacon_packet.pckts_unrecovered_by_FEC), &(all_hk_data->Athena_hk.pckts_uncovered_by_FEC),
           sizeof(uint16_t));

    /*-------Logged Items-------*/
    // TODO: write the function for logged items in beacon_task
    memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.outputConverterState), sizeof(uint8_t));
    memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.outputConverterState), sizeof(uint8_t));
    memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.outputConverterState), sizeof(uint8_t));
    memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.outputConverterState), sizeof(uint8_t));
    memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.outputConverterState), sizeof(uint8_t));
    memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.outputConverterState), sizeof(uint8_t));
    memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.outputConverterState), sizeof(uint8_t));
    memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.outputConverterState), sizeof(uint8_t));
    memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.outputConverterState), sizeof(uint8_t));
    memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.outputConverterState), sizeof(uint8_t));
    memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.outputConverterState), sizeof(uint8_t));
    memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.outputConverterState), sizeof(uint8_t));
    memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.outputConverterState), sizeof(uint8_t));
}
