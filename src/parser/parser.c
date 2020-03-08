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
 * @file parser.c
 * @author Brendan Bruner
 * @date Nov 3, 2015
 */

#include <parser/interpreter/statement_expression.h>
#include <parser/parser.h>
#include <stdio.h>
#include <parser/interpreter/telecommand_expression.h>


/********************************************************************************/
/* #defines																		*/
/********************************************************************************/
/* These defines are used to help map ascii strings to the script_symbol_e enum. */
#define COMMAND 			"COMMAND"
#define SCRIPT_END 			"SCRIPT_END"

/* Used for debug printing parser errors. */
#define SCRIPT_CHARACTER_ERROR( got, expected ) \
	printf( "Error, %d: expected \"%c\", but got \"%c\"\n", (int)__LINE__, expected, *(got)->_.current_character )
#define SCRIPT_EOS_ERROR( character ) \
	printf( "Error, %d: expected \"%c\", but go EOS\n", (int)__LINE__, character )
#define SCRIPT_STRING_ERROR( ) \
	printf( "Error, %d: expected string, but got something else\n", (int)__LINE__ )
#define SCRIPT_INVALID_SYMBOL_ERROR( ) \
	printf( "Error, %d: symbol not recognized\n", (int)__LINE__)

/* Maximum length of a non terminal symbol. */
#define MAX_SYMBOL_LENGTH 15

/********************************************************************************/
/* Singleton Variable Defines													*/
/********************************************************************************/



/********************************************************************************/
/* Private Method Defines														*/
/********************************************************************************/
/**
 * @memberof parser_t
 * @private
 * @brief
 * 		Convert a null terminated string to a unique unsigned long.
 * @details
 * 		Convert a null terminated string to a unique unsigned long.
 * 		This is the djb2 hashing algorithm, first reported by
 * 		Dan Bernstein.
 * 		<br><a href="http://www.cse.yorku.ca/~oz/hash.html">djb2, Nov. 2015</a>
 */
static unsigned long hash( char const *str )
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

/**
 * @memberof parser_t
 * @private
 * @brief
 * 		Convert a non null terminated string to a unique unsigned long.
 * @details
 * 		Convert a non null terminated string to a unique unsigned long.
 * 		This is a modified djb2 hashing algorithm, first reported by
 * 		Dan Bernstein.
 * 		<br><a href="http://www.cse.yorku.ca/~oz/hash.html">djb2, Nov. 2015</a>
 */
