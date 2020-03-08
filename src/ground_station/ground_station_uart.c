// /*
//  * Copyright (C) 2015  Brendan Bruner
//  *
//  * This program is free software; you can redistribute it and/or
//  * modify it under the terms of the GNU General Public License
//  * as published by the Free Software Foundation; either version 2
//  * of the License, or (at your option) any later version.
//  *
//  * This program is distributed in the hope that it will be useful,
//  * but WITHOUT ANY WARRANTY; without even the implied warranty of
//  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  * GNU General Public License for more details.
//  *
//  * bbruner@ualberta.ca
//  */
// /**
//  * @file ground_station_uart.c
//  * @author Brendan Bruner
//  * @date Jun 1, 2015
//  */
// #include <ground_station.h>
// #include <uart/uart.h>
// #include <portable_types.h>
// #include <packets/packet_base.h>

// #define RX_QUEUE_LENGTH 10
// #define LPC_ARDUINO_ESCAPE	0x2f
// #define LPC_ARDUINO_START 	0x02
// #define LPC_ARDUINO_END		0x03
// #define LPC_ARDUINO_BEACON	0x07

// /********************************************************************************/
// /* Private																		*/
// /********************************************************************************/
// #define GROUND_STATION_MAX_RX_LENGTH 50
// typedef struct
// {
// 	uint8_t rx_buff[GROUND_STATION_MAX_RX_LENGTH];
// 	uint32_t size;
// } gs_buff_t;
// #define GS_GATEWAY_QUEUE_SIZE sizeof( gs_buff_t )
// static uint8_t is_init = 0;
// static queue_t gateway_rx_port0;
// static queue_t gateway_rx_port1;
// static queue_t gateway_rx_port2;
// static mutex_t gs_mutex;
// static uart_t* gs_uart;
// static uint8_t* write_buff;
// #define WRITE_BUFF_SIZE 300


// static void gs_gateway( void *param )
// {
// 	UNUSED( param );

// 	gs_buff_t rx_buff;
// 	uint8_t temp_buff[1];
// 	uint8_t port;
// 	uint32_t size;

// 	for( ;; )
// 	{
// 		size = 0;

// 		/* loop until escape character appears. */
// 		while( gs_uart->read( gs_uart, temp_buff, 1 ) == 0 ){ task_delay( 100 * ONE_MS ); }
// 		if( temp_buff[0] == LPC_ARDUINO_ESCAPE )
// 		{
// 			/* Wait forever for next character. */
// 			while( gs_uart->read( gs_uart, temp_buff, 1 ) == 0 ){ task_delay( 100 * ONE_MS ); }
// 			/* If next character is start character, continue. */
// 			if( temp_buff[0] == LPC_ARDUINO_START )
// 			{
// 				/* have start of packet. */
// 				/* Wait forever for port number. */
// 				while( gs_uart->read( gs_uart, temp_buff, 1 ) == 0 ){ task_delay( 100 * ONE_MS ); }
// 				port = temp_buff[0];

// 				while( 1 )
// 				{
// 					/* Wait forever for next character. */
// 					while( gs_uart->read( gs_uart, temp_buff, 1 ) == 0 ){ task_delay( 100 * ONE_MS ); }
// 					if( temp_buff[0] == LPC_ARDUINO_ESCAPE )
// 					{
// 						while( gs_uart->read( gs_uart, temp_buff, 1 ) == 0 ){ task_delay( 100 * ONE_MS ); }
// 						if( temp_buff[0] == LPC_ARDUINO_ESCAPE )
// 						{
// 							/* escape character as regular text. */
// 							if( size >= GROUND_STATION_MAX_RX_LENGTH )
// 							{
// 								/* corruption. */
// 								size = 0;
// 								break;
// 							}

// 							rx_buff.rx_buff[size] = temp_buff[0];
// 							++size;
// 						}
// 						else if( temp_buff[0] == LPC_ARDUINO_END )
// 						{
// 							/* End of packet. */
// 							break;
// 						}
// 						else if (temp_buff[0] == LPC_ARDUINO_BEACON )
// 						{
// 							/* ground station beaconing */
// 							/* ensure correct tail. */
// 							while( gs_uart->read( gs_uart, temp_buff, 1 ) == 0 ){ task_delay( 100 * ONE_MS ); }
// 							if( temp_buff[0] == LPC_ARDUINO_ESCAPE )
// 							{
// 								while( gs_uart->read( gs_uart, temp_buff, 1 ) == 0 ){ task_delay( 100 * ONE_MS ); }
// 								if( temp_buff[0] == LPC_ARDUINO_END )
// 								{
// 									/* Ground station sent a beacon. */
// 								}
// 							}
// 						}
// 						else
// 						{
// 							/* corruption. */
// 							size = 0;
// 							break;
// 						}
// 					}
// 					else
// 					{
// 						if( size >= GROUND_STATION_MAX_RX_LENGTH )
// 						{
// 							/* corruption. */
// 							size = 0;
// 							break;
// 						}

// 						rx_buff.rx_buff[size] = temp_buff[0];
// 						++size;
// 					}
// 				}
// 			}
// 		}

