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


/* This is a test to see if the data is sent back successfully.
 * Once decided on how, the real hal connected to equipment handler
 * will be implemented */
void HAL_comm_getTemp(uint32_t *sensor_temperature) {
  #ifdef Stubbed
    *sensor_temperature = 18;
  #else
    HAL_get_temperature(sensor_temperature);
  #endif
}

void HAL_S_getFreq (uint32_t *S_freq) {
  #ifdef Stubbed
    *S_freq = 2210;
  #else
    get_S_frequency(*S_freq);
  #endif
};

void HAL_S_getpaPower (uint32_t *S_paPower) {
  #ifdef Stubbed
    *S_paPower = 2210;
  #else
    get_S_paPower(*S_paPower);
  #endif
};
