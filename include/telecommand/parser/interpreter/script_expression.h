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
 * @file command_expression.h
 * @author Brendan Bruner
 * @date Oct 13, 2015
 */
#ifndef INCLUDE_INTERPRETER_COMMAND_EXPRESSION_H_
#define INCLUDE_INTERPRETER_COMMAND_EXPRESSION_H_

#include <portable_types.h>
#include <telecommand/telecommand.h>


/********************************************************************************/
/* #defines																		*/
/********************************************************************************/



/********************************************************************************/
/* Structure Documentation														*/
/********************************************************************************/
/**
 * @struct script_expression_t
 * @brief
 * 		Abstract class for grammar rules.
 * @details
 * 		Abstract class for grammar rules. Defines an interface for
 * 		Interpreting an expression.
 *		<br>Research the 'interpreter pattern' for insight
 *		into the design of this interpreter.
 * @attention
 * 		IMPORTANT
 * 		<br>All expression objects are assumed to be allocated with pvPortMalloc( ).
 * 		Invoking the destructor of an expression will make it free itself with
 * 		vPortFree( ). When creating expressions, always allocate space for them
 * 		with pvPortMalloc( ) then initialize them.
 * @var command_expression_t::interpret
 * 		<b>Virtual<br>Public</b>
 * 		@code
 * 			void interpret( command_expression_t* );
 * 		@endcode
 * 		Interface used for interpreting expression. Subclasses override
 * 		this to define how to interpret their particular rule.
 */
typedef struct script_expression_t script_expression_t;


/********************************************************************************/
/* Structure Defines															*/
/********************************************************************************/
struct script_expression_t
{
	void (*interpret)( script_expression_t* );
	void (*destroy)( script_expression_t* );
};


/********************************************************************************/
/* Constructor Declare															*/
/********************************************************************************/


/********************************************************************************/
/* Public Method Declares														*/
/********************************************************************************/



#endif /* INCLUDE_INTERPRETER_COMMAND_EXPRESSION_H_ */
