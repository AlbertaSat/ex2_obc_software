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
 * @file packet_base.h
 * @author Brendan Bruner
 * @date Jul 2, 2015
 */
#ifndef INCLUDE_PACKET_BASE_H_
#define INCLUDE_PACKET_BASE_H_

#include <stdint.h>

/********************************************************************************/
/* Public Method Declares														*/
/********************************************************************************/
/*
 * Convert a uint32_t into a network endian 4 byte array.
 */
void to_network_from32( uint32_t *, uint8_t * );
/*
 * Convert a uint16_t into a network endian 2 byte array.
 */
void to_network_from16( uint16_t *, uint8_t * );
/*
 * Convert a uint8_t into a network endian 1 byte array.
 */
void to_network_from8( uint8_t *, uint8_t * );


/*
 * Convert a uint8 network endian 4 byte array to a uint32_t.
 */
void to32_from_network( uint32_t *, uint8_t * );
/*
 * Convert a uint8 network endian 2 byte array to a uint16_t.
 */
void to16_from_network( uint16_t *, uint8_t * );
/*
 * Convert a uint8 network endian 1 byte array to a uint8_t.
 */
void to8_from_network( uint8_t *, uint8_t * );


void to_little_endian_from32( uint32_t*, uint8_t* );
void to_little_endian_from16( uint16_t*, uint8_t* );
void to_little_endian_from8( uint8_t*, uint8_t* );

void to32_from_little_endian( uint32_t*, uint8_t* );
void to16_from_little_endian( uint16_t*, uint8_t* );
void to8_from_little_endian( uint8_t*, uint8_t* );


void to_big_endian_from32( uint32_t*, uint8_t* );
void to_big_endian_from16( uint16_t*, uint8_t* );
void to_big_endian_from8( uint8_t*, uint8_t* );


void to32_from_big_endian( uint32_t*, uint8_t* );
void to16_from_big_endian( uint16_t*, uint8_t* );
void to8_from_big_endian( uint8_t*, uint8_t* );


#endif /* INCLUDE_PACKET_BASE_H_ */
