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
#include <task.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <stdio.h>

#include "service_response.h"
#include "service_utilities.h"
#include "services.h"
#include "system.h"

SAT_returnState time_management_app(csp_packet_t *packet);

/**
 * @brief
 *      FreeRTOS time management server task
 * @details
 *      Accepts incoming time management service packets and executes the application
 * @param void* param
 * @return None
 */
void time_management_service(void * param) {
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_RDPREQ); // require RDP connection
    csp_bind(sock, TC_TIME_MANAGEMENT_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN);

    for(;;) {
        csp_conn_t *conn;
        csp_packet_t *packet;
        if ((conn = csp_accept(sock, CSP_MAX_TIMEOUT)) == NULL) {
          /* timeout */
          continue;
        }
        while ((packet = csp_read(conn, 50)) != NULL) {
          if (time_management_app(packet) != SATR_OK) {
            // something went wrong, this shouldn't happen
            csp_buffer_free(packet);
          } else {
              if (!csp_send(conn, packet, 50)) {
                  csp_buffer_free(packet);
              }
          }
        }
        csp_close(conn);
    }
}

/**
 * @brief
 *      Start the time management server task
 * @details
 *      Starts the FreeRTOS task responsible for accepting incoming
 *      time management packets
 * @param None
 * @return SAT_returnState
 *      success report
 */
SAT_returnState start_time_management_service(void) {
  if (xTaskCreate((TaskFunction_t)time_management_service,
                  "time_management_service", 300, NULL, NORMAL_SERVICE_PRIO,
                  NULL) != pdPASS) {
    ex2_log("FAILED TO CREATE TASK time_management_service\n");
    return SATR_ERROR;
  }
  ex2_log("Service handlers started\n");
  return SATR_OK;
}

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
  int8_t status;
  struct time_utc temp_time;

  switch (ser_subtype) {
    case SET_TIME:
      cnv8_32(&packet->data[IN_DATA_BYTE], &temp_time.unix_timestamp);
      temp_time.unix_timestamp = csp_ntoh32(temp_time.unix_timestamp);

      if (!TIMESTAMP_ISOK(temp_time.unix_timestamp)) {
        status = -1;
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      } else {
        HAL_sys_setTime(temp_time.unix_timestamp);
        status = 0;
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      }

      set_packet_length(packet, sizeof(int8_t) + 1);  // +1 for subservice

      break;

    case GET_TIME:
      // Step 1: get the data
      HAL_sys_getTime(&temp_time.unix_timestamp);
      // Step 2: convert to network order
      temp_time.unix_timestamp = csp_hton32(temp_time.unix_timestamp);
      // step3: copy data & status byte into packet
      status = 0;
      memcpy(&packet->data[STATUS_BYTE], &status,
             sizeof(int8_t));  // 0 for success
      memcpy(&packet->data[OUT_DATA_BYTE], &temp_time.unix_timestamp,
             sizeof(uint32_t));
      // Step 4: set packet length
      set_packet_length(packet, sizeof(int8_t) + sizeof(uint32_t) +
                                    1);  // plus one for sub-service
      break;

    default:
      ex2_log("No such subservice\n");
      return SATR_PKT_ILLEGAL_SUBSERVICE;
  }
  return SATR_OK;
}
