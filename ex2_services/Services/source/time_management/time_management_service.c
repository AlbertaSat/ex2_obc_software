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
 * @author Andrew Rooney, Robert Taylor
 * @date 2020-06-06
 */

#include <FreeRTOS.h>
#include <os_task.h>

#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <main/system.h>
#include <stdio.h>
#include "time_management/time_management_service.h"
#include "util/service_utilities.h"
#include "services.h"
#include "skytraq_gps_driver.h"
#include "nmea_service.h"
#include "time_struct.h"
#include "mocks/rtc.h"

#define GPS_TASK_SIZE 200
#define NMEA_TASK_SIZE 100

#define MIN_YEAR 1577836800  // 2020-01-01
#define MAX_YEAR 1893456000  // 2030-01-01

#define DISCIPLINE_DELAY 10000 // every 10 seconds for testing purposes

SAT_returnState time_management_app(csp_packet_t *packet);

/**
 * @brief
 *      FreeRTOS service for disciplining RTC
 * @details
 *      Disicplines rtc by asking the gps subsystem for the time and writing that time to the rtc
 * @param none
 * @return none. use FreeRTOS task features to poll
 */
void RTC_discipline_service(void) {

    ex2_log("GPS Task Started");

    if (!gps_skytraq_driver_init()) {
        ex2_log("failed to init skytraq\r\n");
        vTaskDelete(NULL);
    }

    ex2_time_t utc_time;
    date_t utc_date;

    for (;;) {
        vTaskDelay(DISCIPLINE_DELAY);
        if(!(gps_get_utc_time(&utc_time))){
            ex2_log("Couldn't get gps time");
            continue; // delay wait until gps signal acquired
        }
        mock_RTC_set_time(utc_time);
        mock_RTC_set_date(utc_date);
    }
}

/**
 * @brief
 *      Start the gps service tasks
 * @details
 *      Starts the FreeRTOS tasks responsible for disciplining the RTC and for decoding incoming NMEA strings
 * @param rtc_handle: TaskHandle_t* to store RTC discipline service handle as a return
 * @param nmea_handle: TaskHandle_t* to store nmea decoding service handle as a return
 * @return SAT_returnState
 *      success report
 */
SAT_returnState start_gps_services(TaskHandle_t *rtc_handle, TaskHandle_t *nmea_handle) {
    if (xTaskCreate((TaskFunction_t)RTC_discipline_service, "RTC_service", GPS_TASK_SIZE, NULL, 3, rtc_handle) != pdPASS) {
        return SATR_ERROR;
    }

    if (xTaskCreate((TaskFunction_t)NMEA_service, "NMEA_service", NMEA_TASK_SIZE, NULL, 4, nmea_handle) != pdPASS) {
        return SATR_ERROR;
    }

    return SATR_OK;
}

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
  TaskHandle_t _;
  if (start_gps_services(&_, &_) != SATR_OK) {
      return SATR_ERROR;
  }

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
  uint32_t temp_time;

  switch (ser_subtype) {
    case SET_TIME:
      cnv8_32(&packet->data[IN_DATA_BYTE], &temp_time);
      temp_time = csp_ntoh32(temp_time);

      if (!TIMESTAMP_ISOK(temp_time)) {
        status = -1;
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      } else {
        mock_RTC_set_unix_time(temp_time);
        status = 0;
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      }

      set_packet_length(packet, sizeof(int8_t) + 1);  // +1 for subservice

      break;

    case GET_TIME:
      // Step 1: get the data
      mock_RTC_get_unix_time(&temp_time);
      // Step 2: convert to network order
      temp_time = csp_hton32(temp_time);
      // step3: copy data & status byte into packet
      status = 0;
      memcpy(&packet->data[STATUS_BYTE], &status,
             sizeof(int8_t));  // 0 for success
      memcpy(&packet->data[OUT_DATA_BYTE], &temp_time,
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
