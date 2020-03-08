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
 * @file command_expression.c
 * @author Brendan Bruner
 * @date Oct 13, 2015
 */

#include <parser/interpreter/script_expression.h>


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
static void interpret( script_expression_t* self )
{
	DEV_ASSERT( self );
}


/********************************************************************************/
/* Destructor Define															*/
/********************************************************************************/
static void destroy( script_expression_t* self )
{
	DEV_ASSERT( self );
}


/********************************************************************************/
/* Constructor Define															*/
/********************************************************************************/
/**
 * @memberof script_expression_t
 * @protected
 * @brief
 * 		Constructor.
 * @details
 * 		Constructor.
 * @returns
 * 		<b>true</b> on successful construction, <b>false</b> otherwise. Unsafe
 * 		to use when <b>false</b> is returned.
 */
_Bool initialize_script_expression_( script_expression_t* self )
{
	DEV_ASSERT( self );

	self->interpret = interpret;
	self->destroy = destroy;

	return true;
}


/********************************************************************************/
/* Public Method Defines														*/
/********************************************************************************/


