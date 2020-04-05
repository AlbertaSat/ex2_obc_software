/*
 * Copyright (C) 2015 Brendan Bruner
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
 * @file telecommand.h
 * @author Brendan Bruner
 * @date 2014-12-28
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <telecommand/telecommand_types.h>
#include <driver_toolkit/driver_toolkit.h>


/********************************************************************************/
/* Defines 																		*/
/********************************************************************************/
#define TELECOMMAND_SUCCESS 1
#define TELECOMMAND_FAILURE 0

#define TELECOMMAND_MAX_ARGUMENT_LENGTH 30


/************************************************************************/
/* Structure Documentation												*/
/************************************************************************/
/**
 * @struct telecommand_t
 * @brief
 * 		The abstract structure of a telecommand.
 * @details
 * 		The abstract structure of a telecommand.
 * 		A telecommand_t structure must remain valid in memory until it is
 * 		finished execution. Whether it is in .bss, stack, or heap does
 * 		not matter.
 * 		<br>For example,
 * 			@code
 * 			telecommand_reboot_t reboot_command;
 *
 * 			initialize_telecommand_reboot( &reboot_command, kit );
 * 			// Other processing..
 * 			telecommand_execute( (telecommand_t *) &reboot_command );
 * 			@endcode
 * 		In this example, the variable 'kit' is an instance of a
 * 		driver_toolkit_t structure.
 * @var telecommand_t::_id_
 * 		<b>Private</b>
 * 		A unique identifier of the telecommand. When logging of telecommand execution
 * 		status is done, this identifier gets logged too.
 * @var telecommand_t::_type_
 * 		<b>Private</b>
 * 		An identifier of the telecommand. Logged like telecommand_t::_id_, but not unique.
 * 		This is used to identify what the command is. For example, all commands that perform
 * 		operation X will have the same <b>_type_</b> and only commands which perform
 * 		operation X have that <b>_type_</b>.
 * @var telecommand_t::_execution_status_
 * 		<b>Private</b>
 * 		Contains the execution status of the telecommand instance. This is used in logging
 * 		of telecommands.
 * @var telecommand_t::_execution_date_
 * 		<b>Private</b>
 * 		Specifies a point in time to wait until the telecommand can be executed. Any attempts
 * 		to execute the command through the telecommand_execute( ) method will fail until this
 * 		time has been reached. A telecommands default execution date is:
 * 		<br>January 1, 0000 @ 00h:00m:00s
 * @var telecommand_t::_kit;
 * 		<b>Protected</b>
 * 		Used to access RTC and logger API. The RTC is used to enable the time delay functionality
 * 		of telecommand. The logger API is used to log execution status'.
 * @var telecommand_t::_execute;
 * 		<b>Protected</b>
 * 			@code
 * 			void execute( telecommand_t *self );
 * 			@endcode
 * 		This method defines the procedure that is run when telecommand_execute( ) is called.
 * 		Inheriting structures will override this method to define what happens on execution.
 * @var telecommand_t::destroy
 * 		<b>Public</b>
 * 			@code
 * 			void destroy( telecommand_t * );
 * 			@endcode
 * 		The destructor for telecommands. Inheriting classes must override this method if
 * 		the destruction is not sufficient.
 * @var telecommand_t::clone
 * 		<b>Public</b>
 * 		@code
 * 			telecommand_t* clone( telecommand_t * );
 * 		@endcode
 * 		Return a deep clone of the telecommand. The clone's memory is allocated
 * 		with pvPortMalloc( ) and therefore must be freed with vPortFree( ).
 *
 * 		<b>Returns</b>
 * 		<br>A pointer to the new clone, <b>NULL</b> if cloning failed.
 */
typedef struct telecommand_t telecommand_t;

/** Possible states of command execution. */
typedef enum
{
	CMND_EXECUTING = 0,	/*!< (0) In the process of executing. */
	CMND_EXECUTED,		/*!< (1) Finished executing. */
	CMND_PENDING,		/*!< (2) Awaiting execution. */
	CMND_FAILED,		/*!< (3) Failed to execute correctly. */
	CMND_OVERFLOW,		/*!< (4) Failed due to buffer overflow. */
	CMND_MEM_FULL,		/*!< (5) Failed due to full non volatile memory. */
	CMND_NVMEM_ERR,		/*!< (6) Failed due to error interacting with non volatile memory. */
	CMND_TIME_ERR,		/*!< (7) Failure to apply a time stamp. */
	CMND_SYNTX_ERR		/*!< (8) Invalid syntax. */
} telecommand_status_t;


