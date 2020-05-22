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
 * @file constant_expression.c
 * @author Brendan Bruner
 * @date Oct 13, 2015
 */

#include <telecommand/parser/interpreter/telecommand_expression.h>
#include <telecommand/telecommand_prototype_manager.h>
#include <string.h>

/********************************************************************************/
/* #defines																		*/
/********************************************************************************/



/********************************************************************************/
/* Singleton Variable Defines													*/
/********************************************************************************/



/********************************************************************************/
/* Private Method Defines														*/
/********************************************************************************/



/********************************************************************************/
/* Virtual Method Defines														*/
/********************************************************************************/
static void interpret( script_expression_t* self_ )
{
	telecommand_expression_t* self = (telecommand_expression_t*) self_;
	DEV_ASSERT( self );

	/* Execute the command. */
	if( self->_.telecommand != NULL )
	{
		telecommand_execute( self->_.telecommand );
	}
}


/********************************************************************************/
/* Destructor Define															*/
/********************************************************************************/
static void destroy( script_expression_t* self_ )
{
	telecommand_expression_t* self = (telecommand_expression_t*) self_;
	DEV_ASSERT( self );

	if( self->_.telecommand != NULL )
	{
		self->_.telecommand->destroy( self->_.telecommand );
		vPortFree( (void*) self->_.telecommand );
	}
	vPortFree( (void*) self );
}


/********************************************************************************/
/* Constructor Define															*/
/********************************************************************************/
_Bool initialize_telecommand_expression
(
	telecommand_expression_t* self,
	char const* command_string,
	uint32_t command_length,
	char const* argument_string,
	uint32_t argument_length
)
{
	DEV_ASSERT( self );
	DEV_ASSERT( command_string );

	telecommand_t*						prototype;
	_Bool 								err;
	telecommand_prototype_manager_t* 	prototypes;
	char 								command_string_copy[TELECOMMAND_PROTOTYPE_MANAGER_KEY_LENGTH+1];

	/* Initialize super class. */
	extern _Bool initialize_script_expression_( script_expression_t* );
	err = initialize_script_expression_( (script_expression_t*) self );
	if( !err ){ return false; }

	/* Override super class' interpret method. */
	((script_expression_t*) self)->interpret = interpret;
	((script_expression_t*) self)->destroy = destroy;

	/* Get the command prototype manager instance. */
	prototypes = get_telecommand_prototype_manager( );
	if( prototypes == NULL ){ return false; }

	/* Get the command prototype. */
	strncpy( command_string_copy, command_string, TELECOMMAND_PROTOTYPE_MANAGER_KEY_LENGTH );
	if( command_length <= TELECOMMAND_PROTOTYPE_MANAGER_KEY_LENGTH )
	{
		command_string_copy[command_length] = '\0';
	}
	else
	{
		command_string_copy[TELECOMMAND_PROTOTYPE_MANAGER_KEY_LENGTH] = '\0';
	}
	prototype = prototypes->get_prototype( prototypes, command_string_copy );
	if( prototype == NULL ){ return false; }

	/* Clone the prototype to get personal copy of telecommand. */
	self->_.telecommand = prototype->clone( prototype );
	if( self->_.telecommand == NULL ){ return false; }

	/* Add the argument to the command. */
	if( argument_string != NULL && argument_length > 0 )
	{
		telecommand_argument( self->_.telecommand, argument_string, argument_length );
	}

	return true;
}



/********************************************************************************/
/* Public Method Defines														*/
/********************************************************************************/
