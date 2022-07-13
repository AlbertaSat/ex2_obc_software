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
 * @file leop.c
 * @author Grace Yi, Thomas Ganley, Robert Taylor
 * @date Oct. 2021
 */

#include "leop.h"
#include "logger/logger.h"

#define TWO_MIN_DELAY pdMS_TO_TICKS(120 * 1000)
#define FOUR_MIN_DELAY pdMS_TO_TICKS(240 * 1000)
#define TWENTY_SEC_DELAY pdMS_TO_TICKS(20 * 1000)

char *deployable_to_str(Deployable_t sw) {
    switch (sw) {
    case DFGM:
        return "DFGM";
    case UHF_P:
        return "UHF_P";
    case UHF_Z:
        return "UHF_Z";
    case UHF_S:
        return "UHF_S";
    case UHF_N:
        return "UHF_N";
    case Port:
        return "PORT";
    case Payload:
        return "PAYLOAD";
    case Starboard:
        return "STARBOARD";
    default:
        return "Unknown";
    }
}

/**
 * @brief
 *      Deploye specific deployable
 * @param sw
 *      Specific Deployable_t to attempt to deploy
 * @param attempts
 *      Number of times to retry deploying the Deployable_t
 * @param deployed_state
 *      Expected switch status after deployment
 */
void deploy(Deployable_t sw, int attempts, int deployed_state) {
    for (int deployment_attempt = 0; deployment_attempt < attempts; deployment_attempt++) {
        sys_log(INFO, "Starting burn %d for %s\n", deployment_attempt, deployable_to_str(sw));
        activate(sw);
    }
    if ((switchstatus(sw) != deployed_state)) {
        sys_log(WARN, "%s does not report deployed\n", deployable_to_str(sw));
    } else {
        sys_log(INFO, "%s reports deployed", deployable_to_str(sw));
    }
}

/**
 * @brief
 *      Deploy all deployable systems
 * @return void
 *      No need for a return, LEOP success is determined by hope
 */
void deploy_all_deployables() {
    int dfgm_switch_deployed_state = 1;
    deploy(DFGM, MAX_RETRIES, dfgm_switch_deployed_state);
    vTaskDelay(TWO_MIN_DELAY);

    int uhf_switch_deployed_state = 1;
    for (Deployable_t sw = UHF_P; sw <= UHF_N; sw++) {
        deploy(sw, MAX_RETRIES, uhf_switch_deployed_state);
    }

    vTaskDelay(FOUR_MIN_DELAY);
#if HYPERION_PANEL_2U == 1
    int hyperion_switch_deployed_state = 0; // flight hardware switched soldered on backwards :)
#else
    int hyperion_switch_deployed_state = 1;
#endif
    for (Deployable_t sw = Port; sw <= Starboard; sw++) {
        deploy(sw, MAX_RETRIES, hyperion_switch_deployed_state);
    }
}

/**
 * @brief
 *      Execute LEOP if it has not been executed before
 * @details
 *      Checks if LEOP sequence has been successfully executed
 *      If not, execute LEOP sequence
 *      Otherwise, skip LEOP sequence
 * @return void
 */
void execute_leop() {
    bool eeprom_flag = false;
    eeprom_flag = eeprom_get_leop_status();
    if (eeprom_flag != true) {
        sys_log(INFO, "Attempting to execute LEOP sequence");
        // If leop sequence was never executed, execute it and hope for the best
        deploy_all_deployables();
        // Set EEPROM flag to true. LEOP has been attempted, it is up to the operators now to verify it.
        eeprom_set_leop_status();
    }
}
