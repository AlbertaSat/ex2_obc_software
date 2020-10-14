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

#ifndef COMMUNICATION_SERVICE_H
#define COMMUNICATION_SERVICE_H

#include <FreeRTOS.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <stdio.h>

#include "services.h"
#include "service_response.h"
#include "service_utilities.h"
#include "system.h"

struct temp_utc {
  uint32_t sensor_temperature;
};

typedef struct __attribute__((packed)) {
  float freq;
  uint32_t PA_Power;
} Sband_config;

typedef struct __attribute__((packed)) {//must be uint8_t
    uint8_t status;
    uint8_t mode;
} Sband_PowerAmplifier;

typedef struct __attribute__((packed)) {
    uint8_t scrambler;
    uint8_t filter;
    uint8_t modulation;
    uint8_t rate;
} Sband_Encoder;

typedef struct __attribute__((packed)) {
    uint8_t PWRGD;
    uint8_t TXL;
} Sband_Status;

typedef struct __attribute__((packed)) {
    int transmit;
} Sband_TR;


typedef struct __attribute__((packed)) {
  float Output_Power;
  float PA_Temp;
  float Top_Temp;
  float Bottom_Temp;
  float Bat_Current;
  float Bat_Voltage;
  float PA_Current;
  float PA_Voltage;
} Sband_Housekeeping; //might be better if called from sTransmiter.h?

typedef struct __attribute__((packed)) {
  uint16_t pointer[3];
} Sband_Buffer;

typedef struct __attribute__((packed)) {
    Sband_Status status;
    Sband_TR transmit;
    Sband_Buffer buffer;
    Sband_Housekeeping HK;
    float Firmware_Version;
} Sband_Full_Status;

/*Valid values*/
#define MIN_FREQ = 2200;
#define MAX_FREQ = 2290;

SAT_returnState communication_service_app(csp_packet_t* pkt);

#endif /* COMMUNICATION_SERVICE_H_ */
