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

  Sband_config S_config;
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
        // Step 3: convert to network order
        S_config.freq = csp_htonflt(S_config.freq);
        // step 4: copy data & status byte into packet
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &S_config.freq, sizeof(S_config.freq));
        // Step 5: set packet length
        set_packet_length(packet, sizeof(int8_t) + sizeof(S_config.freq) + 1);
        // Step 6: return packet
        if (queue_response(packet) != SATR_OK) {
          return SATR_ERROR;
        }
        break;

    case GET_CONTROL:
      status = HAL_S_getControl (&S_config.PA);
      if (sizeof(S_config.PA) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      // csp_hton function can not accept structures.
      S_config.PA.status = csp_hton32((uint32_t)S_config.PA.status);
      S_config.PA.mode = csp_hton32((uint32_t)S_config.PA.mode);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_config.PA, sizeof(S_config.PA));
      set_packet_length(packet, sizeof(int8_t) + sizeof(S_config.PA) + 1);

      if (queue_response(packet) != SATR_OK) {
        return SATR_ERROR;
      }
      break;

    case GET_ENCODER:
      status = HAL_S_getEncoder(&S_config.enc);
      if (sizeof(S_config.enc) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      S_config.enc.scrambler = csp_hton32((uint32_t)S_config.enc.scrambler);
      S_config.enc.filter = csp_hton32((uint32_t)S_config.enc.filter);
      S_config.enc.modulation = csp_hton32((uint32_t)S_config.enc.modulation);
      S_config.enc.rate = csp_hton32((uint32_t)S_config.enc.rate);

      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_config.enc, sizeof(S_config.enc));
      set_packet_length(packet, sizeof(int8_t) + sizeof(S_config.enc) + 1);

      if (queue_response(packet) != SATR_OK) {
        return SATR_ERROR;
      }
      break;

    case GET_PA_POWER:
      status = HAL_S_getPAPower(&S_config.PA_Power);
      if (sizeof(S_config.PA_Power) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      S_config.PA_Power = csp_hton32((uint32_t)S_config.PA_Power);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_config.PA_Power, sizeof(S_config.PA_Power));
      set_packet_length(packet, sizeof(int8_t) + sizeof(uint32_t) + 1);
      if (queue_response(packet) != SATR_OK) {
        return SATR_ERROR;
      }
      break;

    case GET_CONFIG:
      status = HAL_S_getFreq (&S_config.freq) + HAL_S_getPAPower(&S_config.PA_Power) +
               HAL_S_getControl (&S_config.PA) + HAL_S_getEncoder(&S_config.enc);

      if (sizeof(S_config) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      S_config.freq = csp_htonflt(S_config.freq);
      S_config.PA_Power = csp_hton32((uint32_t)S_config.PA_Power);
      S_config.PA.status = csp_hton32((uint32_t)S_config.PA.status);
      S_config.PA.mode = csp_hton32((uint32_t)S_config.PA.mode);
      S_config.enc.scrambler = csp_hton32((uint32_t)S_config.enc.scrambler);
      S_config.enc.filter = csp_hton32((uint32_t)S_config.enc.filter);
      S_config.enc.modulation = csp_hton32((uint32_t)S_config.enc.modulation);
      S_config.enc.rate = csp_hton32((uint32_t)S_config.enc.rate);

      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_config, sizeof(S_config));
      set_packet_length(packet, sizeof(int8_t) + sizeof(S_config) + 1);
      if (queue_response(packet) != SATR_OK) {
        return SATR_ERROR;
      }
      break;

    case GET_STATUS:
      status = HAL_S_getStatus (&S_FS.status);
      if (sizeof(S_FS.status) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      S_FS.status.PWRGD = csp_hton32((uint32_t)S_FS.status.PWRGD);
      S_FS.status.TXL = csp_hton32((uint32_t)S_FS.status.TXL);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_FS.status, sizeof(S_FS.status));
      set_packet_length(packet, sizeof(int8_t) + sizeof(S_FS.status) + 1);

      if (queue_response(packet) != SATR_OK) {
        return SATR_ERROR;
      }
      break;

    case GET_TR:
        status = HAL_S_getTR(&S_FS.transmit);
        if (sizeof(S_FS.transmit) + 1 > csp_buffer_data_size()) {
          return SATR_ERROR;
        }
        S_FS.transmit.transmit = csp_hton32((uint32_t)S_FS.transmit.transmit);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &S_FS.transmit, sizeof(S_FS.transmit));
        set_packet_length(packet, sizeof(int8_t) + sizeof(S_FS.transmit) + 1);
        if (queue_response(packet) != SATR_OK) {
          return SATR_ERROR;
        }
        break;

    case GET_HK:
        status = HAL_S_getHK (&S_FS.HK);
        if (sizeof(S_FS.HK) + 1 > csp_buffer_data_size()) {
          return SATR_ERROR;
        }
        S_FS.HK.Output_Power = csp_htonflt(S_FS.HK.Output_Power);
        S_FS.HK.PA_Temp = csp_htonflt(S_FS.HK.PA_Temp);
        S_FS.HK.Top_Temp = csp_htonflt(S_FS.HK.Top_Temp);
        S_FS.HK.Bottom_Temp = csp_htonflt(S_FS.HK.Bottom_Temp);
        S_FS.HK.Bat_Current = csp_htonflt(S_FS.HK.Bat_Current);
        S_FS.HK.Bat_Voltage = csp_htonflt(S_FS.HK.Bat_Voltage);
        S_FS.HK.PA_Current = csp_htonflt(S_FS.HK.PA_Current);
        S_FS.HK.PA_Voltage = csp_htonflt(S_FS.HK.PA_Voltage);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &S_FS.HK, sizeof(S_FS.HK));
        set_packet_length(packet, sizeof(int8_t) + sizeof(S_FS.HK) + 1);

        if (queue_response(packet) != SATR_OK) {
          return SATR_ERROR;
        }
        break;

    case GET_BUFFER:
      SID = packet->data[SID_byte];
      if (SID<0 || SID > 2){
          return SATR_PKT_ILLEGAL_SUBSERVICE;
      } else {
          status = HAL_S_getBuffer(SID, &S_FS.buffer);
          if (sizeof(S_FS.buffer) + 1 > csp_buffer_data_size()) {
              return SATR_ERROR;
          }
          S_FS.buffer.pointer[SID] = csp_hton16(S_FS.buffer.pointer[SID]);
          memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
          memcpy(&packet->data[OUT_DATA_BYTE], &S_FS.buffer.pointer[SID], sizeof(S_FS.buffer.pointer[SID]));
          set_packet_length(packet, sizeof(int8_t) + sizeof(S_FS.buffer.pointer[SID]) + 1);
          if (queue_response(packet) != SATR_OK) {
              return SATR_ERROR;
          }
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
      status += HAL_S_getFV(&S_FS.Firmware_Version);
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

      case SET_FREQ:
          cnv8_F(&packet->data[IN_DATA_BYTE], &S_config.freq);
          //S_config.freq = (float) packet->data[IN_DATA_BYTE];
          S_config.freq = csp_ntohflt(S_config.freq);
          status = HAL_S_setFreq(S_config.freq);
          memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
          set_packet_length(packet, sizeof(int8_t) +  1);
          if (queue_response(packet) != SATR_OK) {
            return SATR_ERROR;
          }
          break;


      case SET_PA_POWER:
          S_config.PA_Power = (uint8_t)packet->data[IN_DATA_BYTE];
          S_config.PA_Power = csp_ntoh32((uint32_t)S_config.PA_Power);
          status = HAL_S_setPAPower(S_config.PA_Power);
          memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
          set_packet_length(packet, sizeof(int8_t) + 1);
          if (queue_response(packet) != SATR_OK) {
            return SATR_ERROR;
          }
          break;

      case SET_CONTROL:
          S_config.PA.status = (uint8_t)packet->data[IN_DATA_BYTE];
          S_config.PA.mode = (uint8_t)packet->data[IN_DATA_BYTE + 1];
          S_config.PA.status = csp_ntoh32((uint32_t)S_config.PA.status);
          S_config.PA.mode = csp_ntoh32((uint32_t)S_config.PA.mode);
          status = HAL_S_setControl(S_config.PA);
          memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
          set_packet_length(packet, sizeof(int8_t) +  1);
          if (queue_response(packet) != SATR_OK) {
            return SATR_ERROR;
          }
          break;

      case SET_ENCODER:
          S_config.enc.scrambler = (uint8_t)packet->data[IN_DATA_BYTE];
          S_config.enc.filter = (uint8_t)packet->data[IN_DATA_BYTE + 1];
          S_config.enc.modulation = (uint8_t)packet->data[IN_DATA_BYTE + 2];
          S_config.enc.rate = (uint8_t)packet->data[IN_DATA_BYTE + 3];
          S_config.enc.scrambler = csp_ntoh32((uint32_t)S_config.enc.scrambler);
          S_config.enc.filter = csp_ntoh32((uint32_t)S_config.enc.filter);
          S_config.enc.modulation = csp_ntoh32((uint32_t)S_config.enc.modulation);
          S_config.enc.rate = csp_ntoh32((uint32_t)S_config.enc.rate);
          status = HAL_S_setEncoder(S_config.enc);
          memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
          set_packet_length(packet, sizeof(int8_t) +  1);
          if (queue_response(packet) != SATR_OK) {
            return SATR_ERROR;
          }
          break;

      case SET_CONFIG:
          cnv8_F(&packet->data[IN_DATA_BYTE], &S_config.freq);
          S_config.freq = csp_ntohflt(S_config.freq);
          S_config.PA_Power = (uint8_t)packet->data[IN_DATA_BYTE + 4];//plus 4 because float takes 4B
          S_config.PA_Power = csp_ntoh32((uint32_t)S_config.PA_Power);
          S_config.PA.status = (uint8_t)packet->data[IN_DATA_BYTE + 5];
          S_config.PA.mode = (uint8_t)packet->data[IN_DATA_BYTE + 6];
          S_config.PA.status = csp_ntoh32((uint32_t)S_config.PA.status);
          S_config.PA.mode = csp_ntoh32((uint32_t)S_config.PA.mode);
          S_config.enc.scrambler = (uint8_t)packet->data[IN_DATA_BYTE + 7];
          S_config.enc.filter = (uint8_t)packet->data[IN_DATA_BYTE + 8];
          S_config.enc.modulation = (uint8_t)packet->data[IN_DATA_BYTE + 9];
          S_config.enc.rate = (uint8_t)packet->data[IN_DATA_BYTE + 10];
          S_config.enc.scrambler = csp_ntoh32((uint32_t)S_config.enc.scrambler);
          S_config.enc.filter = csp_ntoh32((uint32_t)S_config.enc.filter);
          S_config.enc.modulation = csp_ntoh32((uint32_t)S_config.enc.modulation);
          S_config.enc.rate = csp_ntoh32((uint32_t)S_config.enc.rate);
          status = HAL_S_setFreq (S_config.freq) + HAL_S_setPAPower(S_config.PA_Power) +
                   HAL_S_setControl (S_config.PA) + HAL_S_setEncoder(S_config.enc);
          memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
          set_packet_length(packet, sizeof(int8_t) +  1);
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


