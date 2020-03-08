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
 * @file ground_station_lpc_local.c
 * @author Brendan Bruner
 * @date May 27, 2015
 */
#include "ground_station.h"
#include <string.h>
struct gs_packet
{
	char* packet;
	size_t length;
};
static queue_t queue;
static _Bool is_init = false;


/********************************************************************************/
/* Virtual Method Declares														*/
/********************************************************************************/
static _Bool is_in_range( ground_station_t *gs )
{
	DEV_ASSERT( gs );

	/* Incoming CSP packet tells us if a ground station is beaconing us. */
	return false;
}

static uint32_t read( ground_station_t *gs, uint8_t *data, uint32_t size, uint8_t port, block_time_t block )
{
	DEV_ASSERT( gs );
	DEV_ASSERT( data );

	if( port != TELECOMMAND_PORT )
	{
		return 0;
	}

	struct gs_packet packet;

	if( queue_receive( queue, &packet, block ) == pdFALSE )
	{
		return 0;
	}

	size_t copy_size = packet.length <= size ? packet.length : size;
	memcpy( data, packet.packet, copy_size );
	vPortFree( packet.packet );

	return copy_size;
}

static uint32_t write( ground_station_t *gs, uint8_t* packet, uint32_t size, uint8_t port, block_time_t block )
{
	DEV_ASSERT( gs );
	DEV_ASSERT( packet );

	/* Send CSP packets to ground station (only if being beaconed) containing telemetry. */
	return 0;
}


_Bool ground_station_nanomind_insert( ground_station_t* gs, char const *data, size_t length )
{
	DEV_ASSERT( gs );
	DEV_ASSERT( data );

	struct gs_packet packet;
	packet.packet = pvPortMalloc( sizeof(char) * length );
	if(packet.packet == NULL)
	{
		return false;
	}
	memcpy( packet.packet, data, length );
	packet.length = length;
	if(queue_send(queue, &packet, 0) == pdFALSE)
	{
		vPortFree( packet.packet );
		return false;
	}
	return true;
}


/********************************************************************************/
/* Initialization Method														*/
/********************************************************************************/
_Bool initialize_ground_station_nanomind( ground_station_t *gs )
{
	DEV_ASSERT( gs );

	gs->is_in_range = is_in_range;
	gs->read = read;
	gs->write = write;

	if( is_init )
		return true;

	new_queue( queue, 20, sizeof(struct gs_packet) );
	if( queue == NULL )
		return false;

	is_init = true;
	return true;
}
