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
 * @file gs_mocks.c
 * @author Brendan Bruner
 * @date May 29, 2015
 */

#include "dependency_injection.h"

#define GROUND_STATION_MAX_TX_LENGTH 50
static _Bool in_range;
static uint8_t p1 = 0, p2 = 0, p3 = 0;
static uint8_t telem_buffer1[GROUND_STATION_MAX_TX_LENGTH], telem_buffer2[GROUND_STATION_MAX_TX_LENGTH], telem_buffer3[GROUND_STATION_MAX_TX_LENGTH];
static uint32_t next_read1 = 0, next_read2 = 0, next_read3 = 0;

static _Bool is_in_range( ground_station_t *gs )
{
	return in_range;
}

static uint32_t read( ground_station_t *gs, uint8_t *packet, uint32_t size, uint8_t port, block_time_t block )
{
	/* copy 'telem_buffer' into 'packet' */
	printf("reading...\n");
	uint32_t iter;
	for( iter = 0; (iter < size && iter < GROUND_STATION_MAX_TX_LENGTH); ++iter )
	{
		packet[iter] = telem_buffer1[iter];
	}

	return iter;
}

static uint32_t read_dynamic( ground_station_t *gs, uint8_t *packet, uint32_t count, uint8_t port, block_time_t block )
{
	/* copy 'telem_buffer' into 'packet' */
	uint32_t iter;
	uint8_t *telem_buffer;
	uint32_t *next_read;

	if( p1 == 1 )
	{
		p1 = 0;
		next_read = &next_read1;
		telem_buffer = telem_buffer1;
	}
	else if( p2 == 1 )
	{
		p2 = 0;
		next_read = &next_read2;
		telem_buffer = telem_buffer2;
	}
	else if( p3 == 1 )
	{
		p3 = 0;
		next_read = &next_read3;
		telem_buffer = telem_buffer3;
	}
	else
	{
		return 0;
	}

	for( iter = 0; (iter < *next_read && iter < GROUND_STATION_MAX_TX_LENGTH); ++iter )
	{
		packet[iter] = telem_buffer[iter];
	}

	return iter;
}


static uint32_t write( ground_station_t *gs, uint8_t *packet, uint32_t size, uint8_t port, block_time_t block )
{
	/* copy 'packet' into 'telem_buffer' */
	uint32_t iter;

	for( iter = 0; (iter < size && iter < GROUND_STATION_MAX_TX_LENGTH); ++iter )
	{
		telem_buffer1[iter] = ((uint8_t *) packet)[iter];
	}

	return iter;
}

static uint32_t write_dynamic( ground_station_t *gs, uint8_t *packet, uint32_t size, uint8_t port, block_time_t block )
{
	/* copy 'packet' into 'telem_buffer' */
	uint32_t iter;
	uint8_t *telem_buffer;
	uint32_t *next_read;

	if( p1 == 0 )
	{
		p1 = 1;
		next_read = &next_read1;
		telem_buffer = telem_buffer1;
	}
	else if( p2 == 0 )
	{
		p2 = 1;
		next_read = &next_read2;
		telem_buffer = telem_buffer2;
	}
	else if( p3 == 0 )
	{
		p3 = 1;
		next_read = &next_read3;
		telem_buffer = telem_buffer3;
	}
	else
	{
		return 0;
	}

	for( iter = 0; (iter < size && iter < GROUND_STATION_MAX_TX_LENGTH); ++iter )
	{
		telem_buffer[iter] = packet[iter];
	}

	*next_read = iter;
	return iter;
}

void set_mock_up_gs_in_range( _Bool param )
{
	in_range = param;
}

void initialize_mock_up_ground_station( ground_station_t *gs )
{
	gs->is_in_range= is_in_range;
	gs->read = read;
	gs->write = write;
	in_range = true;
}

void initialize_mock_up_ground_station_dynamic_sizes( ground_station_t *gs )
{
	p1 = 0; p2 = 0; p3 = 0;
	next_read1 = 0; next_read2 = 0; next_read3 = 0;
	gs->is_in_range= is_in_range;
	gs->read = read_dynamic;
	gs->write = write_dynamic;
	in_range = true;
}
