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
 * @file script_daemon.h
 * @author Brendan Bruner
 * @date Nov 4, 2015
 */
#ifndef INCLUDE_SCRIPT_DAEMON_H_
#define INCLUDE_SCRIPT_DAEMON_H_

#include <systems/ground_station/ground_station.h>
#include <telecommand/parser/parser.h>


/********************************************************************************/
/* #defines																		*/
/********************************************************************************/



/********************************************************************************/
/* Structure Documentation														*/
/********************************************************************************/
/**
 * @struct script_daemon_t
 * @brief
 * 		An active class which listens for scripts from the ground station.
 * @details
 * 		An active class which listens for scripts from the ground station.
 * 		Incoming scripts are parsed into an expression (syntax tree) then
 * 		interpreted. This process loops for ever.
 */
typedef struct script_daemon_t script_daemon_t;


/********************************************************************************/
/* Structure Defines															*/
/********************************************************************************/
struct script_daemon_t
{
	struct
	{
		parser_t parser;
		uint8_t* script;
		ground_station_t* gs;
		task_t task_handle;
	}_; /* Private. */
	void (*destroy)( script_daemon_t* );
};


/********************************************************************************/
/* Constructor Declare															*/
/********************************************************************************/
/**
 * @memberof script_daemon_t
 * @brief
 * 		Constructor.
 * @details
 * 		Constructor.
 * @param gs[in]
 * 		The ground station where listing will be done for incoming scripts.
 */
_Bool initialize_script_daemon( script_daemon_t*, ground_station_t* gs );


/********************************************************************************/
/* Public Method Declares														*/
/********************************************************************************/

#endif /* INCLUDE_SCRIPT_DAEMON_H_ */
