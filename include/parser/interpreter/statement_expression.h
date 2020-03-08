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
 * @file argument_expression.h
 * @author Brendan Bruner
 * @date Nov 2, 2015
 */
#ifndef INCLUDE_INTERPRETER_STATEMENT_EXPRESSION_H_
#define INCLUDE_INTERPRETER_STATEMENT_EXPRESSION_H_

#include "script_expression.h"


/********************************************************************************/
/* #defines																		*/
/********************************************************************************/



/********************************************************************************/
/* Structure Documentation														*/
/********************************************************************************/
/**
 * @struct statement_expression_t
 * @extends script_expression_t
 * @brief
 * 		Represents a statement in the grammar.
 * @details
 * 		Represents a statement in the grammar.
 */
typedef struct statement_expression_t statement_expression_t;


/********************************************************************************/
/* Structure Defines															*/
/********************************************************************************/
struct statement_expression_t
{
	script_expression_t super_; /* MUST be first. */
	struct
	{
		script_expression_t* statement;
	} _; /* Private. */
};


/********************************************************************************/
/* Constructor Declare															*/
/********************************************************************************/
/**
 * @memberof statement_expression_t
 * @brief
 * 		Constructor.
 * @details
 * 		Constructor.
 * @param statement[in]
 * 		The expression this statement invokes.
 * @returns
 * 		NULL on failures.
 */
_Bool initialize_statement_expression
(
	statement_expression_t*,
	script_expression_t* statement
);



/********************************************************************************/
/* Public Method Declares														*/
/********************************************************************************/



#endif /* INCLUDE_INTERPRETER_STATEMENT_EXPRESSION_H_ */
