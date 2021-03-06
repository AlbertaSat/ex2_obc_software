/*
 * eps.h
 *
 *  Created on: Dec. 28, 2020
 *      Author: Andrew
 */

#ifndef EX2_SERVICES_PLATFORM_OBC_EPS_H_
#define EX2_SERVICES_PLATFORM_OBC_EPS_H_

#include <main/system.h>
#include <os_queue.h>

/* Binary Commands to EPS */

#define SET_TELEMETRY_PERIOD    255
#define EPS_GATEKEEPER_LEN      5
#define EPS_REQUEST_TIMEOUT 1000
#define EPS_INSTANTANEOUS_TELEMETRY 20

#define EPS_APP_ID 4

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
    uint16_t mpptConverterVoltage[4]; //mV
    uint16_t curSolarPanels[8]; //mA
    uint16_t vBatt; //mV
    uint16_t curSolar; //mA
    uint16_t curBattIn; //mA
    uint16_t curBattOut; //mA
    uint16_t reserved1; //reserved for future use
    uint16_t curOutput[10]; //mA
    uint16_t AOcurOutput[2]; //mA
    uint16_t OutputConverterVoltage[4]; //mV
    uint8_t outputConverterState[4];
    uint16_t outputStatus; //10-bits
    uint16_t outputFaultStatus; //10-bits
    uint16_t outputOnDelta[10]; //seconds
    uint16_t outputOffDelta[10]; //seconds
    uint32_t outputFaultCnt[10];
    uint16_t reserved2[6]; //reserved for future use
    uint32_t wdt_gs_time_left; //seconds
    uint16_t reserved3[7]; //reserved for future use
    uint32_t wdt_gs_counter;
    uint16_t reserved4[4]; //reserved for future use
    int16_t temp[12]; //0-3 –MPPT converter temp, 4-7 –output converter temp, 8 –on-board battery temp, 9 –11 –external battery pack temp
    uint32_t rstReason; //0x04000000 –NRST_pin, 0x14000000 –Software, 0x0C800000 –Power_on, 0x24000000 –IWDG
    uint32_t bootCnt;
    uint8_t battMode; //0 –critical, 1 –safe, 2 –normal, 3 –full
    uint8_t mpptMode; //0 –HW, 1 –manual, 2 –auto, 3 –auto with timeout
    uint8_t batHeaterMode; //0 –manual, 1 –auto
    uint8_t batHeaterState; //0 –off, 1 –on
    uint16_t reserved5; //reserved for future use

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
eps_mode_e get_eps_batt_mode();
void prv_instantaneous_telemetry_letoh (eps_instantaneous_telemetry_t *telembuf);

#endif /* EX2_SERVICES_PLATFORM_OBC_EPS_H_ */
