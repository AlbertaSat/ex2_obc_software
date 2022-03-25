/*
 * scheduler.c
 *
 *  Created on: May 7, 2021
 *      Author: Robert Taylor
 */

#include "task_manager/task_manager.h"
static uint32_t svc_wdt_counter = 0;
static uint32_t get_svc_wdt_counter() { return svc_wdt_counter; }
