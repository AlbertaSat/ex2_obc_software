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
#include "housekeeping_service.h"

#include <FreeRTOS.h>
#include <csp/csp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "service_response.h"
#include "service_utilities.h"
#include "services.h"
#include "system.h"

static uint8_t SID_byte = 2;
extern Service_Queues_t service_queues;

static SAT_returnState hk_parameter_report(csp_packet_t *packet);

SAT_returnState hk_service_app(csp_packet_t *packet) {
  uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];

  switch (ser_subtype) {
    case TM_HK_PARAMETERS_REPORT:
      if (hk_parameter_report(packet) != SATR_OK) {
        csp_log_info("HK_REPORT_PARAMETERS TASK FINISHED");
        return SATR_ERROR;
      }
      break;

    default:
      csp_log_error("HK SERVICE NOT FOUND SUBTASK");
      return SATR_PKT_ILLEGAL_SUBSERVICE;
  }

  return SATR_OK;
}

static SAT_returnState hk_parameter_report(csp_packet_t *packet) {
  HAL_hk_report(packet->data[SID_byte], packet->data + SID_byte + 1);

  return_packet_header(packet);

  if (queue_response(packet) != SATR_OK) {
    return SATR_ERROR;
  }

  return SATR_OK;
}
