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

#ifndef SERVICE_RESPONSE_H
#define SERVICE_RESPONSE_H

#include <csp/csp.h>

#include "services.h"

SAT_returnState queue_response(csp_packet_t *packet);
SAT_returnState start_service_response();

#endif /* SERVICE_RESPONSE_H */
