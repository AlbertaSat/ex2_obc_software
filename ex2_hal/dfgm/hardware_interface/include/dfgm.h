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
 * @file
 * @author
 * @date
 */

#ifndef DFGM_H
#define DFGM_H

#include "dfgm_handler.h" // Equipment handler
#include <stdint.h>
#include <time.h>

typedef struct __attribute__((packed)) {
    float coreVoltage;
    float sensorTemp;
    float refTemp;
    float boardTemp;
    float posRailVoltage;
    float inputVoltage;
    float refVoltage;
    float inputCurrent;
    float reserved1;
    float reserved2;
    float reserved3;
    float reserved4;
} DFGM_Housekeeping;

typedef struct __attribute__((packed)) {
    uint8_t filterMode;
    uint32_t startTime;
    uint32_t endTime;
} DFGM_Filter_Settings;

DFGM_return HAL_DFGM_run(int32_t givenRuntime);
DFGM_return HAL_DFGM_stop();
DFGM_return HAL_DFGM_filter(DFGM_Filter_Settings *DFGM_filterSettings);
DFGM_return HAL_DFGM_getHK(DFGM_Housekeeping *DFGM_hk);

#endif /* DFGM_H */
