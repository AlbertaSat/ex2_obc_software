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
#include "main/system.h"

/* PACKETS */
#define TM_TC_BUFF_SIZE 256
#define SUBSERVICE_BYTE 0
#define STATUS_BYTE 1
#define IN_DATA_BYTE 1
#define OUT_DATA_BYTE 2

/* SERVICES */
#define MAX_APP_ID 32     // number of CSP nodes (5-bits)
#define MAX_SERVICES 64   // number of CSP ports (6-bits)
#define MAX_SUBTYPES 256  // an 8-bit integer
#define SERVICE_BACKLOG_LEN 3

#define NORMAL_TICKS_TO_WAIT 1
#define SERVICE_QUEUE_LEN 3
#define RESPONSE_QUEUE_LEN 3
#define CSP_PKT_QUEUE_SIZE sizeof(csp_packet_t*)

/* SERVICE SOCKETS */
// HOUSEKEEPING SERVICE
#define TC_HOUSEKEEPING_SERVICE 17
// TIME MANAGEMENT SERVICE
#define TC_TIME_MANAGEMENT_SERVICE 8
// COMMUNICATION_SERVICE
#define TC_COMMUNICATION_SERVICE 10
// GENERAL_SERVICE
#define TC_GENERAL_SERVICE 11
// UPDATER_SERVICE
#define TC_UPDATER_SERVICE 12
// LOGGING SERVICE
#define TC_LOGGER_SERVICE 13

typedef enum {
  OBC_APP_ID = _OBC_APP_ID_,
  EPS_APP_ID = _EPS_APP_ID_,
  ADCS_APP_ID = _ADCS_APP_ID_,
  COMMS_APP_ID = _COMMS_APP_ID_,
  GND_APP_ID = _GND_APP_ID_,
  DEMO_APP_ID = _DEMO_APP_ID_,
  LAST_APP_ID = _LAST_APP_ID_
} TC_TM_app_id;

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
