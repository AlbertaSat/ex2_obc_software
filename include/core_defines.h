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
 * @file core_defines
 * @author Brendan Bruner
 * @author Keith Mills
 * @date Mar 12, 2015
 */
#ifndef INCLUDE_CORE_DEFINES_H_
#define INCLUDE_CORE_DEFINES_H_



#include <portable_types.h>

#define ONE_BYTE 1

/********************************************************************************/
/* EPS																			*/
/********************************************************************************/
// #include <hub/hub.h>
#define POWER_CHANNEL_RETRY			2
#define POWER_CHANNEL_ON			1
#define POWER_CHANNEL_OFF			0

#define ADCS_5V0_POWER_CHANNEL		0
#define ADCS_3V3_POWER_CHANNEL		3
#define DFGM_5V0_POWER_CHANNEL		1
#define ATHENA_5V0_POWER_CHANNEL	1
#define NANOMIND_5V0_POWER_CHANNEL	1
#define GPS_3V3_POWER_CHANNEL		4
#define COMM_3V3_POWER_CHANNEL		2
#define NANOMIND_3V3_POWER_CHANNEL	5
#define NANOHUB_3V3_POWER_CHANNEL	5

/* EPS Mode Threshold Define Statements
 * #define mode_threshold   min_voltage_for_that_mode
 */
// extern volatile eps_vbatt_t OPTIMAL_THRESHOLD;
// extern volatile eps_vbatt_t NORMAL_THRESHOLD;
// extern volatile eps_vbatt_t POWER_SAFE_THRESHOLD;
// extern volatile eps_vbatt_t CRITICAL_THRESHOLD;

/* Systems on the hub's power line. */
#define MNLP_5V0_POWER_LINE 		HUB_POWER_LINE0
#define MNLP_3V3_POWER_LINE			HUB_POWER_LINE1


/********************************************************************************/
/* ADCS telecommand 															*/
/********************************************************************************/
#define ADCS_TELECOMMAND_BUFFER_LENGTH 256

/********************************************************************************/
/* Locale logger 																*/
/********************************************************************************/
#define ADCS_LOCALE_LOGGER_TASK_NAME			(const signed char*) "locale log"
#define ADCS_LOCALE_LOGGER_TASK_STACK			(4*1024)
#define ADCS_LOCALE_LOGGER_TASK_PRIO			(BASE_PRIORITY)

/* Default logging cadence in ms */
#define ADCS_LOCALE_LOGGER_DEFAULT_CADENCE 		(60*1000)

/* ms timeout when disabling locale logger task */
#define ADCS_LOCALE_LOGGER_DISABLE_TIMEOUT		(50)

/* config file which indicates previous boots enable status. */
#define ADCS_LOCALE_LOGGER_CONFIG_PATH			"FV-LLog.bin"

/********************************************************************************/
/* Configuration Files															*/
/********************************************************************************/
/* Delay of the global lock on configuration files. For release, this should be 5 minutes. */
#ifdef DEBUG
#define CONFIG_LOCK_DELAY ( (1000 * 10) / portTICK_RATE_MS ) /* 10 seconds. */
#else
#define CONFIG_LOCK_DELAY ( (1000 * 60 * 5) / portTICK_RATE_MS ) /* 5 minutes. */
#endif


/********************************************************************************/
/* Deployment mag data collection												*/
/********************************************************************************/
/* Time in seconds to collect mag data for before and after deployment */
#define PRE_DEPLOY_TIMEOUT_S	5
#define POST_DEPLOY_TIMEOUT_S	300


/********************************************************************************/
/* ATHENA																		*/
/********************************************************************************/
#define ATHENA_BIN_PATH 			"/sd/ATHENA.bin"
#define ATHENA_COMMAND_QUEUE_SIZE	10
#define ATHENA_TASK_NAME			(const signed char*) "athena"
#define ATHENA_TASK_PRIO 			1
#define ATHENA_TASK_STACK 			(4*1024)

/* Maximum size of an athena binary in bytes */
#define ATHENA_MAX_BINARY_SIZE		(100*1024)

