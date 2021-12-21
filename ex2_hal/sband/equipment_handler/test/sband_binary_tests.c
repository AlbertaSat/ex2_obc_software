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
 * @file sband_binary_test.c
 * @author Thomas Ganley
 * @date 2021-12-06
 */

// A simple test to transmit 20kB of data over S-band
#include "sband_binary_tests.h"

STX_return sband_binary_test(){
    // Enable the SBAND

    STX_Enable();
    vTaskDelay(2*ONE_SECOND);
    STX_setControl(S_PA_DISABLE, S_CONF_MODE);
    STX_setEncoder(S_SCRAMBLER_ENABLE, S_FILTER_ENABLE,S_MOD_QPSK, S_RATE_QUARTER);

    STX_return ret;
    uint8_t pa_status, mode, pa_power;
    uint16_t count = 0, overrun = 0, underrun = 0;
    float freq = 0;

    printf("Configuration at START of test:\n");

    ret = STX_getControl(&pa_status, &mode);
    if(ret != FUNC_PASS) return ret;
    printf("pa_status: %d  mode: %d\n", pa_status, mode);

    ret = STX_getPaPower(&pa_power);
    if(ret != FUNC_PASS) return ret;
    printf("pa_power: %d\n", pa_power);

    ret = STX_getFrequency(&freq);
    if(ret != FUNC_PASS) return ret;
    printf("S-band frequency: %f\n", freq);

    ret = STX_getBuffer(0, &count);
    if(ret != FUNC_PASS) return ret;
    printf("Buffer count: %d\n", count);

    ret = STX_getBuffer(1, &underrun);
    if(ret != FUNC_PASS) return ret;
    printf("Buffer underrun: %d\n", underrun);

    ret = STX_getBuffer(2, &overrun);
    if(ret != FUNC_PASS) return ret;
    printf("Buffer overrun: %d\n\n", overrun);


    uint8_t message[46] = "Quentinen and Tarantined by Writtin Directino ";
    uint16_t data[23] = {0};

    ret = STX_setFrequency(2228.0f);
    if(ret != FUNC_PASS) return ret;
    ret = STX_setPaPower(24u);
    if(ret != FUNC_PASS) return ret;
    ret = STX_setControl(S_PA_ENABLE, S_SYNC_MODE);
    if(ret != FUNC_PASS) return ret;


    for(int k = 0; k++; k<23){
        data[k] = (message[2*k] << 8) || message[2*k+1];
    }

    // Send message 200 times over SPI to fill buffer
    for(int i = 0; i < 400;i++){
        SPISbandTx(data, 23);
    }

    ret = STX_getBuffer(0, &count);
    printf("Buffer count at start of transmission: %d\n", count);
    int transmit = 0;
    int i = 0;

    // Set transmitter to data mode to begin transmission
    ret = STX_setControl(S_PA_ENABLE, S_DATA_MODE);
    while(!transmit){
        ret = STX_getTR(&transmit);
        i++;
    }

    // Set to Synchronization mode, PA on
    ret = STX_setControl(S_PA_ENABLE, S_SYNC_MODE);

    vTaskDelay(0.5* ONE_SECOND);

    printf("The transmit ready flag was checked %d times\n\n", i);

    // Set to Configuration mode, PA off
    ret = STX_setControl(S_PA_DISABLE, S_CONF_MODE);

    printf("Configuration at END of test:\n");

    ret = STX_getControl(&pa_status, &mode);
    if(ret != FUNC_PASS) return ret;
    printf("pa_status: %d  mode: %d\n", pa_status, mode);

    ret = STX_getPaPower(&pa_power);
    if(ret != FUNC_PASS) return ret;
    printf("pa_power: %d\n", pa_power);

    ret = STX_getFrequency(&freq);
    if(ret != FUNC_PASS) return ret;
    printf("S-band frequency: %f\n", freq);

    ret = STX_getBuffer(0, &count);
    if(ret != FUNC_PASS) return ret;
    printf("Buffer count: %d\n", count);

    ret = STX_getBuffer(1, &underrun);
    if(ret != FUNC_PASS) return ret;
    printf("Buffer underrun: %d\n", underrun);

    ret = STX_getBuffer(2, &overrun);
    if(ret != FUNC_PASS) return ret;
    printf("Buffer overrun: %d\n\n", overrun);
}