/************************************************************************/
/* Structure Declares													*/
/************************************************************************/
typedef void (*execution_handler)( telecommand_t * );
struct telecommand_t
{
	uint32_t				_id_;
	uint16_t				_type_;
	telecommand_status_t	_execution_status_;
	driver_toolkit_t		*_kit;
	char 					_argument[TELECOMMAND_MAX_ARGUMENT_LENGTH];
	uint32_t				_argument_length;

	execution_handler _execute;
	telecommand_t* (*clone)( telecommand_t * );
	void (*destroy)( telecommand_t * );
};


/************************************************************************/
/* Constructor															*/
/************************************************************************/
/**
 * @memberof telecommand_t
 * @brief
 * 		Initialize a telecommand.
 * @details
 * 		Initialize a telecommand. This is the base initialization for all telecommands.
 * @param command[in]
 * 		The command to initialize.
 * @param kit[in]
 * 		This structure will be used, in general, for its rtc and cmnd status logger.
 * 		However, additional drivers may be utilized.
 * @returns
 * 		<b>TELECOMMAND_SUCCESS / true</b>: Successful initialization.
 * 		<br><b>TELECOMMAND_FAILURE / false</b>: Fatal error initializing. The structure is not safe to use.
 */
_Bool initialize_telecommand( telecommand_t *command, driver_toolkit_t *kit );


/************************************************************************/
/* Protected Methods Declares											*/
/************************************************************************/
/**
 * @memberof telecommand_t
 * @protected
 * @brief
 * 		Set the execution status of a command.
 * @details
 * 		Set the execution status of a command.
 * @param status[in]
 * 		The status to set the command to.
 */
void _telecommand_set_status( telecommand_t *self, telecommand_status_t status );

/**
 * @memberof telecommand_t
 * @protected
 * @brief
 * 		Set the type of a command.
 * @details
 * 		Set the type of a command.
 * @param status[in]
 * 		The type to set the command to.
 */
void _telecommand_set_type( telecommand_t *self, uint16_t type );


/************************************************************************/
/* Public Methods Declares												*/
/************************************************************************/
/**
 * @memberof telecommand_T
 * @brief
 * 		Adds an additional argument to the telecommand.
 * @details
 * 		Adds an additional argument to the telecommand. This argument is a string. Typically,
 * 		this will be the name of a file for the telecommand to operate on.
 * @param argument[in]
 * 		Copied. Must be less than <b>TELECOMMAND_MAX_ARGUMENT_LENGTH</b> bytes long.
 * @param length
 * 		The length of the <b>argument</b> in bytes.
 */
void telecommand_argument( telecommand_t*, char const* argument, uint32_t length );

/**
 * @brief
 * 		Call this method to execute a command.
 * @details
 * 		Call this method to execute a command.
 * @memberof telecommand_t
 */
void telecommand_execute( telecommand_t *self );

/**
 * @brief
 * 		Returns the current execution status of a command.
 * @details
 * 		Returns the current execution status of a command.
 * @returns
 * 		The command's status.
 * @attention
 * 		The status of a command is not mutex protected. Be aware.
 * @memberof telecommand_t
 */
telecommand_status_t telecommand_status( telecommand_t *self );

/**
 * @memberof telecommand_t
 * @brief
 * 		Set the id of a telecommand.
 * @details
 * 		Set the id of a telecommand. This id is used when logging
 * 		telecommand status to non volatile memory and will be the only
 * 		way to distinguish between commands.
 * 		Default id is 0 (when no id gets set).
 * @remarks
 * 		Not thread safe.
 * @param id
 * 		The id value
 */
void telecommand_set_id( telecommand_t *self, uint32_t id );

/**
 * @memberof telecommand_t
 * @brief
 * 		Get the id of a telecommand.
 * @details
 * 		Get the id of a telecommand.
 * @remarks
 * 		Not thread safe.
 * @returns
 * 		The id of the command
 */
uint32_t telecommand_get_id( telecommand_t *self );

/**
 * @memberof telecommand_t
 * @brief
 * 		Get the telecommand type.
 * @details
 * 		Get the telecommand type. This implies the type of command,
 * 		ex: set state command, poll file size command, etc.
 * @attention
 * 		Never change a commands type!! There is no set_type function
 * 		and the telecommand_t::_type_ variable is private! This should
 * 		be sufficient warning.. do not change a commands type.
 */
uint16_t telecommand_get_type( telecommand_t *self );

/**
 * @memberof telecommand_t
 * @brief
 * 		Logs the execution status of a command.
 * @details
 * 		Logs the execution status of a command. The status logged is that of
 * 		telecommand_t::_execution_status_.
 */
void telecommand_log_status( telecommand_t *command );


#endif /* COMMAND_H_ */
