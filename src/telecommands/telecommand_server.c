/*
 * Copyright (C) 2018  Brendan Bruner
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
 * @file telecommand_server.c
 * @author Brendan Bruner
 * @date April 6, 2018
 */

#include <telecommand_server.h>
#include <script_daemon.h>
#include <stdio.h>
#include <telecommand_prototype_manager.h>
#include <telecommand.h>
#include <systems/telecommand_test.h>

/********************************************************************************/
/* #defines																		*/
/********************************************************************************/

/********************************************************************************/
/* Singleton Variable Defines													*/
/********************************************************************************/
static telecommand_test_t		test_command;

static script_daemon_t script_daemon; // this will need to be simulated

/********************************************************************************/
/* Private Method Defines														*/
/********************************************************************************/
static void register_telecommands( struct driver_toolkit_t* kit )
{
	telecommand_prototype_manager_t*	prototypes;

	// Get the prototype manager.
	prototypes = get_telecommand_prototype_manager( );
	if( prototypes == NULL )
	{
		printf( "Failed to initialize telecommanding backend" );
		return;
	}
	printf("REGISTERED\n");

	initialize_telecommand_test(&test_command, kit);

	prototypes->register_prototype(prototypes, "test", (telecommand_t*) &test_command);
}

/********************************************************************************/
/* Virtual Method Defines														*/
/********************************************************************************/

/********************************************************************************/
/* Destructor Define															*/
/********************************************************************************/

/********************************************************************************/
/* Constructor Define															*/
/********************************************************************************/
void start_telecommand_server( struct driver_toolkit_t* kit )
{
	_Bool err;
	printf("(STARTING SERVER)\n");
	static _Bool is_init = false;

	if( is_init ) {
		return;
	}

	/* Register all possible telecommands and start handler. */
	register_telecommands(kit);
	/* Start the script daemon. */
	err = initialize_script_daemon( &script_daemon, kit->gs );
	if( !err )
		{printf( "Failed to initialzie telecommanding services" );}
	else
		{printf( "Script Daemon up and running" );}
	is_init = true;

}
/********************************************************************************/
/* Public Method Defines														*/
/********************************************************************************/
