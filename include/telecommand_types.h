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
 * @file telecommand_types.h
 * @author Brendan Bruner
 * @date Aug 12, 2015
 */
#ifndef INCLUDE_TELECOMMANDS_TELECOMMAND_TYPES_H_
#define INCLUDE_TELECOMMANDS_TELECOMMAND_TYPES_H_

/* Warning. */
/* Do not include any file in the <telecommands/> folder. The */
/* compiler will blow up. */

/********************************************************************************/
/* #defines																		*/
/********************************************************************************/
#define TELECOMMAND_TOTAL_TYPES					44

#define TELECOMMAND_TYPE_DEFAULT 				0
#define TELECOMMAND_TYPE_BLINK					0
#define TELECOMMAND_TYPE_LOCK_CONFIG			1
#define TELECOMMAND_TYPE_UNLOCK_CONFIG			2
#define TELECOMMAND_TYPE_BEACON_WOD				3
#define TELECOMMAND_TYPE_DEPLOY_BOOM			4
#define TELECOMMAND_TYPE_DFGM_POWER				5
#define TELECOMMAND_TYPE_DFGM_STREAM			6
#define TELECOMMAND_TYPE_DFGM_DIAG				7
#define TELECOMMAND_TYPE_DOSIMETER				8
#define TELECOMMAND_TYPE_ATHENA					9
#define TELECOMMAND_TYPE_LOCALE_LOGGER			10
#define TELECOMMAND_TYPE_TELEMETRY_CLEANUP		11
#define TELECOMMAND_TYPE_DOWNLINK				12
#define TELECOMMAND_TYPE_ADCS					13
#define TELECOMMAND_TYPE_SCV_CONFIG				14
#define TELECOMMAND_TYPE_UART_FLUSH				15
#define TELECOMMAND_TYPE_OPERATE_ADCS			16
#define TELECOMMAND_TYPE_DEBUG_SET				17
#define TELECOMMAND_TYPE_TEST			18
/*#define TELECOMMAND_TYPE_						19
#define TELECOMMAND_TYPE_						20
#define TELECOMMAND_TYPE_						21
#define TELECOMMAND_TYPE_						22
#define TELECOMMAND_TYPE_ 						23*/
#define TELECOMMAND_TYPE_REBOOT					24
/* #define TELECOMMAND_TYPE_ 					25*/
#define TELECOMMAND_TYPE_SYNC_RTC				26
/* #define TELECOMMAND_TYPE_ 					27*/
#define TELECOMMAND_TYPE_PRIORITIZE_DOWNLINK	28
/* #define TELECOMMAND_TYPE_					29*/
/* #define TELECOMMAND_TYPE_ 					28*/
#define TELECOMMAND_TYPE_LOG_WOD				31
/* #define TELECOMMAND_TYPE_ 					29*/
#define TELECOMMAND_TYPE_CONFIGURE_EPS			33
/* #define TELECOMMAND_TYPE_ 					34*/
#define TELECOMMAND_TYPE_CONFIGURE_ADCS			35
/* #define TELECOMMAND_TYPE_ 					36*/
#define TELECOMMAND_TYPE_CONFIGURE_COMM			37
/* #define TELECOMMAND_TYPE_ 					38*/
#define TELECOMMAND_TYPE_AUTOMATED_DOWNLINK		39

#define TELECOMMAND_TYPE_DFGM_ON				40
#define TELECOMMAND_TYPE_DFGM_OFF				41
#define TELECOMMAND_TYPE_MNLP_REINIT			42
#define TELECOMMAND_TYPE_MNLP_OFF				43


/********************************************************************************/
/* Structure Documentation														*/
/********************************************************************************/



/********************************************************************************/
/* Structure Defines															*/
/********************************************************************************/



/********************************************************************************/
/* Constructor Declare															*/
/********************************************************************************/



/********************************************************************************/
/* Public Method Declares														*/
/********************************************************************************/


#endif /* INCLUDE_TELECOMMANDS_TELECOMMAND_TYPES_H_ */
