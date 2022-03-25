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
    GET_UHF_WATCHDOG_TIMEOUT = 1,
    SET_UHF_WATCHDOG_TIMEOUT = 2,
} General_Subtype;

typedef enum { bootloader = 'B', golden = 'G', application = 'A' } reboot_mode;

#endif /* EX2_SERVICES_GENERAL_ */
