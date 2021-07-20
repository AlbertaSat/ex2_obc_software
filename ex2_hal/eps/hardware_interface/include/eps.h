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
 * @file eps.h
 * @author Andrew Rooney, Dustin Wagner
 * @date 2020-12-28
 */
#ifndef EX2_SERVICES_PLATFORM_OBC_EPS_H_
#define EX2_SERVICES_PLATFORM_OBC_EPS_H_

#include <main/system.h>
#include <os_queue.h>

/* Binary Commands to EPS */

#define SET_TELEMETRY_PERIOD    255
#define EPS_REQUEST_TIMEOUT 1000
#define EPS_INSTANTANEOUS_TELEMETRY 7
#define EPS_POWER_CONTROL 14

QueueHandle_t eps_gatekeeper_q;

typedef struct eps_t eps_t;

typedef struct __attribute__((packed)) {
    uint32_t magicWord;
    uint8_t telemetry_id;
    uint32_t period_ms;
    uint32_t duration_s;
} set_telemetry_period_req_t;

typedef struct __attribute__((packed)) {
    uint8_t err;
    uint32_t unixTimestamp;
} get_time_resp_t;

typedef struct __attribute__((packed)) {
    uint8_t err;
    uint32_t unixTimestampInS;
    uint32_t secondFraction;
} get_last_pps_time_resp_t;

typedef struct __attribute__((packed)) {
    uint8_t error_code;
    uint32_t timeLeftInS;
} get_gs_wd_time_left_resp_t;

struct __attribute__((packed)) eps_instantaneous_telemetry {
    uint8_t cmd; //value 0
    int8_t status; //0 –on success
    double timestampInS; // with ms precision
    uint32_t uptimeInS;
    uint32_t bootCnt; // system startup count
    uint32_t wdt_gs_time_left; //seconds
    uint32_t wdt_gs_counter;
    uint16_t mpptConverterVoltage[4]; //mV
    uint16_t curSolarPanels[8]; //mA
    uint16_t vBatt; //mV
    uint16_t curSolar; //mA
    uint16_t curBattIn; //mA
    uint16_t curBattOut; //mA
    uint16_t curOutput[18]; //mA
    uint16_t AOcurOutput[2]; //mA
    uint16_t OutputConverterVoltage[8]; //mV
    uint8_t outputConverterState;
    uint32_t outputStatus; //18-bits
    uint32_t outputFaultStatus; //18-bits
    uint16_t protectedOutputAccessCnt;
    uint16_t outputOnDelta[18]; //seconds
    uint16_t outputOffDelta[18]; //seconds
    uint8_t outputFaultCnt[18];
    int8_t temp[14]; //1-4 –MPPT converter temp, 5-8 –output converter temp, 9 –on-board battery temp, 10–12 –external battery pack temp, 13-14 -output expander temp
    uint8_t battMode; //0 –critical, 1 –safe, 2 –normal, 3 –full
    uint8_t mpptMode; //0 –HW, 1 –manual, 2 –auto, 3 –auto with timeout
    uint8_t batHeaterMode; //0 –manual, 1 –auto
    uint8_t batHeaterState; //0 –off, 1 –on
    uint16_t PingWdt_toggles; // Total number of power channel toggles caused by failed ping watchdog
    uint8_t PingWdt_turnOffs; // Total number of power channel offs caused by failed ping watchdog
};

enum eps_mode {
    critical = 0,
    safe = 1,
    normal = 2,
    full = 3
};

typedef struct eps_instantaneous_telemetry eps_instantaneous_telemetry_t;
typedef enum eps_mode eps_mode_e;

SAT_returnState eps_refresh_instantaneous_telemetry();
eps_instantaneous_telemetry_t get_eps_instantaneous_telemetry();
void EPS_getHK(eps_instantaneous_telemetry_t* telembuf);
eps_mode_e get_eps_batt_mode();
void prv_instantaneous_telemetry_letoh (eps_instantaneous_telemetry_t *telembuf);
// If changing the two functions below, update system tasks, too.
uint8_t eps_get_pwr_chnl(uint8_t pwr_chnl_port);
int8_t eps_set_pwr_chnl(uint8_t pwr_chnl_port, bool status);
double csp_letohd(double d);

#endif /* EX2_SERVICES_PLATFORM_OBC_EPS_H_ */
