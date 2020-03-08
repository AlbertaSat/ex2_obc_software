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
 * @file portable_types.h
 * @author Brendan Bruner
 * @date May 27, 2015
 */
#ifndef PORTABLE_PORTABLE_TYPES_H_
#define PORTABLE_PORTABLE_TYPES_H_
/* Must be compiled with C99 and onwards. */

#ifdef __LPC17XX__
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
//#include "lpc17xx_libcfg_default.h"
#include <timers.h>
#else
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#endif

#include <stdlib.h>
#include <stdbool.h>

/* Bool enum. */
//typedef bool _Bool;
#define _Bool _Bool

/* Unused */
#define UNUSED( x )	do{ (void) x; }while( 0 )

/* Assertion. */
#ifndef FINAL_RELEASE_LPC1769
#ifdef __LPC17XX__
#include <stdio.h>
#define DEV_ASSERT( pointer )													\
	do {																		\
		if( (pointer) == NULL ) {												\
			printf( "DEV_ASSERT:\n file %s\nline %d\n", __FILE__, __LINE__ );					\
		}																		\
	} while( 0 )
#define PORT_PRINT( ... )
#else
#define PORT_PRINT( msg, ... ) do { printf( msg, ##__VA_ARGS__ );} while( 0 )
#define DEV_ASSERT( pointer ) configASSERT( (pointer) )
#endif
#else
#define DEV_ASSERT( pointer ) configASSERT( (pointer) )
#define PORT_PRINT( msg, ...)
#endif

/* Base */
#ifdef __LPC17XX__
typedef portBASE_TYPE base_t;
#define OS_ENTER_CRITICAL taskENTER_CRITICAL( )
#define OS_EXIT_CRITICAL taskEXIT_CRITICAL()
#else
typedef portBASE_TYPE base_t;
#endif

/* Timing */
#ifdef __LPC17XX__
typedef portTickType block_time_t;
typedef portTickType timer_value_t;
#else
typedef portTickType block_time_t;
#endif
#define BLOCK_FOREVER ((block_time_t) portMAX_DELAY)
#define TICK_RATE_MS portTICK_RATE_MS

/* Timers */
#ifdef __LPC17XX__
#define AUTO_RESTART pdTRUE
#define MANUAL_RESTART pdFALSE
typedef xTimerHandle software_timer_t;
#define new_timer( handle, ... ) 	do{ handle = xTimerCreate( __VA_ARGS__ ); }while( 0 )
#define restart_timer( handle, ... )xTimerReset( handle, __VA_ARGS__ )
#define start_timer( handle, ... ) 	xTimerStart( handle, __VA_ARGS__ )
#define stop_timer( handle, ... )	xTimerStop( handle, __VA_ARGS__ )
#define delete_timer( handle, ...)	xTimerDelete( handle, __VA_ARGS__ )
#define get_timer_id( handle ) pvTimerGetTimerID( handle )
#else
#define AUTO_RESTART pdTRUE
#define MANUAL_RESTART pdFALSE
typedef xTimerHandle software_timer_t;
#define new_timer( handle, ... ) 	do{ handle = xTimerCreate( __VA_ARGS__ ); }while( 0 )
#define restart_timer( handle, ... )xTimerReset( handle, __VA_ARGS__ )
#define start_timer( handle, ... ) 	xTimerStart( handle, __VA_ARGS__ )
#define stop_timer( handle, ... )	xTimerStop( handle, __VA_ARGS__ )
#define delete_timer( handle, ...)	xTimerDelete( handle, __VA_ARGS__ )
#define get_timer_id( handle ) pvTimerGetTimerID( handle )
#endif /* __LPC17XX__ */
typedef software_timer_t time_value_t;

/* Delays */
/* Blocking delay - give ms to block for. */
static inline void task_delay( uint32_t blck )
{
	vTaskDelay( ((blck) == portMAX_DELAY ? (blck) : (blck)/TICK_RATE_MS) );
}
static inline uint32_t task_time_elapsed( )
{
	return xTaskGetTickCount( )/TICK_RATE_MS;
}
static inline void task_delay_until( uint32_t start, uint32_t stop )
{
	portTickType ticks_passed = (portTickType) start*TICK_RATE_MS;
	vTaskDelayUntil( &ticks_passed, (portTickType) stop*TICK_RATE_MS );
}

