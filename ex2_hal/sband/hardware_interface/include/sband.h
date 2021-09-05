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

#ifndef SBAND_H
#define SBAND_H

#include <inttypes.h>

#ifdef SBAND_IS_STUBBED
typedef enum {
    IS_STUBBED = 0,
} STX_return;
#else
#include "sTransmitter.h"
#endif

typedef enum {
    COUNT = 0,
    UNDERRUN,
    OVERRUN,
} Buffer_Quantity;

typedef struct __attribute__((packed)) {
    uint8_t status;
    uint8_t mode;
} Sband_PowerAmplifier;

typedef struct __attribute__((packed)) {
    uint8_t scrambler;
    uint8_t filter;
    uint8_t modulation;
    uint8_t rate;
} Sband_Encoder;

typedef struct __attribute__((packed)) {
    float freq;
    uint8_t PA_Power;
    Sband_PowerAmplifier PA;
    Sband_Encoder enc;
} Sband_config;

typedef struct __attribute__((packed)) {
    uint8_t PWRGD;
    uint8_t TXL;
} Sband_Status;

typedef struct __attribute__((packed)) {
    int transmit;
} Sband_TR;

typedef struct __attribute__((packed)) {
    float Output_Power;
    float PA_Temp;
    float Top_Temp;
    float Bottom_Temp;
    float Bat_Current;
    float Bat_Voltage;
    float PA_Current;
    float PA_Voltage;
} Sband_Housekeeping;

typedef struct __attribute__((packed)) {
    uint16_t pointer[3];
} Sband_Buffer;

typedef struct __attribute__((packed)) {
    Sband_Status status;
    Sband_TR transmit;
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
STX_return HAL_S_hk_convert_endianness(Sband_Housekeeping *S_hk);
STX_return HAL_S_getBuffer(int quantity, Sband_Buffer *S_buffer);
STX_return HAL_S_softResetFPGA(void);
STX_return HAL_S_setFreq(float S_freq_new);
STX_return HAL_S_setPAPower(uint8_t S_PA_Power_new);
STX_return HAL_S_setControl(Sband_PowerAmplifier S_PA_new);
STX_return HAL_S_setEncoder(Sband_Encoder S_enc_new);

#endif /* SBAND_H */
