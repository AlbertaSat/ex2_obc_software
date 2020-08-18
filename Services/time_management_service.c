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
 * @file time_management_service.c
 * @author Andrew Rooney
 * @date 2020-06-06
 */

#include "time_management_service.h"

#include <FreeRTOS.h>
#include <csp/csp.h>
#include <stdio.h>

#include "service_response.h"
#include "service_utilities.h"
#include "system.h"

/**
 * @brief
 * 		Handle incoming csp_packet_t
 * @details
 * 		Takes a csp packet destined for the time_management service, 
 *              and will handle the packet based on it's subservice type.
 * @param csp_packet_t *packet
 *              Incoming CSP packet - we can be sure that this packet is 
 *              valid and destined for this service.
 * @return SAT_returnState
 * 		success report
 */
SAT_returnState time_management_app(csp_packet_t *packet) {
  uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];
  struct time_utc temp_time;

  switch (ser_subtype) {
    case SET_TIME:
      cnv8_32(&packet->data[DATA_BYTE], &temp_time.unix_timestamp);
      if (!TIMESTAMP_ISOK(temp_time.unix_timestamp)) {
        printf("Bad timestamp format\n");
        return SATR_ERROR;
      }
      printf("Set Time: %u\n", (uint32_t)temp_time.unix_timestamp);

      HAL_sys_setTime(temp_time.unix_timestamp);
      csp_buffer_free(packet); // TODO: send success report?
      break;

    case GET_TIME:
      HAL_sys_getTime(&temp_time.unix_timestamp);
      printf("Get Time: %u\n", temp_time.unix_timestamp);

      packet->data[DATA_BYTE] = (uint32_t) temp_time.unix_timestamp;
      
      return_packet_header(packet); // get packet ready to return
      cnv32_8(temp_time.unix_timestamp, &packet->data[DATA_BYTE]); 
      if (queue_response(packet) != SATR_OK) {
        return SATR_ERROR;
      }
      break;

    default:
      ex2_log("No such subservice\n");
      return SATR_PKT_ILLEGAL_SUBSERVICE;
  }
  return SATR_OK;
}
