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
 * @file communication_service.c
 * @author Arash Yazdani
 * @date 2020-09-25
 */

#include "communication_service.h"

/**
 * @brief
 *      Takes a CSP packet and switches based on the subservice command
 * @details
 *      Reads/Writes data from communication EHs as subservices
 * @attention
 *      More subservices are to be added.
 * @param *packet
 *      The CSP packet
 * @return SAT_returnState
 *      Success or failure
 */

SAT_returnState communication_service_app(csp_packet_t *packet) {
  uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];
  int8_t status;
  struct temp_utc temp_temp;
  struct Sband_config S_config;
  struct Sband_PowerAmplifier S_PA;

  switch (ser_subtype) {
    case GET_TEMP:
      // Step 1: get the data
      HAL_comm_getTemp(&temp_temp.sensor_temperature);
      // Step 2: convert to network order
      temp_temp.sensor_temperature = csp_hton32(temp_temp.sensor_temperature);
      // step3: copy data & status byte into packet
      status = 0;
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t)); // 0 for success
      memcpy(&packet->data[OUT_DATA_BYTE], &temp_temp.sensor_temperature, sizeof(uint32_t));
      // Step 4: set packet length
      set_packet_length(packet, sizeof(int8_t) + sizeof(uint32_t) + 1); // +1 for subservice
      // Step 5: return packet
      if (queue_response(packet) != SATR_OK) {
        return SATR_ERROR;
      }

      break;

    case GET_FREQ:
      HAL_S_getFreq(&S_config.S_freq);
      S_config.S_freq = csp_hton32(S_config.S_freq);
      status = 0;
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_config.S_freq, sizeof(uint32_t));
      set_packet_length(packet, sizeof(int8_t) + sizeof(uint32_t) + 1);
      if (queue_response(packet) != SATR_OK) {
        return SATR_ERROR;
      }
      break;

    case GET_PApower:
      HAL_S_getpaPower(&S_config.S_paPower);
      S_config.S_paPower = csp_hton32(S_config.S_paPower);
      status = 0;
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_config.S_paPower, sizeof(uint32_t));
      set_packet_length(packet, sizeof(int8_t) + sizeof(uint32_t) + 1);
      if (queue_response(packet) != SATR_OK) {
        return SATR_ERROR;
      }
      break;

    case GET_CONTROL:
      HAL_S_getControl(&S_PA.PA_status);
      S_PA.PA_status = csp_hton32(S_PA.PA_status);
      status = 0;
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_PA.PA_status, sizeof(uint32_t));///WIP
      set_packet_length(packet, sizeof(int8_t) + sizeof(uint32_t) + 1);
      if (queue_response(packet) != SATR_OK) {
        return SATR_ERROR;
      }
      break;


    default:
      ex2_log("No such subservice\n"); // Is it doing anything?
      return SATR_PKT_ILLEGAL_SUBSERVICE;
  }
  return SATR_OK;
}
