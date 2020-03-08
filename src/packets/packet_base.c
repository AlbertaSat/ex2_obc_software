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
 * @file packet_base.c
 * @author Brendan Bruner
 * @date Jul 2, 2015
 */

#include <packets/packet_base.h>

/********************************************************************************/
/* Public Method Defines														*/
/********************************************************************************/
void to_big_endian_from32( uint32_t* source, uint8_t* dest )
{
	dest[0] = (uint8_t) (*source >> 24);
	dest[1] = (uint8_t) (*source >> 16);
	dest[2] = (uint8_t) (*source >> 8);
	dest[3] = (uint8_t) (*source >> 0);
}
void to_big_endian_from16( uint16_t* source, uint8_t* dest )
{
	dest[0] = (uint8_t) (*source >> 8);
	dest[1] = (uint8_t) (*source >> 0);
}
void to_big_endian_from8( uint8_t* source, uint8_t* dest )
{
	dest[0] = (uint8_t) (*source >> 0);
}
void to32_from_big_endian( uint32_t* dest, uint8_t* source )
{
	*dest = (uint32_t) (source[0] << 24);
	*dest += (uint32_t) (source[1] << 16);
	*dest += (uint32_t) (source[2] << 8);
	*dest += (uint32_t) (source[3] << 0);
}
void to16_from_big_endian( uint16_t* dest, uint8_t* source )
{
	*dest = (uint32_t) (source[0] << 8);
	*dest += (uint32_t) (source[1] << 0);
}
void to8_from_big_endian( uint8_t* dest, uint8_t* source )
{
	*dest = (source[0]);
}
void to_little_endian_from32( uint32_t* source, uint8_t* dest )
{
	dest[0] = (uint8_t) (*source >> 0);
	dest[1] = (uint8_t) (*source >> 8);
	dest[2] = (uint8_t) (*source >> 16);
	dest[3] = (uint8_t) (*source >> 24);
}
void to_little_endian_from16( uint16_t* source, uint8_t* dest )
{
	dest[0] = (uint8_t) (*source >> 0);
	dest[1] = (uint8_t) (*source >> 8);
}
void to_little_endian_from8( uint8_t* source, uint8_t* dest )
{
	dest[0] = (uint8_t) (*source >> 0);
}
void to32_from_little_endian( uint32_t* dest, uint8_t* source )
{
	*dest = (uint32_t) (source[0] << 0);
	*dest += (uint32_t) (source[1] << 8);
	*dest += (uint32_t) (source[2] << 16);
	*dest += (uint32_t) (source[3] << 24);
}
void to16_from_little_endian( uint16_t* dest, uint8_t* source )
{
	*dest = (uint32_t) (source[0] << 0);
	*dest += (uint32_t) (source[1] << 8);
}
void to8_from_little_endian( uint8_t* dest, uint8_t* source )
{
	*dest = (source[0]);
}


/****************************************************************************/
/* Network Endian Conversions												*/
/****************************************************************************/
void to_network_from32( uint32_t *source, uint8_t *dest )
{
	/* Network byte order is big endian, most sig byte is first. */
	to_big_endian_from32( source, dest );
}
void to_network_from16( uint16_t *source, uint8_t *dest )
{
	/* Network byte order is big endian, most sig byte is first. */
	to_big_endian_from16( source, dest );
}
void to_network_from8( uint8_t *source, uint8_t *dest )
{
	/* Network byte order is big endian, most sig byte is first. */
	to_big_endian_from8( source, dest );
}
void to32_from_network( uint32_t *dest, uint8_t *source )
{
	/* Network byte order is big endian, most sig byte is first. */
	to32_from_big_endian( dest, source );
}
void to16_from_network( uint16_t *dest, uint8_t *source )
{
	/* Network byte order is big endian, most sig byte is first. */
	to16_from_big_endian( dest, source );
}
void to8_from_network( uint8_t *dest, uint8_t *source )
{
	/* Network byte order is big endian, most sig byte is first. */
	to8_from_big_endian( dest, source );
}

