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
 * @file sTransmitter.c
 * @author Thomas Ganley
 * @date 2020-05-13
 */

#include "sTransmitter.h"
#include <stdint.h>
#include "HL_i2c.h"
#include "i2c_io.h"

/**
 * @brief
 *      Function to read a register
 * @details
 *      Called in other functions
 * @param address
 *      Register address as defined in the HSTXC User Manual
 * @param ptr
 * 		Pointer to where the register's value should be stored
 * @return STX_return
 *      Success of the function defined in sTransmitter.h
 */
STX_return read_reg(uint8_t internal_address, uint8_t *answer) {
#if SBAND_IS_STUBBED == 0
    uint8_t command = internal_address;
    if (i2c_Send(SBAND_I2C, SBAND_I2C_ADD, MAX_SBAND_R_CMDLEN, &command) != 0) {
        return S_BAD_READ;
    }
    if (i2c_Receive(SBAND_I2C, SBAND_I2C_ADD, MAX_SBAND_R_ANSLEN, answer) != 0) {
        return S_BAD_READ;
    }
#else
    *answer = 0;
#endif
    return S_SUCCESS;
}

/**
 * @brief
 *      Function to write to a register
 * @details
 *      Called in other functions
 * @param address
 *      Register address as defined in the HSTXC User Manual
 * @param ptr
 *      Pointer to the write value
 * @return STX_return
 *      Success of the function defined in sTransmitter.h
 */
STX_return write_reg(uint8_t internal_address, uint8_t val) {
#if SBAND_IS_STUBBED == 0
    uint8_t command[2] = {internal_address, val};
    if (i2c_Send(SBAND_I2C, SBAND_I2C_ADD, MAX_SBAND_W_CMDLEN, command) != 0) {
        return S_BAD_WRITE;
    }
#endif
    return S_SUCCESS;
}

/**
 * @brief
 *      Function to combine readings from two registers
 * @details
 *      Called in set/get functions. Some values are stored in "16 bit
 *      registers" using two 8-bit registers
 * @param b1
 *      Most significant byte of desired 16 bit value
 * @param b2
 *      Least significant byte of desired 16 bit value
 * @return uint16_t
 *      Returns appended value
 */
uint16_t append_bytes(uint8_t b1, uint8_t b2) {
    uint16_t b = (b1 << 8) | b2;
    return b;
}

/**
 * @brief
 *      Function to combine readings from two temperature sensor
 *      registers
 * @details
 *      Called in temperature get functions. Some values are stored in
 *      "16 bit registers" using two 8-bit registers
 * @param b1
 *      Most significant byte of desired 16 bit value
 * @param b2
 *      Least significant byte of desired 16 bit value
 * @return float
 *      Returns temperature in degrees Celsius
 */
float calculateTemp(uint16_t b) {
    float temperature = 0;
    b = b >> S_TEMP_BITSHIFT;

    // Check signed bit
    if (b & 0b100000000000) {
        temperature = -S_TEMP_SCALING * (float)((~b & S_TEMP_BITMASK) + 1);
    } else {
        temperature = S_TEMP_SCALING * (float)(b & S_TEMP_BITMASK);
    }
    return temperature;
}

/**
 * @brief
 *      Enable the S-band transmitter
 * @return void
 */
void STX_Enable(void) {
    uint32_t bit = gioGetBit(hetPORT2, 23);
    if (bit == 0) {
        gioSetBit(hetPORT2, 23, 1); // Het2 23 is the S-band enable pin
    }
    gioSetBit(hetPORT2, 21, 1); // Het2 21 is the S-band nRESET pin
}

/**
 * @brief
 *      Disable the S-band transmitter
 * @return void
 */
void STX_Disable(void) {
    gioSetBit(hetPORT2, 23, 0); // 23 is the S-band enable pin on hetPort2
}

/**
 * @brief
 *      Register 0x00: Get the power amplifier and mode values
 * @param pa
 *      Where the read value for the power amplifier will be stored
 *      (1 = enabled, 0 = disabled)
 * @param mode
 *      Where the read value for the mode will be stored
 *      (0 = Configuration, 1 = Synchronization, 2 = Data, 3 = Test data)
 * @return STX_return
 *      Success of the function defined in sTransmitter.h
 */
