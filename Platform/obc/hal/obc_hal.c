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
 * @file demo_hal.c
 * @author Andrew Rooney, Haoran Qi
 * @date 2020-06-06
 */
#include "obc_hal.h"

#include <stddef.h>
#include <FreeRTOS.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <TempSensor/TempSensor.h>
#include "hal.h"
#include "services.h"
#include "queue.h"

#define BATTERY_1 0
#define BATTERY_2 1
#define TEMP 2

uint32_t current_time;

/* some of the different structures provided by this platform */
typedef struct __attribute__((packed)) {
  float current, voltage, temperature;
} HK_battery;

typedef struct __attribute__((packed)) {
  float temperature;
} HK_temperature;

/**
 * These functions are WIP stubs to a non-existent RTC
 */

void HAL_sys_setTime(uint32_t unix_timestamp) {
  HAL_RTC_SetTime(unix_timestamp);
}

void HAL_sys_getTime(uint32_t *unix_timestamp) {
  HAL_RTC_GetTime(unix_timestamp);
}

size_t HAL_hk_report(uint8_t sid, void *output) {
  switch (sid) {
    case BATTERY_1:
      if (sizeof(HK_battery) > csp_buffer_data_size() - 2) {
          // struct won't fit.. Don't try
          return 0;
      };
      HK_battery *battery1 = (HK_battery *)output;
      HAL_get_current_1(&(*battery1).current);
      HAL_get_voltage_1(&(*battery1).voltage);
      HAL_get_temperature(&(*battery1).temperature);
      return sizeof(HK_battery);

    case BATTERY_2:
      return sizeof(HK_battery);

    case TEMP:
        if (sizeof(HK_temperature) > csp_buffer_data_size() - 2) {
            return 0;
        };
        /*Get packet from temp queue*/
        HK_temperature *temp = (HK_temperature *)output;
        temp->temperature = csp_htonflt(HAL_get_temp_data());
      return sizeof(HK_temperature);

    default:
      return 0;
  }
}

void HAL_comm_getTemp(uint32_t *sensor_temperature){
    //
    HAL_get_temperature(*sensor_temperature);
}
