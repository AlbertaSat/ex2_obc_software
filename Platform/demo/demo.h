/*
 * Copyright (C) 2015  University of Alberta
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
 */

#ifndef DEMO_H
#define DEMO_H

#include <FreeRTOS.h>
#include <csp/csp.h>

#include "queue.h"
#include "services.h"

// gcc Src/*.c Src/demo/*.c -c -I Inc/ -I ../upsat-ecss-services/services/
// -I Src/ -I Src/demo -I ../ex2_on_board_computer/Source/include/ -I
// ../ex2_on_board_computer/Project/ -I ../ex2_on_board_computer/libcsp/include/
// -I ../ex2_on_board_computer/Source/portable/GCC/POSIX/ -I
// ../ex2_on_board_computer/libcsp/build/include/ -m32 -lpthread -std=c99 -lrt
// && ar -rsc client_server.a *.o^C

extern unsigned int sent_count;

#define NORMAL_TICKS_TO_WAIT 1
#define NORMAL_SERVICE_PRIO 5
#define SERVICE_QUEUE_LEN 3
#define RESPONSE_QUEUE_LEN 3
#define CSP_PKT_QUEUE_SIZE sizeof(csp_packet_t*)

// Define all the services that the module implements
// Defined here are the services implemented by this platform
typedef struct {
  xQueueHandle response_queue,  // Each platform must define a response queue
      hk_app_queue, time_management_app_queue, communication_app_queue;
} Service_Queues_t;

SAT_returnState start_service_handlers();

SAT_returnState ground_response_task();

#endif
