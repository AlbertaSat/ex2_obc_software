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
/**
 * @file leop.h
 * @author Grace Yi
 * @date Oct. 2021
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

#include "deployablescontrol.h"
#include "leop_eeprom.h"

#define MAX_RETRIES 3
bool execute_leop();
bool deploy_all_deployables();

#endif /* LEOP_H_ */
