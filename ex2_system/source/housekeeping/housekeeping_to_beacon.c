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
#include "deployablescontrol.h"

static EPS_reset_TypeDef determine_eps_last_reset_reason(uint32_t reg);

/**
 * @brief
 *      Updates the beacon packet with the latest housekeeping data
 * @param all_hk_data
 *      The pointer to where housekeeping data is kept
 */

void update_beacon(All_systems_housekeeping *all_hk_data, beacon_packet_1_t *beacon_packet_one,
                   beacon_packet_2_t *beacon_packet_two) {

    /*-------META-------*/
    int i;
    beacon_packet_one->time = all_hk_data->hk_timeorder.UNIXtimestamp;
    beacon_packet_two->time = all_hk_data->hk_timeorder.UNIXtimestamp;
    beacon_packet_one->packet_number = 1;
    beacon_packet_two->packet_number = 2;

    /*-------Deployables-------*/
    for(Deployable_t i = 0; i <= Starboard; i++){
        bool status = switchstatus(i);
        beacon_packet_one->switch_stat |= (status << i);
    }

    /*-------EPS-------*/
    beacon_packet_one->eps_mode = all_hk_data->EPS_hk.battMode;
    beacon_packet_one->battery_voltage = all_hk_data->EPS_hk.vBatt;
    beacon_packet_one->battery_input_current = all_hk_data->EPS_hk.curBattIn;
    memcpy(&(beacon_packet_one->current_channels), &(all_hk_data->EPS_hk.curOutput),
           sizeof(beacon_packet_one->current_channels));
    beacon_packet_one->output_status = all_hk_data->EPS_hk.outputStatus;
    memcpy(&(beacon_packet_one->output_faults), &(all_hk_data->EPS_hk.outputFaultCnt),
           sizeof(beacon_packet_one->output_faults));
    beacon_packet_one->EPS_boot_count = all_hk_data->EPS_hk.bootCnt;

    beacon_packet_one->eps_last_reset_reason = (uint8_t)determine_eps_last_reset_reason(all_hk_data->EPS_startup_hk.last_reset_reason_reg);

    /*-------Watchdog-------*/
    // TODO: More work to be done on watchdogs, using temporary placeholders from hk data
    beacon_packet_one->gs_wdt_time = all_hk_data->EPS_hk.wdt_gs_time_left;
    beacon_packet_one->gs_wdt_cnt = all_hk_data->EPS_hk.wdt_gs_counter;
    beacon_packet_one->obc_wdt_toggles = all_hk_data->EPS_hk.PingWdt_toggles;
    beacon_packet_one->obc_wdt_turnoffs = all_hk_data->EPS_hk.PingWdt_turnOffs;

    /*-------Temperatures-------*/
    beacon_packet_two->temps[0] = all_hk_data->EPS_hk.temp[10]; // Battery pack temp
    beacon_packet_two->temps[1] = all_hk_data->EPS_hk.temp[8]; // On-board battery temp
    beacon_packet_two->temps[2] = (int8_t)all_hk_data->Athena_hk.temparray[0];
    beacon_packet_two->temps[3] = (int8_t)all_hk_data->Athena_hk.temparray[1];
    beacon_packet_two->temps[4] = (int8_t)all_hk_data->UHF_hk.temperature;
    beacon_packet_two->temps[5] = (int8_t)all_hk_data->S_band_hk.Top_Temp;
    beacon_packet_two->temps[6] = (int8_t)all_hk_data->adcs_hk.MCU_Temp;
    int8_t max_adcs_rate_temp = (int8_t)max(all_hk_data->adcs_hk.Rate_Sensor_Temp_X, all_hk_data->adcs_hk.Rate_Sensor_Temp_Y);
    beacon_packet_two->temps[7] = (int8_t)max(all_hk_data->adcs_hk.Rate_Sensor_Temp_Z, max_adcs_rate_temp);
    beacon_packet_two->temps[8] = (int8_t)all_hk_data->DFGM_hk.boardTemp;
#if IS_EXALTA2 == 1
    beacon_packet_two->temps[9] = all_hk_data->IRIS_hk.vis_temp;
    beacon_packet_two->temps[10] = all_hk_data->IRIS_hk.nir_temp;
#else
    beacon_packet_two->temps[9] = all_hk_data->NS_hk.temp0;
    beacon_packet_two->temps[10] = all_hk_data->NS_hk.temp1;
#endif
    beacon_packet_two->temps[11] = all_hk_data->hyperion_hk.Nadir_Temp1;
    beacon_packet_two->temps[12] = all_hk_data->hyperion_hk.Zenith_Temp1;
    beacon_packet_two->temps[13] = all_hk_data->hyperion_hk.Port_Temp1;
    beacon_packet_two->temps[14] = all_hk_data->hyperion_hk.Star_Temp1;
    beacon_packet_two->temps[15] = all_hk_data->hyperion_hk.Port_Dep_Temp1;
    beacon_packet_two->temps[16] = all_hk_data->hyperion_hk.Star_Dep_Temp1;

    /*-------ADCS-------*/
    beacon_packet_two->angular_rate_X = all_hk_data->adcs_hk.Estimated_Angular_Rate_X;
    beacon_packet_two->angular_rate_Y = all_hk_data->adcs_hk.Estimated_Angular_Rate_Y;
    beacon_packet_two->angular_rate_Z = all_hk_data->adcs_hk.Estimated_Angular_Rate_Z;
    beacon_packet_two->adcs_control_mode = all_hk_data->adcs_hk.att_control_mode;

    /*-------UHF-------*/
    // Convert uint32_t to uint16_t, seconds = UHF_uptime*10. Max = 655350 seconds (7.6 days)
    beacon_packet_two->uhf_uptime = (uint16_t)(all_hk_data->UHF_hk.uptime / 10);

    /*-------OBC-------*/
    beacon_packet_two->boot_cnt = all_hk_data->Athena_hk.boot_cnt;
    beacon_packet_two->last_reset_reason = all_hk_data->Athena_hk.last_reset_reason;
    beacon_packet_two->OBC_mode = all_hk_data->Athena_hk.OBC_mode;
    beacon_packet_two->OBC_uptime = all_hk_data->Athena_hk.OBC_uptime;
    beacon_packet_two->solar_panel_supply_curr = all_hk_data->Athena_hk.solar_panel_supply_curr;
    beacon_packet_two->OBC_software_ver = all_hk_data->Athena_hk.OBC_software_ver;
    beacon_packet_two->cmds_received = all_hk_data->Athena_hk.cmds_received;
    beacon_packet_two->pckts_uncovered_by_FEC = all_hk_data->Athena_hk.pckts_uncovered_by_FEC;
}

static EPS_reset_TypeDef determine_eps_last_reset_reason(uint32_t reg){

    EPS_reset_TypeDef EPS_last_reset_reason;
    switch (reg) {
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
    return EPS_last_reset_reason;
}
