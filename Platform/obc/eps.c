/*
 * Copyright (C) 2020  University of Alberta
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
 * @file eps.c
 * @author Andrew Rooney
 * @date 2020-12-28
 */

/*
 * This code needs to:
 *  - send requests to the EPS over the network as per the NanoAvionics specs
 *  - provide an API around those calls for the on-board applications to use
 */

#include <FreeRTOS.h>
#include <task.h>
#include <csp/csp.h>

#include "eps.h"
#include "system.h"

SAT_returnState eps_get_instantaneous_telemetery(eps_instantaneous_telemetry_t *telembuf) {
    uint8_t cmd = 0;
    if (telembuf == NULL) {
        return SATR_ERROR;
    }
    csp_transaction_w_opts(CSP_PRIO_LOW, (TC_TM_app_id) EPS_APP_ID,
                           EPS_INSTANTANEOUS_TELEMETRY, EPS_REQUEST_TIMEOUT,
                           &cmd, sizeof(cmd), telembuf, sizeof(eps_instantaneous_telemetry_t),
                           CSP_O_CRC32);
    // data is little endian, must convert to host order
    // refer to the NanoAvionics datasheet for details
    instantaneous_telemetry_letoh(telembuf);
    return SATR_OK;
}

static inline void instantaneous_telemetry_letoh (eps_instantaneous_telemetry_t *telembuf) {
    uint8_t i;
    for (i = 0; i < 2; i++) {
        telembuf->AOcurOutput[i] = csp_letoh16(telembuf->AOcurOutput[i]);
    }
    for (i = 0; i < 4; i++) {
        telembuf->mpptConverterVoltage[i] = csp_letoh16(telembuf->mpptConverterVoltage[i]);
        telembuf->OutputConverterVoltage[i] = csp_letoh16(telembuf->OutputConverterVoltage[i]);
    }
    for (i = 0; i < 8; i++) {
        telembuf->curSolarPanels[i] = csp_letoh16(telembuf->curSolarPanels[i]);
    }
    for (i = 0; i < 10; i++) {
        telembuf->curOutput[i] = csp_letoh16(telembuf->curOutput[i]);
        telembuf->outputOnDelta[i] = csp_letoh16(telembuf->outputOnDelta[i]);
        telembuf->outputOffDelta[i] = csp_letoh16(telembuf->outputOffDelta[i]);
        telembuf->outputFaultCnt[i] = csp_letoh32(telembuf->outputFaultCnt[i]);
    }
    for (i = 0; i < 12; i++) {
        telembuf->temp[i] = csp_letoh16(telembuf->temp[i]);
    }
    telembuf->vBatt = csp_letoh16(telembuf->vBatt);
    telembuf->curSolar = csp_letoh16(telembuf->curSolar);
    telembuf->curBattIn = csp_letoh16(telembuf->curBattIn);
    telembuf->curBattOut = csp_letoh16(telembuf->curBattOut);
    telembuf->outputStatus = csp_letoh16(telembuf->outputStatus);

    telembuf->outputFaultStatus = csp_letoh16(telembuf->outputFaultStatus);
    telembuf->wdt_gs_time_left = csp_letoh32(telembuf->wdt_gs_time_left);
    telembuf->wdt_gs_counter = csp_letoh32(telembuf->wdt_gs_counter);
    telembuf->rstReason = csp_letoh32(telembuf->rstReason);
    telembuf->bootCnt = csp_letoh32(telembuf->bootCnt);
}
