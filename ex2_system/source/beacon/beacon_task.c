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

static void *beacon_daemon(void *pvParameters);
SAT_returnState start_beacon_daemon(void);

static TickType_t beacon_delay = pdMS_TO_TICKS(1000);

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
    // TODO: call the appropriate HAL functions to get the most updated or
    // cached information of the components + state machine, RTC, etc.

    uhf_status = HAL_UHF_setBeaconMsg(beacon_msg);

    /* Sending the beacon */
    // The beacon transmission period is configurable through comms service
    // by the operator or here through HAL_UHF_getBeaconT().
    uint8_t scw[SCW_LEN];
    uhf_status = HAL_UHF_getSCW(scw);
#ifndef UHF_IS_STUBBED
    if (uhf_status == U_GOOD_CONFIG) {
#else
    if (uhf_status == 0) {
#endif
      scw[SCW_BCN_FLAG] = SCW_BCN_ON;
      uhf_status = HAL_UHF_setSCW(scw);
    }
#ifndef UHF_IS_STUBBED
    if (uhf_status != U_GOOD_CONFIG) {
#else
    if (uhf_status != 0) {
#endif
      if (eps_get_pwr_chnl(UHF_PWR_CHNL) == 1 &&
          gioGetBit(UHF_GIO_PORT, UHF_GIO_PIN) == 1) {
        printf("Beacon failed");
      } else
        printf("UHF is off.");
    }

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
