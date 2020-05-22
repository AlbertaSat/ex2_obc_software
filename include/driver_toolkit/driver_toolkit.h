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
 * @file driver_toolkit
 * @author Brendan Bruner
 * @date Feb 4, 2015
 */
#ifndef INCLUDE_DRIVER_TOOLKIT_H_
#define INCLUDE_DRIVER_TOOLKIT_H_

#include "systems/ground_station/ground_station.h"

/* typedefs */
typedef struct driver_toolkit_t driver_toolkit_t;

/**
 * @struct driver_toolkit_t
 * @brief
 * 		A container for platform specific API.
 * @details
 * 		A container to decouple platform specific API's from their platform.
 * 		This class contains the Interface definitions for all APIs / drivers.
 * 		Super classes (of this class) then create the appropriate platform specific API / driver
 * 		and assign them to their interface.
 */
struct driver_toolkit_t
{
	/* Subsystem Drivers. */
	ground_station_t*	gs;
	/* Scripting time server. */
	/* Initialization of this is done in state_bring_up.c */
	// script_time_server_t time_server;

	struct
	{
		// loggers will go here
	}_; /* Private. */
};

/********************************************************************************/
/* Constructor Declare															*/
/********************************************************************************/
/**
 * @memberof driver_toolkit_lpc_t
 * @brief
 * 		Constructor.
 * @details
 * 		Constructor.
 */
_Bool initialize_driver_toolkit( driver_toolkit_t *toolkit );


/**
 * @brief
 * 		Destroy a driver_toolkit_t structure.
 * @details
 * 		Destroy a driver_toolkit_t structure. This must be called when the structure
 * 		is no longer need.
 * @param kit
 * 		A pointer to the driver_toolkit_t to destroy.
 */
void destroy_driver_toolkit( driver_toolkit_t *kit );

#endif /* INCLUDE_DRIVER_TOOLKIT_H_ */
