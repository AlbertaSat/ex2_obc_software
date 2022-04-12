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
 * @file
 * @author Daniel Sacro
 * @date
 */

#ifndef 2U_PAYLOAD_FT_SERVICE_H
#define 2U_PAYLOAD_FT_SERVICE_H

#include <FreeRTOS.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <stdio.h>

#include "services.h"

typedef enum {
    2U_PAYLOAD_DOWNLINK = 0,
    2U_PAYLOAD_UPLINK,
    2U_PAYLOAD_STOP_FT,
    2U_PAYLOAD_SEND_BYTES, // Prompts OBC to send N file bytes
    2U_PAYLOAD_PROCESS_BYTES, // Prompts OBC to process N file bytes
} 2U_payload_FT_Subtype;

SAT_returnState 2U_payload_FT_service_app(csp_packet_t *pkt);

SAT_returnState start_2U_payload_FT_service(void);

#endif /* 2U_PAYLOAD_FT_SERVICE_H */
