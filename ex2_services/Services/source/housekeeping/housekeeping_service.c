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
 * @file housekeeping_service.c
 * @author Haoran Qi, Andrew Rooney, Yuan Wang
 * @date 2020-07-07
 */
#include "housekeeping/housekeeping_service.h"

#include <FreeRTOS.h>
#include <os_task.h>
#include <csp/csp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/service_utilities.h"
#include "services.h"

static uint8_t SID_byte = 1;

SAT_returnState start_housekeeping_service(void);

/**
 * @brief
 *      FreeRTOS housekeeping server task
 * @details
 *      Accepts incoming housekeeping service packets and executes the application
 * @param void* param
 * @return None
 */
void housekeeping_service(void * param) {
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_RDPREQ);
    csp_bind(sock, TC_HOUSEKEEPING_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN);

    for(;;) {
        csp_conn_t *conn;
        csp_packet_t *packet;
        if ((conn = csp_accept(sock, CSP_MAX_TIMEOUT)) == NULL) {
          /* timeout */
          continue;
        }
        while ((packet = csp_read(conn, 50)) != NULL) {
          if (hk_service_app(packet) != SATR_OK) {
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
 *      Start the housekeeping server task
 * @details
 *      Starts the FreeRTOS task responsible for accepting incoming
 *      housekeeping service requests
 * @param None
 * @return SAT_returnState
 *      success report
 */
SAT_returnState start_housekeeping_service(void) {
  if (xTaskCreate((TaskFunction_t)housekeeping_service,
                  "start_housekeeping_service", 300, NULL, NORMAL_SERVICE_PRIO,
                  NULL) != pdPASS) {
    ex2_log("FAILED TO CREATE TASK start_housekeeping_service\n");
    return SATR_ERROR;
  }
  ex2_log("Service handlers started\n");
  return SATR_OK;
}


static SAT_returnState hk_parameter_report(csp_packet_t *packet);

SAT_returnState hk_service_app(csp_packet_t *packet) {
  uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];

  switch (ser_subtype) {
    case TM_HK_PARAMETERS_REPORT:
      if (hk_parameter_report(packet) != SATR_OK) {
        ex2_log("HK_REPORT_PARAMETERS failed");
        return SATR_ERROR;
      }
      break;
    default:
      ex2_log("HK SERVICE NOT FOUND SUBTASK");
      return SATR_PKT_ILLEGAL_SUBSERVICE;
  }

  return SATR_OK;
}

static SAT_returnState hk_parameter_report(csp_packet_t *packet) {
  size_t size =
      HAL_hk_report(packet->data[SID_byte], packet->data + SID_byte + 1);

  set_packet_length(packet, size + 2);  // plus one for sub-service + SID

  return SATR_OK;
}
