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
 * @file ground_station.h
 * @author Brendan Bruner
 * @date May 27, 2015
 */
#ifndef INCLUDE_GROUND_STATION_H_
#define INCLUDE_GROUND_STATION_H_

// #include <packets/telemetry_packet.h>
#include <portable_types.h>
// #include <uart/uart.h>

/********************************************************************************/
/* Defines																		*/
/********************************************************************************/
#define GROUND_STATION_IN_RANGE 1
#define GROUND_STATION_OUT_OF_RANGE 0

#define PORT0 0
#define PORT1 1
#define PORT2 2
/* ... */
#define PORT255 255

#define FTP_PORT 			PORT0
#define TELECOMMAND_PORT 	PORT1
#define BEACON_PORT			PORT255

/********************************************************************************/
/* Structure Documentation														*/
/********************************************************************************/
/**
 * @struct ground_station_t
 * @brief
 * 		Provides methods for communicating to a ground station.
 *
 * @details
 * 		Provides methods for communicating to a ground station. This includes
 * 		polling when a ground station is near, transmitting data to a ground
 * 		station, and receiving data from a ground station.
 *
 * @var ground_station_t::is_in_range
 * 		A function pointer with prototype:
 * 			@code
 * 				_Bool is_in_range( ground_station_t * );
 * 			@endcode
 * 		The method returns a bool indicating if a ground station is in range of
 * 		the satellite for transmission and reception.
 *
 * 		<b>Returns</b>
 * 		<br><b>true</b>: If the ground station is in range.
 * 		<br><b>false</b>: If the ground station is out of range.
 *
 * @var ground_station_t::read
 * 		A function pointer with prototype:
 * 			@code
 * 				uint32_t read( ground_station_t *gs, uint8_t *data, uint32_t size, uint8_t port, block_time_t block );
 * 			@endcode
 * 		The method fills in the array <b>data</b> with incoming data from the specified port. A timeout can be specified,
 * 		which will suspend the calling task until incoming data is received or the timeout expires.
 *
 * 		<b>Parameters</b>
 * 		<ul>
 * 		<li><b>gs</b>: A pointer to the ground_station_t structure being used.</li>
 * 		<li><b>data</b>: A pointer to a uint8_t array which will hold the data. This must be at least
 * 		as long as <b>size</b>, the next argument.</li>
 * 		<li><b>size</b>: The number of bytes the <b>data</b> array can hold.</li>
 * 		<li><b>port</b>: The port to read data from.</li>
 * 		<li><b>block</b>: The maximum number of ms to suspend the calling task while waiting for data
 * 		from a ground station. Use <b>BLOCK_FOREVER</b> to wait forever.</li>
 * 		</ul>
 *
 * 		<b>Returns</b>
 * 		The amount of bytes read into the <b>data</b> array.
 *
 * @var ground_station_t::write
 * 		A function pointer with prototype:
 * 			@code
 * 				uint32_t write( ground_station_t *gs, uint8_t* data, uint32_t size, uint8_t port, block_time_t block );
 * 			@endcode
 * 		The method will transmit the given data to a ground station using
 * 		the specified port and time out.
 *
 * 		<b>Parameters</b>
 * 		<ul>
 * 		<li><b>gs</b>: A pointer to the ground_station_t structure being used.</li>
 * 		<li><b>data</b>: A pointer to a byte array of data to transmit.</li>
 * 		<li><b>size</b>: The size of the data array to write.</li>
 * 		<li><b>port</b>: The port to transmit the data over.</li>
 * 		<li><b>block</b>: The maximum number of ms to suspend the calling task while trying
 * 		to transmit. Use <b>BLOCK_FOREVER</b> to wait forever.</li>
 * 		</ul>
 *
 * 		<b>Returns</b>
 * 		<br>The number of bytes written to the ground station.
 */
typedef struct ground_station_t ground_station_t;


/********************************************************************************/
/* Structure Defines 															*/
/********************************************************************************/
struct ground_station_t
{
	_Bool (*is_in_range)( ground_station_t * );
	uint32_t (*read)( ground_station_t *, uint8_t *, uint32_t, uint8_t, block_time_t );
	uint32_t (*write)( ground_station_t *, uint8_t*, uint32_t, uint8_t, block_time_t );
};


/********************************************************************************/
/* Initialization Functions														*/
/********************************************************************************/
/**
 * @memberof ground_station_t
 * @brief
 * 		Initialize a ground station mock up that runs locally on an LPC17xx board.
 * @details
 * 		Initialize a ground station mock up that runs locally on an LPC17xx board.
 *		With this mock up, the ground station will randomly come in and out of range,
 *		generate pseudo random commands, and confirm most telemetry written.
 * @param gs
 * 		A pointer to the ground_station_t structure to initialize.
 * @returns
 * 		true or false. Do not use struct if false is returned.
 */
// _Bool initialize_ground_station_lpc_local( ground_station_t *gs );


// _Bool initialize_ground_station_nanomind( ground_station_t *gs );

// typedef void (*gs_free_packet_hook_t)(void*);
// _Bool ground_station_nanomind_insert( ground_station_t*, char const*, size_t );

// *
//  * @memberof ground_station_t
//  * @brief
//  * 		Initialize a ground station mock up that uses an arduino as the ground station.
//  * @details
//  * 		Initialize a ground station mock up that uses an arduino as the ground station.
//  * 		The arduino will function very closely to a real ground station.
//  * @param gs
//  * 		A pointer to the ground_station_t strucutre to initialize.
//  * @param uart
//  * 		A pointer to a uart_t structure to use for serial data transfer.
//  * @returns
//  * 		true or false. Do not use struct if false is returned.
 
// _Bool initialize_ground_station_uart( ground_station_t *gs, uart_t* uart );


/********************************************************************************/
/* Destructor																	*/
/********************************************************************************/
/**
 * @memberof @memberof ground_station_t
 * @brief destroy the structure
 * @details destroy the structure
 * @param[in] self A pointer to the structure to destroy
 */
void destroy_ground_station( ground_station_t *self );

#endif /* INCLUDE_GROUND_STATION_H_ */