STX_return STX_getControl(uint8_t *pa, uint8_t *mode) {
    uint8_t rawValue = 0;
    if (read_reg(S_CONTROL_REG, &rawValue) != S_SUCCESS) {
        return S_BAD_READ;
    } else {
        *pa = rawValue >> S_CONTROL_PA_BIT_INDEX;
        *mode = rawValue & S_CONTROL_MODE_BITMASK;
        return S_SUCCESS;
    }
}

/**
 * @brief
 *      Register 0x00: Set the power amplifier and mode values
 * @param new_pa
 *      PA write value
 *      (1 = enabled, 0 = disabled)
 * @param new_mode
 *      Mode write value
 *      (0 = Configuration, 1 = Synchronization, 2 = Data, 3 = Test data)
 * @return STX_return
 *              Success of the function defined in sTransmitter.h
 */
STX_return STX_setControl(uint8_t new_pa, uint8_t new_mode) {
    if (new_mode > S_TEST_MODE || new_pa > S_PA_ENABLE) {
        return S_BAD_PARAM;
    }

    uint8_t new_control = (new_mode << S_CONTROL_MODE_BIT_INDEX) | (new_pa << S_CONTROL_PA_BIT_INDEX);

    if (write_reg(S_CONTROL_REG, new_control) != S_SUCCESS) {
        return S_BAD_WRITE;
    } else {
        return S_SUCCESS;
    }
}

/**
 * @brief
 *      Register 0x01: Get the scrambler, filter, modulation, rate, and bit order
 * @param scrambler
 *      Where the read value for the scrambler will be stored
 *      (0 = enabled, 1 = disabled)
 * @param filter
 *      Where the read value for the filter will be stored
 *      (0 = enabled, 1 = disabled)
 * @param mod
 * 		Where the read value for the modulation scheme will be stored
 * 		(0 = QPSK, 1 = OQPSK)
 * @param rate
 * 		Where the read value for the rate will be stored
 * 		(0 = Full rate, 1 = Half rate)
 * @param bit_order
 *      Where the read value for the bit order will be stored
 *      (0 = MSB, 1 = LSB)
 * @return STX_return
 *      Success of the function defined in sTransmitter.h
 */
STX_return STX_getEncoder(uint8_t *bit_order, uint8_t *scrambler, uint8_t *filter, uint8_t *mod, uint8_t *rate) {
    uint8_t rawValue = 0;
    if (read_reg(S_ENCODER_REG, &rawValue) != S_SUCCESS) {
        return S_BAD_READ;
    } else {
        *rate = (rawValue >> S_ENCODER_RATE_BIT_INDEX) & S_ENCODER_RATE_BITMASK;
        *mod = (rawValue >> S_ENCODER_MOD_BIT_INDEX) & 0b1;
        *filter = (rawValue >> S_ENCODER_FILTER_BIT_INDEX) & 0b1;
        *scrambler = (rawValue >> S_ENCODER_SCRAMBLER_BIT_INDEX) & 0b1;
        *bit_order = (rawValue >> S_ENCODER_BITORDER_BIT_INDEX) & 0b1;
        return S_SUCCESS;
    }
}

/**
 * @brief
 *      Register 0x03: Set the scrambler, filter, modulation, rate, and bit order
 * @param new_scrambler
 *      Write value for the scrambler
 *      (0 = enabled, 1 = disabled)
 * @param new_filter
 *      Write value for the filter
 *      (0 = enabled, 1 = disabled)
 * @param new_mod
 *      Write value for the modulation scheme
 *      (0 = QPSK, 1 = OQPSK)
 * @param new_rate
 *      Write value for the rate
 *      (0 = Full rate, 1 = Half rate)
 * @param new_bit_order
 *      Write value for the bit order
 *      (0 = MSB, 1 = LSB)
 * @return STX_return
 *      Success of the function defined in sTransmitter.h
 */
