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
 * @file ground_station_base.c
 * @author Brendan Bruner
 * @date Jun 3, 2015
 */

#include <systems/ground_station/ground_station.h>
#include <portable_types.h>

/********************************************************************************/
/* Destructor																	*/
/********************************************************************************/
void destroy_ground_station( ground_station_t *gs )
{
	DEV_ASSERT( gs );
}
