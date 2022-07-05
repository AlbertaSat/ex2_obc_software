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
 * @file dfgm_service.h
 * @author Daniel Sacro
 * @date 2022-02-08
 */

#ifndef DFGM_SERVICE_H
#define DFGM_SERVICE_H

#include <FreeRTOS.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <stdio.h>

#include "services.h"

typedef enum { DFGM_RUN = 0, DFGM_START, DFGM_STOP, DFGM_GET_HK } DFGM_Subtype;

SAT_returnState dfgm_service_app(csp_packet_t *pkt);

SAT_returnState start_dfgm_service(void);

#endif /* DFGM_SERVICE_H */