STX_return STX_setEncoder(uint8_t new_bit_order, uint8_t new_scrambler, uint8_t new_filter, uint8_t new_mod,
                          uint8_t new_rate) {
    if (new_rate > S_RATE_QUARTER || new_mod > S_MOD_OQPSK || new_filter > S_FILTER_DISABLE ||
        new_scrambler > S_SCRAMBLER_DISABLE || new_bit_order > S_BIT_ORDER_LSB) {
        return S_BAD_PARAM;
    }

    uint8_t new_encoder = (new_rate << S_ENCODER_RATE_BIT_INDEX) | (new_mod << S_ENCODER_MOD_BIT_INDEX) |
                          (new_filter << S_ENCODER_FILTER_BIT_INDEX) |
                          (new_scrambler << S_ENCODER_SCRAMBLER_BIT_INDEX) |
                          (new_bit_order << S_ENCODER_BITORDER_BIT_INDEX);

    uint8_t mode = 0, pa = 0;
    if (STX_getControl(&pa, &mode) == S_SUCCESS) {
        if (mode == S_CONF_MODE) {
            if (write_reg(S_ENCODER_REG, new_encoder)) {
                return S_BAD_WRITE;
            } else {
                return S_SUCCESS;
            }
        } else {
            return S_BAD_PARAM;
        }

    } else {
        return S_BAD_READ;
    }
}

/**
 * @brief
 *      Register 0x03: Get the power amplifier power
 * @attention
 * 		Stores the actual value (24, 26, 28, 30 dBm) at pointer
 * @param power
 *      Where the read value for the power will be stored
 * @return STX_return
 *      Success of the function defined in sTransmitter.h
 */
STX_return STX_getPaPower(uint8_t *power) {
    uint8_t rawValue = 0;

    if (read_reg(S_PAPOWER_REG, &rawValue) != S_SUCCESS) {
        return S_BAD_READ;
    } else {
        switch (rawValue) {
        case 0:
            *power = S_PAPWR_24DBM;
            break;
        case 1:
            *power = S_PAPWR_26DBM;
            break;
        case 2:
            *power = S_PAPWR_28DBM;
            break;
        case 3:
            *power = S_PAPWR_30DBM;
            break;
        default:
            return S_BAD_PARAM;
        }
        return S_SUCCESS;
    }
}

/**
 * @brief
 *      Register 0x03: Set the power amplifier power
 * @attention
 *      Input is the actual power (24, 26, 28, 30 dBm)
 * @param new_paPower
 *      Write value of the new PA power
 * @return STX_return
 *      Success of the function defined in sTransmitter.h
 */
STX_return STX_setPaPower(uint8_t new_paPower) {
    uint8_t rawValue = 0;

    switch (new_paPower) {
    case S_PAPWR_24DBM:
        rawValue = 0;
        break;
    case S_PAPWR_26DBM:
        rawValue = 1;
        break;
    case S_PAPWR_28DBM:
        rawValue = 2;
        break;
    case S_PAPWR_30DBM:
        rawValue = 3;
        break;
    default:
        return S_BAD_PARAM;
    }

    if (write_reg(S_PAPOWER_REG, rawValue) != S_SUCCESS) {
        return S_BAD_WRITE;
    } else {
        return S_SUCCESS;
    }
}

/**
 * @brief
 *      Register 0x04: Get the frequency
 * @attention
 *      Stores the actual frequency (MHz) at pointer
 * @param freq
 *      Where the read value for the frequency will be stored
 * @return STX_return
 *      Success of the function defined in sTransmitter.h
 */
STX_return STX_getFrequency(float *freq) {
    uint8_t offset = 0;
    if (read_reg(S_FREQ_REG, &offset) != S_SUCCESS) {
        return S_BAD_READ;
    } else {

#ifdef SBAND_COMMERCIAL_FREQUENCY
        *freq = (float)offset / S_FREQ_OFFSET_SCALING + S_FREQ_COMMERCIAL_MIN;
#else
        *freq = (float)offset / S_FREQ_OFFSET_SCALING + S_FREQ_AMATEUR_MIN;
#endif

        return S_SUCCESS;
    }
}

/**
 * @brief
 *      Register 0x04: Set a new frequency
 * @attention
 *      Input is the actual frequency (MHz)
 * @param new_frequency
 *      Write value for the frequency
 * @return STX_return
 *      Success of the function defined in sTransmitter.h
 */
