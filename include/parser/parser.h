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
 * @file parser.h
 * @author Brendan Bruner
 * @date Nov 2, 2015
 */
#ifndef INCLUDE_PARSER_PARSER_H_
#define INCLUDE_PARSER_PARSER_H_

#include <parser/interpreter/script_expression.h>

/********************************************************************************/
/* #defines																		*/
/********************************************************************************/


/********************************************************************************/
/* Structure Documentation														*/
/********************************************************************************/
/**
 * @struct parser_t
 * @brief
 * 		Structure used to maintain state data while parsing a script.
 * @details
 * 		Structure used to maintain state data while parsing a script.
 */
typedef struct parser_t parser_t;

/** These are the possible non terminal nodes of the script, and some terminal nodes. */
typedef enum
{
	COMMAND_SYM = 0,			/*!< (0) COMMAND non terminal. */
	SCRIPT_END_SYM,				/*!< (1) End of script terminal node. */
	INVALID_SYM					/*!< (2) Symbol encountered in script is unrecognized. */
} script_symbol_e;


/********************************************************************************/
/* Structure Defines															*/
/********************************************************************************/
struct parser_t
{
	struct
	{
		script_symbol_e	symbol;
		char const* 	input_stream;
		uint32_t		stream_length;
		uint32_t		bytes_parsed;
		char const*		current_character;
		script_expression_t* syntax_tree;
	} _; /* Private. */
};


/********************************************************************************/
/* Constructor Declare															*/
/********************************************************************************/
/**
 * @memberof parser_t
 * @brief
 * 		Constructor.
 * @details
 * 		Constructor.
 */
void initialize_parser( parser_t* );


/********************************************************************************/
/* Public Method Declares														*/
/********************************************************************************/
/**
 * @memberof parser_t
 * @brief
 * 		Converts an input script into a syntax tree.
 * @details
 * 		Converts an input script into a syntax tree. When the interpret method of the
 * 		returned syntax tree is invoked, the script will begin execution.
 * @attention
 * 		The returned expression must always be destroyed when it is no longer needed.
 * 		command_expression_t::destroy. Failure to do this will result in memory leaks.
 * @param script[in]
 * 		A pointer to the ASCII string which is the script.
 * @param script_legnth
 * 		The length of the script in bytes. Scripts of length zero will be rejected.
 * @returns
 * 		A command_expression_t instance which can be interpreted. However, if parsing fails,
 * 		then <b>NULL</b> is returned.
 */
script_expression_t* parser_parse_string( parser_t*, char const* script, uint32_t script_length );

#endif /* INCLUDE_PARSER_PARSER_H_ */
