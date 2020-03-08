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
 * @file TelecommandPacket.h
 * @author Brendan Bruner
 * @date 2015-01-12
 *
 * Defines the packet format for a telecommand from gound station.
 */

#ifndef TELECOMMANDPACKET_H_
#define TELECOMMANDPACKET_H_

#include <stdint.h>

#define PRIORITY_MASK	0xfe
#define	OPTIONS_MASK	0x01

#define TELECOMMAND_MAX_PAYLOAD_SIZE			50
#define TELECOMMAND_PACKET_SIZE			sizeof( telecommand_packet_t )

#define TELECOMMAND_MAX_PRIORITIES				10
#define TELECOMMAND_PRIORITY_COLLECT_HK			0
#define TELECOMMAND_PRIORITY_TRANSMIT_DATA		1
#define TELECOMMAND_PRIORITY_HANDLE_MNLP		2
#define TELECOMMAND_PRIORITY_HANDLE_DFGM		3
#define TELECOMMAND_PRIORITY_REQUEST_RESPONCE	4
#define TELECOMMAND_PRIORITY_DIAGNOSTICS		5


/**
 * @struct telecommand_metadata_t
 *
 * @brief
 * 		Packet format of the meta data for telecommand packets.
 *
 * @details
 * 		Packet format of the meta data for telecommand packets.
 *
 * @var telecommand_metadata_t::priority
 * 		 Priority of telecommand.
 *
 * @var telecommand_metadata_t::data_size
 * 		If there is additional data beyond this meta data then this is
 * 		the number of bytes of extra data.
 *
 * @var telecommand_metadata_t::exec_date
 * 		Date at which the command can be executed.
 *
 * @var telecommand_metadata_t::type
 * 		The type of telecommand. This field tells the OBC what telecommand
 * 		this packet refers to. For example, a value of zero might indicate
 * 		a soft reset command should be executed.
 *
 * @var telecommand_metadata_t::id
 * 		The unique id of the telecommand. No telecommands have the same id,
 * 		even if their type is the same.
 */
typedef struct __attribute__((packed))
{
	uint8_t 	priority;
	uint16_t 	data_size;
	uint16_t	type;
	uint32_t	id;
} telecommand_metadata_t;

/**
 * @struct telecommand_packet_t
 * @brief
 * 		Defines the meta data and data of a telecommand packet.
 * @details
 * 		Defines the meta data and data of a telecommand packet.
 * @var telecommand_packet_t::meta_data
 * 		The meta data of the packet.
 * @var telecommand_packet_t::payload_location
 * 		A pointer to the location of payload data. The array pointed to is
 * 		telecommand_metadata_t::data_size bytes long.
 */
typedef struct __attribute__((packed))
{
	telecommand_metadata_t meta_data;
	uint8_t *data;
} telecommand_packet_t;

#define NO_OPTIONS 			0
#define OPTIONS_AVAILABLE 	1

#define extract_priority( tc )			( (tc)->meta_data.priority )
#define extract_options_bool( tc ) 		( (tc)->meta_data.data_size > 0 ? OPTIONS_AVAILABLE : NO_OPTIONS )
#define extract_payload_size( tc )		( (tc)->meta_data.data_size )
#define extract_payload_location( tc )	( (tc)->data )
#define extract_exec_date( tc )			( (tc)->meta_data.exec_date )
#define extract_code( tc )				( (tc)->meta_data.type )
#define extract_id( tc )				( (tc)->meta_data.id )

#define packetize_telecommand( raw, formatted ) \
	do {																			\
		(formatted)->meta_data.priority = (raw)[0];									\
																					\
		(formatted)->meta_data.data_size = (uint16_t) ((raw)[1] << 8);				\
		(formatted)->meta_data.data_size += (uint16_t) ((raw)[2] << 0);				\
																					\
		(formatted)->meta_data.exec_date.seconds = (uint32_t) ((raw)[3] << 24);		\
		(formatted)->meta_data.exec_date.seconds += (uint32_t) ((raw)[4] << 16);	\
		(formatted)->meta_data.exec_date.seconds += (uint32_t) ((raw)[5] << 8);		\
		(formatted)->meta_data.exec_date.seconds += (uint32_t) ((raw)[6] << 0);		\
																					\
		(formatted)->meta_data.exec_date.minutes = (uint32_t) ((raw)[7] << 24);		\
		(formatted)->meta_data.exec_date.minutes += (uint32_t) ((raw)[8] << 16);	\
		(formatted)->meta_data.exec_date.minutes += (uint32_t) ((raw)[9] << 8);		\
		(formatted)->meta_data.exec_date.minutes += (uint32_t) ((raw)[10] << 0);	\
																					\
		(formatted)->meta_data.exec_date.hours = (uint32_t) ((raw)[11] << 24);		\
		(formatted)->meta_data.exec_date.hours += (uint32_t) ((raw)[12] << 16);		\
		(formatted)->meta_data.exec_date.hours += (uint32_t) ((raw)[13] << 8);		\
		(formatted)->meta_data.exec_date.hours += (uint32_t) ((raw)[14] << 0);		\
																					\
		(formatted)->meta_data.exec_date.day_of_month = (uint32_t) ((raw)[15] << 24);	\
		(formatted)->meta_data.exec_date.day_of_month += (uint32_t) ((raw)[16] << 16);	\
		(formatted)->meta_data.exec_date.day_of_month += (uint32_t) ((raw)[17] << 8);	\
		(formatted)->meta_data.exec_date.day_of_month += (uint32_t) ((raw)[18] << 0);	\
																					\
		(formatted)->meta_data.exec_date.month = (uint32_t) ((raw)[19] << 24);		\
		(formatted)->meta_data.exec_date.month += (uint32_t) ((raw)[20] << 16);		\
		(formatted)->meta_data.exec_date.month += (uint32_t) ((raw)[21] << 8);		\
		(formatted)->meta_data.exec_date.month += (uint32_t) ((raw)[22] << 0);		\
																					\
		(formatted)->meta_data.exec_date.year = (uint32_t) ((raw)[23] << 24);		\
		(formatted)->meta_data.exec_date.year += (uint32_t) ((raw)[24] << 16);		\
		(formatted)->meta_data.exec_date.year += (uint32_t) ((raw)[25] << 8);		\
		(formatted)->meta_data.exec_date.year += (uint32_t) ((raw)[26] << 0);		\
																					\
		(formatted)->meta_data.type = (uint16_t) ((raw)[27] << 8);					\
		(formatted)->meta_data.type = (uint16_t) ((formatted)->meta_data.type +		\
									  ((raw)[28] << 0));							\
										 	 	 	 	 	 	 	 	 	 	 	\
		to32_from_network( &(formatted)->meta_data.id, (raw) + 29 );				\
																					\
		(formatted)->data = (raw) + 34;												\
	} while( 0 )


#endif /* TELECOMMANDPACKET_H_ */
