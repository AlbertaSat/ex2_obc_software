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
 * @author Andrew Rooney
 * @date 2020-06-06
 */
#include "obc_hal.h"

#include <stddef.h>
#include <FreeRTOS.h>
#include <csp/csp.h>
#include <TempSensor.h>

#include "hal.h"
#include "services.h"
#include "queue.h"

#define BATTERY_1 0
#define BATTERY_2 1
#define TEMP 2

uint32_t current_time;

/* some of the different structures provided by this platform */
typedef struct {
  float current, voltage, temperature;
} HK_battery;

typedef struct {
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

SAT_returnState HAL_hk_report(uint8_t sid, void *output) {
  switch (sid) {
    case BATTERY_1:
      if ((sizeof((char *) output) + 1) > csp_buffer_data_size()) {
            return SATR_BUFFER_ERR;
        };
      HK_battery *battery1 = (HK_battery *)output;
      HAL_get_current_1(&(*battery1).current);
      HAL_get_voltage_1(&(*battery1).voltage);
      HAL_get_temperature(&(*battery1).temperature);
      return SATR_OK;

    case BATTERY_2:
      return SATR_OK;

    case TEMP:
        if ((sizeof((char *) output) + 1) > csp_buffer_data_size()) {
                    return SATR_BUFFER_ERR;
                };
        /*Get packet from temp queue*/
        csp_packet_t *packet;
        for(;;){
            if (xQueueReceive(equipment_queues.temp_sensor_queue, &packet,
                              NORMAL_TICKS_TO_WAIT) == pdPASS) {
                output = packet->data;
                csp_buffer_free(packet);
                break;
            }
          }
      return SATR_OK;

    default:
      return SATR_OK;
  }
}
