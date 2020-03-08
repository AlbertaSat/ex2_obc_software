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
 * @file central_telecomander.c
 * @author Brendan Bruner
 * @date Nov 4, 2015
 */

#include <script_daemon.h>
#include <portable_types.h>
#include <core_defines.h>
#include <parser/interpreter/script_expression.h>
#include "stdio.h"
// #include <io/nanomind.h>
// #include <printing.h>

/********************************************************************************/
/* #defines																		*/
/********************************************************************************/


/********************************************************************************/
/* Singleton Variable Defines													*/
/********************************************************************************/



/********************************************************************************/
/* Private Method Defines														*/
/********************************************************************************/
static void script_daemon_task( void* self_ )
{
	script_daemon_t* self = (script_daemon_t*) self_;
	DEV_ASSERT( self );

	script_expression_t* expression;
	uint32_t script_size;
	uint8_t* script = self->_.script;

	for( ;; )
	{
		/* Block until a script comes in. */
		printf(".");
		script_size = self->_.gs->read( self->_.gs, script, SCRIPT_MAX_LENGTH, TELECOMMAND_PORT, BLOCK_FOREVER );
		if( script_size > 0 )
		{
			printf("Got a script\n");
			/* Parse the script into a syntax tree. */
			expression = parser_parse_string( &self->_.parser, (char const*) script, script_size );
			if( expression != NULL )
			{
				/* Interpret the syntax (which will run the script). */
				expression->interpret( expression );
				/* Script has run, free up memory used by the syntax tree. */
				expression->destroy( expression );
			}
		}
		task_delay( SCRIPT_DAEMON_SLEEP_PERIOD );
	}
}


/********************************************************************************/
/* Virtual Method Defines														*/
/********************************************************************************/



/********************************************************************************/
/* Destructor Define															*/
/********************************************************************************/
static void destroy( script_daemon_t* self )
{
	DEV_ASSERT( self );
	//vPortFree( self->_.script );
	delete_task( self->_.task_handle );
}


/********************************************************************************/
/* Constructor Define															*/
/********************************************************************************/
_Bool initialize_script_daemon( script_daemon_t* self, ground_station_t* gs )
{
	DEV_ASSERT( self );
	printf("BEFIRE GS ASSERT");
	DEV_ASSERT( gs );
	printf("AFTER GS ASSERT");

	/* Link virtual methods. */
	self->destroy = destroy;

	/* Allocate space to hold incoming scripts. */
	self->_.script = (uint8_t*) pvPortMalloc( SCRIPT_MAX_LENGTH * sizeof(uint8_t) );
	if( self->_.script == NULL ){ return false; }

	/* Assign ground station. */
	self->_.gs = gs;
	/* Initialize parser. */
	initialize_parser( &self->_.parser );
	/* Create daemon task. */
	base_t err = create_task(	script_daemon_task,
								SCRIPT_DAEMON_NAME,
								SCRIPT_DAEMON_STACK,
								(void*) self,
								SCRIPT_DAEMON_PRIO,
								&self->_.task_handle );
	if( err != TASK_CREATED )
	{
		vPortFree( self->_.script );
		return false;
	}

	return true;
}


/********************************************************************************/
/* Public Method Defines														*/
/********************************************************************************/