// 		if( size != 0 )
// 		{
// 			rx_buff.size = size;
// 			switch( port )
// 			{
// 				case PORT0:
// 					queue_send( gateway_rx_port0, (void const *) &rx_buff, BLOCK_FOREVER );
// 					break;
// 				case PORT1:
// 					queue_send( gateway_rx_port1, (void const *) &rx_buff, BLOCK_FOREVER );
// 					break;
// 				case PORT2:
// 					queue_send( gateway_rx_port2, (void const *) &rx_buff, BLOCK_FOREVER );
// 					break;
// 			}
// 		}
// 		task_delay( 100 * ONE_MS );
// 	}
// }


// /********************************************************************************/
// /* Virtual Method Defines														*/
// /********************************************************************************/
// static uint32_t read( ground_station_t *gs, uint8_t *packet, uint32_t size, uint8_t port, block_time_t block )
// {
// 	DEV_ASSERT( gs );
// 	DEV_ASSERT( packet );

// 	uint32_t iter;
// 	base_t	err1;
// 	gs_buff_t buff;

// 	switch( port )
// 	{
// 		case PORT0:
// 			err1 = queue_receive( gateway_rx_port0, (void *) &buff, block );
// 			break;
// 		case PORT1:
// 			err1 = queue_receive( gateway_rx_port1, (void *) &buff, block );
// 			break;
// 		case PORT2:
// 			err1 = queue_receive( gateway_rx_port2, (void *) &buff, block )
// 			break;
// 		default:
// 			return 0;
// 	}

// 	if( err1 != QUEUE_OK )
// 	{
// 		return 0;
// 	}

// 	for( iter = 0; (iter < size && iter < buff.size); ++iter )
// 	{
// 		packet[iter] = buff.rx_buff[iter];
// 	}

// 	return iter;
// }

// static uint32_t write( ground_station_t *gs, uint8_t* data, uint32_t size, uint8_t port, block_time_t block )
// {
// 	DEV_ASSERT( gs );
// 	DEV_ASSERT( data );
// 	UNUSED( block );

// 	uint32_t iter2;
// 	uint32_t sent;
// 	uint32_t packet_index;

// 	lock_mutex( gs_mutex, BLOCK_FOREVER );

// 	/* Add escape characters to payload data where needed. */
// 	packet_index = 0;

// 	write_buff[packet_index++] = LPC_ARDUINO_ESCAPE;
// 	write_buff[packet_index++] = LPC_ARDUINO_START;
// 	write_buff[packet_index++] = port;

// 	for( iter2 = 0; iter2 < size; ++iter2 )
// 	{
// 		if( iter2 >= WRITE_BUFF_SIZE-2-3 ){ break; }

// 		if( data[iter2] == LPC_ARDUINO_ESCAPE )
// 		{
// 			write_buff[packet_index++] = LPC_ARDUINO_ESCAPE;
// 			write_buff[packet_index++] = data[iter2];
// 		}
// 		else
// 		{
// 			write_buff[packet_index++] = data[iter2];
// 		}
// 	}

// 	write_buff[packet_index++] = LPC_ARDUINO_ESCAPE;
// 	write_buff[packet_index++] = LPC_ARDUINO_END;

// #ifdef __LPC17XX__
// 	/* Poor timmy is just so dumb.. he needs a delay between uart calls. */
// 	task_delay( 500 * ONE_MS );
// #endif
// 	sent =  gs_uart->send( gs_uart, write_buff, packet_index );

// 	unlock_mutex( gs_mutex );
// 	return sent;
// }


// /********************************************************************************/
// /* Initialization Method														*/
// /********************************************************************************/
// _Bool initialize_ground_station_uart( ground_station_t *gs, uart_t* uart )
// {
// 	DEV_ASSERT( gs );

// 	base_t err;

// 	 /* Snag lpc local's is_in_range( ). */
// 	if( !initialize_ground_station_lpc_local( gs ) ){ return false; }
// 	gs->read = read;
// 	gs->write = write;

// 	/* Only run this code once. */
// 	if( is_init == 0 )
// 	{
// 		gs_uart = uart;
// 		uart->reset( uart );

// 		new_queue( gateway_rx_port0, RX_QUEUE_LENGTH, GS_GATEWAY_QUEUE_SIZE );
// 		new_queue( gateway_rx_port1, RX_QUEUE_LENGTH, GS_GATEWAY_QUEUE_SIZE );
// 		new_queue( gateway_rx_port2, RX_QUEUE_LENGTH, GS_GATEWAY_QUEUE_SIZE );
// 		new_mutex( gs_mutex );
// 		err = create_task( gs_gateway, "gsuart", 400, NO_PARAMETERS, BASE_PRIORITY+1, NO_HANDLE );
// 		write_buff = pvPortMalloc( WRITE_BUFF_SIZE );

// 		if( write_buff == NULL || err != TASK_CREATED || gateway_rx_port0 == NULL || gateway_rx_port1 == NULL || gateway_rx_port2 == NULL || gs_mutex == NULL )
// 		{
// 			return false;
// 		}
// 		is_init = 1;
// 	}
// 	return true;
// }
