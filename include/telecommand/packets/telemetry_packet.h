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
 * @file TelemetryPacket.h
 * @author Brendan Bruner
 * @date 2015-01-12
 *
 * Defines the packet format for telemetry and priority convention.
 */

#ifndef TELEMETRYPACKET_H_
#define TELEMETRYPACKET_H_

#include <stdint.h>

/********************************************************************************/
/* Defines																		*/
/********************************************************************************/
#define META_DATA_SIZE 				sizeof( telemetry_metadata_t )

#define PACKET_MAX_TYPES				5
#define PACKET_TYPE_WOD					0
#define PACKET_TYPE_DFGM				1
#define PACKET_TYPE_MNLP				2
#define PACKET_TYPE_CMND_STATUS			3
#define PACKET_TYPE_STATE				4
#define PACKET_TYPE_LIST_FILES			5
#define PACKET_TYPE_DOWNLINK_FILE		6
#define PACKET_TYPE_BEACON_RESPONSE		7

#define PACKET_TYPE_DFGM_SIZE			32
#define PACKET_TYPE_MNLP_SIZE			32
#define PACKET_TYPE_CMND_STATUS_SIZE	((2 + 4 + 1) * 30) /* Up to 30 telecommands */
#define PACKET_TYPE_STATE_SIZE			150
#define PACKET_TYPE_FILE_SIZE_SIZE		5

#define MAX_PACKET_SIZE 				260
#define MAX_TELEMETRY_PACKET_SIZE		MAX_PACKET_SIZE
#define TELEMETRY_PACKET_SIZE		sizeof( telemetry_packet_t )




/********************************************************************************/
/* Public Globals																*/
/********************************************************************************/
extern uint32_t const packet_map_type_to_size[PACKET_MAX_TYPES];


/********************************************************************************/
/* Structure Documentation														*/
/********************************************************************************/
/**
 * @struct telemetry_metadata_t
 * @brief
 * 		Defines the meta data in a telemetry packet.
 *
 * @details
 * 		Defines the meta data in a telemetry packet. The structure is packed
 * 		so that no padding bytes are added.
 *
 * @var telemetry_metadata_t::type
 * 		Indicates what type of packet it is.
 *
 * @var telemetry_metadata_t::id
 * 		The id of the packet.
 *
 * @var telemetry_metadata_t::data_size
 * 		If there is additional data beyond this meta data, this field
 * 		indicates how many bytes of extra data there are.
 */
typedef struct telemetry_metadata_t telemetry_metadata_t;

/**
 * @struct telemetry_packet_t
 * @brief
 * 		Defines the meta data and data of telemetry.
 * @details
 * 		Defines the meta data and data of telemetry. The structure is packed
 * 		so that no padding bytes are added.
 * @var telemetry_packet_t::meta_data
 * 		Defines the meta data for the packet.
 * @var telemetry_packet_t::data
 * 		A pointer to a uint8_t array containing extra data. The array pointed to is
 * 		telemetry_metadata_t::data_size bytes long.
 */
typedef struct telemetry_packet_t telemetry_packet_t;


/********************************************************************************/
/* Structure Define																*/
/********************************************************************************/
struct __attribute__((packed)) telemetry_metadata_t
{
	uint8_t type;
	uint32_t id;
	uint32_t data_size;
};

struct __attribute__((packed)) telemetry_packet_t
{
	telemetry_metadata_t meta_data;
	uint8_t *data;
};


/********************************************************************************/
/* Function Defines																*/
/********************************************************************************/

#endif /* TELEMETRYPACKET_H_ */
