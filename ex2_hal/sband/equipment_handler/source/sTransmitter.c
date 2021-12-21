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

/**
 * @brief
 *              Function to read a register
 * @details
 *              Called in other functions
 * @param address
 *              Register address as defined in the HSTXC User Manual
 * @param ptr
 * 		Pointer to where the register's value should be stored
 * @return STX_return
 *              Success of the function defined in sTransmitter.h
 */
STX_return read_reg(uint8_t internal_address, uint8_t *answer) {
    i2cSetBaudrate(SBAND_I2C, 400);
    uint8_t command = internal_address;
    if (i2c_Send(SBAND_I2C, SBAND_I2C_ADD, MAX_SBAND_R_CMDLEN, &command) != 0) {
        return BAD_READ;
    }
    if (i2c_Receive(SBAND_I2C, SBAND_I2C_ADD, MAX_SBAND_R_ANSLEN, answer) != 0) {
        return BAD_READ;
    }
    return FUNC_PASS;

    // TODO: Reset I2C speed to what it was previously
}

/**
 * @brief
 *              Function to write to a register
 * @details
 *              Called in other functions
 * @param address
 *              Register address as defined in the HSTXC User Manual
 * @param ptr
 *              Pointer to the write value
 * @return STX_return
 *              Success of the function defined in sTransmitter.h
 */
STX_return write_reg(uint8_t internal_address, uint8_t val) {
    i2cSetBaudrate(SBAND_I2C, 400);
    uint8_t command[2] = {internal_address, val};
    if (i2c_Send(SBAND_I2C, SBAND_I2C_ADD, MAX_SBAND_W_CMDLEN, command) != 0) {
        return BAD_WRITE;
    }
    return FUNC_PASS;

    // TODO: Reset I2C speed to what it was previously
}

/**
 * @brief
 *              Function to combine readings from two registers
 * @details
 *              Called in set/get functions. Some values are stored in "16 bit
 * registers" using two 8-bit registers
 * @param b1
 *              Most significant byte of desired 16 bit value
 * @param b2
 *              Least significant byte of desired 16 bit value
 * @return uint16_t
 *              Returns appended value
 */
uint16_t append_bytes(uint8_t b1, uint8_t b2) {
    uint16_t b = (b1 << 8) | b2;
    return b;
}

/**
 * @brief
 *              Function to combine readings from two temperature sensor
 * registers
 * @details
 *              Called in temperature get functions. Some values are stored in
 * "16 bit registers" using two 8-bit registers
 * @param b1
 *              Most significant byte of desired 16 bit value
 * @param b2
 *              Least significant byte of desired 16 bit value
 * @return float
 *              Returns temperature in degrees Celsius
 */
float calculateTemp(uint16_t b) {
    float temperature = 0;
    b = b >> 4;
    if (b & 2048) {
        temperature = -0.0625f * (float)((~b & 4095) + 1);
    } else {
        temperature = 0.0625f * (float)(b);
    }
    return temperature;
}

/**
 * @brief
 *         Enable the S-band transmitter
 * @return void
 */
void STX_Enable(void) {
    gioSetBit(hetPORT2, 23, 1); // Het2 23 is the S-band enable pin
    gioSetBit(hetPORT2, 21, 1); // Het2 21 is the S-band nRESET pin
}

/**
 * @brief
 *         Disable the S-band transmitter
 * @return void
 */
void STX_Disable(void) {
    gioSetBit(hetPORT2, 23, 0); // 23 is the S-band enable pin on hetPort2
}



/**
 * @brief
 *              Register 0x00: Get the power amplifier and mode values
 * @param pa
 *              Where the read value for the power amplifier will be stored
 *              (1 = enabled, 0 = disabled)
 * @param mode
 *              Where the read value for the mode will be stored
 *              (0 = Configuration, 1 = Synchronization, 2 = Data, 3 = Test
 * data)
 * @return STX_return
 *              Success of the function defined in sTransmitter.h
 */
STX_return STX_getControl(uint8_t *pa, uint8_t *mode) {
    uint8_t rawValue = 0;
    if (read_reg(0x0, &rawValue) != FUNC_PASS) {
        return BAD_READ;
    } else {
        *pa = rawValue >> 7;
        *mode = rawValue & 3;
        return FUNC_PASS;
    }
}