/* Size in bytes of a packet from athena. */
#define ATHENA_PAYLOAD_PACKET_SIZE	137

/* SPI defines */
#define ATHENA_SPI_BAUD					100000
#define ATHENA_SPI_MODE 				0
#define ATHENA_SPI_BITS_PER_TRANSFER 	8
#define ATHENA_SPI_CS 					4
#define ATHENA_SPI_CS_REG				0
#define ATHENA_SPI_SPCK_DELAY			0
#define ATHENA_SPI_TRANS_DELAY			0

/* Payload extraction cadence in ms */
#define ATHENA_PAYLOAD_EXTRACT_CADENCE	10000

/********************************************************************************/
/* DFGM 																		*/
/********************************************************************************/
/* OS delay time between each packet, must not be zero. */
#define DFGM_INTER_PACKET_DELAY (ONE_MS*50)

/* Block time when stopping data logging task. Must not be block forever. */
#define DFGM_STOP_BLOCK_TIME (60*ONE_SECOND)

/* Time out in ms when reading data off usart. */
#define DFGM_INTER_PACKET_TIME_OUT (500/portTICK_RATE_MS)

/* Maximum number of raw packets which can be put in gateway queue */
#define DFGM_NUM_GATEWAY_ITEMS (60)

/* FIR filter configuration options for DFGM. */
#define DFGM_CONFIG_FILE_PATH 	"/sd/DFGM_FIR.txt"
#define DFGM_SELECT_FILE_PATH 	"/sd/DFGM_SEL.txt"
#define DFGM_DEFAULT_STREAM 	FILTER_ENABLE

/* Cadence at which HK00 to HK11 and PPSOFFSET are logged. */
#define DFGM_HK_LOG_CADENCE_MS	(1000*60*5) /* 5 minutes */
/* Must be greater than or equal to max total bytes in dfgm config file */
#define CONFIG_STREAM_MAX_LENGTH 5000
/* This define must be greater than: */
/* 		(max #taps in stream 1) + (max #taps in stream 2) + 50 */
#define MAX_JSMN_TOKENS 500

/* Log a time stamp on filtered data after this many raw packets have been received. */
#define DFGM_TIME_STAMP_CADENCE  500 /* Every 500'th packet */

#define DFGM_TASK_PRIO	(BASE_PRIORITY+1)
#define DFGM_TASK_STACK (1024*4)
#define DFGM_TASK_NAME	(const signed char*) "DFGM"

#define DFGM_GATEWAY_TASK_PRIO	(BASE_PRIORITY)
#define DFGM_GATEWAY_TASK_STACK (1024*4)
#define DFGM_GATEWAY_TASK_NAME	(const signed char*) "DFGM GW"

/* Max number of FIR taps that can ever be used. */
#define MAX_TAPS 1001
/* minimum period between samples. 10 means at minimum every 10'th sample is taken. */
/* 100 means at minimum every 100'th sample is taken, if this was defined as 100, it would */
/* be impossible to take every 10'th sample because the minimum is 100. */
/* This must never be zero. */
#define FIR_MIN_DOWN_SAMPLE 1

/* Task stats for the nanomind mag. */
#define NM_MAG_TASK_NAME	(const signed char*) "nm mag"
#define NM_MAG_TASK_STACK	(1024)
#define	NM_MAG_TASK_PRIO	(BASE_PRIORITY)

#define NM_MAG_LOG_FILE_PATH "/sd/NMMag.bin"
#define NM_MAG_DISABLE_TIMEOUT (50)
#define NM_MAG_LOG_PERIOD (1000)


/********************************************************************************/
/* Dosimeter																	*/
/********************************************************************************/
/* Maximum amount of time to wait when trying to resume dosimeter */
#define DOSIMETER_RESUME_TIMEOUT (ONE_MS*1000*5)

#define DOSIMETER_TASK_PRIO 	1
#define DOSIMETER_TASK_STACK 	1024
#define DOSIMETER_TASK_NAME 	(const signed char *) "UDOS"

