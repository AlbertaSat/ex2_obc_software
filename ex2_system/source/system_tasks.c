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

    TaskHandle_t _ = 0;
    typedef SAT_returnState (*system_tasks)();
    system_tasks start_task[NUMBER_OF_SYSTEM_TASKS] = {
        &start_task_manager,      &start_beacon_daemon,       &start_coordinate_management_daemon,
        &start_diagnostic_daemon, &start_housekeeping_daemon, &start_system_stats_daemon,
        &start_NMEA_daemon,       &start_RTC_daemon,          &start_logger_daemon};

    uint8_t start_task_flag[NUMBER_OF_SYSTEM_TASKS];
    memset(start_task_flag, 0, NUMBER_OF_SYSTEM_TASKS * sizeof(uint8_t));
    int start_task_retry;

    for (int i = 0; i < NUMBER_OF_SYSTEM_TASKS; i++) {
        start_task_retry = 0;
        SAT_returnState state;
        while (start_task_retry <= 3) {
            state = start_task[i]();
            if (state != SATR_OK && start_task_retry < 3) {
                sys_log(WARN, "start_task_flag[%d] failed, try again", i);
                vTaskDelay(500);
            } else if (state != SATR_OK && start_task_retry == 3) {
                sys_log(ERROR, "start_task_flag[%d] failed", i);
                break;
            } else {
                start_task_flag[i] = 1;
                sys_log(INFO, "start_task_flag[%d] succeeded", i);
                break;
            }
            start_task_retry++;
        }
    }
    //  if (start_task_manager() != SATR_OK ||
    //      //start_beacon_daemon() != SATR_OK ||
    //      //start_coordinate_management_daemon() != SATR_OK ||
    //      start_diagnostic_daemon() != SATR_OK ||
    //      start_housekeeping_daemon() != SATR_OK ||
    //      //start_system_stats_daemon() != SATR_OK ||
    //      start_NMEA_daemon() != SATR_OK ||
    //      start_RTC_daemon() != SATR_OK ||
    //      start_logger_daemon(_) != SATR_OK) {
    //    ex2_log("Error starting system tasks\r\n");
    //    return SATR_ERROR;
    //  }

    return SATR_OK;
}
