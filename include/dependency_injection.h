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
 * @file dependency_injection
 * @author Brendan Bruner
 * @date Jan 29, 2015
 *
 * Prototypes methods used for dependency injection.
 */
#ifndef INCLUDE_DEPENDENCY_INJECTION_H_
#define INCLUDE_DEPENDENCY_INJECTION_H_

// #include <states/state_relay.h>
#include <telecommand.h>
#include <ground_station.h>
// #include <non_volatile/non_volatile_variable.h>

extern driver_toolkit_t kit_mem;
extern driver_toolkit_t *kitp;

/* Can be initialized to execute all commands or none of the commands
 * relayed to it.
 * 		initialize_tester_state( system_state_t *, STATE_EXE_CONTROLLED_ALL );
 * or
 * 		initialize_tester_state( system_state_t *, STATE_EXE_CONTROLLED_NONE );*/
#define STATE_EXE_CONTROLLED_ID ((uint8_t) 200)
// typedef struct state_exe_all_t{ state_t _super_; }state_exe_all_t;
// typedef struct state_exe_none_t{ state_t _super_; }state_exe_none_t;
// void initialize_exe_all_state( state_exe_all_t *, filesystem_t *fs );
// void initialize_exe_none_state( state_exe_none_t *, filesystem_t *fs );


/* Increments a counter every time its entry/exit/next state routine is called.
 * This state will always say the next state is itself (ie, always loop back
 * to itself when changing states).
 * Must pass in a pointer to a probe_counter_t which has already been
 * allocated in memory. This state is configured to execute all commands
 * passed to it. */
#define STATE_PROBE_ID  	((uint8_t) 201)
// typedef struct probe_counter_t{ int enters, exits, nexts; }probe_counter_t;
// typedef struct state_probe_t{ state_t _super_; probe_counter_t *_counter_; }state_probe_t;
// void initialize_probe_state( state_probe_t *, probe_counter_t *, filesystem_t *fs );


/* Increment an int when executed. A pointer to the int is passed in
 * at initialization and set to zero. */
typedef struct telecommand_counter_t{ telecommand_t _super_; int *count; } telecommand_counter_t;
void initialize_command_counter( telecommand_counter_t *, int *, driver_toolkit_t * );
// /* Initialize a driver_toolkit_t for use with testing. */
// void initialize_toolkit_testing( driver_toolkit_t * );

// /* eps which always outputs very low battery voltage / power safe mode. */
// void initialize_eps_always_safe( eps_t * );
// /* eps which always outputs low battery voltage / normal mode. */
// void initialize_eps_always_normal( eps_t * );
//  eps which always outputs very high voltage / optimal mode.
// void initialize_eps_always_optimal( eps_t * );

// /* adcs mock which is always in ADCS_MODE_STABILIZED. */
// void initialize_adcs_always_stabilized( adcs_t * );
// /* adcs mock which is always in ADCS_MODE_IDLE. */
// void initialize_adcs_always_idle( adcs_t * );

// /* Mock up telemetry logger for testing. */
// void initialize_mock_up_logger( logger_t * );
// void mock_up_logger_reset( void );
// void initialize_mock_up_logger_custom( logger_t *, uint8_t );
#define MOCK_UP_LOGGER_PACKET_SIZE 32
#define USE_NM_SPC_1 0
#define USE_NM_SPC_2 1
#define USE_NM_SPC_3 2
#define USE_NM_SPC_4 3
#define USE_NM_SPC_5 4
#define USE_NM_SPC_6 5

/*  Mock up ground station which will 'read' out the telemetry that was 'written' */
/* telemetry_packet_t::packet_type <-> telecommand_packet_t::telecommand_code */
/* telemetry_packet_t::confirmation_id <-> telecommand_packet_t::unique_id */
/* telemetry_packet_t::data <-> telecommand_packet_t::payload_location */
void initialize_mock_up_ground_station( ground_station_t *gs );
/* Same as above, but extra data size is telemetry_packet_t::data_size, */
/* not MOCK_UP_LOGGER_PACKET_SIZE. */
void initialize_mock_up_ground_station_dynamic_sizes( ground_station_t *gs );
void set_mock_up_gs_in_range( _Bool );

// typedef struct{ non_volatile_variable_t s_; void* var; uint32_t size; } ram_variable_t;
// void initialize_ram_variable( ram_variable_t*, void*, uint32_t );

#endif /* INCLUDE_DEPENDENCY_INJECTION_H_ */
