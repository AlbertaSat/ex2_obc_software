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
 * @author Grace Yi
 * @date Oct. 2021
 */

#include "../include/leop.h"

static void *leop_daemon(void *pvParameters);
SAT_returnState start_leop_daemon(void);


Deployable_t sw;

//LEOP Sequence

/**
 * @brief
 *      Check hardswitch status to ensure all deployables 
 *      have been successfully deployed
 * @return bool
 *      Returns TRUE if all deployables have been deployed
 *      Returns FALSE otherwise
 */
bool hard_switch_status() {
    TickType_t seconds_delay = pdMS_TO_TICKS(1000);
    uint32_t two_min_delay = 120;
    uint32_t four_min_delay = 240;
    int getStatus_retries;
    int successful_deployment = 0;
    //sw = {Port, UHF_P, UHF_Z, Payload, UHF_S, UHF_N, Starboard, DFGM};
    for (getStatus_retries = 1; getStatus_retries <= 3; getStatus_retries++) {
        sw = 0;
        //Deploy DFGM
            if (switchstatus(sw) != 1) {
                ex2_log("Check #%d: %c not deployed\n", &getStatus_retries, sw);           
                ex2_log("Manually activated %c\n", sw); 
                activate(sw);
            }
            else if (getStatus_retries == 3 && switchstatus(sw == 1)) {
                successful_deployment++;
            }
            seconds_delay = pdMS_TO_TICKS(two_min_delay * 1000);
            vTaskDelay(seconds_delay);
        //Deploy UHF
        for (sw = 1; sw < 5; sw++) {
            if (switchstatus(sw) != 1) {
                ex2_log("Check #%d: %c not deployed\n", &getStatus_retries, sw);           
                ex2_log("Manually activated %c\n", sw); 
                activate(sw);
            }
            else if (getStatus_retries == 3 && switchstatus(sw == 1)) {
                successful_deployment++;
            }
            seconds_delay = pdMS_TO_TICKS(four_min_delay * 1000);
            vTaskDelay(seconds_delay);
        }
        //Deploy solar panels
        if (successful_deployment = 5) {
            for (sw = 5; sw < 8; sw++) {
                if (switchstatus(sw) != 1) {
                    ex2_log("Check #%d: %c not deployed\n", &getStatus_retries, sw);           
                    ex2_log("Manually activated %c\n", sw); 
                    activate(sw);
                }
                else if (getStatus_retries == 3 && switchstatus(sw == 1)) {
                    successful_deployment++;
                }
            }
        }
    } 
    if (successful_deployment == 8) {
        return true; 
    }
    else {
        return false;
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
void leop_init() {
    if (eeprom_get_leop_status() != true) {
        //If leop sequence was never executed, check that all hard switches have been deployed
        hard_switch_status();
        if (hard_switch_status() == true) {
            //If all hard switch have been deployed, set eeprom flag to TRUE
            eeprom_set_leop_status();
        }
    }
}


/**
 * @brief
 *    leop task
 *
 * @param pvParameters
 *    task parameters (not used)
 */
static void *leop_daemon(void *pvParameters) {
   
    //for (;;) {
        leop_init();
    //}
}

/**
 * @brief
 *      Start LEOP
 * @details
 *      Starts the FreeRTOS task responsible for LEOP
 * @param None
 * @return SAT_returnState
 *      success report
 */
SAT_returnState start_leop_daemon(void) {
  if (xTaskCreate((TaskFunction_t)leop_daemon,
                  "start_leop", 600, NULL, NORMAL_SERVICE_PRIO,
                  NULL) != pdPASS) {
    ex2_log("FAILED TO CREATE TASK start_leop\n");
    return SATR_ERROR;
  }
  ex2_log("leop started\n");
  return SATR_OK;
}