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
 * @file diagnoistic.h
 * @author Andrew R. Rooney
 * @date Mar. 6, 2021
 */

#ifndef EX2_SYSTEM_INCLUDE_DIAGNOSTIC_DIAGNOSTIC_H_
#define EX2_SYSTEM_INCLUDE_DIAGNOSTIC_DIAGNOSTIC_H_

#include "main/system.h"

SAT_returnState start_diagnostic_daemon(void);
TickType_t get_uhf_watchdog_delay(void);
TickType_t get_sband_watchdog_delay(void);
TickType_t get_charon_watchdog_delay(void);

SAT_returnState set_uhf_watchdog_delay(const TickType_t delay);
SAT_returnState set_sband_watchdog_delay(const TickType_t delay);
SAT_returnState set_charon_watchdog_delay(const TickType_t delay);

#endif /* EX2_SYSTEM_INCLUDE_DIAGNOSTIC_DIAGNOSTIC_H_ */
