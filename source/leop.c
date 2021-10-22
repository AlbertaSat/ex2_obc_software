/*
 * leop.c
 *
 *  Created on: Oct, 2021
 *      Author: Grace Yi
 */

#include "../include/leop.h"

//LEOP Sequence

//Check hardware switch status
bool hard_switch_status() {
    int status_counter = 1;
    int successful_deployment = 0;
    int i = 0;
    char hard_switches[8] = {Port, UHF_P, UHF_Z, Payload, UHF_S, UHF_N, Starboard, DFGM};
    while (status_counter <= 3) {
        for (i = 0; i < 8; i++) {
            if (deploy(hard_switches[i]) != 1) {
                ex2_log("Check #%d: %c not deployed\n", &status_counter, hard_switches[i]);           
                ex2_log("Manually activated %c\n", hard_switches[i]); 
                deploy(hard_switches[i]);
            }
            else if (status_counter == 3 && deploy(hard_switches[i] == 1)) {
                successful_deployment++;
            }
        }
        status_counter++;
    } 
    if (successful_deployment == 8) {
        return true; 
    }
    else {
        return false;
    }
} 

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