static unsigned long nhash( char const* str, uint32_t length )
{
    unsigned long hash = 5381;
    int c;

    while ( length-- )
    {
    	c = *str++;
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

/**
 * @memberof parser_t
 * @private
 * @brief
 * 		Consumes the empty space of a script.
 * @details
 * 		Consumes the empty space of a script. ASCII characters: ' ', '\n', and '\r'
 * 		are considered to be empty space. This script advances the location in the stream
 * 		past the empty space.
 */
static void parser_consume_space( parser_t* self )
{
	DEV_ASSERT( self );
	#define ASCII_EMPTY_SPACE( ch ) ( (ch) == ' ' || (ch) == '\n' || (ch) == '\r' )

	while( ASCII_EMPTY_SPACE( *self->_.current_character ) )
	{
		++self->_.bytes_parsed;
		++self->_.current_character;
		if( self->_.bytes_parsed >= self->_.stream_length )
		{
			/* Script end reached. */
			return;
		}
	}
}

/**
 * @memberof parser_t
 * @private
 * @brief
 * 		Consumes one symbol in the script.
 * @details
 * 		Consumes one symbol in the script. All ASCII characters between 'A' and 'Z',
 * 		'a' and 'z', and '_' are valid symbol characters. This method will advance the
 * 		location in the stream until there are no more valid symbol characters. If there
 * 		are no valid symbol characters at the current location in the stream, nothing is done.
 */
static void parser_consume_symbol( parser_t* self )
{
	DEV_ASSERT( self );
	#define ASCII_LETTER( ch )				( ((ch) >= 'A' && (ch) <= 'Z') || ((ch) >= 'a' && (ch) <= 'z') )
	#define ASCII_UNDERSCORE( ch ) 			( (ch) == '_' )
	#define ASCII_VALID_SYMBOL_CHAR( ch )	ASCII_LETTER( (ch) ) || ASCII_UNDERSCORE( (ch) )

	while( ASCII_VALID_SYMBOL_CHAR( *self->_.current_character ) )
	{
		++self->_.bytes_parsed;
		++self->_.current_character;
		if( self->_.bytes_parsed >= self->_.stream_length )
		{
			/* Script end reached. */
			return;
		}
	}
}

/**
 * @memberof parser_t
 * @private
 * @brief
 * 		Asserts the next character.
 * @details
 * 		Asserts the next character. This method will consume empty space. Then it
 * 		will check the character currently in the stream and assert it matches
 * 		the input argument. On success, the stream is incremented to the next
 * 		characters, on failure, the stream is not incremented.
 * @param character
 * 		The character to assert.
 * @return
 * 		<b>true</b> if there is a match, <b>false</b> otherwise.
 */
static _Bool parser_accept_character( parser_t* self, char character )
{
	DEV_ASSERT( self );

	parser_consume_space( self );

	if( self->_.bytes_parsed >= self->_.stream_length )
	{
		/* No character to accept if at end of script. */
		SCRIPT_EOS_ERROR( character );
		return false;
	}

	if( *self->_.current_character != character )
	{
		/* The characters do not match. */
		return false;
	}

	++self->_.bytes_parsed;
	++self->_.current_character;
	return true;
}

/**
 * @memberof parser_t
 * @private
 * @brief
 * 		Asserts the current symbol.
 * @details
 * 		Asserts the current symbol the parser is working with matches the
 * 		function argument.
 * @param sym
 * 		The symbol to match
 * @returns
 * 		<b>true</b> if matched, <b>false</b> otherwise.
 */
static _Bool parser_accept_symbol( parser_t* self, script_symbol_e sym )
{
	DEV_ASSERT( self );

	if( self->_.symbol == sym ){ return true; }
	return false;
}

/**
 * @memberof parser_t
 * @private
 * @brief
 * 		Extracts a string literal from the script.
 * @details
 * 		Extracts a string literal from the script. The method start by accepting
 * 		the character '"' (ie, it will try to find the next non empty character
 * 		and assert it is '"'). It will then extract all characters
 * 		up to another '"'.
 * @attention
 * 		If this method fails, the location in the stream will
 * 		be where the invalid character was encountered.
 * @param string[out]
 * 		A pointer to a pointer to a char. It will be dereferenced and set to
 * 		point to the location in the stream where the string starts. For example,
 * 		suppose the stream had these characters:
 * 		<br>"string1"
 * 		Then, call the method like so:
 * 		@code
 * 			uint32_t length
 * 			char const* string_start;
 * 			parser_extract_string( self, &string_start, &length );
 * 			// *string_start will now equal 's'
 * 		@endcode
 * @param length[out]
 * 		Will be dereferenced and filled in with the length of the string extracted.
 * @returns
 * 		<b>true</b> when a string is successfully extracted, <b>false</b> otherwise.
 * 		On failure, the input arguments <b>string</b> and <b>length</b> will be invalid.
 */
static _Bool parser_extract_string( parser_t* self, char const** string, uint32_t* length )
{
	DEV_ASSERT( self );
	DEV_ASSERT( string );
	DEV_ASSERT( length );

	uint32_t string_start = 0;
	uint32_t string_end = 0;

	/* Look for string start. */
	if( !parser_accept_character( self, '"' ) )
	{
		return false;
	}

	/* Save string start. */
	string_start = self->_.bytes_parsed;

	/* Go to string end. */
	while( *self->_.current_character != '"' )
	{
		++self->_.bytes_parsed;
		++self->_.current_character;
		if( self->_.bytes_parsed >= self->_.stream_length )
		{
			/* Script end reached. */
			return false;
		}
	}

	/* Save string end. */
	string_end = self->_.bytes_parsed;

	/* String should end with '"', otherwise the string has invalid characters. */
	if( !parser_accept_character( self, '"' ) )
	{
		return false;
	}

	/* Extract the string. */
	*length = string_end - string_start;
	*string = self->_.input_stream + string_start;

	return true;
}

/* Look for the next symbol and put it into the symbolBuffer argument. */
/**
 * @memberof parser_t
 * @private
 * @brief
 * 		Get the next non terminal symbol in the stream.
 * @details
 * 		Get the next non terminal symbol in the stream. This method consume empty space. Then, it
 * 		consumes what ever symbol is in the stream. It will update
 * 		parser_t::_::symbol with the results.
 */
static void parser_next_symbol( parser_t* self )
{
	DEV_ASSERT( self );

	unsigned long symbol_hash;
	script_symbol_e symbol;

	parser_consume_space( self );
	if( self->_.bytes_parsed >= self->_.stream_length )
	{
		/* At script end. Hash the script end symbol. */
		self->_.symbol = SCRIPT_END_SYM;
		return;
	}
	else
	{
		int symbol_start = 0;
		int symbol_end = 0;

		/* Look for symbol end. */
		symbol_start = self->_.bytes_parsed;
		parser_consume_symbol( self );
		symbol_end = self->_.bytes_parsed;

		/* Hash the symbol. */
		symbol_hash = nhash( self->_.input_stream + symbol_start, symbol_end - symbol_start );
	}

	/* Check the hashed symbol against all known symbols. */
	if(	  	 symbol_hash == hash( COMMAND ) )			{ symbol = COMMAND_SYM; }
	else if( symbol_hash == hash( SCRIPT_END ) )		{ symbol = SCRIPT_END_SYM; }
	else
	{
		/* Symbol is not recognized. */
		symbol = INVALID_SYM;
		SCRIPT_INVALID_SYMBOL_ERROR( );
	}

	self->_.symbol = symbol;
}

/**
 * @memberof parser_t
 * @private
 * @brief
 * 		Called when the symbol being worked with is a COMMAND symbol.
 * @details
 * 		Called when the symbol being worked with is a COMMAND symbol.
 * 		This method will parse the command symbol and create a syntax tree.
 * @returns
 * 		A valid script expression which will execute the telecommand parsed.
 * 		<b>NULL</b> if parsing failed.
 */
static script_expression_t* parser_command( parser_t* self )
{
	DEV_ASSERT( self );

	char const* 			command_name;
	uint32_t 				command_name_length;
	char const* 			argument_name;
	uint32_t 				argument_name_length;
	telecommand_expression_t* 	expression;

	/* Look for opening bracked. */
	if( !parser_accept_character( self, '(' ) )
	{
		SCRIPT_CHARACTER_ERROR( self, '(' );
		return NULL;
	}

	/* extract command name. */
	if( !parser_extract_string( self, &command_name, &command_name_length ) )
	{
		SCRIPT_STRING_ERROR( );
		return NULL;
	}

	/* Check if optional argument is included. */
	if( parser_accept_character( self, ',' ) )
	{
		/* Extract optional argument. */
		if( !parser_extract_string( self, &argument_name, &argument_name_length ) )
		{
			SCRIPT_STRING_ERROR( );
			return NULL;
		}
	}
	else
	{
		argument_name = NULL;
		argument_name_length = 0;
	}

	/* Look for closing bracket. */
	if( !parser_accept_character( self, ')' ) )
	{
		SCRIPT_CHARACTER_ERROR( self, ')' );
		return NULL;
	}

	/* Look for symbol finisher. */
	if( !parser_accept_character( self, ';' ) )
	{
		SCRIPT_CHARACTER_ERROR( self, ';' );
		return NULL;
	}

	/* Construct expression. */
	expression = (telecommand_expression_t*) pvPortMalloc( sizeof (telecommand_expression_t) );
	if( expression == NULL )
	{
		/* Failed to allocate space for expression. */
		return NULL;
	}

	_Bool err = initialize_telecommand_expression
	(
		expression,
		command_name,
		command_name_length,
		argument_name,
		argument_name_length
	);
	if( err == false )
	{
		/* Failed to construct expression. */
		vPortFree( (void*) expression );
		return NULL;
	}
	return (script_expression_t*) expression;
}

/**
 * @memberof parser_t
 * @private
 * @brief
 * 		Called when a statement is entered.
 * @details
 * 		Called when a statement is entered.
 * @returns
 * 		A valid expression which will execute the statement parsed. <b>NULL</b>
 * 		on failure to parse.
 */
static script_expression_t* parser_statement( parser_t* self )
{
	DEV_ASSERT( self );

	statement_expression_t* statement;
	script_expression_t* telecommand;

	/* Allocate space for statement. */
	statement = (statement_expression_t*) pvPortMalloc( sizeof(statement_expression_t) );
	if( statement == NULL )
	{
		/* Failed to allocate space for expression. */
		return NULL;
	}

	/* Expecting a command or blank script. */
	parser_next_symbol( self );

	if( parser_accept_symbol( self, COMMAND_SYM ) )
	{
		/* Got a command symbol in script. */
		telecommand = parser_command( self );
		if( telecommand == NULL )
		{
			/* Failed to parse command. */
			vPortFree( (void*) statement );
			return NULL;
		}
		_Bool err = initialize_statement_expression( statement, telecommand );
		if( err == false )
		{
			/* Failed to construct expression. */
			vPortFree( (void*) statement );
			telecommand->destroy( telecommand );
			return NULL;
		}
	}
	else if( parser_accept_symbol( self, SCRIPT_END_SYM ) )
	{
		/* No command, no statement. At end of script. */
		vPortFree( (void*) statement );
		return NULL;
	}
	else
	{
		/* Script cannot be parsed. */
		vPortFree( (void*) statement );
		return NULL;
	}

	/* Expecting EOS. */
	parser_next_symbol( self );
	if( !parser_accept_symbol( self, SCRIPT_END_SYM ) )
	{
		/* Script cannot be parsed. */
		((script_expression_t*) statement)->destroy( (script_expression_t*) statement );
		return NULL;
	}

	return (script_expression_t*) statement;
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
void initialize_parser( parser_t* self )
{
	DEV_ASSERT( self );
}


/********************************************************************************/
/* Public Method Defines														*/
/********************************************************************************/
script_expression_t* parser_parse_string( parser_t* self, char const* script, uint32_t script_length )
{
	DEV_ASSERT( self );
	DEV_ASSERT( script );

	script_expression_t* parsed_script;

	/* Do not parse an empty script. */
	if( script_length <= 0 ){ return NULL; }

	self->_.bytes_parsed = 0;
	self->_.stream_length = script_length;
	self->_.input_stream = script;
	self->_.current_character = script;
	self->_.syntax_tree = NULL;

	/* Begin parsing. */
	parsed_script = parser_statement( self );
	return parsed_script;
}

