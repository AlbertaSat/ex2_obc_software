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
 * @file eps.c
 * @author Andrew Rooney, Dustin Wagner
 * @date 2020-12-28
 */

/*
 * This code needs to:
 *  - send requests to the EPS over the network as per the NanoAvionics specs
 *  - provide an API around those calls for the on-board applications to use
 */

#include <FreeRTOS.h>
#include <os_task.h>
#include <os_semphr.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <main/system.h>

#include "services.h"
#include "eps.h"

void prv_instantaneous_telemetry_letoh (eps_instantaneous_telemetry_t *telembuf);
static inline void prv_set_instantaneous_telemetry (eps_instantaneous_telemetry_t telembuf);
static inline void prv_get_lock(eps_t *eps);
static inline void prv_give_lock(eps_t *eps);
static eps_t* prv_get_eps();

struct eps_t {
    eps_instantaneous_telemetry_t hk_telemetery;
    SemaphoreHandle_t eps_lock;
};

static eps_t prvEps;

/*------------------------------Public-------------------------------------*/

SAT_returnState eps_refresh_instantaneous_telemetry() {
    uint8_t cmd = 0; // 'subservice' command
    eps_instantaneous_telemetry_t telembuf;
    int res = csp_ping(EPS_APP_ID, 10000, 100, CSP_O_NONE);

    csp_transaction_w_opts(CSP_PRIO_LOW, EPS_APP_ID, EPS_INSTANTANEOUS_TELEMETRY,
                           10000, &cmd, sizeof(cmd), &telembuf,
                           sizeof(eps_instantaneous_telemetry_t), CSP_O_CRC32);
    // data is little endian, must convert to host order
    // refer to the NanoAvionics datasheet for details
//    prv_instantaneous_telemetry_letoh(&telembuf);
    prv_set_instantaneous_telemetry(telembuf);
    return SATR_OK;
}

eps_instantaneous_telemetry_t get_eps_instantaneous_telemetry() {
    eps_instantaneous_telemetry_t telembuf;
    eps_t *eps;
    eps = prv_get_eps();
    prv_get_lock(eps);
    configASSERT(eps);
    telembuf = eps->hk_telemetery;
    prv_give_lock(eps);
    return telembuf;
}

/**
 * @brief
 *      Electronic Power System get Housekeeping data
 * @details
 *      May replace get_eps_instantaneous_telemetry() if deemed a suitable 
 *      replacement
 * @param telembuf
 *      pointer to struct
 * 
 * @return 
 *      Will likely want to return at least a boolean for success
 *      but currently no return 
 * 
 */
void EPS_getHK(eps_instantaneous_telemetry_t* telembuf) {
    eps_t *eps;
    eps = prv_get_eps();
    prv_get_lock(eps);
    configASSERT(eps);

    telembuf->cmd = eps->hk_telemetery.cmd;
    telembuf->status = eps->hk_telemetery.status;
    telembuf->vBatt = eps->hk_telemetery.vBatt;
    telembuf->curSolar = eps->hk_telemetery.curSolar;
    telembuf->curBattIn = eps->hk_telemetery.curBattIn;
    telembuf->curBattOut = eps->hk_telemetery.curBattOut;
    telembuf->reserved1 = eps->hk_telemetery.reserved1;
    telembuf->outputStatus = eps->hk_telemetery.outputStatus;
    telembuf->outputFaultStatus = eps->hk_telemetery.outputFaultStatus;
    telembuf->wdt_gs_time_left = eps->hk_telemetery.wdt_gs_time_left;
    telembuf->wdt_gs_counter = eps->hk_telemetery.wdt_gs_counter;
    telembuf->rstReason = eps->hk_telemetery.rstReason;
    telembuf->bootCnt = eps->hk_telemetery.bootCnt;
    telembuf->battMode = eps->hk_telemetery.battMode;
    telembuf->mpptMode = eps->hk_telemetery.mpptMode;
    telembuf->batHeaterMode = eps->hk_telemetery.batHeaterMode;
    telembuf->batHeaterState = eps->hk_telemetery.batHeaterState;
    telembuf->reserved5 = eps->hk_telemetery.reserved5;

    uint8_t i;
    for (i = 0; i < 2;  i++) {
        telembuf->AOcurOutput[i] = eps->hk_telemetery.AOcurOutput[i];
    }
    for (i = 0; i < 4;  i++) {
        telembuf->mpptConverterVoltage[i] = eps->hk_telemetery.mpptConverterVoltage[i];
        telembuf->OutputConverterVoltage[i] = eps->hk_telemetery.OutputConverterVoltage[i];
        telembuf->outputConverterState[i] = eps->hk_telemetery.outputConverterState[i];
        telembuf->reserved4[i] = eps->hk_telemetery.reserved4[i];
    }
    for (i = 0; i < 6;  i++) {
        telembuf->reserved2[i] = eps->hk_telemetery.reserved2[i];
    }
    for (i = 0; i < 7;  i++) {
        telembuf->reserved3[i] = eps->hk_telemetery.reserved3[i];
    }
    for (i = 0; i < 8;  i++) {
        telembuf->curSolarPanels[i] = eps->hk_telemetery.curSolarPanels[i];
    }
    for (i = 0; i < 10; i++) {
        telembuf->curOutput[i] = eps->hk_telemetery.curOutput[i];
        telembuf->outputOnDelta[i] = eps->hk_telemetery.outputOnDelta[i];
        telembuf->outputOffDelta[i] = eps->hk_telemetery.outputOffDelta[i];
        telembuf->outputFaultCnt[i] = eps->hk_telemetery.outputFaultCnt[i];
    }
    for (i = 0; i < 12; i++) {
        telembuf->temp[i] = eps->hk_telemetery.temp[i];
    }

    prv_give_lock(eps);
}

eps_mode_e get_eps_batt_mode() {
    eps_instantaneous_telemetry_t eps = get_eps_instantaneous_telemetry();
    return (eps_mode_e) eps.battMode;
}

int8_t eps_get_pwr_chnl(uint8_t pwr_chnl_port){

}

void eps_set_pwr_chnl(uint8_t pwr_chnl_port, bool bit){

}

/*------------------------------Private-------------------------------------*/

static eps_t* prv_get_eps() {
    if (!prvEps.eps_lock) {
        prvEps.eps_lock = xSemaphoreCreateMutex();
    }
    configASSERT(prvEps.eps_lock);
    return &prvEps;
}

static inline void prv_get_lock(eps_t *eps) {
    configASSERT(eps->eps_lock);
    xSemaphoreTake(eps->eps_lock, portMAX_DELAY);
}

static inline void prv_give_lock(eps_t *eps) {
    configASSERT(eps->eps_lock);
    xSemaphoreGive(eps->eps_lock);
}

static inline void prv_set_instantaneous_telemetry (eps_instantaneous_telemetry_t telembuf) {
    eps_t *eps = prv_get_eps();
    prv_get_lock(eps);
    eps->hk_telemetery = telembuf;
    prv_give_lock(eps);
    return;
}

void prv_instantaneous_telemetry_letoh (eps_instantaneous_telemetry_t *telembuf) {
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