STX_return STX_setFrequency(float new_frequency) {
#ifdef SBAND_COMMERCIAL_FREQUENCY

    // Check if commercial frequency is within allowed bounds
    if ((new_frequency >= S_FREQ_COMMERCIAL_MIN) && (new_frequency <= S_FREQ_COMMERCIAL_MAX)) {
        uint8_t offset = (uint8_t)((new_frequency - S_FREQ_COMMERCIAL_MIN) * S_FREQ_OFFSET_SCALING);

        if (write_reg(S_FREQ_REG, offset) != S_SUCCESS) {
            return S_BAD_WRITE;
        } else {
            return S_SUCCESS;
        }
    } else {
        return S_BAD_PARAM;
    }

#else

    // Check if amateur frequency is within allowed bounds
    if (new_frequency >= S_FREQ_AMATEUR_MIN && new_frequency <= S_FREQ_AMATEUR_MAX) {
        uint8_t offset = (uint8_t)((new_frequency - S_FREQ_AMATEUR_MIN) * S_FREQ_OFFSET_SCALING);

        if (write_reg(S_FREQ_REG, offset) != S_SUCCESS) {
            return S_BAD_WRITE;
        } else {
            return S_SUCCESS;
        }
    } else {
        return S_BAD_PARAM;
    }

#endif
}
/**
 * @brief
 *      Register 0x05: Reset the FPGA logic
 * @attention
 * 		Resets all register to defaults (see options sheet)
 * @return STX_return
 *      Success of the function defined in sTransmitter.h
 */
STX_return STX_softResetFPGA(void) {
    if (write_reg(S_SOFTRST_REG, 0x0) != S_SUCCESS) {
        return S_BAD_WRITE;
    } else {
        return S_SUCCESS;
    }
}

/**
 * @brief
 *      Register 0x11: Get the firmware version
 * @attention
 *      Value of 701 means "7.1", value of 714 means "7.14"
 * @param version
 * 		Where the read value of the firmware version will be stored
 * @return STX_return
 *      Success of the function defined in sTransmitter.h
 */
STX_return STX_getFirmwareV(uint16_t *version) {
    uint8_t rawValue = 0;
    if (read_reg(S_FWVER_REG, &rawValue) != S_SUCCESS) {
        return S_BAD_READ;
    } else {
        uint8_t major = rawValue >> S_FWVER_MAJORNUM_BIT_INDEX;
        uint8_t minor = rawValue & S_FWVER_MINORNUM_BITMASK;
        *version = S_FWVER_MAJORNUM_SCALING * major + minor;
        return S_SUCCESS;
    }
}

/**
 * @brief
 *      Register 0x12: Get the status of the frequency lock and the PA
 * @param pwrgd
 *      Where the read value of the PA power status will be stored
 *		(0 = bad, 1 = good)
 * @param txl
 *      Where the read value of the frequency lock will be stored
 *      (0 = achieved, 1 = not achieved)
 * @return STX_return
 *      Success of the function defined in sTransmitter.h
 */
STX_return STX_getStatus(uint8_t *pwrgd, uint8_t *txl) {
    uint8_t rawValue = 0;
    if (read_reg(S_STATUS_REG, &rawValue) != S_SUCCESS) {
        return S_BAD_READ;
    } else {
        *pwrgd = (rawValue >> S_STATUS_PWRGD_BIT_INDEX) & 0b1;
        *txl = (rawValue >> S_STATUS_TXL_BIT_INDEX) & 0b1;
        return S_SUCCESS;
    }
}

/**
 * @brief
 *      Register 0x13: Get the status of the frequency lock and the PA
 * @attention
 * 		Same as hard line on PC/104 header, but as a pollable register
 * @param trasmit
 *      Where the read value of the Transmit Ready indicator will be
 *      stored (0 = more than 2650 bytes in buffer, 1 = less than 2561 bytes in
 *      buffer)
 * @return STX_return
 *      Success of the function defined in sTransmitter.h
 */
STX_return STX_getTR(uint8_t *transmit) {
    uint8_t rawValue = 0;
    if (read_reg(S_TXREADY_REG, &rawValue) != S_SUCCESS) {
        return S_BAD_READ;
    } else {
        *transmit = rawValue & 0b1;
        return S_SUCCESS;
    }
}

