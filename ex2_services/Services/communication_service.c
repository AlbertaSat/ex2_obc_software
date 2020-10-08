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

  /* Comment for PR:
   * 1- Perhaps initial value assignment should occur here if we want to
   * call getX after setX command.
   * 2- Also I think I'll merge most of these structs.
   * 3- Steps 2-5 in each case can be replaced by a single function.
   */
  struct temp_utc temp_temp;
  struct Sband_config S_config;
  struct Sband_PowerAmplifier S_PA;
  struct Sband_Encoder S_enc;
  struct Sband_Firmware S_Firmware;
  struct Sband_Status S_status;
  struct Sband_buffer S_buffer;

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

    case GET_CONTROL:
      HAL_S_getControl (&S_PA.PA_status, &S_PA.PA_mode);
      S_PA.PA_status = csp_hton32(S_PA.PA_status);
      S_PA.PA_mode = csp_hton32(S_PA.PA_mode);
      status = 0;
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_PA.PA_status, sizeof(uint32_t));
      memcpy(&packet->data[OUT_DATA_BYTE + sizeof(uint32_t)], &S_PA.PA_mode, sizeof(uint32_t));
      set_packet_length(packet, sizeof(int8_t) + sizeof(uint32_t) + sizeof(uint32_t) + 1);

      if (queue_response(packet) != SATR_OK) {
        return SATR_ERROR;
      }
      break;

    case GET_ENCODER:
      HAL_S_getEncoder(&S_enc.Enc_scrambler, &S_enc.Enc_filter, &S_enc.Enc_modulation, &S_enc.Enc_rate);
      S_enc.Enc_scrambler = csp_hton32(S_enc.Enc_scrambler);
      S_enc.Enc_filter = csp_hton32(S_enc.Enc_filter);
      S_enc.Enc_modulation = csp_hton32(S_enc.Enc_modulation);
      S_enc.Enc_rate = csp_hton32(S_enc.Enc_rate);
      status = 0;

      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_enc.Enc_scrambler, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE + sizeof(int8_t)], &S_enc.Enc_filter, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE + 2*sizeof(int8_t)], &S_enc.Enc_modulation, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE + 3*sizeof(int8_t)], &S_enc.Enc_rate, sizeof(int8_t));

      set_packet_length(packet, sizeof(int8_t) + 4*sizeof(int8_t) + 1);

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

    case GET_FirmwareV:
        HAL_S_getFirmwareV(&S_Firmware.Version);
        S_Firmware.Version = csp_hton32(S_Firmware.Version);// check if 32 is correct/necessary
        status = 0;
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &S_Firmware.Version, sizeof(float));
        set_packet_length(packet, sizeof(int8_t) + sizeof(float) + 1);
        if (queue_response(packet) != SATR_OK) {
          return SATR_ERROR;
        }
        break;

    case GET_STATUS:
      HAL_S_getStatus (&S_status.PWRGD, &S_status.TXL);
      S_status.PWRGD = csp_hton32(S_status.PWRGD);
      S_status.TXL = csp_hton32(S_status.TXL);
      status = 0;
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_status.PWRGD, sizeof(uint8_t));
      memcpy(&packet->data[OUT_DATA_BYTE + sizeof(uint8_t)], &S_status.TXL, sizeof(uint8_t));
      set_packet_length(packet, sizeof(int8_t) + 2*sizeof(uint8_t) + 1);

      if (queue_response(packet) != SATR_OK) {
        return SATR_ERROR;
      }
      break;

    case GET_TR:
        HAL_S_getTR(&S_buffer.transmit);
        S_buffer.transmit = csp_hton32(S_buffer.transmit);
        // what to do for int?
        status = 0;
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &S_buffer.transmit, sizeof(int));
        set_packet_length(packet, sizeof(int8_t) + sizeof(int) + 1);
        if (queue_response(packet) != SATR_OK) {
          return SATR_ERROR;
        }
        break;


/*    case SET_FREQ:
      cnv8_32(&packet->data[IN_DATA_BYTE],&S_config.S_freq); //check what happens if skipped
      S_config.S_freq = csp_ntoh32(S_config.S_freq);

      if (!WRITEDATA_ISOK(S_config.S_freq, ser_subtype)){
          status = -1;
      } else {
          HAL_S_setFreq(S_config.S_freq);
          status = 0;
      }
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      if (queue_response(packet) != SATR_OK) {
        return SATR_ERROR;
      }
      break;
*/

    default:
      ex2_log("No such subservice\n"); // Is it doing anything?
      return SATR_PKT_ILLEGAL_SUBSERVICE;
  }
  return SATR_OK;
}

/* Not sure why it's not working.
int WRITEDATA_ISOK(int val, int subservice){//replace int to binary
  switch (subservice) {
    case SET_FREQ:
        /* Could be more complete (including incremental steps)*//*
      return ((val > MIN_FREQ) && (val < MAX_FREQ)) ? 1 : 0;
      /*{ // can be simplified.
          return 1;
      } else {
          return 0;
      }
      break;*//*

    default:
      return 0;
  }
  return 0;
}
*/
