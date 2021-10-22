/*
 * leop.h
 *
 *  Created on: Oct, 2021
 *      Author: Grace Yi
 */

#ifndef LEOP_H_
#define LEOP_H_

#include <FreeRTOS.h>
#include <csp/csp.h>
#include <csp/drivers/usart.h>
#include <csp/interfaces/csp_if_can.h>
#include <os_task.h>
#include <performance_monitor/system_stats.h>
#include <redconf.h>
#include <redfs.h>
#include <redfse.h>
#include <redposix.h>
#include <redvolume.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/service_utilities.h>

#include "../include/leop.h"
#include "../ex2_hal/ex2_athena_software/equipment_handler/include/deployablescontrol.h"
#include "../include/leop_eeprom.h"

void leop_init();
bool hard_switch_status();

#endif /* LEOP_H_ */
