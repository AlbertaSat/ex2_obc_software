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

#ifndef IRIS_SERVICE_H
#define IRIS_SERVICE_H

#include <stdint.h>
#include <csp_types.h>
#include "services.h"

SAT_returnState start_iris_service(void);

SAT_returnState iris_service_app(csp_packet_t *pkt);

#define IRIS_SERVICE_IMAGE_TRANSFER_DELAY vTaskDelay(pdMS_TO_TICKS(1000))

typedef enum {
    IRIS_POWER_ON = 0,
    IRIS_POWER_OFF = 1,
    IRIS_TURN_ON_IMAGE_SENSORS = 2,
    IRIS_TURN_OFF_IMAGE_SENSORS = 3,
    IRIS_TAKE_IMAGE = 4,
    IRIS_DELIVER_IMAGE = 5,
    IRIS_COUNT_IMAGES = 6,
    IRIS_PROGRAM_FLASH = 7,
    IRIS_GET_HK = 8,
    IRIS_DELIVER_LOG = 9,
    IRIS_SET_TIME = 10,
    IRIS_SET_CONFIG = 11,
} IRIS_Subtype;

#endif /* IRIS_SERVICE_H */
