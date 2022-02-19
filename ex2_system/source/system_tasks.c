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

/**
 * Start all system daemon tasks
 *
 * @return status
 *  error report of daemon creation
 */
SAT_returnState start_system_tasks(void) {

  TaskHandle_t _;
  if (//start_beacon_daemon() != SATR_OK ||
      start_coordinate_management_daemon() != SATR_OK ||
      start_diagnostic_daemon() != SATR_OK ||
      start_housekeeping_daemon() != SATR_OK ||
      //start_system_stats_daemon() != SATR_OK ||
      start_logger_daemon(_) != SATR_OK) {
    ex2_log("Error starting system tasks\r\n");
    return SATR_ERROR;
  }
  ex2_log("All system tasks started\r\n");
  return SATR_OK;
}