/**
 * @brief
 *      Registers 0x14 through 0x19: Get various buffer paramters
 * @details
 *    	One of three buffer quantities can be selected: Count, Underrun,
 *      Overrun
 * @param quantity
 *      Select the buffer quantity
 *      (0 = Count, 1 = Underrun, 2 = Overrun)
 * @param ptr
 * 		Where the read quantity will be stored
 * @return STX_return
 *      Success of the function defined in sTransmitter.h
 */
STX_return STX_getBuffer(Sband_Buffer_t quantity, uint16_t *ptr) {
    uint8_t rawValue1 = 0;
    uint8_t rawValue2 = 0;
    uint8_t address = 0;

    switch (quantity) {
    case S_BUFFER_COUNT: // Buffer Count
        address = S_BUFCNT_REG_1;
        break;
    case S_BUFFER_UNDERRUN: // Buffer Underrun
        address = S_BUFUND_REG_1;
        break;
    case S_BUFFER_OVERRUN: // Buffer Overrun
        address = S_BUFOVR_REG_1;
        break;
    default:
        return S_BAD_PARAM;
    }

    if (read_reg(address, &rawValue1) != S_SUCCESS) {
        return S_BAD_READ;
    } else if (read_reg(address + 1, &rawValue2) != S_SUCCESS) {
        return S_BAD_READ;
    }

    *ptr = append_bytes(rawValue1, rawValue2);
    return S_SUCCESS;
}

/**
 * @brief
 *      Registers 0x1A through 0x29: Collect housekeeping data
 * @details
 *		Collects RF ouput power, PA, board top, and board bottom
 *      temperature, Battery & PA Current and Voltage
 * @attention
 *      Stored in an sBand_housekeeping structure (see sTransmitter.h)
 * @param hkStruct
 *      Pointer to structure
 * @return STX_return
 *      Success of the function defined in sTransmitter.h
 */
STX_return STX_getHK(Sband_Housekeeping *hkStruct) {
    uint16_t val = 0;
    int16_t temp = 0;

    uint8_t address = S_OUTPWR_REG_1; // Output power is the first hk value to collect

    // Loop to collect all housekeeping. Values are stored across two 8-bit registers
    // TODO: use ints instead of floats
    for (; address < S_LAST_REG; address = address + 2) {
        uint8_t val1 = 0, val2 = 0;

        if (read_reg(address, &val1) != S_SUCCESS) {
            return S_BAD_READ;
        } else if (read_reg(1 + address, &val2) != S_SUCCESS) {
            return S_BAD_READ;
        } else {
            val = append_bytes(val1, val2);
            switch (address) {
            case S_OUTPWR_REG_1:
                val &= S_POWER_BITMASK;
                hkStruct->Output_Power = (float)val * S_OUTPWR_SCALING;
                break;

            case S_PATEMP_REG_1:
                val &= S_POWER_BITMASK;
                hkStruct->PA_Temp = (float)val * S_PATEMP_SCALING + S_PATEMP_OFFSET;
                break;

            case S_TOPTEMP_REG_1:
                hkStruct->Top_Temp = calculateTemp(val);
                break;

            case S_BOTTEMP_REG_1:
                hkStruct->Bottom_Temp = calculateTemp(val);
                break;

            case S_CURRENT_REG_1:
                temp = (int16_t)val;
                hkStruct->Bat_Current = (float)temp * S_CURRENT_SCALING;
                break;

            case S_VOLTAGE_REG_1:
                val &= S_VOLTAGE_BITMASK;
                hkStruct->Bat_Voltage = (float)val * S_VOLTAGE_SCALING;
                break;

            case S_PACURRENT_REG_1:
                temp = (int16_t)val;
                hkStruct->PA_Current = (float)temp * S_CURRENT_SCALING;
                break;

            case S_PAVOLTAGE_REG_1:
                val &= S_VOLTAGE_BITMASK;
                hkStruct->PA_Voltage = (float)val * S_VOLTAGE_SCALING;
                break;
            }
        }
    }
    return S_SUCCESS;
}
