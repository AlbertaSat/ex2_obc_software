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
#define Stubbed = 1;

#include "obc_hal.h"
#include "comms_hal.h"
#include <stddef.h>
#include <FreeRTOS.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <TempSensor/TempSensor.h>
#include "hal.h"
#include "services.h"
#include "queue.h"


/* This function is kept for test purposes. Will be deleted */
void HAL_comm_getTemp(uint32_t *sensor_temperature) {
  #ifdef Stubbed
    *sensor_temperature = 18;
  #else
    HAL_get_temperature(sensor_temperature);
  #endif
}

/*Comments for the PR reviewer:
 * 1- STX functions return value. Should we use them for the
 * SAT_returnState in communication_service_app?
 * 2- Like RTC we can check if the value is stored after the
 * set functions (change the get functions).
 * But there is no point when eventually * Stubbed will be 0.
 */

void HAL_S_getFreq (uint32_t *S_freq) {
  #ifdef Stubbed
    *S_freq = 2210;
  #else
    STX_getFrequency(*S_freq);
  #endif
};

void HAL_S_getpaPower (uint32_t *S_paPower) {
  #ifdef Stubbed
    *S_paPower = 2;
  #else
    STX_getPaPower(*S_paPower);
  #endif
};

void HAL_S_getControl (uint32_t *S_paStatus, uint32_t *S_paMode) { //must be uint8-t
  #ifdef Stubbed
    *S_paStatus = 1;
    *S_paMode = 3;
  #else
    STX_getControl(*S_paStatus, *S_paMode);
  #endif
};

void HAL_S_getEncoder (uint8_t *S_enc_scrambler, uint8_t * S_enc_filter, uint8_t * S_enc_mod, uint8_t * S_enc_rate){
  #ifdef Stubbed
    *S_enc_scrambler = 0;
    *S_enc_filter = 0;
    *S_enc_mod = 0;
    *S_enc_rate = 0;
  #else
    STX_getEncoder(uint8_t * S_enc_scrambler, uint8_t * S_enc_filter, uint8_t * S_enc_mod, uint8_t * S_enc_rate);
  #endif
}
