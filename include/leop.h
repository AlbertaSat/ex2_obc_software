/*
 * leop.h
 *
 *  Created on: Oct, 2021
 *      Author: Grace Yi
 */

#ifndef LEOP_H_
#define LEOP_H_

#include <FreeRTOS.h>
#include <os_task.h>
#include <os_queue.h>
#include <os_timer.h>
#include <system_stats.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "leop.h"
#include "deployablescontrol.h"
#include "leop_eeprom.h"

void leop_init();
bool hard_switch_status();

#endif /* LEOP_H_ */
