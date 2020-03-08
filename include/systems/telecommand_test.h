/*
 * Copyright (C) 2018  Brendan Bruner
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
 * @file telecommand_rtos_status.h
 * @author Brendan Bruner
 * @date April 2018
 */
#ifndef INCLUDE_TELECOMMANDS_test_H_
#define INCLUDE_TELECOMMANDS_test_H_

#include <telecommand.h>

/********************************************************************************/
/* #defines																		*/
/********************************************************************************/
#define TELECOMMAND_RTOS_TASK_LISTING 50*30 /* 30 tasks, 50 bytes per status message. */


/********************************************************************************/
/* Structure Documentation														*/
/********************************************************************************/
/**
 * @extends telecommand_t
 * @struct telecommand_test_t
 * @brief
 * 		Print diagnostics to csp-term
 * @details
 * 		Print diagnostics to csp-term
 */
typedef struct telecommand_test_t telecommand_test_t;


/********************************************************************************/
/* Structure Defines															*/
/********************************************************************************/
struct telecommand_test_t
{
	telecommand_t _super_;
	char task_list[TELECOMMAND_RTOS_TASK_LISTING];
};


/********************************************************************************/
/* Constructor Declare															*/
/********************************************************************************/
/**
 * @memberof telecommand_test_t
 * @brief
 * 		Initialize
 * @details
 * 		Initializer
 * @param kit[in]
 * 		Used by the command to access the rtc API and logger API.
 * @returns
 * 		<b>TELECOMMAND_SUCCESS</b>: Successful initialization.
 * 		<br><b>TELECOMMAND_FAILURE</b>: Fatal error initializing. The structure is not safe to use.
 */
_Bool initialize_telecommand_test( telecommand_test_t *self, driver_toolkit_t *kit );


/********************************************************************************/
/* Public Method Declares														*/
/********************************************************************************/


#endif /* INCLUDE_TELECOMMANDS_rtos_status_H_ */
