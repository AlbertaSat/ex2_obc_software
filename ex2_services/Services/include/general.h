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
 * general.h
 *
 *  Created on: May 14, 2021
 *      Author: Robert Taylor
 */

#ifndef EX2_SERVICES_GENERAL
#define EX2_SERVICES_GENERAL
#include "system.h"

SAT_returnState start_general_service(void);

typedef enum {
    REBOOT = 0,
    DEPLOY_DEPLOYABLES = 1,
    GET_SWITCH_STATUS = 2,
    GET_UHF_WATCHDOG_TIMEOUT = 3,
    SET_UHF_WATCHDOG_TIMEOUT = 4,
    GET_SBAND_WATCHDOG_TIMEOUT = 5,
    SET_SBAND_WATCHDOG_TIMEOUT = 6,
    GET_CHARON_WATCHDOG_TIMEOUT = 7,
    SET_CHARON_WATCHDOG_TIMEOUT = 8,
    GET_ADCS_WATCHDOG_TIMEOUT = 9,
    SET_ADCS_WATCHDOG_TIMEOUT = 10,

    GET_PAYLOAD_WATCHDOG_TIMEOUT = 12,
    SET_PAYLOAD_WATCHDOG_TIMEOUT = 13,
    ENABLE_BEACON_TASK = 14,
    DISABLE_BEACON_TASK = 15,
    BEACON_TASK_GET_STATE = 16,
    GET_SOLAR_SWITCH_STATUS = 17,
    SET_SOLAR_SWITCH = 18
} General_Subtype;

typedef enum { bootloader = 'B', golden = 'G', application = 'A' } reboot_mode;

#endif /* EX2_SERVICES_GENERAL_ */
