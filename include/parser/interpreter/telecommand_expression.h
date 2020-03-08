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
 * @file constant_expression.h
 * @author Brendan Bruner
 * @date Oct 13, 2015
 */
#ifndef INCLUDE_INTERPRETER_TELECOMMAND_EXPRESSION_H_
#define INCLUDE_INTERPRETER_TELECOMMAND_EXPRESSION_H_

#include "script_expression.h"

/********************************************************************************/
/* #defines																		*/
/********************************************************************************/
#define CONSTANT_EXPRESSION_COMMAND_STRING_LENGTH 15


/********************************************************************************/
/* Structure Documentation														*/
/********************************************************************************/
/**
 * @struct telecommand_expression_t
 * @extends script_expression_t
 * @brief
 * 		A telecommand expression has a 1-to-1 mapping
 * 		with a telecommand_t type.
 * @details
 * 		A telecommand expression has a 1-to-1 mapping
 * 		with a telecommand_t type.
 * 		For example:
 * 		<br><ul><li>"downlink"</li>
 * 		<li>"dfgm disable"</li>
 * 		<li>"lock configuration"</li>
 * 		<li>"unlock configuration"</li>
 * 		</ul>
 * 		Interpreting this command will invoke the execution of
 * 		the telecommand it is mapped to. It can be safetly invoked
 * 		multiple times.
 */
typedef struct telecommand_expression_t telecommand_expression_t;


/********************************************************************************/
/* Structure Defines															*/
/********************************************************************************/
struct telecommand_expression_t
{
	script_expression_t super_; /* MUST be first. */
	struct
	{
		telecommand_t* 	telecommand;
		char const*		telecommand_argument;
		uint32_t 		argument_length;
	} _; /* Private. */
};


/********************************************************************************/
/* Constructor Declare															*/
/********************************************************************************/
/**
 * @memberof telecommand_expression_t
 * @brief
 * 		Constructor.
 * @details
 * 		Constructor.
 * @param command_string[in]
 * 		A string representation of the command. This string is copied into the
 * 		telecommand expression object. All valid command_strings are registered
 * 		via telecommand_prototype_manager_t::register_prototype( ). If the
 * 		<b>command_string</b> has not been registered with the prototype manager
 * 		then the expression will fail to be constructed.
 * @param command_length
 * 		Length, in bytes, of the command_string.
 * @param argument_string[in]
 * 		An additional argument to provide to the telecommand. If there is no
 * 		additional argument, pass in <b>NULL</b>.
 * @param argument_length
 * 		Length, in bytes, of the argument_string. Should be zero if there is no
 * 		argument.
 * @returns
 * 		<b>true</b> on success, <b>false</b> on failure.
 */
_Bool initialize_telecommand_expression
(
	telecommand_expression_t*,
	char const* command_string,
	uint32_t command_length,
	char const* argument_string,
	uint32_t argument_length
);


/********************************************************************************/
/* Public Method Declares														*/
/********************************************************************************/


#endif /* INCLUDE_INTERPRETER_TELECOMMAND_EXPRESSION_H_ */
