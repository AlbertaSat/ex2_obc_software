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
 * @file telecommand_prototype_manager.h
 * @author Brendan Bruner
 * @date Oct 13, 2015
 */
#ifndef INCLUDE_TELECOMMANDS_TELECOMMAND_PROTOTYPE_MANAGER_H_
#define INCLUDE_TELECOMMANDS_TELECOMMAND_PROTOTYPE_MANAGER_H_

#include "telecommand.h"

/********************************************************************************/
/* #defines																		*/
/********************************************************************************/
#define TELECOMMAND_PROTOTYPE_MANAGER_KEY_LENGTH 20


/********************************************************************************/
/* Structure Documentation														*/
/********************************************************************************/
/**
 * @struct teleccommand_prototype_manager_t
 * @brief
 * 		Manages dynamically loaded telecommand prototypes.
 * @details
 * 		Manages dynamically loaded telecommand prototypes. This class makes it easy to register
 * 		telecommand_t subclasses with a string name at run time. This class is used by the telecommand
 * 		interpreter to allocate commands as they are issued from a ground station or script.
 *
 * 		In addition, this is a singleton class.
 * @var telecommand_prototype_manager_t::register_prototype
 * 		<b>Public</b>
 * 		@code
 * 			_Bool register_prototype( telecommand_prototype_manager_t*, const char* command_string, telecommand_t* telecommand );
 * 		@endcode
 * 		Registers a prototype with the manager. The prototype is then retrieved via the
 * 		key <b>command_string</b>. <b>command_string</b> collisions are not handled, they will result
 * 		in run time logical errors. There should never be two commands with the same name anyways.
 *
 * 		<b>Parameters</b>
 * 		<ul>
 * 		<li><b>command_string</b>: The key that is used to register the telecommand instance.
 * 		Must be null terminated and be less than or equal to TELECOMMAND_PROTOTYPE_MANAGER_KEY_LENGTH
 * 		bytes long.</li>
 * 		<li><b>telecommand</b>: The prototype to register. This pointer must point to valid memory
 * 		and must continue to point to valid memory for the life of the program.</li>
 * 		</ul>
 *
 * 		<b>Returns</b>
 * 		<br><b>true</b> on successful registration of the prototype, <b>false</b> otherwise.
 * @var telecommand_prototype_manager_t::get_prototype
 * 		<b>Public</b>
 * 		@code
 * 			telecommand_t* get_prototype( telecommand_prototype_manager_t*, const char* command_string );
 * 		@endcode
 * 		Get a registered prototype.
 *
 * 		<b>Paramters</b>
 * 		<ul><li><b>command_string</b>: The key of the prototype to access.</li></ul>
 *
 * 		<b>Returns</b>
 * 		<br>A pointer to the prototype, <b>NULL</b> if there is no prototype at the given
 * 		key, <b>command_string</b>.
 */
typedef struct telecommand_prototype_manager_t telecommand_prototype_manager_t;


/********************************************************************************/
/* Structure Defines															*/
/********************************************************************************/
struct telecommand_prototype_manager_t
{
	_Bool (*register_prototype)( telecommand_prototype_manager_t*, const char*, telecommand_t* );
	telecommand_t* (*get_prototype)( telecommand_prototype_manager_t*, const char* );
};


/********************************************************************************/
/* Constructor Declare															*/
/********************************************************************************/
/**
 * @memberof telecommand_prototype_manager_t
 * @brief
 * 		Get the instance of the prototype manager.
 * @details
 * 		Get the instance of the prototype manager. This is a singleton class
 * 		therefore this method will always return a pointer to the same instance.
 * @returns
 * 		A pointer to the only valid instance of this struct type.
 */
telecommand_prototype_manager_t* get_telecommand_prototype_manager( );


/********************************************************************************/
/* Public Method Declares														*/
/********************************************************************************/



#endif /* INCLUDE_TELECOMMANDS_TELECOMMAND_PROTOTYPE_MANAGER_H_ */
