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

#define TM_TC_BUFF_SIZE 256
#define NORMAL_TICKS_TO_WAIT 1
#define NORMAL_SERVICE_PRIO 5
#define NORMAL_QUEUE_LEN 3
#define NORMAL_QUEUE_SIZE 256

// Define all the services that the module implements
typedef struct {
  xQueueHandle verification_app_queue;
  xQueueHandle hk_app_queue;
  xQueueHandle test_app_queue;
  xQueueHandle time_management_app_queue;
} service_queues_t;

SAT_returnState start_service_handlers();

#endif /* DEMO_H */
