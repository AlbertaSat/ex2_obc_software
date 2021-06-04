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
#include "bl_eeprom.h"


void reboot_system(char type) {
    prvRaisePrivilege();
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
}

#endif /* INCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS_H_ */
