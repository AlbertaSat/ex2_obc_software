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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <system.h>

#include "leop.h"
#include "../ex2_hal/ex2_athena_software/equipment_handler/include/deployablescontrol.h"
//#include "deployablescontrol.h"
#include "leop_eeprom.h"

bool leop_init();
bool hard_switch_status();

#endif /* LEOP_H_ */
