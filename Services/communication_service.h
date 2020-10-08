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

struct Sband_config {
  uint32_t S_freq;
  uint32_t S_paPower;
};

struct Sband_PowerAmplifier {//must be uint8_t
    uint32_t PA_status;
    uint32_t PA_mode;
};

struct Sband_Encoder{
    uint8_t Enc_scrambler;
    uint8_t Enc_filter;
    uint8_t Enc_modulation;
    uint8_t Enc_rate;
};

struct Sband_Firmware{
    float Version;
};

struct Sband_Status{
    uint8_t PWRGD;
    uint8_t TXL;
};

struct Sband_buffer{
    int transmit;
};

/*Valid values*/
#define MIN_FREQ = 2200;
#define MAX_FREQ = 2290;

SAT_returnState communication_service_app(csp_packet_t* pkt);
//int WRITEDATA_ISOK(int val, int subservice);

#endif /* COMMUNICATION_SERVICE_H_ */