#define UDOS_SAMPLES_PER_EVENT		2
#define UDOS_EVENTS_PER_PACKET		60
#define UDOS_PACKET_VERSION			1



/********************************************************************************/
/* Black Out State																*/
/********************************************************************************/
#define BLACK_OUT_FILE "lsbof.txt"
//#ifdef DEBUG
//#define BLACK_OUT_RESOLUTION 5 /* 5 seconds */
//#else
#define BLACK_OUT_RESOLUTION 30 /* 60 seconds */
//#endif

#define BLACKOUT_CONTINGENCY 1800
//#define BLACKOUT_CONTINGENCY 10

/********************************************************************************/
/* LEOP Detumble																*/
/********************************************************************************/
//#ifdef DEBUG
//#define LEOP_DETUMBLE_RESOLUTION 5 /* 5 seconds. */
//#else
#define LEOP_DETUMBLE_RESOLUTION 60 /* 60 seconds. */
//#endif

/********************************************************************************/
/* State Relay																	*/
/********************************************************************************/
#define LEOP_FLAG_FILE "lsff.txt"
#define BLACK_OUT_TIME_OUT_LOG "lsbo_tl.txt"
#define DETUMBLE_TIME_OUT_LOG "lsd_tl.txt"
//#ifdef DEBUG
//#define BLACK_OUT_TIME_OUT (10) /* 10 seconds. */
//#define DETUMBLE_TIME_OUT (60) /* 1 minute. */
//#else
#define BLACK_OUT_TIME_OUT 1800 /* 30 minutes. */
#define DETUMBLE_TIME_OUT (3 * 24 * 60 * 60) /* 3 days. */
//#endif

/********************************************************************************/
/* States																		*/
/********************************************************************************/
#define STATE_LEOP_CHECK_LOG			"slc_l.txt"
#define STATE_BLACK_OUT_LOG				"lsbo_l.txt"
#define STATE_DETUMBLE_LOG				"lsd_l.txt"
#define STATE_LEOP_POWER_CHARGE_LOG 	"lslpc_l.txt"
#define STATE_BOOM_DEPLOYMENT_LOG 		"lsbd_l.txt"
#define STATE_ANTENNA_DEPLOYMENT_LOG 	"lsad_l.txt"
#define STATE_ALIGNMENT_LOG 			"osa_l.txt"
#define STATE_BRING_UP_LOG				"osbu_l.txt"
#define STATE_LOW_POWER_LOG				"oslp_l.txt"
#define STATE_SCIENCE_LOG				"oss_l.txt"

/********************************************************************************/
/* Persistent Timer																*/
/********************************************************************************/
#define PTIMER_DEFAULT_RESOLUTION (60) /* 1 minute */
//#ifdef DEBUG
//#define PTIMER_CONTROLLER_UPDATE_CADENCE_MS (1000) /* 1 seconds */
//#else
#define PTIMER_CONTROLLER_UPDATE_CADENCE_MS (5 * 1000) /* 5 seconds */
//#endif
#define PTIMER_MANAGER_STACK_DEPTH 700
#define PTIMER_MANAGER_PRIORITY (BASE_PRIORITY+1)
#define PTIMER_MANAGER_NAME "ptimer mnger"

/********************************************************************************/
/* FTP																			*/
/********************************************************************************/
#define FTP_SLEEP_PERIOD 1000 /* 1 second */
#define FTP_NAME "ftp task"
#define FTP_STACK 700
#define FTP_PRIO (BASE_PRIORITY+1)

/********************************************************************************/
/* LEOP flag values																*/
/********************************************************************************/
#define LEOP_FLAG_UNFINISHED 0
#define LEOP_FLAG_FINISHED 2


/********************************************************************************/
/* File to store telemetry priorities											*/
/********************************************************************************/
#define PRIORITY_LOG_FILE "odp.txt"


