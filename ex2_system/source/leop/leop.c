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
 * @author Grace Yi, Thomas Ganley, Robert Taylor, Josh Lazaruk
 * @date Jul. 2022
 */

#include "leop.h"
#include "logger/logger.h"

#define TWO_MIN_DELAY pdMS_TO_TICKS(120 * 1000)
#define FOUR_MIN_DELAY pdMS_TO_TICKS(240 * 1000)
#define TWENTY_SEC_DELAY pdMS_TO_TICKS(20 * 1000)
#define FIVE_SEC_DELAY pdMS_TO_TICKS(5 * 1000)

// LEOP Sequence

/**
 * @brief
 *      Deploy all deployable systems
 * @return bool
 *      Returns TRUE if all deployables have been deployed
 *      Returns FALSE otherwise
 */

bool deploy_all_deployables() {
    Deployable_t sw;

    int deployment_attempt;
    uint16_t burnwire_currents[8] = {0};

    for (deployment_attempt = 1; deployment_attempt <= MAX_ATTEMPTS; deployment_attempt++) {
        sw = DFGM;
        // Deploy DFGM
        ex2_log("Activated burnwire %c\n", sw);
        activate(sw);
        vTaskDelay(TWENTY_SEC_DELAY);
    }
    ex2_log("DFGM deployment attempts complete\n");

    vTaskDelay(TWO_MIN_DELAY);

    for (deployment_attempt = 1; deployment_attempt <= MAX_ATTEMPTS; deployment_attempt++) {
        // Deploy UHF
        for (sw = UHF_P; sw <= UHF_N; sw++) {
            if (deployment_attempt != MAX_ATTEMPTS) {
                ex2_log("Activated burnwire %c\n", sw);
                activate(sw);
                vTaskDelay(FIVE_SEC_DELAY);
            }
        }
    }
    ex2_log("UHF deployment attempts complete\n");

    vTaskDelay(FOUR_MIN_DELAY);
    return true;
}

/**
 * @brief
 *      Log all switch statuses after LEOP
 * @details
 *      Logs all switch statuses after LEOP
 * @return void
 */
void log_switch_status() {
    bool switch_val = 0;
    for (Deployable_t sw = DFGM; sw <= Starboard; sw++) {
        switch_val = switchstatus(sw);
        switch (sw) {
        case DFGM:
            ex2_log("DFGM switch status = %d\n", switch_val);
            break;
        case UHF_P:
            ex2_log("UHF_P switch status = %d\n", switch_val);
            break;
        case UHF_Z:
            ex2_log("UHF_Z switch status = %d\n", switch_val);
            break;
        case UHF_S:
            ex2_log("UHF_S switch status = %d\n", switch_val);
            break;
        case UHF_N:
            ex2_log("UHF_N switch status = %d\n", switch_val);
            break;
        case Port:
            ex2_log("Port switch status = %d\n", switch_val);
            break;
        case Payload:
            ex2_log("Payload switch status = %d\n", switch_val);
            break;
        case Starboard:
            ex2_log("Starboard switch status = %d\n", switch_val);
            break;
        }
    }
}

/**
 * @brief
 *      Set an eeprom flag so LEOP only gets executed once
 * @details
 *      Checks if LEOP sequence has been successfully executed
 *      If not, execute LEOP sequence
 *      Otherwise, skip LEOP sequence
 * @return void
 */
bool execute_leop() {
    bool eeprom_flag = false;
    eeprom_flag = eeprom_get_leop_status();
    if (eeprom_flag != true) {
        // If leop sequence was never executed, doit
        deploy_all_deployables();
        log_switch_status();
        // After leop attempt, set eeprom flag to TRUE
        eeprom_set_leop_status(true);
        return true;
    } else if (eeprom_flag == true) {
        return true;
    }
}
