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
 * @file    housekeeping_athena.c
 * @author  Dustin Wagner
 * @date    2021-05-13
 */

#include "housekeeping_athena.h"
#include <stdlib.h>

/**
 * @brief
 *      Getter to supply data from temperature sensors on athena
 * 
 * @param temparray
 * 		  the array to store temperature readings
 * @return
 * 		0 for success. other for failure
 */
int HAL_get_temp_all(long* temparray) {
  #ifdef ATHENA_IS_STUBBED
        return 0;
    #else
        return gettemp_all(temparray);
    #endif
}

/**
 * @brief
 *      Getter to Athena uptime
 * @return
 * 		  integer OBC_uptime to store Athena uptime
 *      Seconds = OBC_uptime*10 
 *      Max = 655350 seconds (7.6 days)
 */
uint16_t Athena_get_OBC_uptime() {
  TickType_t OBC_ticks = xTaskGetTickCount(); //1 tick/ms
  uint32_t OBC_uptime_s = (OBC_ticks * portTICK_PERIOD_MS) / 1000;
  
  //convert OBC_uptime from 32 bit to 16 bit. Max = 655350 seconds (7.6 days)
  //Unit will be deca-seconds due to size restraint, seconds = OBC_uptime*10
  return (uint16_t) OBC_uptime_s/10;
}

/**
 * @brief
 *      Getter to Athena solar panel supply current
 * @return
 * 		  integer solar_panel_supply_curr to store solar panel supply current
 */ 
uint8_t Athena_get_solar_supply_curr() {
  //insert getter function for solar panel supply current;
}

/**
 * @brief
 * 		Athena composition housekeeping getter
 * @details
 * 		Contains calls to other functions that each return a portion of Athena
 *    telemetry data. Generally reports back to housekeeping_service.
 * @attention
 * 		New components should follow the structure of the existing components.
 *    All data needs to be deep copied
 * @param athena_hk
 * 		Pointer to struct of athena housekeeping data
 * @return
 * 		Last found error will be returned. else no error returned
 */
int Athena_getHK(athena_housekeeping* athena_hk) {
  int temporary;
  int return_code = 0;

  /*Add athena HAL housekeeping getters here and put fields in h file
  create HAL functions here following format of existing
  also add endianness conversion in Athena_hk_convert_endianness*/
  temporary = HAL_get_temp_all(&athena_hk->temparray);

  //Get OBC uptime: Seconds = value*10. Max = 655360 seconds (7.6 days)
  athena_hk->OBC_uptime = Athena_get_OBC_uptime();

  //Get solar panel supply current
  athena_hk->solar_panel_supply_curr = Athena_get_solar_supply_curr();
 
  if (temporary != 0) return_code = temporary;

  return return_code;
}

/**
 * @brief
 *      Converts endianness of values in athena_housekeeping struct
 * 
 * @param athena_hk
 * 		  struct of athena housekeeping data
 * @return
 * 		0 for success. other for failure
 */
int Athena_hk_convert_endianness(athena_housekeeping* athena_hk) {
  uint8_t i;
  for (i = 0; i < NUM_TEMP_SENSOR; i++) {
    athena_hk->temparray[i] = (long)csp_ntoh32((uint32_t)athena_hk->temparray[i]);
  }
  athena_hk->boot_cnt = csp_ntoh16(athena_hk->boot_cnt);
  athena_hk->last_reset_reason = csp_ntoh8(athena_hk->last_reset_reason);
  athena_hk->OBC_mode = csp_ntoh8(athena_hk->OBC_mode);
  athena_hk->OBC_uptime = csp_ntoh16(athena_hk->OBC_uptime);
  athena_hk->solar_panel_supply_curr = csp_ntoh8(athena_hk->solar_panel_supply_curr);
  athena_hk->OBC_software_ver = csp_ntoh8(athena_hk->OBC_software_ver);
  athena_hk->cmds_received = csp_ntoh16(athena_hk->cmds_received);
  athena_hk->pckts_uncovered_by_FEC = csp_ntoh16(athena_hk->pckts_uncovered_by_FEC);
  return 0;
}

