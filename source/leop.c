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

#include "leop.h"

static void *leop_daemon(void *pvParameters);
SAT_returnState start_leop_daemon(void);


Deployable_t sw;

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
    //TODO: uncomment ex2_log and vTaskDelay, they are commented out for testing purposes since Cgreen cannot recognize them
    TickType_t two_min_delay = pdMS_TO_TICKS(120 * 1000);
    TickType_t four_min_delay = pdMS_TO_TICKS(240 * 1000);
    TickType_t twenty_sec_delay = pdMS_TO_TICKS(20 * 1000);
    int getStatus_retries;
    int successful_deployment = 0;
    //sw = {Port, UHF_P, UHF_Z, Payload, UHF_S, UHF_N, Starboard, DFGM};
    for (getStatus_retries = 0; getStatus_retries <= MAX_RETRIES; getStatus_retries++) {
        sw = 0;
        //Deploy DFGM
        if (switchstatus(sw) != 1 && getStatus_retries != MAX_RETRIES) {
            //ex2_log("Check #%d: %c not deployed\n", &getStatus_retries, sw);           
            //ex2_log("Manually activated %c\n", sw);
            activate(sw);
            //vTaskDelay(twenty_sec_delay);
        }
        else if (getStatus_retries == MAX_RETRIES) {
            //ex2_log("Check #%d: %c not deployed, exiting the LEOP sequence.\n", &getStatus_retries, sw);
            return false;
        }
    }
    //vTaskDelay(two_min_delay);
    for (getStatus_retries = 0; getStatus_retries <= MAX_RETRIES; getStatus_retries++) {
        //Deploy UHF
        for (sw = 1; sw < 5; sw++) {
            if (switchstatus(sw) != 1 && getStatus_retries != MAX_RETRIES) {
                //ex2_log("Check #%d: %c not deployed\n", &getStatus_retries, sw);           
                //ex2_log("Manually activated %c\n", sw); 
                activate(sw);
                //vTaskDelay(twenty_sec_delay);
            }
            else if (getStatus_retries == MAX_RETRIES) {
                //ex2_log("Check #%d: %c not deployed, exiting the LEOP sequence.\n", &getStatus_retries, sw);
                return false;
            }
            
        }
    }
    //vTaskDelay(four_min_delay);
    for (getStatus_retries = 0; getStatus_retries <= MAX_RETRIES; getStatus_retries++) {
        //Deploy solar panels
        for (sw = 5; sw < 8; sw++) {
            if (switchstatus(sw) != 1 && getStatus_retries != MAX_RETRIES) {
                //ex2_log("Check #%d: %c not deployed\n", &getStatus_retries, sw);           
                //ex2_log("Manually activated %c\n", sw); 
                activate(sw);
                //vTaskDelay(twenty_sec_delay);
            }
            else if (getStatus_retries == MAX_RETRIES) {
                //ex2_log("Check #%d: %c not deployed, exiting the LEOP sequence.\n", &getStatus_retries, sw);
                return false;
            }
        }
    }
    
    /*if (switchstatus(0) & switchstatus(1) & switchstatus(2) & switchstatus(3) & switchstatus(4) & switchstatus(5) & switchstatus(6) & switchstatus(7) == 1) {
        return true; 
    }
    else {
        return false;
    }*/
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
bool leop_init() {
    if (eeprom_get_leop_status() != true) {
        //If leop sequence was never executed, check that all hard switches have been deployed
        if (hard_switch_status() == true) {
            //If all hard switch have been deployed, set eeprom flag to TRUE
            eeprom_set_leop_status();
            return true;
        }
        return false;
    }
    else if (eeprom_get_leop_status() == true) {
        return true;
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