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
 * @file driver_toolkit_base.c
 * @author Brendan Bruner
 * @date Jun 3, 2015
 */

#include <driver_toolkit/driver_toolkit.h>
#include <portable_types.h>
#include <core_defines.h>
#include <stdio.h>

/**
 * @memberof driver_toolkit_t
 * @protected
 * @brief
 * 		Constructor.
 * @details
 * 		Constructor.
 */
_Bool initialize_driver_toolkit( driver_toolkit_t* toolkit )
{
	DEV_ASSERT( toolkit );

  toolkit = pvPortMalloc(sizeof(driver_toolkit_t));
  initialize_mock_up_ground_station_dynamic_sizes( toolkit->gs );

	return true;
}

void destroy_driver_toolkit( driver_toolkit_t *kit )
{
	DEV_ASSERT( kit );

	destroy_ground_station( kit->gs );
}
