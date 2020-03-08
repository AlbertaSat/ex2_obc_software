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
 * @file argument_expression.c
 * @author Brendan Bruner
 * @date Nov 2, 2015
 */

#include <parser/interpreter/statement_expression.h>

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
	statement_expression_t* self = (statement_expression_t*) self_;
	DEV_ASSERT( self );

	self->_.statement->interpret( self->_.statement );
}


/********************************************************************************/
/* Destructor Define															*/
/********************************************************************************/
static void destroy( script_expression_t * self_ )
{
	statement_expression_t* self = (statement_expression_t*) self_;
	DEV_ASSERT( self );

	if( self->_.statement != NULL )
	{
		self->_.statement->destroy( self->_.statement );
	}
	vPortFree( (void*) self );
}


/********************************************************************************/
/* Constructor Define															*/
/********************************************************************************/
_Bool initialize_statement_expression
(
	statement_expression_t* self,
	script_expression_t* statement
)
{
	DEV_ASSERT( statement );
	DEV_ASSERT( self );

	_Bool err;

	/* Initialize super class. */
	extern _Bool initialize_script_expression_( script_expression_t* );
	err = initialize_script_expression_( (script_expression_t*) self );
	if( !err ){ return NULL; }

	/* Override interpret method. */
	((script_expression_t*) self)->interpret = interpret;
	((script_expression_t*) self)->destroy = destroy;

	/* Assign statement. */
	self->_.statement = statement;

	return true;
}


/********************************************************************************/
/* Public Method Defines														*/
/********************************************************************************/


