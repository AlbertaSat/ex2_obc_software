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
 * @file comms_hal.c
 * @author Arash Yazdani
 * @date 2020-10-01
 */

/*Once the hardware is connected, toggle this Macro*/
//#define Stubbed = 1;

#include "obc_hal.h"
#include "comms_hal.h"
#include "hal.h"
#include "services.h"
#include "queue.h"



/*Comments for the PR reviewer:
 * 2- Like RTC we can check if the value is stored after the
 * set functions (change the get functions).
 * But there is no point when eventually Stubbed will be 0.
 * 3- Do we need comments for each function?
 */

STX_return HAL_S_getFreq (float *S_freq) {
  #ifdef SBAND_IS_STUBBED
    *S_freq = (float)2210.5;
    return IS_STUBBED;
  #else
    return STX_getFrequency(*S_freq);
  #endif
};

STX_return HAL_S_getControl (Sband_PowerAmplifier *S_PA) {
  #ifdef SBAND_IS_STUBBED
    S_PA->status = 1;
    S_PA->mode = 3;
    return IS_STUBBED;
  #else
    return STX_getControl(&S_PA->status, &S_PA->mode);
  #endif
};

STX_return HAL_S_getEncoder (Sband_Encoder *S_Enc){
  #ifdef SBAND_IS_STUBBED
    S_Enc->scrambler = 1;
    S_Enc->filter = 6;
    S_Enc->modulation = 5;
    S_Enc->rate = 4;
    return IS_STUBBED;
  #else
    return STX_getEncoder&S_Enc->scrambler, &S_Enc->filter, &S_Enc->filter, &S_Enc->filter);
  #endif
}

STX_return HAL_S_getPAPower (uint32_t *S_PA_Power) {
  #ifdef SBAND_IS_STUBBED
    *S_PA_Power = 2;
    return IS_STUBBED;
  #else
    return STX_getPaPower(*S_paPower);
  #endif
};


STX_return HAL_S_getStatus (Sband_Status *S_status){
  #ifdef SBAND_IS_STUBBED
    S_status->PWRGD = 1;
    S_status->TXL = 1;
    return IS_STUBBED;
  #else
    return STX_getStatus(&S_status->PWRGD, &S_status->PWRGD);
  #endif
}

STX_return HAL_S_getTR (Sband_TR *S_transmit){
  #ifdef SBAND_IS_STUBBED
    S_transmit->transmit = 1;
    return IS_STUBBED;
  #else
    return STX_getTR(&S_transmit->transmit);
  #endif
}

STX_return HAL_S_getHK (Sband_Housekeeping *S_hk) {
  #ifdef SBAND_IS_STUBBED
    S_hk->Output_Power = 26;
    S_hk->PA_Temp = 27.3;
    S_hk->Top_Temp = -2.8;
    S_hk->Bottom_Temp = 11.7;
    S_hk->Bat_Current = 95;
    S_hk->Bat_Voltage = 7.2;
    S_hk->PA_Current = 0.48;
    S_hk->PA_Voltage = 5.1;
    return IS_STUBBED;
  #else
    return STX_getHK(*S_hk);
  #endif
}

/* The switch operation might be better implemented here than in EH */
STX_return HAL_S_getBuffer (int quantity, Sband_Buffer *S_buffer) {
    /* Although there is no writing data, we can call a function like them*/
  #ifdef SBAND_IS_STUBBED
    S_buffer->pointer[quantity] = quantity;
    return IS_STUBBED;
  #else
    return STX_getBuffer(quantity, &S_buffer->pointer[quantity])
  #endif
}

STX_return HAL_S_softResetFPGA (void) {
  #ifdef SBAND_IS_STUBBED
    return IS_STUBBED;
  #else
    return STX_softResetFPGA(void);
  #endif
}

STX_return HAL_S_getFS (float * S_firmware_Version) {
  #ifdef SBAND_IS_STUBBED
    *S_firmware_Version = 7.14;
    return IS_STUBBED;
  #else
    return STX_getFirmwareV( *S_firmware_Version);
  #endif
}

/*
void HAL_S_setFreq (uint32_t S_freq){
#ifdef Stubbed
    S_freq =
}
*/
