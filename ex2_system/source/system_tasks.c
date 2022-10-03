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
 * @file system_tasks.c
 * @author Andrew R. Rooney
 * @date Mar. 6, 2021
 */

#include <beacon_task.h>
#include "system_tasks.h"

#include "coordinate_management/coordinate_management.h"
#include "diagnostic/diagnostic.h"
#include "housekeeping/housekeeping_task.h"
#include "logger/logger.h"
#include "nmea_daemon.h"
#include "time_management/rtc_daemon.h"
#include "sw_wdt.h"

#include "sband_sender/sband_sender.h"

/**
 * Start all system daemon tasks
 *
 * @return status
 *  error report of daemon creation
 */
SAT_returnState start_system_tasks(void) {

    const static char *system_task_names[] = {"beacon_daemon",
                                              "coordinate_management_daemon",
                                              "diagnostic_daemon",
                                              "housekeeping_daemon",
                                              "NMEA_daemon",
                                              "RTC_daemon",
                                              "logger_daemon",
                                              "sband_daemon",
                                              "sw_wdt"};

    const system_tasks start_task[] = {start_beacon_daemon,     start_coordinate_management_daemon,
                                       start_diagnostic_daemon, start_housekeeping_daemon,
                                       start_NMEA_daemon,       start_RTC_daemon,
                                       start_logger_daemon,     start_sband_daemon,
                                       start_sw_watchdog,       NULL};

    for (int i = 0; start_task[i]; i++) {
        SAT_returnState state;
        const char *task_name = system_task_names[i];
        sys_log(INFO, "Starting %s", task_name);
        state = start_task[i]();
        sys_log(INFO, "Start daemon %s reports %d", task_name, state);
    }
    return SATR_OK;
}
