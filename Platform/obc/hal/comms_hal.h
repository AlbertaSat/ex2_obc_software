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

#include <inttypes.h>
#include <csp/csp.h>
#include "services.h"


void HAL_comm_getTemp(uint32_t *sensor_temperature);
void HAL_S_getFreq (uint32_t *S_freq);
void HAL_S_getpaPower (uint32_t *S_paPower);
void HAL_S_getControl (uint32_t *S_paStatus, uint32_t *S_paMode);
void HAL_S_getEncoder (uint8_t *S_enc_scrambler, uint8_t * S_enc_filter, uint8_t * S_enc_mod, uint8_t * S_enc_rate);
void HAL_S_getFirmwareV (float *S_Firmware_Version);
void HAL_S_getStatus (uint8_t * S_PWRGD, uint8_t *S_TXL);
void HAL_S_getTR (int *transmit);


#endif /* COMMS_HAL_H */
