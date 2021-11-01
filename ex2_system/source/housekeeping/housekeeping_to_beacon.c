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
#include "housekeeping/housekeeping_to_beacon.h"
#include "ex2_services/Services/include/housekeeping/housekeeping_service.h" //contains all housekeeping data
#include "rtcmk.h" //to get time from RTC
#include "services.h"
#include "ex2_system/include/beacon/beacon_task.h" //contains beacon packets (ie. a summary of housekeeping data)

/**
 * @brief
 *      Updates the beacon packet with the latest housekeeping data
 * @param all_hk_data
 *      The pointer to where housekeeping data is kept
 */

beacon_t beacon_packet;
int i;

/* Updates Beacon Packet with the latest housekeeping data*/
void update_beacon(All_systems_housekeeping* all_hk_data) {
  //get the unix time from RTC, and convert it to a struct using RTCMK_GetUnix
  // RTCMK_GetUnix(&(beacon_packet.time));
  memcpy(&(beacon_packet.time), &(all_hk_data->hk_timeorder.UNIXtimestamp), sizeof(beacon_packet.time));

  //Populate the beacon packet by fetching relevant data from housekeeping_services.c
 
  /*-------EPS-------*/
  memcpy(&(beacon_packet.eps_mode), &(all_hk_data->EPS_hk.battMode), sizeof(beacon_packet.eps_mode));
  memcpy(&(beacon_packet.battery_voltage), &(all_hk_data->EPS_hk.vBatt), sizeof(beacon_packet.battery_voltage));
  memcpy(&(beacon_packet.battery_input_current), &(all_hk_data->EPS_hk.curBattIn), sizeof(beacon_packet.battery_input_current));
  for (i = 0; i < 10; i++) {
    memcpy(&(beacon_packet.current_channels[i]), &(all_hk_data->EPS_hk.curOutput[i]), sizeof(uint16_t));
  }  
  memcpy(&(beacon_packet.output_states), &(all_hk_data->EPS_hk.outputStatus), sizeof(uint32_t));
  for (i = 0; i < 10; i++) {
    memcpy(&(beacon_packet.output_faults[0]), &(all_hk_data->EPS_hk.outputFaultCnt[i]), sizeof(uint8_t));
  }  
  memcpy(&(beacon_packet.EPS_boot_count), &(all_hk_data->EPS_hk.bootCnt), sizeof(beacon_packet.EPS_boot_count));
  //TEST: eps_last_reset_reason did not exist, define the function based on EPS ICD manual
  memcpy(&(beacon_packet.eps_last_reset_reason), &(all_hk_data->EPS_startup_hk.last_reset_reason_reg), sizeof(beacon_packet.eps_last_reset_reason));
  
  /*-------Watchdog-------*/
  //TODO: More work to be done on watchdogs
  memcpy(&(beacon_packet.gs_wdt), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.gs_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  
  /*-------Temperatures-------*/
  //Onboard battery pack temp location defined in ICD
  memcpy(&(beacon_packet.temps[0]), &(all_hk_data->EPS_hk.temp[8]), sizeof(uint8_t));
  //TODO: Athena only has 2 board temperatures, rest of Athena housekeeping TBD
  memcpy(&(beacon_packet.temps[1]), &(all_hk_data->Athena_hk.temparray[0]), sizeof(uint8_t));
  memcpy(&(beacon_packet.temps[2]), &(all_hk_data->Athena_hk.temparray[1]), sizeof(uint8_t));
  //TODO: find out where payload temp is (write a function/preprocesor to determine 2U or 3U)
  memcpy(&(beacon_packet.temps[3]), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.temps[4]), &(all_hk_data->hyperion_hk.Nadir_Temp1), sizeof(uint8_t));
  memcpy(&(beacon_packet.temps[5]), &(all_hk_data->hyperion_hk.Zenith_Temp1), sizeof(uint8_t));
  memcpy(&(beacon_packet.temps[6]), &(all_hk_data->hyperion_hk.Port_Temp1), sizeof(uint8_t));
  memcpy(&(beacon_packet.temps[7]), &(all_hk_data->hyperion_hk.Star_Temp1), sizeof(uint8_t));

  /*-------ADCS-------*/
  //Of the x, y, and z angular rates, the maximum angular rate is used
  uint8_t max_rate = max(all_hk_data->adcs_hk.Estimated_Angular_Rate_X, all_hk_data->adcs_hk.Estimated_Angular_Rate_Y);
  max_rate = max(max_rate, all_hk_data->adcs_hk.Estimated_Angular_Rate_Z);
  memcpy(&(beacon_packet.angular_rate), &max_rate, sizeof(uint8_t));
  //TODO: write function to get ADCS control mode
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));

  /*-------UHF-------*/
  memcpy(&(beacon_packet.uhf_uptime), &(all_hk_data->UHF_hk.uptime), sizeof(uint16_t));

  /*-------Payload-------*/
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));

  /*-------OBC-------*/
  //TODO: write the function for OBC/athena housekeeping
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));

  /*-------Logged Items-------*/
  //TODO: write the function for logged items in beacon_task
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));
  memcpy(&(beacon_packet.obc_wdt_expr), &(all_hk_data->EPS_hk.curBattIn), sizeof(uint8_t));

}