/**
 * @brief
 *              Register 0x00: Set the power amplifier and mode values
 * @param new_pa
 *              PA write value
 *              (1 = enabled, 0 = disabled)
 * @param new_mode
 *              Mode write value
 *              (0 = Configuration, 1 = Synchronization, 2 = Data, 3 = Test
 * data)
 * @return STX_return
 *              Success of the function defined in sTransmitter.h
 */
STX_return STX_setControl(uint8_t new_pa, uint8_t new_mode) {
    if (new_mode > 3 || new_pa > 1) {
        return BAD_PARAM;
    }

    new_mode |= (new_pa << 7);

    if (write_reg(0x0, new_mode) != FUNC_PASS) {
        return BAD_WRITE;
    } else {
        return FUNC_PASS;
    }
}

/**
 * @brief
 *              Register 0x01: Get the scrambler, filter, modulation, and rate
 * values
 * @param scrambler
 *              Where the read value for the scrambler will be stored
 *              (0 = enabled, 1 = disabled)
 * @param filter
 *              Where the read value for the filter will be stored
 *              (0 = enabled, 1 = disabled)
 * @param mod
 * 		Where the read value for the modulation scheme will be stored
 * 		(0 = QPSK, 1 = OQPSK)
 * @param rate
 * 		Where the read value for the rate will be stored
 * 		(0 = Full rate, 1 = Half rate)
 * @return STX_return
 *              Success of the function defined in sTransmitter.h
 */
STX_return STX_getEncoder(uint8_t *scrambler, uint8_t *filter, uint8_t *mod, uint8_t *rate) {
    uint8_t rawValue = 0;
    if (read_reg(0x01, &rawValue) != FUNC_PASS) {
        return BAD_READ;
    } else {
        *rate = rawValue & 3;
        *mod = 1 & (rawValue >> 2);
        *filter = 1 & (rawValue >> 3);
        *scrambler = 1 & (rawValue >> 4);
        return FUNC_PASS;
    }
}

/**
 * @brief
 *              Register 0x03: Set the scrambler, filter, modulation, and rate
 * values
 * @param new_scrambler
 *              Write value for the scrambler
 *              (0 = enabled, 1 = disabled)
 * @param new_filter
 *              Write value for the filter
 *              (0 = enabled, 1 = disabled)
 * @param new_mod
 *              Write value for the modulation scheme
 *              (0 = QPSK, 1 = OQPSK)
 * @param new_rate
 *              Write value for the rate
 *              (0 = Full rate, 1 = Half rate)
 * @return STX_return
 *              Success of the function defined in sTransmitter.h
 */
STX_return STX_setEncoder(uint8_t new_scrambler, uint8_t new_filter, uint8_t new_mod, uint8_t new_rate) {
    if (new_rate > 2 || new_mod > 1 || new_filter > 1 || new_scrambler > 1) {
        return BAD_PARAM;
    }

    new_rate = (new_rate) | (new_mod << 2) | (new_filter << 3) | (new_scrambler << 4);

    uint8_t mode = 0, pa = 0;
    if (STX_getControl(&pa, &mode) == FUNC_PASS) {
        if (mode == 0) {
            if (write_reg(0x01, new_rate)) {
                return BAD_WRITE;
            } else {
                return FUNC_PASS;
            }
        } else {
            return BAD_PARAM;
        }

    } else {
        return BAD_READ;
    }
}

/**
 * @brief
 *              Register 0x03: Get the power amplifier power
 * @attention
 * 		Stores the actual value (24, 26, 28, 30 dBm) at pointer
 * @param power
 *              Where the read value for the power will be stored
 * @return STX_return
 *              Success of the function defined in sTransmitter.h
 */
STX_return STX_getPaPower(uint8_t *power) {
    uint8_t rawValue = 0;

    if (read_reg(0x03, &rawValue) != FUNC_PASS) {
        return BAD_READ;
    } else {
        switch (rawValue) {
        case 0:
            *power = 24;
            break;
        case 1:
            *power = 26;
            break;
        case 2:
            *power = 28;
            break;
        case 3:
            *power = 30;
            break;
        default:
            return BAD_PARAM;
        }
        return FUNC_PASS;
    }
}

