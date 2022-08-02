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
 * @file sband_binary_tests.c
 * @author Thomas Ganley
 * @date 2021-12-06
 */

// A simple test to transmit 20kB of data over S-band
#include "sband_binary_tests.h"

STX_return sband_binary_test(){
     STX_return ret = S_SUCCESS;
     // Enable the SBAND

     STX_Enable();
    vTaskDelay(2*ONE_SECOND);
    STX_setControl(S_PA_DISABLE, S_CONF_MODE);
    STX_setEncoder(S_BIT_ORDER_MSB, S_SCRAMBLER_DISABLE, S_FILTER_ENABLE,S_MOD_QPSK, S_RATE_FULL);
    uint8_t order, scrambler, filter, mod, rate;
    STX_getEncoder(&order, &scrambler, &filter, &mod, &rate);
    uint8_t pa_status, mode, pa_power;
    uint16_t count = 0, overrun = 0, underrun = 0;
    float freq = 0;

    printf("Configuration at START of test:\n");

    ret = STX_getControl(&pa_status, &mode);
    if(ret != S_SUCCESS) return ret;
    printf("pa_status: %d  mode: %d\n", pa_status, mode);

    ret = STX_getPaPower(&pa_power);
    if(ret != S_SUCCESS) return ret;
    printf("pa_power: %d\n", pa_power);

    ret = STX_getFrequency(&freq);
    if(ret != S_SUCCESS) return ret;
    printf("S-band frequency: %f\n", freq);

    ret = STX_getBuffer(S_BUFFER_COUNT, &count);
    if(ret != S_SUCCESS) return ret;
    printf("Buffer count: %d\n", count);

    ret = STX_getBuffer(S_BUFFER_UNDERRUN, &underrun);
    if(ret != S_SUCCESS) return ret;
    printf("Buffer underrun: %d\n", underrun);

    ret = STX_getBuffer(S_BUFFER_OVERRUN, &overrun);
    if(ret != S_SUCCESS) return ret;
    printf("Buffer overrun: %d\n\n", overrun);

    uint8_t filler[50] = "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU";
    uint8_t message[50] = "Quentinen and Tarantined by Writtin Directinoooooo";
    uint16_t filler_16[25] = {0};

    freq = 2228.0;
    ret = STX_setFrequency(freq);
    if(ret != S_SUCCESS) return ret;
    ret = STX_setPaPower(30u);
    if(ret != S_SUCCESS) return ret;
    ret = STX_setControl(S_PA_ENABLE, S_SYNC_MODE);
    if(ret != S_SUCCESS) return ret;

    for(int k = 0; k<25; k++){
        filler_16[k] = (filler[2*k] << 8) || filler[2*k+1];
    }
    uint8_t syncword[6] = {0xda, 0xdb, 0x0d, 0x3d};

    // Send message forever over SPI to fill buffer
    ret = STX_setControl(S_PA_ENABLE, S_DATA_MODE);
    for(int i = 0; i < 20; i++){
        // Loop 20 times aka 1kB
        SPISbandTx((uint8_t *) filler_16, 25);
    }
    SPISbandTx(syncword, sizeof(syncword));
    SPISbandTx(message, sizeof(message));

    ret = STX_getBuffer(0, &count);
    printf("Buffer count at start of transmission: %d\n", count);
    int transmit = 0;
    int i = 0;

    // Set transmitter to data mode to begin transmission
    ret = STX_setControl(S_PA_ENABLE, S_DATA_MODE);
    while(!transmit){
        ret = STX_getTR((uint8_t *)&transmit);
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
    if(ret != S_SUCCESS) return ret;
    printf("pa_status: %d  mode: %d\n", pa_status, mode);

    ret = STX_getPaPower(&pa_power);
    if(ret != S_SUCCESS) return ret;
    printf("pa_power: %d\n", pa_power);

    ret = STX_getFrequency(&freq);
    if(ret != S_SUCCESS) return ret;
    printf("S-band frequency: %f\n", freq);

    ret = STX_getBuffer(0, &count);
    if(ret != S_SUCCESS) return ret;
    printf("Buffer count: %d\n", count);

    ret = STX_getBuffer(1, &underrun);
    if(ret != S_SUCCESS) return ret;
    printf("Buffer underrun: %d\n", underrun);

    ret = STX_getBuffer(2, &overrun);
    if(ret != S_SUCCESS) return ret;
    printf("Buffer overrun: %d\n\n", overrun);
    return ret;
}

//Function behaviour:
//Configure device...
//Set to sync mode
//Fill buffer ("UUUUUUUU...Quentinen and Tarantined by Writtin DirectinoUUUUUU...")
//Set to data mode until buffer depleted
//Set to sync mode... repeat
STX_return sband_inf_tx(){

   STX_return ret = S_SUCCESS;
    // Enable the SBAND

   STX_Enable();
   vTaskDelay(2*ONE_SECOND);
   STX_setControl(S_PA_DISABLE, S_CONF_MODE);
   STX_setEncoder(S_BIT_ORDER_MSB, S_SCRAMBLER_DISABLE, S_FILTER_ENABLE,S_MOD_QPSK, S_RATE_FULL);

   uint8_t filler[50] = "UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU";
   uint8_t message[50] = "Quentinen and Tarantined by Writtin Directinoooooo";
   uint16_t filler_16[25] = {0};

   float freq = 2228;
   ret = STX_setFrequency(freq);
   if(ret != S_SUCCESS) return ret;
   ret = STX_setPaPower(30u);
   if(ret != S_SUCCESS) return ret;
   ret = STX_setControl(S_PA_ENABLE, S_SYNC_MODE);
   if(ret != S_SUCCESS) return ret;

   for(int k = 0; k<25; k++){
       filler_16[k] = (filler[2*k] << 8) || filler[2*k+1];
   }
   uint8_t syncword[6] = {0xda, 0xdb, 0x0d, 0x3d};

   // Send message forever over SPI to fill buffer
   while(1){

       ret = STX_setControl(S_PA_ENABLE, S_SYNC_MODE);
       for(int j = 0; j < 10; j++){
           for(int i = 0; i < 20; i++){
               // Loop 20 times aka 1kB
               SPISbandTx((uint8_t *) filler_16, sizeof(filler_16));
           }
           SPISbandTx(syncword, sizeof(syncword));
           SPISbandTx(message, sizeof(message));
           ret = STX_setControl(S_PA_ENABLE, S_DATA_MODE);
       }
       while(gioGetBit(hetPORT1, 25) == 0);//cpu hog
   }
}
