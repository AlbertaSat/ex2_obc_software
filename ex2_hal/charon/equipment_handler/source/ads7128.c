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
 * @file ads7128.c
 * @author Josh Lazaruk
 * @date 2021-09-10
 */

// No initial configuration, calibration, averaging filter currently implemented
// 01M1002JF 10 kOhm thermistors in series w/ 4.7 kOhm: voltage via
// https://www.vishay.com/thermistors/ntc-rt-calculator/ See TX2-ES-039 Charon Thermistor Calculations for
// derivation of values in voltageToTemperature()

#include "ads7128.h"
#include <i2c_io.h>

/**
 * @brief
 *      Convert voltage value from ADC to temperature value based off of thermistor properties and implemented
 * circuit
 * @param[in] voltage value where LSB = 0.8 mV
 * @param [out] temperature converted from voltage
 */
bool ads7128Init(void) {

    uint8_t data[3] = {0b00001000, 0x05, 0x00};

    if (i2c_Send(ADS7128_PORT, ADS7128_ADDR, 3, data) != 0) { // Set all channels as analog inputs
        return 1;
    }

    data[1] = 0x10;

    if (i2c_Send(ADS7128_PORT, ADS7128_ADDR, 3, data) != 0) { // Set SEQ_MODE to manual mode
        return 1;
    }

    data[1] = 0x04;

    if (i2c_Send(ADS7128_PORT, ADS7128_ADDR, 3, data) != 0) { // Set CONV_MODE to manual mode
        return 1;
    }
    return 0;
}

void voltageToTemperature(uint16_t voltage, int8_t *temperature) {

    // Inefficient as balls, currently
    // LUT based off of https://embeddedgurus.com/stack-overflow/2010/01/a-tutorial-on-lookup-tables-in-c/

    static const uint16_t lookup[126] = {
        // need directive to store in non-RAM?
        4040, 4036, 4032, 4027, 4023, 4018, 4013, 4008, 4002, 3996, 3990, 3983, 3976, 3969, 3962, 3953, 3945, 3936,
        3927, 3917, 3907, 3896, 3885, 3873, 3861, 3848, 3835, 3821, 3806, 3791, 3775, 3759, 3742, 3724, 3706, 3687,
        3667, 3646, 3625, 3603, 3581, 3557, 3533, 3508, 3483, 3456, 3429, 3401, 3373, 3344, 3313, 3283, 3251, 3219,
        3186, 3153, 3119, 3084, 3048, 3013, 2976, 2939, 2902, 2863, 2825, 2786, 2747, 2707, 2667, 2627, 2587, 2546,
        2505, 2464, 2423, 2382, 2341, 2299, 2258, 2217, 2176, 2135, 2094, 2053, 2013, 1973, 1933, 1894, 1854, 1816,
        1777, 1739, 1701, 1664, 1628, 1592, 1556, 1521, 1486, 1452, 1418, 1385, 1352, 1320, 1289, 1258, 1228, 1198,
        1169, 1141, 1113, 1086, 1059, 1032, 1007, 982,  957,  933,  910,  887,  864,  843,  821,  801,  780,  761};

    uint16_t olddelta = 5000; // larger than biggest possible newdelta to avoid issues
    uint16_t newdelta = 0;
    for (int i = 0; i < 126; i++) {
        newdelta = abs(voltage - lookup[i]);
        if (newdelta > olddelta) {

            *temperature = -40 + (i - 1); // Index 0 in lookup corresponds to -40 deg C. 1 index = +1 deg C
            break;
        }
        olddelta = newdelta;
    }
}

/*
 * @brief
 *      Gets the temperature of a single thermistor
 * @param[in] channel from 0 to 7 (location of specific temp sensor channels TBD)
 * @param[out] temperature value in deg C
 * @return
 *      1 if error, 0 if successful
 */
uint8_t readSingleTemp(uint8_t channel, int8_t *temperature) { // note int8 max = 127

    uint8_t txdata[3] = {0b00001000, 0x11, (0x00 + channel)};

    if (i2c_Send(ADS7128_PORT, ADS7128_ADDR, 3, txdata) != 0) { // set channel ID for readout
        return 1;
    }

    int8_t rxdata[2] = {0x00, 0x00};

    if (i2c_Receive(ADS7128_PORT, ADS7128_ADDR, 2, &rxdata) != 0) { // Conversion Start and Read Frames
        return 1;
    }

    uint16_t voltage = 0;
    voltage = ((uint16_t)(rxdata[0] << 8) | (uint16_t)(rxdata[1])) >> 4; // 1 LSB = 3.3V / 2^12 = 0.806 mV

    voltageToTemperature(voltage, temperature);

    return 0;
}

/*
 * @brief
 *      Gets the temperature of all 8 thermistors
 * @param[out] temperatures 8-cell array of temperature values in deg C
 * @return
 *      1 if error, 0 if successful
 */
uint8_t
readAllTemps(int8_t *temperatures) { // could rework to read out all 16 registers at once instead of in 8 chunks
    for (int i = 0; i < 8; i++) {
        if (readSingleTemp(i, &temperatures[i])) {
            return 1;
        }
    }
    return 0;
}