/**
 * @brief
 *              Register 0x03: Set the power amplifier power
 * @attention
 *              Input is the actual power (24, 26, 28, 30 dBm)
 * @param new_paPower
 *              Write value of the new PA power
 * @return STX_return
 *              Success of the function defined in sTransmitter.h
 */
STX_return STX_setPaPower(uint8_t new_paPower) {
    uint8_t rawValue = 0;

    switch (new_paPower) {
    case 24:
        rawValue = 0;
        break;
    case 26:
        rawValue = 1;
        break;
    case 28:
        rawValue = 2;
        break;
    case 30:
        rawValue = 3;
        break;
    default:
        return BAD_PARAM;
    }

    if (write_reg(0x03, rawValue) != FUNC_PASS) {
        return BAD_WRITE;
    } else {
        return FUNC_PASS;
    }
}

/**
 * @brief
 *              Register 0x04: Get the frequency
 * @attention
 *              Stores the actual frequency (MHz) at pointer
 * @param freq
 *       	Where the read value for the frequency will be stored
 * @return STX_return
 *              Success of the function defined in sTransmitter.h
 */
STX_return STX_getFrequency(float *freq) {
    uint8_t offset = 0;
    if (read_reg(0x04, &offset) != FUNC_PASS) {
        return BAD_READ;
    } else {
        *freq = (float)offset / 2 + 2200.0f;
        return FUNC_PASS;
    }
}

/**
 * @brief
 *              Register 0x04: Set a new frequency
 * @attention
 *              Input is the actual frequency (MHz)
 * @param new_frequency
 *              Write value for the frequency
 * @return STX_return
 *              Success of the function defined in sTransmitter.h
 */
STX_return STX_setFrequency(float new_frequency) {
    // TODO: This code will need to be modified for use on FMs which use amateur frequencies
    if (new_frequency >= 2200.0f && new_frequency <= 2300.0f) {
        uint8_t offset = (uint8_t)((new_frequency - 2200.0f) * 2);

        if (write_reg(0x04, offset) != FUNC_PASS) {
            return BAD_WRITE;
        } else {
            return FUNC_PASS; // Successful Write
        }
    } else {
        return BAD_PARAM;
    }
}

/**
 * @brief
 *              Register 0x05: Reset the FPGA logic
 * @attention
 * 		Resets all register to defaults (see options sheet)
 * @return STX_return
 *              Success of the function defined in sTransmitter.h
 */
STX_return STX_softResetFPGA(void) {
    if (write_reg(0x05, 0x0) != FUNC_PASS) {
        return BAD_WRITE;
    } else {
        return FUNC_PASS;
    }
}

/**
 * @brief
 *              Register 0x11: Get the firmware version
 * @attention
 *              Value of 7.01 means "7.1", value of 7.14 means "7.14"
 * @param version
 * 		Where the read value of the firmware version will be stored
 * @return STX_return
 *              Success of the function defined in sTransmitter.h
 */
STX_return STX_getFirmwareV(float *version) {
    uint8_t rawValue = 0;
    if (read_reg(0x11, &rawValue) != FUNC_PASS) {
        return BAD_READ;
    } else {
        float major = (float)(rawValue >> 4);
        float minor = (float)(rawValue & 15);
        minor /= 100.0f;
        *version = (major + minor);
        return FUNC_PASS;
    }
}

/**
 * @brief
 *              Register 0x12: Get the status of the frequency lock and the PA
 * @param pwrgd
 *              Where the read value of the PA power status will be stored
 *		(0 = not good, 1 = good)
 * @param txl
 *              Where the read value of the frequency lock will be stored
 *              (0 = achieved, 1 = not achieved)
 * @return STX_return
 *              Success of the function defined in sTransmitter.h
 */
STX_return STX_getStatus(uint8_t *pwrgd, uint8_t *txl) {
    uint8_t rawValue = 0;
    if (read_reg(0x12, &rawValue) != FUNC_PASS) {
        return BAD_READ;
    } else {
        *pwrgd = (rawValue & 2) >> 1;
        *txl = rawValue & 1;
        return FUNC_PASS;
    }
}

