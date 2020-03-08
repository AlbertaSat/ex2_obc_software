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
 * @file commands
 * @author Brendan Bruner
 * @date Feb 1, 2015
 */
#include <telecommand.h>

#include <dependency_injection.h>

static void execute_count( telecommand_t *command )
{
	printf("EXECUTE COUNT: %d\n", ++*((telecommand_counter_t *) command)->count);
}
static telecommand_t* clone( telecommand_t* self_ )
{
	telecommand_counter_t* self = (telecommand_counter_t*) self_;
	DEV_ASSERT( self );

	telecommand_counter_t* clone;

	clone = (telecommand_counter_t*) pvPortMalloc( sizeof(telecommand_counter_t) );
	if( clone == NULL ){ return NULL; }

	initialize_command_counter( clone, self->count, ((telecommand_t*) self)->_kit );

	return (telecommand_t*) clone;
}

void initialize_command_counter( telecommand_counter_t *command, int *count, driver_toolkit_t *kit )
{
	DEV_ASSERT( command );
	DEV_ASSERT( count );
	DEV_ASSERT( kit );

	initialize_telecommand( (telecommand_t *) command, kit );

	*count = 0;
	((telecommand_t *) command)->_execute = 		&execute_count;
	((telecommand_t*) command)->clone = clone;
	command->count = count;
}
