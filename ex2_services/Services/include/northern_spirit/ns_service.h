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
 * @file ns_service.h
 * @author Daniel Sacro, Thomas Ganley
 * @date 2022-06-24
 */

#ifndef FT_2U_PAYLOAD_SERVICE_H
#define FT_2U_PAYLOAD_SERVICE_H

#include "service_utilities.h"
#include "services.h"
#include <FreeRTOS.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <main/system.h>
#include "ns_payload.h"
#include <stdio.h>
#include <string.h>

typedef enum {
    NS_UPLOAD_ARTWORK,
    NS_CAPTURE_IMAGE,
    NS_CONFIRM_DOWNLINK,
    NS_GET_HEARTBEAT,
    NS_GET_FLAG,
    NS_GET_TELEMETRY,
    NS_GET_SW_VERSION
} ns_payload_service_subtype;

void ns_payload_service(void *param);
SAT_returnState ns_payload_service_app(csp_packet_t *pkt);

SAT_returnState start_ns_payload_service(void);

#endif /* FT_2U_PAYLOAD_SERVICE_H */
