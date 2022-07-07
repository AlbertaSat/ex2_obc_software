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

    int getStatus_retries;
    uint16_t burnwire_currents[8] = {0};

    for (getStatus_retries = 0; getStatus_retries <= MAX_RETRIES; getStatus_retries++) {
        sw = DFGM;
        // Deploy DFGM
        if ((switchstatus(sw) != 1) && (getStatus_retries != MAX_RETRIES)) {
            ex2_log("Check #%d: %c not deployed\n", &getStatus_retries, sw);
            ex2_log("Activated %c\n", sw);
            activate(sw);
            vTaskDelay(TWENTY_SEC_DELAY);
        } else if ((switchstatus(sw) != 1) && (getStatus_retries == MAX_RETRIES)) {
            ex2_log("Check #%d: %c not deployed, exiting the LEOP sequence.\n", &getStatus_retries, sw);
            return false;
        }
    }
    ex2_log("DFGM deployed, burnwire current = %d\n", burnwire_currents[0]);
    vTaskDelay(TWO_MIN_DELAY);

    for (getStatus_retries = 0; getStatus_retries <= MAX_RETRIES; getStatus_retries++) {
        // Deploy UHF
        for (sw = UHF_P; sw <= UHF_N; sw++) {
            if ((switchstatus(sw) != 1) && (getStatus_retries != MAX_RETRIES)) {
                ex2_log("Check #%d: %c not deployed\n", &getStatus_retries, sw);
                ex2_log("Activated %c\n", sw);
                activate(sw);
                vTaskDelay(TWENTY_SEC_DELAY);
            } else if ((switchstatus(sw) != 1) && (getStatus_retries == MAX_RETRIES)) {
                ex2_log("Check #%d: %c not deployed, exiting the LEOP sequence.\n", &getStatus_retries, sw);
                return false;
            }
        }
    }
    ex2_log("UHF Port deployed, burnwire current = %d\n", burnwire_currents[1]);
    ex2_log("UHF Zenith deployed, burnwire current = %d\n", burnwire_currents[2]);
    ex2_log("UHF Starboard deployed, burnwire current = %d\n", burnwire_currents[3]);
    ex2_log("UHF Nadir deployed, burnwire current = %d\n", burnwire_currents[4]);

    vTaskDelay(FOUR_MIN_DELAY);

    for (getStatus_retries = 0; getStatus_retries <= MAX_RETRIES; getStatus_retries++) {
        // Deploy solar panels
#if HYPERION_PANEL_2U == 1
        bool deployed_state = 0; // flight hardware switched soldered on backwards :)
#else
        bool deployed_state = 1;
#endif
        for (sw = Port; sw <= Starboard; sw++) {
            if ((switchstatus(sw) != deployed_state) && (getStatus_retries != MAX_RETRIES)) {
                ex2_log("Check #%d: %c not deployed\n", &getStatus_retries, sw);
                ex2_log("Activated %c\n", sw);
                activate(sw);
                vTaskDelay(TWENTY_SEC_DELAY);
            } else if ((switchstatus(sw) != deployed_state) && (getStatus_retries == MAX_RETRIES)) {
                ex2_log("Check #%d: %c not deployed, exiting the LEOP sequence.\n", &getStatus_retries, sw);
                return false;
            }
        }
        ex2_log("Port Deployable Panel deployed, burnwire current = %d\n", burnwire_currents[5]);
        ex2_log("Deployable Payload deployed, burnwire current = %d\n", burnwire_currents[6]);
        ex2_log("Starboard Deployable deployed, burnwire current = %d\n", burnwire_currents[7]);
    }
    return true;
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
    // TODO: When eeprom is working, use commended code
    bool eeprom_flag = false;
    eeprom_flag = eeprom_get_leop_status();
    if (eeprom_flag != true) {
        // If leop sequence was never executed, check that all hard switches have been deployed
        if (deploy_all_deployables() == true) {
            // If all hard switch have been deployed, set eeprom flag to TRUE
            eeprom_set_leop_status();
            return true;
        }
        return false;
    }
    //    else if (eeprom_get_leop_status() == true) {
    else if (eeprom_flag == true) {
        return true;
    }
}
