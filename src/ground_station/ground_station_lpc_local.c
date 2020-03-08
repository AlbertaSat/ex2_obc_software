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
#include <ground_station.h>
#include <packets/packet_base.h>
#ifdef PRINT_DIAG
#endif
#include <stdio.h>


/********************************************************************************/
/* Defines																		*/
/********************************************************************************/
#define GS_PERIOD 15


/********************************************************************************/
/* Private Singletons															*/
/********************************************************************************/


/********************************************************************************/
/* Virtual Method Declares														*/
/********************************************************************************/
static _Bool is_in_range( ground_station_t *gs )
{
	DEV_ASSERT( gs );

	static int counter = 0;

	if( counter > GS_PERIOD )
	{
		return true;
		counter = 0;
	}
	else
	{
		++counter;
		return false;
	}
}

static uint32_t read( ground_station_t *gs, uint8_t *data, uint32_t size, uint8_t port, block_time_t block )
{
	DEV_ASSERT( gs );
	DEV_ASSERT( data );
	printf("pee\n");

	return 0;
}

static uint32_t write( ground_station_t *gs, uint8_t *data, uint32_t size, uint8_t port, block_time_t block )
{
	DEV_ASSERT( gs );
	DEV_ASSERT( data );

	return size;
}


/********************************************************************************/
/* Initialization Method														*/
/********************************************************************************/
_Bool initialize_ground_station_lpc_local( ground_station_t *gs )
{
	DEV_ASSERT( gs );

	gs->is_in_range = is_in_range;
	gs->read = read;
	gs->write = write;

	return true;
}
