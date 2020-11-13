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

#ifndef COMMS_HAL_H
#define COMMS_HAL_H

#include <csp/csp.h>
#include <inttypes.h>

#include "communication_service.h"
#include "services.h"

/* Should optimize this*/
#ifdef SBAND_IS_STUBBED
typedef enum {
  FUNC_PASS = 0,
  BAD_READ = 1,
  BAD_WRITE = 1,
  BAD_PARAM = 2,
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

STX_return HAL_S_getFreq(float *S_freq);
STX_return HAL_S_getPAPower(uint8_t *S_PA_power);
STX_return HAL_S_getControl(Sband_PowerAmplifier *S_PA);
STX_return HAL_S_getEncoder(Sband_Encoder *S_Enc);
STX_return HAL_S_getStatus(Sband_Status *S_status);
STX_return HAL_S_getTR(Sband_TR *S_transmit);
STX_return HAL_S_getHK(Sband_Housekeeping *S_hk);
STX_return HAL_S_getBuffer(int quantity, Sband_Buffer *S_buffer);
STX_return HAL_S_softResetFPGA(void);
STX_return HAL_S_getFV(float *S_firmware_Version);
STX_return HAL_S_setFreq(float S_freq_new);
STX_return HAL_S_setPAPower(uint8_t S_PA_Power_new);
STX_return HAL_S_setControl(Sband_PowerAmplifier S_PA_new);
STX_return HAL_S_setEncoder(Sband_Encoder S_enc_new);

#endif /* COMMS_HAL_H */
