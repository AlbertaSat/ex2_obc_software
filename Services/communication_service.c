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
static uint8_t SID_byte = 1;

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
   * Steps 2-5 in each case can be replaced by a single function.
   */

  Sband_config S_config;
  Sband_PowerAmplifier S_PA;
  Sband_Encoder S_enc;
  Sband_Status S_status;
  Sband_TR S_TR;
  Sband_Housekeeping S_HK;
  Sband_Buffer S_buffer;
  Sband_Full_Status S_FS; // FS: Full Status

  int SID;

  switch (ser_subtype) {

    case GET_FREQ:
        // Step 1: get the data
        status = HAL_S_getFreq (&S_config.freq);
        // Step 2: Check the size before proceeding
        if (sizeof(S_config.freq) + 1 > csp_buffer_data_size()) { // plus one for subservice
          return SATR_ERROR;
        }
        // Step 2: convert to network order
        S_config.freq = csp_htonflt(S_config.freq);
        // step3: copy data & status byte into packet
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &S_config.freq, sizeof(float));
        // Step 4: set packet length
        set_packet_length(packet, sizeof(int8_t) + sizeof(float) + 1);
        // Step 5: return packet
        if (queue_response(packet) != SATR_OK) {
          return SATR_ERROR;
        }
        break;

    case GET_CONTROL:
      status = HAL_S_getControl (&S_PA);
      if (sizeof(S_PA) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      // csp_hton function can not accept structures.
      S_PA.status = csp_hton32((uint32_t)S_PA.status);
      S_PA.mode = csp_hton32((uint32_t)S_PA.mode);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_PA, sizeof(S_PA));
      set_packet_length(packet, sizeof(int8_t) + sizeof(S_PA) + 1);

      if (queue_response(packet) != SATR_OK) {
        return SATR_ERROR;
      }
      break;

    case GET_ENCODER:
      status = HAL_S_getEncoder(&S_enc);
      if (sizeof(S_enc) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      S_enc.scrambler = csp_hton32((uint32_t)S_enc.scrambler);
      S_enc.filter = csp_hton32((uint32_t)S_enc.filter);
      S_enc.modulation = csp_hton32((uint32_t)S_enc.modulation);
      S_enc.rate = csp_hton32((uint32_t)S_enc.rate);

      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_enc, sizeof(S_enc));
      set_packet_length(packet, sizeof(int8_t) + sizeof(S_enc) + 1);

      if (queue_response(packet) != SATR_OK) {
        return SATR_ERROR;
      }
      break;

    case GET_PAPOWER:
      status = HAL_S_getPAPower(&S_config.PA_Power);
      if (sizeof(S_config.PA_Power) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      S_config.PA_Power = csp_hton32(S_config.PA_Power);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_config.PA_Power, sizeof(uint32_t));
      set_packet_length(packet, sizeof(int8_t) + sizeof(uint32_t) + 1);
      if (queue_response(packet) != SATR_OK) {
        return SATR_ERROR;
      }
      break;

    case GET_STATUS:
      status = HAL_S_getStatus (&S_status);
      if (sizeof(S_status) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      S_status.PWRGD = csp_hton32((uint32_t)S_status.PWRGD);
      S_status.TXL = csp_hton32((uint32_t)S_status.TXL);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_status, sizeof(S_status));
      set_packet_length(packet, sizeof(int8_t) + sizeof(S_status) + 1);

      if (queue_response(packet) != SATR_OK) {
        return SATR_ERROR;
      }
      break;

    case GET_TR:
        status = HAL_S_getTR(&S_TR);
        if (sizeof(S_TR) + 1 > csp_buffer_data_size()) {
          return SATR_ERROR;
        }
        S_TR.transmit = csp_hton32((uint32_t)S_TR.transmit);
        // what to do for int?
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &S_TR, sizeof(S_TR));
        set_packet_length(packet, sizeof(int8_t) + sizeof(S_TR) + 1);
        if (queue_response(packet) != SATR_OK) {
          return SATR_ERROR;
        }
        break;

    case GET_HK:
        status = HAL_S_getHK (&S_HK);
        if (sizeof(S_HK) + 1 > csp_buffer_data_size()) {
          return SATR_ERROR;
        }
        /*Use va_list from stdarg for various number of arguments. */
        S_HK.Output_Power = csp_htonflt(S_HK.Output_Power);
        S_HK.PA_Temp = csp_htonflt(S_HK.PA_Temp);
        S_HK.Top_Temp = csp_htonflt(S_HK.Top_Temp);
        S_HK.Bottom_Temp = csp_htonflt(S_HK.Bottom_Temp);
        S_HK.Bat_Current = csp_htonflt(S_HK.Bat_Current);
        S_HK.Bat_Voltage = csp_htonflt(S_HK.Bat_Voltage);
        S_HK.PA_Current = csp_htonflt(S_HK.PA_Current);
        S_HK.PA_Voltage = csp_htonflt(S_HK.PA_Voltage);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &S_HK, sizeof(S_HK));
        set_packet_length(packet, sizeof(int8_t) + sizeof(S_HK) + 1);

        if (queue_response(packet) != SATR_OK) {
          return SATR_ERROR;
        }
              break;

    case GET_BUFFER:
      SID = packet->data[SID_byte];
      status = HAL_S_getBuffer(SID, &S_buffer);
      if (sizeof(S_buffer) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      S_buffer.pointer[SID] = csp_hton16(S_buffer.pointer[SID]);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_buffer.pointer[SID], sizeof(S_buffer.pointer[SID]));
      set_packet_length(packet, sizeof(int8_t) + sizeof(S_buffer.pointer[SID]) + 1);
      if (queue_response(packet) != SATR_OK) {
        return SATR_ERROR;
      }
      break;

    case SOFT_RESET:
      status = HAL_S_softResetFPGA();
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      set_packet_length(packet, sizeof(int8_t) + 1);
      if (queue_response(packet) != SATR_OK) {
        return SATR_ERROR;
      }
      break;

    case GET_FULL_STATUS:
      status = HAL_S_getStatus (&S_FS.status) + HAL_S_getTR(&S_FS.transmit) + HAL_S_getHK(&S_FS.HK);
      status += HAL_S_getFS(&S_FS.Firmware_Version);
      int i;
      for (i=0 ; i<=2 ; i++){
          status += HAL_S_getBuffer(i,&S_FS.buffer);
      }
      if (sizeof(S_FS) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      S_FS.status.PWRGD = csp_hton32((uint32_t)S_FS.status.PWRGD);
      S_FS.status.TXL = csp_hton32((uint32_t)S_FS.status.TXL);
      S_FS.transmit.transmit = csp_hton32((uint32_t)S_FS.transmit.transmit);
      S_FS.HK.Output_Power = csp_htonflt(S_FS.HK.Output_Power);
      S_FS.HK.PA_Temp = csp_htonflt(S_FS.HK.PA_Temp);
      S_FS.HK.Top_Temp = csp_htonflt(S_FS.HK.Top_Temp);
      S_FS.HK.Bottom_Temp = csp_htonflt(S_FS.HK.Bottom_Temp);
      S_FS.HK.Bat_Current = csp_htonflt(S_FS.HK.Bat_Current);
      S_FS.HK.Bat_Voltage = csp_htonflt(S_FS.HK.Bat_Voltage);
      S_FS.HK.PA_Current = csp_htonflt(S_FS.HK.PA_Current);
      S_FS.HK.PA_Voltage = csp_htonflt(S_FS.HK.PA_Voltage);
      for (i=0 ; i<=2 ; i++){
          S_FS.buffer.pointer[i] = csp_hton16(S_FS.buffer.pointer[i]);
      }
      S_FS.Firmware_Version = csp_htonflt(S_FS.Firmware_Version);

      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_FS, sizeof(S_FS));
      set_packet_length(packet, sizeof(int8_t) + sizeof(S_FS) + 1);
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