/**
 * @brief
 *              Register 0x13: Get the status of the frequency lock and the PA
 * @attention
 * 		Same as hard line on PC/104 header, but as a pollable register
 * @param trasmit
 *              Where the read value of the Transmit Ready indicator will be
 * stored (0 = more than 2650 bytes in buffer, 1 = less than 2561 bytes in
 * buffer)
 * @return STX_return
 *              Success of the function defined in sTransmitter.h
 */
STX_return STX_getTR(int *transmit) {
    uint8_t rawValue = 0;
    if (read_reg(0x13, &rawValue) != FUNC_PASS) {
        return BAD_READ;
    } else {
        *transmit = rawValue & 1;
        return FUNC_PASS;
    }
}

/**
 * @brief
 *              Registers 0x14 through 0x19: Get various buffer paramters
 * @details
 *    		One of three buffer quantities can be selected: Count, Underrun,
 * Overrun
 * @param quantity
 *              Select the buffer quantity
 *              (0 = Count, 1 = Underrun, 2 = Overrun)
 * @param ptr
 * 		Where the read quantity will be stored
 * @return STX_return
 *              Success of the function defined in sTransmitter.h
 */
STX_return STX_getBuffer(uint8_t quantity, uint16_t *ptr) {
    uint8_t rawValue1 = 0;
    uint8_t rawValue2 = 0;
    uint8_t address = 0;

    switch (quantity) {
    case 0: // Buffer Count
        address = 0x18;
        break;
    case 1: // Buffer Underrun
        address = 0x14;
        break;
    case 2: // Buffer Overrun
        address = 0x16;
        break;
    default:
        return BAD_PARAM;
    }

    if (read_reg(address, &rawValue1) != FUNC_PASS) {
        return BAD_READ;
    } else if (read_reg(address + 1, &rawValue2) != FUNC_PASS) {
        return BAD_READ;
    } else {
        *ptr = append_bytes(rawValue1, rawValue2);
        return FUNC_PASS;
    }
}

/**
 * @brief
 *              Registers 0x1A through 0x29: Collect housekeeping data
 * @details
 *		Collects RF ouput power, PA, board top, and board bottom
 *temperature, Battery & PA Current and Voltage
 * @attention
 *      	Stored in an sBand_housekeeping structure (see sTransmitter.h)
 * @param hkStruct
 *              Pointer to structure
 * @return STX_return
 *              Success of the function defined in sTransmitter.h
 */
STX_return STX_getHK(sBand_housekeeping *hkStruct) {
    uint16_t val = 0;
    int16_t temp = 0;

    uint8_t address = 0x1A;

    for (address; address < 0x29; address = address + 2) {
        uint8_t val1 = 0, val2 = 0;

        if (read_reg(address, &val1) != FUNC_PASS) {
            return BAD_READ;
        } else if (read_reg(1 + address, &val2) != FUNC_PASS) {
            return BAD_READ;
        } else {
            val = append_bytes(val1, val2);
            switch (address) {
            case 0x1A:
                val &= 4095;
                hkStruct->outputPower = ((float)val * (7.0f / 6144.0f));
                break;
            case 0x1C:
                val &= 4095;
                hkStruct->paTemp = (((float)val * 3.0f / 4096.0f) - 0.5f) * 100.0f;
                break;
            case 0x1E:
                hkStruct->topTemp = calculateTemp(val);
                break;
            case 0x20:
                hkStruct->bottomTemp = calculateTemp(val);
                break;
            case 0x22:
                temp = (int16_t)val;
                hkStruct->batCurrent = (float)temp * 0.00004f;
                break;
            case 0x24:
                val &= 8191;
                hkStruct->batVoltage = (float)val * 0.004f;
                break;
            case 0x26:
                temp = (int16_t)val;
                hkStruct->paCurrent = (float)temp * 0.00004f;
                break;
            case 0x28:
                val &= 8191;
                hkStruct->paVoltage = (float)val * 0.004f;
                break;
            }
        }
    }
    return FUNC_PASS;
}
