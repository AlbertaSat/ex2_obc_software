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


#define MAX_W_CMDLEN 120 //from uTransceiver.h
#define STAT_WORD_LEN 12

struct temp_utc {
  uint32_t sensor_temperature;
};

typedef struct __attribute__((packed)) {
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
  float freq;
  uint8_t PA_Power;
  Sband_PowerAmplifier PA;
  Sband_Encoder enc;
} Sband_config;

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


typedef struct __attribute__((packed)) {
    uint32_t freq;
    uint16_t PIPE_t;
    uint16_t beacon_t;
    uint16_t audio_t;
} UHF_Settings;

typedef struct __attribute__((packed)) {
    uint8_t status_ctrl[12];
    UHF_Settings set;
    uint32_t uptime;
    uint32_t pckts_out;
    uint32_t pckts_in;
    uint32_t pckts_in_crc16;
    float temperature;
    uint8_t low_pwr_stat;
    uint8_t firmware_ver;
    uint16_t payload_size;
    uint32_t secure_key;
} UHF_Status;

typedef struct __attribute__((packed)) {
    uint8_t confirm;
} UHF_Confirm;

typedef struct __attribute__((packed)) {
    uint8_t add;
} UHF_Address;

typedef struct {//from uTransceiver.h
  uint8_t len;
  uint8_t message[MAX_W_CMDLEN];
} UHF_configStruct;

typedef struct {//from uTransceiver.h
  uint32_t add;//address
  uint8_t data[16];
} UHF_framStruct;

typedef struct __attribute__((packed)) {
    UHF_configStruct dest;
    UHF_configStruct src;
} UHF_Call_Sign;

typedef struct __attribute__((packed)) {
    UHF_configStruct morse;
    UHF_configStruct MIDI;
    UHF_configStruct message;
} UHF_Beacon;


/*Valid values*/
#define MIN_FREQ = 2200;
#define MAX_FREQ = 2290;

SAT_returnState communication_service_app(csp_packet_t* pkt);

#endif /* COMMUNICATION_SERVICE_H_ */
