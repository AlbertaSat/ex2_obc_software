/*
 * Copyright (C) 2015  Brendan Bruner
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
 *
 * bbruner@ualberta.ca
 */
/**
 * @file telemetry_packet_base.c
 * @author Brendan Bruner
 * @date May 29, 2015
 */

#include <packets/telemetry_packet.h>


/********************************************************************************/
/* Public Globals																*/
/********************************************************************************/
uint32_t const packet_map_type_to_size[PACKET_MAX_TYPES] = { 260,
															 PACKET_TYPE_DFGM_SIZE,
															 PACKET_TYPE_MNLP_SIZE,
															 PACKET_TYPE_CMND_STATUS_SIZE,
															 PACKET_TYPE_STATE_SIZE };
