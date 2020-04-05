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
 * @file telecommand_prototype_manager.c
 * @author Brendan Bruner
 * @date Oct 13, 2015
 */
#include <telecommand/telecommand_prototype_manager.h>
#include <telecommand/parser/uthash.h>
#include <string.h>

/********************************************************************************/
/* #defines																		*/
/********************************************************************************/
typedef struct
{
	char key[TELECOMMAND_PROTOTYPE_MANAGER_KEY_LENGTH+1];
	telecommand_t* prototype;
	UT_hash_handle hh;
} prototype_hash_node_t;


/********************************************************************************/
/* Singleton Variable Defines													*/
/********************************************************************************/
static bool is_init = false;
static telecommand_prototype_manager_t prototype_manager;
static prototype_hash_node_t* hash_table = NULL;


/********************************************************************************/
/* Private Method Defines														*/
/********************************************************************************/


/********************************************************************************/
/* Virtual Method Defines														*/
/********************************************************************************/
static _Bool register_prototype( telecommand_prototype_manager_t* self, const char* key, telecommand_t* prototype )
{
	DEV_ASSERT( self );
	DEV_ASSERT( key );
	DEV_ASSERT( prototype );

	prototype_hash_node_t* node;

	node = (prototype_hash_node_t*) pvPortMalloc( sizeof(prototype_hash_node_t) );
	if( node == NULL ){ return false; }

	strncpy( node->key, key, TELECOMMAND_PROTOTYPE_MANAGER_KEY_LENGTH );
	node->key[TELECOMMAND_PROTOTYPE_MANAGER_KEY_LENGTH] = '\0';
	node->prototype = prototype;
	HASH_ADD_STR( hash_table, key, node );

	return true;
}

static telecommand_t* get_prototype( telecommand_prototype_manager_t* self, const char* key )
{
	DEV_ASSERT( self );
	DEV_ASSERT( key );

	char local_key[TELECOMMAND_PROTOTYPE_MANAGER_KEY_LENGTH+1];
	prototype_hash_node_t* node;

	strncpy( local_key, key, TELECOMMAND_PROTOTYPE_MANAGER_KEY_LENGTH );
	local_key[TELECOMMAND_PROTOTYPE_MANAGER_KEY_LENGTH] = '\0';
	HASH_FIND_STR( hash_table, local_key, node );

	if( node == NULL ){ return NULL; }
	return node->prototype;
}


/********************************************************************************/
/* Destructor Define															*/
/********************************************************************************/



/********************************************************************************/
/* Constructor Define															*/
/********************************************************************************/
telecommand_prototype_manager_t* get_telecommand_prototype_manager( )
{
	if( !is_init )
	{
		prototype_manager.get_prototype = get_prototype;
		prototype_manager.register_prototype = register_prototype;
		is_init = true;
	}
	return &prototype_manager;
}


/********************************************************************************/
/* Public Method Defines														*/
/********************************************************************************/
