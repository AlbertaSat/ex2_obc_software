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
#include "system.h"

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
  SATR_OK = 1,
  SATR_ERROR = 2,
  /*LAST*/
  SATR_LAST = 56
} SAT_returnState;

/* services types & subtypes
 * Note: ports 0-7 are RESERVED by CSP. Don't use them.
 */
#define TC_VERIFICATION_SERVICE 8

#define TC_HOUSEKEEPING_SERVICE 9
typedef enum {
  TM_HK_PARAMETERS_REPORT = 0
} Housekeeping_Subtype;

#define TC_EVENT_SERVICE 10

#define TC_FUNCTION_MANAGEMENT_SERVICE 11

#define TC_TIME_MANAGEMENT_SERVICE 12
typedef enum {
  TM_TIME_SET_IN_UTC = 0
} Time_Management_Subtype;

#define TM_HK_PARAMETERS_REPORT 0


/* Utility definitions */
union _cnv {
  double cnvD;
  float cnvF;
  uint32_t cnv32;
  uint16_t cnv16[4];
  uint8_t cnv8[8];
};

#endif /* SERVICES_H */
