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
/**
 * @file dfgm.c
 * @author Daniel Sacro
 * @date 2022-02-08
 */

#include "dfgm.h"

#include <FreeRTOS.h>
#include <csp/csp_endian.h>
#include <os_queue.h>
#include <stdint.h>

#include "services.h"

/**
 * @brief
 *      Tells the DFGM Rx Task to process data for a set runtime
 * @details
 *      Gives the specified runtime to the DFGM Rx Task via an EH function
 * @param int32_t givenRuntime
 *      The total amount of time that the DFGM Rx Task should be processing data for
 *      in seconds
 * @return DFGM_return
 *      Success report of the EH function
 */
DFGM_return HAL_DFGM_run(int32_t givenRuntime) {
    DFGM_return status;
#ifndef DFGM_IS_STUBBED
    // DFGM connected to OBC
    int runtime = (int) givenRuntime;
    status = DFGM_startDataCollection(runtime);
#else
    // DFGM not connected
    status = IS_STUBBED_DFGM;
#endif
    return status;
}

/**
 * @brief
 *      Tells the DFGM Rx Task to stop processing data
 * @details
 *      Calls an EH function to reset the DFGM Rx Task's counters and flags to its default values
 * @param None
 * @return DFGM_return
 *      Success report of the EH function
 */
DFGM_return HAL_DFGM_stop() {
    DFGM_return status;
#ifndef DFGM_IS_STUBBED
    status = DFGM_stopDataCollection();
#else
    status = IS_STUBBED_DFGM;
#endif
    return status;
}

/**
 * @brief
 *      Gets the DFGM's most recent housekeeping (HK) data
 * @details
 *      Retrieves the most recent DFGM HK data. This data will be at most 3 minutes old.
 * @param DFGM_Housekeeping *DFGM_hk
 *      A DFGM_Housekeeping struct that will be populated by the most recent DFGM HK data.
 * @return DFGM_return
 *      Success report of the EH function
 */
DFGM_return HAL_DFGM_get_HK(DFGM_Housekeeping *DFGM_hk) {
    DFGM_return status;
    dfgm_housekeeping hk;
#ifndef DFGM_IS_STUBBED
    status = DFGM_get_HK(&hk);
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
#endif
    return status;
}
