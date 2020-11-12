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

#ifndef SERVICES_H
#define SERVICES_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "subsystems_ids.h"

/* PACKETS */
#define TM_TC_BUFF_SIZE 256
#define SUBSERVICE_BYTE 0
#define STATUS_BYTE 1
#define IN_DATA_BYTE 1
#define OUT_DATA_BYTE 2
#define OUT_DATA_BYTE2 5 // 6 worked! why though? 32 bits?

/* SERVICES */
#define MAX_APP_ID 32     // number of CSP nodes (5-bits)
#define MAX_SERVICES 64   // number of CSP ports (6-bits)
#define MAX_SUBTYPES 256  // an 8-bit integer

typedef enum {
  OBC_APP_ID = _OBC_APP_ID_,
  EPS_APP_ID = _EPS_APP_ID_,
  ADCS_APP_ID = _ADCS_APP_ID_,
  COMMS_APP_ID = _COMMS_APP_ID_,
  GND_APP_ID = _GND_APP_ID_,
  DEMO_APP_ID = _DEMO_APP_ID_,
  LAST_APP_ID = _LAST_APP_ID_
} TC_TM_app_id;

typedef enum {
  SATR_PKT_ILLEGAL_APPID = 0,
  SATR_PKT_ILLEGAL_SUBSERVICE,
  SATR_OK,
  SATR_ERROR,
  SATR_RETURN_FROM_TASK,
  SATR_BUFFER_ERR,
  /*LAST*/
  SATR_LAST
} SAT_returnState;

/* services types & subtypes
 * Note: ports 0-7 are RESERVED by CSP. Don't use them.
 */

// TIME MANAGEMENT SERVICE
#define TC_TIME_MANAGEMENT_SERVICE 8
typedef enum {
  GET_TIME = 0,
  SET_TIME = 1
} Time_Management_Subtype;  // shared with EPS!

// HOUSEKEEPING SERVICE
#define TC_HOUSEKEEPING_SERVICE 9
typedef enum { TM_HK_PARAMETERS_REPORT = 0 } Housekeeping_Subtype;

// COMMUNICATION SERVICE
#define TC_COMMUNICATION_SERVICE 10
typedef enum {
    S_GET_TEMP = 0,
    S_GET_FREQ = 1,
    S_GET_CONTROL = 2,
    S_GET_ENCODER = 3,
    S_GET_PA_POWER = 4,
    S_GET_STATUS = 5,
    S_GET_TR = 6,
    S_GET_BUFFER = 7,
    S_GET_HK = 8,
    S_SOFT_RESET = 9,
    S_GET_FULL_STATUS = 10,
    S_SET_FREQ = 11,
    S_SET_CONTROL = 12,
    S_SET_ENCODER = 13,
    S_SET_PA_POWER = 14,
    S_GET_CONFIG = 15,
    S_SET_CONFIG = 16
} Sband_Subtype;

typedef enum {
    UHF_SET_STAT_CONTROL = 20,
    UHF_SET_FREQ,
    UHF_SET_PIPE_TIMEOUT,
    UHF_SET_BEACON_T,
    UHF_SET_AUDIO_T,
    UHF_SET_PARAMS, //Merge last 4
    UHF_RESTORE_DEFAULT,
    UHF_LOW_PWR,
    UHF_SET_DEST,
    UHF_SET_SRC,
    UHF_SET_MORSE,
    UHF_SET_MIDI,
    UHF_SET_BEACON_MSG,
    UHF_SET_I2C,
    UHF_WRITE_FRAM,
    UHF_SECURE,
    UHF_GET_FULL_STAT, // 0-244,249,250
    UHF_GET_CALL_SIGN, // 245-246
    UHF_GET_MORSE,
    UHF_GET_MIDI,
    UHF_GET_BEACON_MSG,
    UHF_GET_FRAM,
    //UHF_NEW_CMD, //not in here
} UHF_Subtype;

/* Utility definitions */
union _cnv {
  double cnvD;
  float cnvF;
  uint32_t cnv32;
  uint16_t cnv16[4];
  uint8_t cnv8[8];
};

SAT_returnState start_service_server(void);

#endif /* SERVICES_H */
