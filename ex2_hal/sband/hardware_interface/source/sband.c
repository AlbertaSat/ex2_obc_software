/*
 * Copyright (C) 2022 University of Alberta
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
 * @file sband.c
 * @author Ron Unrau
 * @date 2022-07-01
 */
#include <FreeRTOS.h>
#include <os_task.h>
#include <sTransmitter.h>
#include <sband.h>
#include <logger/logger.h>

#define SBAND_SYNC_BYTES 8
static uint16_t syncword[SBAND_SYNC_BYTES/sizeof(uint16_t)] = {0xdadb, 0x0dba, 0xbeee, 0xd00d};

int sband_init() {
    STX_return ret;

#if SBAND_IS_STUBBED == 0
    STX_Enable();
#endif
    vTaskDelay(2*ONE_SECOND);
 
    Sband_PowerAmplifier pa = { .status = PA_STATUS_DISABLE, .mode = PA_MODE_CONF };
    if ((ret = HAL_S_setControl(pa)) != S_SUCCESS) {
        sys_log(WARN, "S-Band can't set CONF mode, rc %d", ret);
        return -1;
    }

    Sband_Encoder encoder = {
        .scrambler = S_SCRAMBLER_ENABLE,
        .filter = S_FILTER_ENABLE,
        .modulation = S_MOD_QPSK,
        .rate = S_RATE_FULL,
        .bit_order = S_BIT_ORDER_MSB
    };
    if ((ret = HAL_S_setEncoder(encoder)) != S_SUCCESS) {
        sys_log(NOTICE, "S-Band can't set encoder, rc %d", ret);
        return -2;
    }
    float freq = SBAND_FREQUENCY;
    if ((ret = HAL_S_setFreq(freq)) != S_SUCCESS) {
        sys_log(NOTICE, "S-Band can't set frequency, rc %d", ret);
        return -3;
    }
    if ((ret = HAL_S_setPAPower(30)) != S_SUCCESS) {
        sys_log(NOTICE, "S-Band can't set power, rc %d", ret);
        return -4;
    }
    return 0;
}

bool sband_enter_conf_mode() {
    Sband_PowerAmplifier pa = { .status = PA_STATUS_DISABLE, .mode = PA_MODE_CONF };
    STX_return ret = HAL_S_setControl(pa);
    if (ret != S_SUCCESS) {
        sys_log(NOTICE, "%s failed: %d", __FUNCTION__, ret);
        return false;
    }
    return true;
}

bool sband_enter_sync_mode() {
    // Call this to get ready to transmit
    Sband_PowerAmplifier pa = { .status = PA_STATUS_ENABLE, .mode = PA_MODE_SYNC };
    STX_return ret = HAL_S_setControl(pa);
    if (ret != S_SUCCESS) {
        sys_log(WARN, "%s failed: %d", __FUNCTION__, ret);
        return false;
    }
    return true;
}

bool sband_enter_data_mode() {
    // Call this to start transmitting once the FIFO is full
    Sband_PowerAmplifier pa = { .status = PA_STATUS_ENABLE, .mode = PA_MODE_DATA };
    STX_return ret = HAL_S_setControl(pa);
    if (ret != S_SUCCESS) {
        sys_log(WARN, "%s failed: %d", __FUNCTION__, ret);
        return false;
    }
    return true;
}

void sband_sync() {
    /* Send this before the first data byte and after every SBAND_SYNC_INTERVAL.
     * Note that it won't actually be transmitted until you enter data mode.
     * Also note that our S-Band SPI is 16-bits wide (hence the divide by 2).
     */
#if SBAND_IS_STUBBED == 1
    ex2_log("S-band SYNC %04x%04x%04x%04x", syncword[0], syncword[1], syncword[2], syncword[3]);
#else
    SPISbandTx(syncword, SBAND_SYNC_BYTES);
#endif
}

int sband_transmit_ready() {
    /* This line goes high when there is less than 2560 bytes in the 20KB FIFO.
     * The manual says to keep the FIFO as full as possible but avoid over-runs.
     */
#if SBAND_IS_STUBBED == 1
    return 1;
#else
    return gioGetBit(hetPORT1, 25);
#endif
}

bool sband_buffer_count(uint16_t *cnt) {
    Sband_Buffer sbuf;
    STX_return ret = HAL_S_getBuffer(S_BUFFER_COUNT, &sbuf);
    if (ret != S_SUCCESS) {
        *cnt = 0;
        return false;
    }

    *cnt = sbuf.pointer[S_BUFFER_COUNT];
    return true;
}

    