/********************************************************************************/
/* Script Daemon																*/
/********************************************************************************/
#define SCRIPT_DAEMON_SLEEP_PERIOD	100 /* 100 ms */
#define SCRIPT_DAEMON_NAME			"script daemon"
#define SCRIPT_DAEMON_STACK			(4*1024)
#define SCRIPT_DAEMON_PRIO			(BASE_PRIORITY+1)
/* Max length of script sent directly from ground station. */
/* Does not include length of scripts in put in file system via ftp. */
#define SCRIPT_MAX_LENGTH			100
#define COMMAND_STRING_MAX_LENGTH 	(4*20+3)
#define MAX_COMMANDS_PER_SCHEDULE 	(4*60+3)

/********************************************************************************/
/* Script Time Server																*/
/********************************************************************************/
#define SCRIPT_TIME_SERVER_PERIOD	1000 /* 1 second */
#define SCRIPT_TIME_SERVER_NAME			"time server"
#define SCRIPT_TIME_SERVER_STACK			1024
#define SCRIPT_TIME_SERVER_PRIO			(BASE_PRIORITY+1)


/********************************************************************************/
/* SCV																			*/
/********************************************************************************/
// extern volatile size_t ATHENA_LOGGER_MAX_CAPACITY;
// extern volatile size_t ATHENA_LOGGER_FILE_SIZE;
// #define ATHENA_LOGGER_IDENTIFIER					'K'
// #define ATHENA_LOGGER_MASTER_TABLE					"MT_KLog.txt"

// extern volatile size_t DFGM_FILT1_LOGGER_MAX_CAPACITY;
// #define DFGM_FILT1_LOGGER_IDENTIFIER				'A'
// #define DFGM_FILT1_LOGGER_MASTER_TABLE				"MT-ALog.txt"

// extern volatile size_t DFGM_FILT2_LOGGER_MAX_CAPACITY;
// #define DFGM_FILT2_LOGGER_IDENTIFIER				'B'
// #define DFGM_FILT2_LOGGER_MASTER_TABLE				"MT-BLog.txt"

// extern volatile size_t MAX_DFGM_FILT_FILE_SIZE;

// extern volatile size_t DFGM_RAW_LOGGER_MAX_CAPACITY;
// extern volatile size_t MAX_DFGM_RAW_FILE_SIZE;
// #define DFGM_RAW_LOGGER_IDENTIFIER					'R'
// #define DFGM_RAW_LOGGER_MASTER_TABLE				"MT-RLog.txt"

// #define ADCS_LOCALE_LOGGER_CONTROL_FILE_PATH 		"MT-LLog.bin"
// #define ADCS_LOCALE_LOGGER_NAME_PATH				'L'
// extern volatile size_t ADCS_LOCALE_LOGGER_CAPACITY;
// extern volatile size_t ADCS_LOCALE_LOGGER_MAX_FILE_SIZE;

// extern volatile size_t DFGM_HK_LOGGER_MAX_CAPACITY;
// #define DFGM_HK_LOGGER_IDENTIFIER					'H'
// #define DFGM_HK_LOGGER_MASTER_TABLE					"MT-HLog.txt"
// extern volatile size_t MAX_DFGM_HK_FILE_SIZE;

// extern volatile size_t WOD_LOGGER_MAX_CAPACITY;
// extern volatile size_t PACKET_TYPE_WOD_SIZE;
// extern volatile size_t WOD_LOG_MULTIPLIER;
// #define WOD_LOGGER_IDENTIFIER						'W'
// #define WOD_LOGGER_MASTER_TABLE						"MT-WLog.txt"
// #define WOD_LOG_CADENCE								30000	// Hard fixed Beacon of 30 seconds

// extern volatile size_t UDOS_LOGGER_MAX_CAPACITY;
// extern volatile size_t UDOS_LOGGER_FILE_SIZE;
// extern volatile size_t UDOS_EVENT_PERIOD;
// #define UDOS_LOGGER_IDENTIFIER						'D'
// #define UDOS_LOGGER_MASTER_TABLE					"MT_DLog.txt"


#endif /* INCLUDE_CORE_DEFINES_H_ */
