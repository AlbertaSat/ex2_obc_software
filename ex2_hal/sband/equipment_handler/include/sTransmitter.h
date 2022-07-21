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

#include "HL_het.h"
#include "HL_gio.h"
#include "hal_sband.h"
#include "system.h"

#define SBAND_I2C_ADD 0x26

// Register Values (read-write)
#define S_CONTROL_REG 0x00
#define S_ENCODER_REG 0x01
#define S_PAPOWER_REG 0x03
#define S_FREQ_REG 0x04

// Register Values (write)
#define S_SOFTRST_REG 0x05

// Register Values (read)
#define S_FWVER_REG 0x11
#define S_STATUS_REG 0x12
#define S_TXREADY_REG 0x13
#define S_BUFUND_REG_1 0x14
#define S_BUFUND_REG_2 0x15
#define S_BUFOVR_REG_1 0x16
#define S_BUFOVR_REG_2 0x17
#define S_BUFCNT_REG_1 0x18
#define S_BUFCNT_REG_2 0x19
#define S_OUTPWR_REG_1 0x1A
#define S_OUTPWR_REG_2 0x1B
#define S_PATEMP_REG_1 0x1C
#define S_PATEMP_REG_2 0x1D
#define S_TOPTEMP_REG_1 0x1E
#define S_TOPTEMP_REG_2 0x1F
#define S_BOTTEMP_REG_1 0x20
#define S_BOTTEMP_REG_2 0x21
#define S_CURRENT_REG_1 0x22
#define S_CURRENT_REG_2 0x23
#define S_VOLTAGE_REG_1 0x24
#define S_VOLTAGE_REG_2 0x25
#define S_PACURRENT_REG_1 0x26
#define S_PACURRENT_REG_2 0x27
#define S_PAVOLTAGE_REG_1 0x28
#define S_PAVOLTAGE_REG_2 0x29

#define S_LAST_REG S_VOLTAGE_REG_2

// Max Lenghts for S-band commands and responses
#define MAX_SBAND_W_CMDLEN 2
#define MAX_SBAND_W_ANSLEN 1
#define MAX_SBAND_R_CMDLEN 1
#define MAX_SBAND_R_ANSLEN 1

// Transmitter modes
#define S_CONF_MODE 0
#define S_SYNC_MODE 1
#define S_DATA_MODE 2
#define S_TEST_MODE 3

// Power Amplifier status
#define S_PA_DISABLE 0
#define S_PA_ENABLE 1

// Power Amplifier powers
#define S_PAPWR_24DBM 24
#define S_PAPWR_26DBM 26
#define S_PAPWR_28DBM 28
#define S_PAPWR_30DBM 30

// Frequency maximums/minimums
#define S_FREQ_COMMERCIAL_MAX 2300.0f
#define S_FREQ_COMMERCIAL_MIN 2200.0f
#define S_FREQ_AMATEUR_MAX 2450.0f
#define S_FREQ_AMATEUR_MIN 2400.0f

// Power Amplifier operating status
#define S_PAPWR_NOTGOOD 0
#define S_PAPWR_GOOD 1

// Frequency lock detect
#define S_FREQ_NOLOCK 0
#define S_FREQ_LOCK 1

// Conversion factors
#define S_FREQ_OFFSET_SCALING 2
#define S_FWVER_MAJORNUM_SCALING 100
#define S_OUTPWR_SCALING 0.00114f
#define S_PATEMP_SCALING 0.07324f
#define S_PATEMP_OFFSET -50.0f
#define S_TEMP_SCALING 0.0625f
#define S_CURRENT_SCALING 0.00004f
#define S_VOLTAGE_SCALING 0.004F

// Bit masks/shifting
#define S_CONTROL_MODE_BIT_INDEX 0
#define S_CONTROL_MODE_BITMASK 0b11
#define S_CONTROL_PA_BIT_INDEX 7
#define S_ENCODER_RATE_BIT_INDEX 0
#define S_ENCODER_RATE_BITMASK 0b11
#define S_ENCODER_MOD_BIT_INDEX 2
#define S_ENCODER_FILTER_BIT_INDEX 3
#define S_ENCODER_SCRAMBLER_BIT_INDEX 4
#define S_ENCODER_BITORDER_BIT_INDEX 5
#define S_FWVER_MAJORNUM_BIT_INDEX 4
#define S_FWVER_MINORNUM_BITMASK 0b1111
#define S_STATUS_TXL_BIT_INDEX 0
#define S_STATUS_PWRGD_BIT_INDEX 1
#define S_TEMP_BITMASK 0x0FFF    // Top & bottom temperature are 12-bit values
#define S_TEMP_BITSHIFT 4        // Temps are stored in the first bits across their two 8-bit registers
#define S_POWER_BITMASK 0x0FFF   // Output power and PA Temp are 12-bit values
#define S_VOLTAGE_BITMASK 0x1FFF // Voltage is a 13-bit value

STX_return read_reg(uint8_t, uint8_t *);
STX_return write_reg(uint8_t, uint8_t);

// Internal bit manipulation functions
uint16_t append_bytes(uint8_t, uint8_t);
float calculateTemp(uint16_t);

// External access/control functions

void STX_Enable(void);

void STX_Disable(void);

STX_return STX_getControl(uint8_t *pa, uint8_t *mode);

STX_return STX_setControl(uint8_t new_pa, uint8_t new_mode);

STX_return STX_getEncoder(uint8_t *bit_order, uint8_t *scrambler, uint8_t *filter, uint8_t *mod, uint8_t *rate);

STX_return STX_setEncoder(uint8_t new_bit_order, uint8_t new_scrambler, uint8_t new_filter, uint8_t new_mod,
                          uint8_t new_rate);

STX_return STX_getPaPower(uint8_t *power);

STX_return STX_setPaPower(uint8_t new_paPower);

STX_return STX_getFrequency(float *freq);

STX_return STX_setFrequency(float new_frequency);

STX_return STX_softResetFPGA(void);

STX_return STX_getFirmwareV(uint16_t *version);

STX_return STX_getStatus(uint8_t *pwrgd, uint8_t *txl);

STX_return STX_getTR(uint8_t *transmit);

STX_return STX_getBuffer(uint8_t quantity, uint16_t *ptr);

STX_return STX_getHK(struct sband_housekeeping *hk);

#endif /* STRANSMITTER_H */
