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
 * @author Andrew R. Rooney, Grace Yi, Thomas Ganley
 * @date Mar. 6, 2021
 */

#include "beacon_task.h"
#include <stdbool.h>

#define SCW_BCN_ON 1
#define BEACON_PACKET_LENGTH 97

static bool beacon_task_enabled = true; /* Beacon starts enabled! */

#if UHF_IS_STUBBED == 0
static void *beacon_daemon();
SAT_returnState start_beacon_daemon();

/**
 * Construct and send out the system beacon at the required frequency.
 *
 * @param pvParameters
 *    task parameters (not used)
 */
static void *beacon_daemon() {

    while(1){

        if(!beacon_task_enabled){
            vTaskDelay(5 * ONE_SECOND);
            continue;
        }

        /* Main beacon loop to update beacon contents with latest housekeeping data */
        while(1) {

            /* Fetch most recent housekeeping */
            All_systems_housekeeping all_hk_data;
            uint16_t max_files = get_max_files();
            Result err = load_historic_hk_data(max_files, &all_hk_data);

            /* Constructing the beacon content */
            beacon_packet_1_t beacon_packet_one;
            beacon_packet_2_t beacon_packet_two;

            update_beacon(&all_hk_data, &beacon_packet_one, &beacon_packet_two);

            /* Get the beacon period from the UHF so that we know how long to wait */
            uint32_t beacon_t_s;
            UHF_return uhf_status = HAL_UHF_getBeaconT(&beacon_t_s);
            if(uhf_status != U_GOOD_CONFIG){
                vTaskDelay(20 * ONE_SECOND);
                continue;
            }

            /* Set first beacon packet */
            UHF_configStruct beacon_msg;
            memcpy(&(beacon_msg.message), &beacon_packet_one, sizeof(beacon_packet_1_t));
            beacon_msg.len = sizeof(beacon_packet_1_t);
            uhf_status = HAL_UHF_setBeaconMsg(beacon_msg);
            if(uhf_status != U_GOOD_CONFIG){
                vTaskDelay(20 * ONE_SECOND);
                continue;
            }

            /* Wait for UHF to send first beacon */
            vTaskDelay(pdMS_TO_TICKS(beacon_t_s * 1000));

            /* Set the second beacon packet */
            memcpy(&(beacon_msg.message), &beacon_packet_two, sizeof(beacon_packet_2_t));
            beacon_msg.len = sizeof(beacon_packet_2_t);
            uhf_status = HAL_UHF_setBeaconMsg(beacon_msg);
            if(uhf_status != U_GOOD_CONFIG){
                vTaskDelay(20 * ONE_SECOND);
                continue;
            }

            /* Wait for UHF to send second beacon */
            vTaskDelay(pdMS_TO_TICKS(beacon_t_s * 1000));
        }

    }

}
#endif

UHF_return enable_beacon_task(void){

    /* Enable the beacon on the UHF */

    /* Read the status control word (SCW) */
    uint8_t scw[SCW_LEN];
    UHF_return uhf_status = HAL_UHF_getSCW(scw);
    if(uhf_status != U_GOOD_CONFIG){
        sys_log(ERROR, "Error %d to start UHF beacon, could not read SCW\n", uhf_status);
        return uhf_status;
    }

    /* Only change the beacon flag and set the SCW */
    scw[UHF_SCW_BCN_INDEX] = UHF_BCN_ON;
    uhf_status = HAL_UHF_setSCW(scw);
    if(uhf_status != U_GOOD_CONFIG){
        sys_log(ERROR, "Error %d to start UHF beacon, could not write SCW\n", uhf_status);
        return uhf_status;
    }

    beacon_task_enabled = true;
    return U_GOOD_CONFIG;
}

UHF_return disable_beacon_task(void){
    beacon_task_enabled = false;

    /* Disable the beacon on the UHF */

    /* Read the status control word (SCW) */
    uint8_t scw[SCW_LEN];
    UHF_return uhf_status = HAL_UHF_getSCW(scw);

    /* Only change the beacon flag and set the SCW */
    scw[UHF_SCW_BCN_INDEX] = UHF_BCN_OFF;

    for(int attempts = 5; attempts > 0; attempts--){
        uhf_status = HAL_UHF_setSCW(scw);
        if(uhf_status == U_GOOD_CONFIG){
            break;
        }
    }
    if(uhf_status != U_GOOD_CONFIG){
        sys_log(ERROR, "Unable to stop UHF beacon, could not write SCW\n");
        return U_BAD_CONFIG;
    }
    return U_GOOD_CONFIG;
}

bool beacon_task_get_state(void){
    return beacon_task_enabled;
}

/**
 * Start the beacon daemon
 *
 * @returns status
 *   error report of task creation
 */
SAT_returnState start_beacon_daemon(void) {
#if UHF_IS_STUBBED == 0
    if (xTaskCreate((TaskFunction_t)beacon_daemon, "beacon_daemon", 1024, NULL, BEACON_TASK_PRIO, NULL) !=
        pdPASS) {
        sys_log(CRITICAL, "FAILED TO CREATE TASK beacon_daemon\n");
        return SATR_ERROR;
    }

    if(enable_beacon_task() != U_GOOD_CONFIG){
        sys_log(CRITICAL, "FAILED to enable beacon\n");
        return SATR_ERROR;
    }

    sys_log(INFO, "Beacon daemon started\n");
    return SATR_OK;
#else
    return SATR_ERROR;
#endif
}