/* Tasks */
#define NO_PARAMETERS				NULL
#define NO_HANDLE					NULL
#define BASE_PRIORITY				tskIDLE_PRIORITY

#ifdef __LPC17XX__
typedef xTaskHandle 	task_t;
#define TASK_CREATED	pdPASS
#define suspend_task( handle )	vTaskSuspend( (handle) )
#define resume_task( handle ) 	vTaskResume( (handle) )
#define create_task( method, name, stack, param, prio, handle ) \
			xTaskCreate( method, (signed char*) name, stack, param, prio, handle )
#define delete_task( tsk ) vTaskDelete( (tsk) );
#define yield_task( tsk )	portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
#else
typedef xTaskHandle		task_t;
#define TASK_CREATED	pdPASS
#define delete_task( handle ) 	vTaskDelete( (handle ) )
#define suspend_task( handle )	vTaskSuspend( (handle) )
#define resume_task( handle ) 	vTaskResume( (handle) )
#define create_task( method, name, stack, param, prio, handle ) \
		xTaskCreate( method, (const char*) name, stack, param, prio, handle )
#define yield_task( tsk ) 	portYIELD_FROM_ISR();
#endif


/* Queues */
#ifdef __LPC17XX__
typedef xQueueHandle 		queue_t;
#else
typedef xQueueHandle queue_t;
#endif

#define QUEUE_FULL errQUEUE_FULL
#define QUEUE_OK	pdTRUE
#define new_queue( queue, ... ) 					\
		do {										\
			(queue) = xQueueCreate( __VA_ARGS__ );	\
		} while( 0 )
#define queue_send( queue, ... )		xQueueSend( (queue), __VA_ARGS__ )
#define queue_receive( queue, ... )		xQueueReceive( (queue), __VA_ARGS__ )

/* Semaphores */
#ifdef __LPC17XX__
typedef xSemaphoreHandle	semaphore_t;
typedef xSemaphoreHandle 	mutex_t;
#else
typedef xSemaphoreHandle 	semaphore_t;
typedef xSemaphoreHandle	mutex_t;
#endif

#define MUTEX_ACQUIRED					pdTRUE
#define SEMAPHORE_ACQUIRED				pdTRUE
#define MUTEX_AVAILABLE					pdTRUE
#define SEMAPHORE_AVAILABLE				pdTRUE
#define MUTEX_BUSY						pdFALSE
#define SEMAPHORE_BUSY					pdFALSE

#define USE_POLLING 					0
#define POLL_MUTEX						0
#define POLL_SEMAPHORE					0

#define BINARY_SEMAPHORE				1
#define SEMAPHORE_EMPTY					0

#define new_mutex( mut ) 				do{ (mut) = xSemaphoreCreateMutex( ); } while( 0 )
#define new_semaphore( mut, max, init )	do{ mut = xSemaphoreCreateCounting( (max), (init) ); }while( 0 )

#define delete_mutex( mut )				do{ vSemaphoreDelete( (mut) ); }while( 0 )
#define delete_semaphore( mut )			do{ vSemaphoreDelete( (mut) ); }while( 0 )

/* For lock and take - give ms to block for. */
#define lock_mutex( mut, blck )			xSemaphoreTake( (mut), ((blck) == portMAX_DELAY ? (blck) : (blck)/TICK_RATE_MS) )
#define take_semaphore( mut, blck )		xSemaphoreTake( (mut), ((blck) == portMAX_DELAY ? (blck) : (blck)/TICK_RATE_MS) )

#define peek_mutex( mut, block_time )	xQueuePeek( (queue_t) (mut), (void*) NULL, block_time )
#define peek_semaphore( sem, block_time ) peek_mutex( sem, block_time )

#define unlock_mutex( mut )				xSemaphoreGive( (mut) )
#define give_semaphore( mut )			xSemaphoreGive( (mut) )
#define post_semaphore( mut )			give_semaphore( (mut ) )
#define give_semaphore_from_isr( mut , task_woken )	xSemaphoreGiveFromISR( mut, task_woken )

#define DELAY_ONE_SECOND	( 1000 )
#define DELAY_ONE_MS		( 1 )
#define ONE_MINUTE 			(60*ONE_SECOND)
#define ONE_SECOND			DELAY_ONE_SECOND
#define ONE_MS				DELAY_ONE_MS

#endif /* PORTABLE_PORTABLE_TYPES_H_ */
