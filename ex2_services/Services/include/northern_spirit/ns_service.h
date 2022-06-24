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
 * @file ft_2u_payload_service.h
 * @author Daniel Sacro
 * @date
 */

#ifndef FT_2U_PAYLOAD_SERVICE_H
#define FT_2U_PAYLOAD_SERVICE_H

#include "service_utilities.h"
#include "services.h"
#include <FreeRTOS.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <main/system.h>
#include <ns_file_transfer.h>

typedef enum {
    FT_2U_PAYLOAD_DOWNLINK = 0,
    FT_2U_PAYLOAD_UPLINK,
    FT_2U_PAYLOAD_STOP_FT,
    FT_2U_PAYLOAD_SEND_BYTES, // Prompts OBC to send N file bytes
    FT_2U_PAYLOAD_PROCESS_BYTES, // Prompts OBC to process N file bytes
} FT_2U_payload_subtype;

SAT_returnState FT_2U_payload_service_app(csp_packet_t *pkt);

SAT_returnState start_FT_2U_payload_service(void);

#endif /* FT_2U_PAYLOAD_SERVICE_H */
