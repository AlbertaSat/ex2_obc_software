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

void reboot_system(char type) {
    prvRaisePrivilege();

    switch(type) {
    case 'A':
        ex2_log("Rebooting Application");
        break;
    case 'B':
        ex2_log("Rebooting to Bootloader");
        break;
    case 'G':
        ex2_log("Rebooting to Golden Image");
        break;
    default:
        ex2_log("Invalid reboot type");
        break;
    }

    systemREG1->SYSECR = (0x10) << 14;
}

#endif /* INCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS_H_ */
