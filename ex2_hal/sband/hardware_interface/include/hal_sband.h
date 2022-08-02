/*
 * Copyright (C) 2021-2022  University of Alberta
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

#ifndef HAL_SBAND_H
#define HAL_SBAND_H

#include <inttypes.h>
#include "HL_reg_het.h"

typedef enum {
    S_SUCCESS = 0,

    // Returned if a read or write command fails
    S_BAD_READ = 1,
    S_BAD_WRITE = 2,

    // Returned if an invalid parameter is passed to a write command at the EH
    S_BAD_PARAM = 3,

    // Returned at HAL if S-band is stubbed
    IS_STUBBED_S = 0,
} STX_return;

// Buffer parameter types
typedef enum {
    S_BUFFER_COUNT = 0,
    S_BUFFER_UNDERRUN,
    S_BUFFER_OVERRUN,
    S_BUFFER_LAST
} Sband_Buffer_t;

typedef enum {
    PA_STATUS_DISABLE = 0,
    PA_STATUS_ENABLE
} Sband_PowerAmplifier_Status_t;

typedef enum {
    PA_MODE_CONF = 0,
    PA_MODE_SYNC,
    PA_MODE_DATA,
    PA_MODE_TEST
} Sband_Transmitter_Mode_t;

#define PACKED __attribute__((packed))
// #define PACKED

typedef struct PACKED {
    Sband_PowerAmplifier_Status_t status;
    Sband_Transmitter_Mode_t mode;
} Sband_PowerAmplifier;

// Data rates
#define S_RATE_FULL 0
#define S_RATE_HALF 1
#define S_RATE_QUARTER 2

// Modulation type
#define S_MOD_QPSK 0
#define S_MOD_OQPSK 1

// Filter status
#define S_FILTER_ENABLE 0
#define S_FILTER_DISABLE 1

// Scrambler status
#define S_SCRAMBLER_ENABLE 0
#define S_SCRAMBLER_DISABLE 1

// Bit Order
#define S_BIT_ORDER_MSB 0
#define S_BIT_ORDER_LSB 1

typedef struct PACKED {
    uint8_t scrambler;
    uint8_t filter;
    uint8_t modulation;
    uint8_t rate;
    uint8_t bit_order;
} Sband_Encoder;

typedef struct PACKED {
    float freq;
    uint8_t PA_Power;
    Sband_PowerAmplifier PA;
    Sband_Encoder enc;
} Sband_config;

typedef struct PACKED {
    uint8_t PWRGD;
    uint8_t TXL;
} Sband_Status;

typedef struct PACKED {
    uint8_t transmit;
} Sband_TR;

typedef struct PACKED {
    uint16_t firmware;
} Sband_FirmwareV;

typedef struct PACKED {
    uint16_t pointer[S_BUFFER_LAST];
} Sband_Buffer;

typedef struct PACKED sband_housekeeping {
    float Output_Power;
    float PA_Temp;
    float Top_Temp;
    float Bottom_Temp;
    float Bat_Current;
    float Bat_Voltage;
    float PA_Current;
    float PA_Voltage;
} Sband_Housekeeping;

typedef struct PACKED {
    Sband_Status status;
    Sband_TR transmit;
    Sband_FirmwareV firmware;
    Sband_Buffer buffer;
    Sband_Housekeeping HK;
} Sband_Full_Status;

STX_return HAL_S_getFreq(float *S_freq);
STX_return HAL_S_getPAPower(uint8_t *S_PA_power);
STX_return HAL_S_getControl(Sband_PowerAmplifier *S_PA);
STX_return HAL_S_getEncoder(Sband_Encoder *S_Enc);
STX_return HAL_S_getStatus(Sband_Status *S_status);
STX_return HAL_S_getTR(Sband_TR *S_transmit);
STX_return HAL_S_getHK(Sband_Housekeeping *S_hk);
STX_return HAL_S_getFirmwareV(Sband_FirmwareV *S_firmwareV);
STX_return HAL_S_hk_convert_endianness(Sband_Housekeeping *S_hk);
STX_return HAL_S_getBuffer(int quantity, Sband_Buffer *S_buffer);
STX_return HAL_S_softResetFPGA(void);
STX_return HAL_S_setFreq(float S_freq_new);
STX_return HAL_S_setPAPower(uint8_t S_PA_Power_new);
STX_return HAL_S_setControl(Sband_PowerAmplifier S_PA_new);
STX_return HAL_S_setEncoder(Sband_Encoder S_enc_new);

#endif /* HAL_SBAND_H */
