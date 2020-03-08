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
 * @file telecommand.c
 * @author Brendan Bruner
 * @date Aug 12, 2015
 */

#include <telecommand.h>
// #include <packets/packet_base.h>
#include <string.h>
// #include <io/nanomind.h>
// #include <csp/csp.h>


/********************************************************************************/
/* #defines																		*/
/********************************************************************************/
#define STATUS_PACKET_SIZE 7


/********************************************************************************/
/* Singleton Variable Defines													*/
/********************************************************************************/



/********************************************************************************/
/* Private Method Defines														*/
/********************************************************************************/
void telecommand_log_status( telecommand_t *command )
{
	DEV_ASSERT( command );
	DEV_ASSERT( command->_kit );

	// rtc_t 				*rtc;
	//uint8_t 			status_packet[STATUS_PACKET_SIZE];
	uint16_t 			cmnd_type;
	uint32_t 			time;
	uint8_t 			status;
	FILE* fid2;

	// rtc = command->_kit->rtc;
	cmnd_type = telecommand_get_type( command );
	// time = rtc->get_ds1302_time();
	status = (uint8_t) telecommand_status( command );

	/* Put the collected data into the status_packet array in */
	/* network byte order. */
	//to_network_from16( &cmnd_type, status_packet );
	//to_network_from32( &time, status_packet + 2 );
	//to_network_from8( &status, status_packet + 6 );

	/* If there is an error logging, nothing can be done to correct it. Therefore */
	/* disregard returned error code. */
	fid2 = fopen("/boot/tc_log.bin","a");
	fwrite(&cmnd_type, 2, 1, fid2);
	// fwrite(&time, 4, 1, fid2);
	fwrite(&status, 1, 1, fid2);
	fclose(fid2);
}


/********************************************************************************/
/* Virtual Method Defines														*/
/********************************************************************************/
static void base_execute( telecommand_t *command )
{
	_telecommand_set_status( command, CMND_FAILED );
}

static telecommand_t* clone( telecommand_t* self )
{
	DEV_ASSERT( self );
	return NULL;
}


/********************************************************************************/
/* Destructor Define															*/
/********************************************************************************/
static void destroy( telecommand_t *self )
{
	DEV_ASSERT( self );
}


/********************************************************************************/
/* Constructor Define															*/
/********************************************************************************/
_Bool initialize_telecommand( telecommand_t *command, driver_toolkit_t *kit )
{
	DEV_ASSERT( (command) );
	DEV_ASSERT( (kit) );
	(command)->_kit = kit;
	(command)->_execute = base_execute;
	(command)->destroy = destroy;
	(command)->_execution_status_ = CMND_PENDING;
	(command)->_id_ = 0;
	(command)->_type_ = TELECOMMAND_TYPE_DEFAULT;
	command->clone = clone;
	command->_argument_length = 0;

	/* Zero out argument string */
	int i;
	for( i = 0; i < TELECOMMAND_MAX_ARGUMENT_LENGTH; ++i ) {
		command->_argument[i] = '\0';
	}

	return TELECOMMAND_SUCCESS;
}


/********************************************************************************/
/* Protected Method Defines														*/
/********************************************************************************/
void _telecommand_set_status( telecommand_t *self, telecommand_status_t status )
{
	self->_execution_status_ = status;
}

void _telecommand_set_type( telecommand_t *self, uint16_t type )
{
	self->_type_ = type;
}


/********************************************************************************/
/* Public Method Defines														*/
/********************************************************************************/
void telecommand_argument( telecommand_t* self, char const* argument, uint32_t length )
{
	DEV_ASSERT( self );
	DEV_ASSERT( argument );

	if( length > TELECOMMAND_MAX_ARGUMENT_LENGTH ) {
		length = TELECOMMAND_MAX_ARGUMENT_LENGTH;
	}

	strncpy( self->_argument, argument, length );
	self->_argument_length = length;
}

void telecommand_execute( telecommand_t *command )
{
	DEV_ASSERT( command );
	DEV_ASSERT( command->_execute );

	/* Set its execution status as */
	/* 'CMND_EXECUTING'. */
	_telecommand_set_status( command, CMND_EXECUTING );

	/* If not a default command, log that it is beginning execution. */
	/* This is done to avoid cluttering the log file with misc commands. */
	if( telecommand_get_type( command ) != TELECOMMAND_TYPE_DEFAULT )
	{
		telecommand_log_status( command );
	}
	/* Execute the command. */
	command->_execute( command );

	/* If the execution has not indicated a failure status, set the */
	/* status as CMND_EXECUTED. */
	if( telecommand_status( command ) == CMND_EXECUTING )
	{
		_telecommand_set_status( command, CMND_EXECUTED );
	}

	/* If not a default command, log its end of execution status. */
	// if( telecommand_get_type( command ) != TELECOMMAND_TYPE_DEFAULT )
	// {
	// 	/* beacon status response if command was not log wod command.
	// 	 */
	// 	if( telecommand_get_type(command) != TELECOMMAND_TYPE_LOG_WOD ) {
	// 		int status = (int) telecommand_status(command);
	// 		csp_transaction(CSP_PRIO_NORM, 10, OBC_CMD_RSP_PORT, 1200, &status, sizeof(status), NULL, 0);
	// 	}
	// 	telecommand_log_status( command );
	// }
}

telecommand_status_t telecommand_status( telecommand_t *command )
{
	return command->_execution_status_;
}

void telecommand_set_id( telecommand_t *command, uint32_t id )
{
	command->_id_ = id;
}

uint32_t telecommand_get_id( telecommand_t *command )
{
	return command->_id_;
}

uint16_t telecommand_get_type( telecommand_t *command )
{
	return command->_type_;
}
