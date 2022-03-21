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
 * @file sband.c
 * @author Arash Yazdani
 * @date 2020-10-01
 */

/*
 * When TRX connected, the stubbed blocks can be used for TRX = off situation.
 */
#include <FreeRTOS.h>
#include <os_queue.h>

#include "sband.h"
#include "services.h"

// For storing the set data
static Sband_config S_config_reg;
static Sband_Full_Status S_FS;

STX_return HAL_S_getFreq(float *S_freq) {
    STX_return status;
#ifndef SBAND_IS_STUBBED
  status = STX_getFrequency(&S_config_reg.freq);
#else
  status = IS_STUBBED;
#endif
  *S_freq = S_config_reg.freq;
  return status;
};

STX_return HAL_S_getControl(Sband_PowerAmplifier *S_PA) {
    STX_return status;
#ifndef SBAND_IS_STUBBED
  status = STX_getControl(&S_config_reg.PA.status, &S_config_reg.PA.mode);
#else
  status = IS_STUBBED;
#endif
  *S_PA = S_config_reg.PA;
  return status;
};

STX_return HAL_S_getEncoder(Sband_Encoder *S_Enc) {
    STX_return status;
#ifndef SBAND_IS_STUBBED
  status = STX_getEncoder(&S_config_reg.enc.scrambler, &S_config_reg.enc.filter,
                          &S_config_reg.enc.modulation, &S_config_reg.enc.rate);
#else
  status = IS_STUBBED;
#endif
  *S_Enc = S_config_reg.enc;
  return status;
}

STX_return HAL_S_getPAPower(uint8_t *S_PA_Power) {
    STX_return status;
#ifndef SBAND_IS_STUBBED
  status = STX_getPaPower(&S_config_reg.PA_Power);
#else
  status = IS_STUBBED;
#endif
  *S_PA_Power = S_config_reg.PA_Power;
  return status;
};

STX_return HAL_S_getStatus(Sband_Status *S_status) {
    STX_return status;
#ifndef SBAND_IS_STUBBED
  status = STX_getStatus(&S_FS.status.PWRGD, &S_FS.status.TXL);
#else
  S_FS.status.PWRGD = 1;
  S_FS.status.TXL = 1;
  status = IS_STUBBED;
#endif
  *S_status = S_FS.status;
  return status;
}

STX_return HAL_S_getTR(Sband_TR *S_transmit) {
    STX_return status;
#ifndef SBAND_IS_STUBBED
  status = STX_getTR(&S_FS.transmit.transmit);
#else
  S_FS.transmit.transmit = 1;
  status = IS_STUBBED;
#endif
  *S_transmit = S_FS.transmit;
  return status;
}

STX_return HAL_S_getHK(Sband_Housekeeping *S_hk) {
    STX_return status;
#ifndef SBAND_IS_STUBBED
  status = STX_getHK(&S_FS.HK);
#else
  S_FS.HK.Output_Power = 26;
  S_FS.HK.PA_Temp = 27.3;
  S_FS.HK.Top_Temp = -2.8;
  S_FS.HK.Bottom_Temp = 11.7;
  S_FS.HK.Bat_Current = 95;
  S_FS.HK.Bat_Voltage = 7.2;
  S_FS.HK.PA_Current = 0.48;
  S_FS.HK.PA_Voltage = 5.1;
  status = IS_STUBBED;
#endif
  *S_hk = S_FS.HK;
  return status;
}

STX_return HAL_S_hk_convert_endianness(Sband_Housekeeping *S_hk) {
  STX_return status;
  S_hk->Output_Power = csp_htonflt(S_hk->Output_Power);
  S_hk->PA_Temp = csp_htonflt(S_hk->PA_Temp);
  S_hk->Top_Temp = csp_htonflt(S_hk->Top_Temp);
  S_hk->Bottom_Temp = csp_htonflt(S_hk->Bottom_Temp);
  S_hk->Bat_Current = csp_htonflt(S_hk->Bat_Current);
  S_hk->Bat_Voltage = csp_htonflt(S_hk->Bat_Voltage);
  S_hk->PA_Current = csp_htonflt(S_hk->PA_Current);
  S_hk->PA_Voltage = csp_htonflt(S_hk->PA_Voltage);
  S_hk->PA_Voltage = csp_htonflt(S_hk->PA_Voltage);
  return status;
}

/* The switch operation might be better implemented here than in EH */
STX_return HAL_S_getBuffer(int quantity, Sband_Buffer *S_buffer) {
    STX_return status;
  /* Although there is no writing data, we can call a function like them*/
#ifndef SBAND_IS_STUBBED
  status = STX_getBuffer(quantity, &S_FS.buffer.pointer[quantity]);
#else
  S_FS.buffer.pointer[quantity] = quantity;
  status = IS_STUBBED;
#endif
  *S_buffer = S_FS.buffer;
  return status;
}

STX_return HAL_S_softResetFPGA(void) {
#ifdef SBAND_IS_STUBBED
  return IS_STUBBED;
#else
  return STX_softResetFPGA();
#endif
}

STX_return HAL_S_getFV(float *S_firmware_Version) {
    STX_return status;
#ifndef SBAND_IS_STUBBED
  status = STX_getFirmwareV(&S_FS.Firmware_Version);
#else
  S_FS.Firmware_Version = 7.14;
  status = IS_STUBBED;
#endif
  *S_firmware_Version = S_FS.Firmware_Version;
  return status;
}

STX_return HAL_S_setFreq(float S_freq_new) {
  S_config_reg.freq = S_freq_new;
#ifdef SBAND_IS_STUBBED
  return IS_STUBBED;
#else
  return STX_setFreq(S_config_reg.freq);
#endif
}

STX_return HAL_S_setPAPower(uint8_t S_PA_Power_new) {
  S_config_reg.PA_Power = S_PA_Power_new;
#ifdef SBAND_IS_STUBBED
  return IS_STUBBED;
#else
  return STX_setPaPower(S_config_reg.PA_Power);
#endif
}

STX_return HAL_S_setControl(Sband_PowerAmplifier S_PA_new) {
  S_config_reg.PA = S_PA_new;
#ifdef SBAND_IS_STUBBED
  return IS_STUBBED;
#else
  return STX_setControl(S_config_reg.PA.status, S_config_reg.PA.mode);
#endif
}

STX_return HAL_S_setEncoder(Sband_Encoder S_enc_new) {
  S_config_reg.enc = S_enc_new;
#ifdef SBAND_IS_STUBBED
  return IS_STUBBED;
#else
  return STX_setEncoder(S_config_reg.enc.scrambler, S_config_reg.enc.filter,
                        S_config_reg.enc.modulation, S_config_reg.enc.rate);
#endif
}
