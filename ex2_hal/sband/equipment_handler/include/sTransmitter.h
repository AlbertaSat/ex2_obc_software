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
 * @file sTransmitter.h
 * @author Thomas Ganley
 * @date 2020-05-13
 */

#ifndef STRANSMITTER_H
#define STRANSMITTER_H

#include <stdint.h>
#define S_DATA_TIME 0.000002

// Transmitter modes
#define S_CONF_MODE 0
#define S_SYNC_MODE 1
#define S_DATA_MODE 2
#define S_TEST_MODE 3

// Power Amplifier status
#define S_PA_DISABLE 0
#define S_PA_ENABLE 1

// Scrambler status
#define S_SCRAMBLER_ENABLE 0
#define S_SCRAMBLER_DISABLE 1

// Filter status
#define S_FILTER_ENABLE 0
#define S_FILTER_DISABLE 1

// Modulation type
#define S_MOD_QPSK 0
#define S_MOD_OQPSK 1

// Data rates
#define S_RATE_FULL 0
#define S_RATE_HALF 1

// Power Amplifier operating status
#define S_PAPWR_NOTGOOD 0
#define S_PAPWR_GOOD 1

// Frequency lock detect
#define S_FREQ_NOLOCK 0
#define S_FREQ_LOCK 1

// Buffer parameter types
#define S_BUFFER_COUNT 0
#define S_BUFFER_UNDERRUN 1
#define S_BUFFER_OVERRUN 2

typedef enum{
	FUNC_PASS  = 0,
	BAD_READ   = 1,
	BAD_WRITE  = 1,
	BAD_PARAM  = 2,
}STX_return;

typedef struct {
  float outputPower;
  float paTemp;
  float topTemp;
  float bottomTemp;
  float batCurrent;
  float batVoltage;
  float paCurrent;
  float paVoltage;
} sBand_housekeeping;

//* Simulated buffer functions
STX_return add_vBuffer(int);
STX_return transmit_vBuffer(int);
void empty_vBuffer(void);

//* Simulated register functions
STX_return read_reg(uint8_t, uint8_t *);
STX_return write_reg(uint8_t, uint8_t);

// Internal bit manipulation functions
uint16_t append_bytes(uint8_t, uint8_t);
float calculateTemp(uint16_t);

// External access/control functions

STX_return STX_getControl(uint8_t * pa, uint8_t * mode);

STX_return STX_setControl(uint8_t new_pa, uint8_t new_mode);

STX_return STX_getEncoder(uint8_t * scrambler, uint8_t * filter, uint8_t * mod, uint8_t * rate);

STX_return STX_setEncoder(uint8_t new_scrambler, uint8_t new_filter, uint8_t new_mod, uint8_t new_rate);

STX_return STX_getPaPower(uint8_t * power);

STX_return STX_setPaPower(uint8_t new_paPower);

STX_return STX_getFrequency(float * freq);

STX_return STX_setFrequency(float new_frequency);

STX_return STX_softResetFPGA(void);

STX_return STX_getFirmwareV(float * version);

STX_return STX_getStatus(uint8_t * pwrgd, uint8_t * txl);

STX_return STX_getTR(int * transmit);

STX_return STX_getBuffer(uint8_t quantity, uint16_t * ptr);

STX_return STX_getHK(sBand_housekeeping *hk);

#endif /* STRANSMITTER_H */
