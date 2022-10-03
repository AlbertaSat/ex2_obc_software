/*
 * Copyright (C) 2020  University of Alberta
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
 * @file system_tasks.h
 * @author Andrew
 * @date Mar. 6, 2021
 */

#ifndef EX2_SYSTEM_INCLUDE_SYSTEM_TASKS_H_
#define EX2_SYSTEM_INCLUDE_SYSTEM_TASKS_H_

#include "main/system.h"
#include "uhf.h"
#include "uTransceiver.h"
#include "housekeeping_service.h"

SAT_returnState start_system_tasks(void);
typedef SAT_returnState (*system_tasks)();

#endif /* EX2_SYSTEM_INCLUDE_SYSTEM_TASKS_H_ */
