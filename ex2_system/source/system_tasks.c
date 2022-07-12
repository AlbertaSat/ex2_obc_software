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
#include "performance_monitor/system_stats.h"
#include "logger/logger.h"
#include "nmea_daemon.h"
#include "time_management/rtc_daemon.h"
#include "task_manager/task_manager.h"

/**
 * Start all system daemon tasks
 *
 * @return status
 *  error report of daemon creation
 */
SAT_returnState start_system_tasks(void) {

    system_tasks start_task[] = {&start_task_manager,
                                 &start_beacon_daemon,
                                 &start_coordinate_management_daemon,
                                 &start_diagnostic_daemon,
                                 &start_housekeeping_daemon,
                                 &start_system_stats_daemon,
                                 &start_NMEA_daemon,
                                 &start_RTC_daemon,
                                 &start_logger_daemon,
                                 NULL};

    int number_of_system_tasks = (sizeof(start_task) - 1) / sizeof(system_tasks);
    uint8_t *start_task_flag = pvPortMalloc(number_of_system_tasks * sizeof(uint8_t));
    memset(start_task_flag, 0, number_of_system_tasks * sizeof(uint8_t));
    int start_task_attempt;
    SAT_returnState state;

    for (int i = 0; start_task[i]; i++) {
        start_task_attempt = 0;
        char *task_name = system_task_names[i];
        while (start_task_attempt <= 3) {
            state = start_task[i]();
            if (state != SATR_OK && start_task_attempt < 3) {
                sys_log(WARN, "start %s failed, try again", task_name);
                vTaskDelay(10);
            } else if (state != SATR_OK && start_task_attempt == 3) {
                sys_log(ERROR, "start %s failed", task_name);
                break;
            } else {
                start_task_flag[i] = 1;
                sys_log(INFO, "start %s succeeded", task_name);
                break;
            }
            start_task_attempt++;
        }
    }
    vPortFree(start_task_flag);
    return SATR_OK;
}
