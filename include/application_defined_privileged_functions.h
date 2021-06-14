/*
 * Copyright (C) 2021  University of Alberta
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

/*
 * application_defined_privileged_functions.h
 *
 *  Created on: May 14, 2021
 *      Author: Robert Taylor
 */

#ifndef APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS_H_
#define APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS_H_

#include "FreeRTOS.h"
#include "HL_reg_system.h"
#include "bl_eeprom.h"


void reboot_system(char type) {
    RAISE_PRIVILEGE;
    eeprom_init();
    switch(type) {
    case 'A':
        eeprom_set_boot_type('A');
        ex2_log("Rebooting Application");
        break;
    case 'B':
        eeprom_set_boot_type('B');
        ex2_log("Rebooting to Bootloader");
        break;
    case 'G':
        eeprom_set_boot_type('G');
        ex2_log("Rebooting to Golden Image");
        break;
    default:
        eeprom_shutdown();
        ex2_log("Invalid reboot type");
        return;
    }
    eeprom_shutdown();
    systemREG1->SYSECR = (0x10) << 14;
    RESET_PRIVILEGE;
}

bool init_eeprom() {
    RAISE_PRIVILEGE;
    bool success = eeprom_init();
    RESET_PRIVILEGE;
    return success;
}

void shutdown_eeprom() {
    RAISE_PRIVILEGE;
    eeprom_shutdown();
    RESET_PRIVILEGE;
}

image_info priv_eeprom_get_app_info() {
    RAISE_PRIVILEGE;
    image_info app_info = eeprom_get_app_info();
    RESET_PRIVILEGE;
    return app_info;
}

image_info priv_eeprom_get_golden_info() {
    RAISE_PRIVILEGE;
    image_info app_info = eeprom_get_golden_info();
    RESET_PRIVILEGE;
    return app_info;
}

void priv_eeprom_set_app_info(image_info app_info) {
    RAISE_PRIVILEGE;
    eeprom_set_app_info(app_info);
    RESET_PRIVILEGE;
}

void priv_eeprom_set_golden_info(image_info app_info) {
    RAISE_PRIVILEGE;
    eeprom_set_golden_info(app_info);
    RESET_PRIVILEGE;
}


#endif /* INCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS_H_ */
