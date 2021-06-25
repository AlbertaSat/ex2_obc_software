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
 * updater.h
 *
 *  Created on: Jun. 4, 2021
 *      Author: Robert Taylor
 */

#ifndef UPDATER_H_
#define UPDATER_H_

#include "services.h"

SAT_returnState start_updater_service(void);

typedef enum {
    FLASH_UPDATE,
    GET_GOLDEN_INFO,
    GET_APP_INFO,
    SET_GOLDEN_INFO,
    SET_APP_INFO,
    VERIFY_GOLDEN_IMAGE,
    VERIFY_APPLICATION_IMAGE
} updater_subtype;  // shared with EPS!

#endif /* UPDATER_H_ */
