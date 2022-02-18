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

#include "dfgm.h"

#include <FreeRTOS.h>
#include <csp/csp_endian.h>
#include <os_queue.h>
#include <stdint.h>

#include "services.h"

DFGM_return HAL_DFGM_run(int32_t givenRuntime) {
    DFGM_return status;
#ifndef DFGM_IS_STUBBED
    // If DFGM is connected, run
    int runtime = (int) givenRuntime;
    status = STX_startDFGM(runtime);
#else
    // If DFGM is not connected, can't run
    status = IS_STUBBED_DFGM;
#endif
    return status;
}

DFGM_return HAL_DFGM_stop() {
    DFGM_return status;
#ifndef DFGM_IS_STUBBED
    // DFGM is connected
    status = STX_stopDFGM();
#else
    // DFGM is not connected
    status = IS_STUBBED_DFGM;
#endif
    return status;
}

DFGM_return HAL_DFGM_getHK(DFGM_Housekeeping *DFGM_hk) {
    DFGM_return status;
    dfgm_housekeeping hk;
#ifndef DFGM_IS_STUBBED
    status = STX_getDFGMHK(&hk);
    DFGM_hk->coreVoltage = hk.coreVoltage;
    DFGM_hk->sensorTemp = hk.sensorTemp;
    DFGM_hk->refTemp = hk.refTemp;
    DFGM_hk->boardTemp = hk.boardTemp;
    DFGM_hk->posRailVoltage = hk.posRailVoltage;
    DFGM_hk->inputVoltage = hk.inputVoltage;
    DFGM_hk->refVoltage = hk.refVoltage;
    DFGM_hk->inputCurrent = hk.inputCurrent;
    DFGM_hk->reserved1 = hk.reserved1;
    DFGM_hk->reserved2 = hk.reserved2;
    DFGM_hk->reserved3 = hk.reserved3;
    DFGM_hk->reserved4 = hk.reserved4;
#else
    status = IS_STUBBED_DFGM;
    // Default values to signify DFGM is not connected
    DFGM_hk->coreVoltage = 1;
    DFGM_hk->sensorTemp = 1;
    DFGM_hk->refTemp = 1;
    DFGM_hk->boardTemp = 1;
    DFGM_hk->posRailVoltage = 1;
    DFGM_hk->inputVoltage = 1;
    DFGM_hk->refVoltage = 1;
    DFGM_hk->inputCurrent = 1;
    DFGM_hk->reserved1 = 1;
    DFGM_hk->reserved2 = 1;
    DFGM_hk->reserved3 = 1;
    DFGM_hk->reserved4 = 1;
#endif
    return status;
}
