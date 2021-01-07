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

#include <FreeRTOS.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>

#include "hal/comms_hal.h"
#include "service_response.h"
#include "service_utilities.h"
#include "services.h"
#include "system.h"
#include "hal/uhf_hal.h"

#define CHAR_LEN 4  // Numpy unicode string character length
#define CALLSIGN_LEN 6
#define BEACON_MSG_LEN_MAX 36
#define FRAM_SIZE 16
#define SID_byte 1

static Sband_config S_config;  // In two different situations, globalization has
                               // resolved issues in set_freq

SAT_returnState communication_service_app(csp_packet_t *packet);

/**
 * @brief
 *      FreeRTOS communication server task
 * @details
 *      Accepts incoming communication service packets and executes the application
 * @param void* param
 * @return None
 */
void communication_service(void * param) {
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_RDPREQ);
    csp_bind(sock, TC_COMMUNICATION_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN);

    for(;;) {
        csp_conn_t *conn;
        csp_packet_t *packet;
        if ((conn = csp_accept(sock, CSP_MAX_TIMEOUT)) == NULL) {
          /* timeout */
          continue;
        }
        while ((packet = csp_read(conn, 50)) != NULL) {
          if (communication_service_app(packet) != SATR_OK) {
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
 *      Start the communication server task
 * @details
 *      Starts the FreeRTOS task responsible for accepting incoming
 *      communication service requests
 * @param None
 * @return SAT_returnState
 *      success report
 */
SAT_returnState start_communication_service(void) {
  if (xTaskCreate((TaskFunction_t)communication_service,
                  "communication_service", 1024, NULL, NORMAL_SERVICE_PRIO,
                  NULL) != pdPASS) {
    ex2_log("FAILED TO CREATE TASK start_communication_service\n");
    return SATR_ERROR;
  }
  ex2_log("Service handlers started\n");
  return SATR_OK;
}


/**
 * @brief
 *      Takes a CSP packet and switches based on the subservice command
 * @details
 *      Reads/Writes data from communication EHs as subservices
 * @attention
 *      Some subservices return the aggregation of error status of multiple HALs
 * @param *packet
 *      The CSP packet
 * @return SAT_returnState
 *      Success or failure
 */

SAT_returnState communication_service_app(csp_packet_t *packet) {
  uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];
  int8_t status;  // Status of HAL functions success
  uint8_t uhf_struct_len;
  int i;    // For indexing in multiple loops
  int SID;  // The identifier in the packet

  // Sband_config S_config;
  Sband_Full_Status S_FS;  // FS: Full Status

  UHF_Status U_stat;
  UHF_Beacon U_beacon;
  UHF_framStruct U_FRAM;
  UHF_Address U_I2C_add;
  UHF_Confirm U_restore;
  UHF_Confirm U_is_secure;
  UHF_Call_Sign U_callsign;

  switch (ser_subtype) {
    case S_GET_FREQ:
      // Step 1: get the data
      status = HAL_S_getFreq(&S_config.freq);
      // Step 2: Check the size before proceeding
      if (sizeof(S_config.freq) + 1 >
          csp_buffer_data_size()) {  // plus one for subservice
        return SATR_ERROR;
      }
      // Step 3: convert to network order
      S_config.freq = csp_htonflt(S_config.freq);
      // step 4: copy data & status byte into packet
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_config.freq,
             sizeof(S_config.freq));
      // Step 5: set packet length
      set_packet_length(packet, sizeof(int8_t) + sizeof(S_config.freq) + 1);

      break;

    case S_GET_CONTROL:
      status = HAL_S_getControl(&S_config.PA);
      if (sizeof(S_config.PA) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      // csp_hton function can not accept structures.
      S_config.PA.status = csp_hton32((uint32_t)S_config.PA.status);
      S_config.PA.mode = csp_hton32((uint32_t)S_config.PA.mode);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_config.PA, sizeof(S_config.PA));
      set_packet_length(packet, sizeof(int8_t) + sizeof(S_config.PA) + 1);

      break;

    case S_GET_ENCODER:
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

      break;

    case S_GET_PA_POWER:
      status = HAL_S_getPAPower(&S_config.PA_Power);
      if (sizeof(S_config.PA_Power) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      S_config.PA_Power = csp_hton32((uint32_t)S_config.PA_Power);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_config.PA_Power,
             sizeof(S_config.PA_Power));
      set_packet_length(packet, sizeof(int8_t) + sizeof(uint32_t) + 1);

      break;

    case S_GET_CONFIG:
      status = HAL_S_getFreq(&S_config.freq) +
               HAL_S_getPAPower(&S_config.PA_Power) +
               HAL_S_getControl(&S_config.PA) + HAL_S_getEncoder(&S_config.enc);

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

      break;

    case S_GET_STATUS:
      status = HAL_S_getStatus(&S_FS.status);
      if (sizeof(S_FS.status) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      S_FS.status.PWRGD = csp_hton32((uint32_t)S_FS.status.PWRGD);
      S_FS.status.TXL = csp_hton32((uint32_t)S_FS.status.TXL);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_FS.status, sizeof(S_FS.status));
      set_packet_length(packet, sizeof(int8_t) + sizeof(S_FS.status) + 1);

      break;

    case S_GET_TR:
      status = HAL_S_getTR(&S_FS.transmit);
      if (sizeof(S_FS.transmit) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      S_FS.transmit.transmit = csp_hton32((uint32_t)S_FS.transmit.transmit);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_FS.transmit,
             sizeof(S_FS.transmit));
      set_packet_length(packet, sizeof(int8_t) + sizeof(S_FS.transmit) + 1);

      break;

    case S_GET_HK:
      status = HAL_S_getHK(&S_FS.HK);
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

      break;

    case S_GET_BUFFER:
      SID = packet->data[SID_byte];
      if (SID < 0 || SID > 2) {
        return SATR_PKT_ILLEGAL_SUBSERVICE;
      } else {
        status = HAL_S_getBuffer(SID, &S_FS.buffer);
        if (sizeof(S_FS.buffer) + 1 > csp_buffer_data_size()) {
          return SATR_ERROR;
        }
        S_FS.buffer.pointer[SID] = csp_hton16(S_FS.buffer.pointer[SID]);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &S_FS.buffer.pointer[SID],
               sizeof(S_FS.buffer.pointer[SID]));
        set_packet_length(
            packet, sizeof(int8_t) + sizeof(S_FS.buffer.pointer[SID]) + 1);

      }
      break;

    case S_SOFT_RESET:
      status = HAL_S_softResetFPGA();
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      set_packet_length(packet, sizeof(int8_t) + 1);

      break;

    case S_GET_FULL_STATUS:
      status = HAL_S_getStatus(&S_FS.status) + HAL_S_getTR(&S_FS.transmit) +
               HAL_S_getHK(&S_FS.HK);
      status += HAL_S_getFV(&S_FS.Firmware_Version);
      for (i = 0; i <= 2; i++) {
        status += HAL_S_getBuffer(i, &S_FS.buffer);
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
      for (i = 0; i <= 2; i++) {
        S_FS.buffer.pointer[i] = csp_hton16(S_FS.buffer.pointer[i]);
      }
      S_FS.Firmware_Version = csp_htonflt(S_FS.Firmware_Version);

      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_FS, sizeof(S_FS));
      set_packet_length(packet, sizeof(int8_t) + sizeof(S_FS) + 1);

      break;

    case S_SET_FREQ:
      cnv8_F(&packet->data[IN_DATA_BYTE], &S_config.freq);
      S_config.freq = csp_ntohflt(S_config.freq);
      status = HAL_S_setFreq(S_config.freq);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      set_packet_length(packet, sizeof(int8_t) + 1);

      break;

    case S_SET_PA_POWER:
      S_config.PA_Power = (uint8_t)packet->data[IN_DATA_BYTE];
      S_config.PA_Power = csp_ntoh32((uint32_t)S_config.PA_Power);
      status = HAL_S_setPAPower(S_config.PA_Power);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      set_packet_length(packet, sizeof(int8_t) + 1);

      break;

    case S_SET_CONTROL:
      S_config.PA.status = (uint8_t)packet->data[IN_DATA_BYTE];
      S_config.PA.mode = (uint8_t)packet->data[IN_DATA_BYTE + 1];
      S_config.PA.status = csp_ntoh32((uint32_t)S_config.PA.status);
      S_config.PA.mode = csp_ntoh32((uint32_t)S_config.PA.mode);
      status = HAL_S_setControl(S_config.PA);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      set_packet_length(packet, sizeof(int8_t) + 1);

      break;

    case S_SET_ENCODER:
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
      set_packet_length(packet, sizeof(int8_t) + 1);

      break;

    case S_SET_CONFIG:
      cnv8_F(&packet->data[IN_DATA_BYTE], &S_config.freq);
      S_config.freq = csp_ntohflt(S_config.freq);
      S_config.PA_Power =
          (uint8_t)
              packet->data[IN_DATA_BYTE + 4];  // plus 4 because float takes 4B
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
      status = HAL_S_setFreq(S_config.freq) +
               HAL_S_setPAPower(S_config.PA_Power) +
               HAL_S_setControl(S_config.PA) + HAL_S_setEncoder(S_config.enc);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      set_packet_length(packet, sizeof(int8_t) + 1);

      break;

      /* UHF Subservices */
    case UHF_SET_STAT_CONTROL:
      for (i = 0; i < STAT_WORD_LEN; i++) {
        U_stat.status_ctrl[i] = (uint8_t)packet->data[IN_DATA_BYTE + i];
        U_stat.status_ctrl[i] = csp_ntoh32((uint32_t)U_stat.status_ctrl[i]);
      }
      status = HAL_UHF_setStatus(&U_stat.status_ctrl);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      set_packet_length(packet, sizeof(int8_t) + 1);

      break;

    case UHF_SET_FREQ:
      cnv8_32(&packet->data[IN_DATA_BYTE], &U_stat.set.freq);
      U_stat.set.freq = csp_ntoh32(U_stat.set.freq);
      status = HAL_UHF_setFreq(U_stat.set.freq);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      set_packet_length(packet, sizeof(int8_t) + 1);

      break;

    case UHF_SET_PIPE_TIMEOUT:
      cnv8_16LE(&packet->data[IN_DATA_BYTE], &U_stat.set.pipe_t);
      U_stat.set.pipe_t = csp_ntoh16(U_stat.set.pipe_t);
      status = HAL_UHF_setPipeT(U_stat.set.pipe_t);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      set_packet_length(packet, sizeof(int8_t) + 1);

      break;

    case UHF_SET_BEACON_T:
      cnv8_16LE(&packet->data[IN_DATA_BYTE], &U_stat.set.beacon_t);
      U_stat.set.beacon_t = csp_ntoh16(U_stat.set.beacon_t);
      status = HAL_UHF_setBeaconT(U_stat.set.beacon_t);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      set_packet_length(packet, sizeof(int8_t) + 1);

      break;

    case UHF_SET_AUDIO_T:
      cnv8_16LE(&packet->data[IN_DATA_BYTE], &U_stat.set.audio_t);
      U_stat.set.audio_t = csp_ntoh16(U_stat.set.audio_t);
      status = HAL_UHF_setAudioT(U_stat.set.audio_t);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      set_packet_length(packet, sizeof(int8_t) + 1);

      break;

    case UHF_SET_PARAMS:
      cnv8_32(&packet->data[IN_DATA_BYTE], &U_stat.set.freq);
      U_stat.set.freq = csp_ntoh32(U_stat.set.freq);
      cnv8_16LE(&packet->data[IN_DATA_BYTE + 4], &U_stat.set.pipe_t);
      U_stat.set.pipe_t = csp_ntoh16(U_stat.set.pipe_t);
      cnv8_16LE(&packet->data[IN_DATA_BYTE + 6], &U_stat.set.beacon_t);
      U_stat.set.beacon_t = csp_ntoh16(U_stat.set.beacon_t);
      cnv8_16LE(&packet->data[IN_DATA_BYTE + 8], &U_stat.set.audio_t);
      U_stat.set.audio_t = csp_ntoh16(U_stat.set.audio_t);
      status = HAL_UHF_setFreq(U_stat.set.freq) +
               HAL_UHF_setPipeT(U_stat.set.pipe_t) +
               HAL_UHF_setBeaconT(U_stat.set.beacon_t) +
               HAL_UHF_setAudioT(U_stat.set.audio_t);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      set_packet_length(packet, sizeof(int8_t) + 1);

      break;

    case UHF_RESTORE_DEFAULT:
      status = HAL_UHF_restore(&U_restore);
      if (sizeof(U_restore) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &U_restore, sizeof(U_restore));
      set_packet_length(packet, sizeof(int8_t) + sizeof(U_restore) + 1);

      break;

    case UHF_LOW_PWR:
      status = HAL_UHF_lowPwr(&U_stat.low_pwr_stat);
      if (sizeof(U_stat.low_pwr_stat) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &U_stat.low_pwr_stat,
             sizeof(U_stat.low_pwr_stat));
      set_packet_length(packet,
                        sizeof(int8_t) + sizeof(U_stat.low_pwr_stat) + 1);

      break;

    case UHF_SET_DEST:
      uhf_struct_len = CALLSIGN_LEN;
      U_callsign.dest.len = uhf_struct_len;
      for (i = 0; i < uhf_struct_len; i++) {
        U_callsign.dest.message[i] =
            (uint8_t)packet->data[IN_DATA_BYTE + (CHAR_LEN - 1) + CHAR_LEN * i];
        U_callsign.dest.message[i] =
            csp_ntoh32((uint32_t)U_callsign.dest.message[i]);
      }
      status = HAL_UHF_setDestination(U_callsign.dest);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      set_packet_length(packet, sizeof(int8_t) + 1);

      break;

    case UHF_SET_SRC:
      uhf_struct_len = CALLSIGN_LEN;
      U_callsign.src.len = uhf_struct_len;
      for (i = 0; i < uhf_struct_len; i++) {
        U_callsign.src.message[i] =
            (uint8_t)packet->data[IN_DATA_BYTE + (CHAR_LEN - 1) + CHAR_LEN * i];
        U_callsign.src.message[i] =
            csp_ntoh32((uint32_t)U_callsign.src.message[i]);
      }
      status = HAL_UHF_setSource(U_callsign.src);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      set_packet_length(packet, sizeof(int8_t) + 1);

      break;

    case UHF_SET_MORSE:
      uhf_struct_len = BEACON_MSG_LEN_MAX;
      U_beacon.morse.len = uhf_struct_len;
      for (i = 0; i < uhf_struct_len; i++) {
        U_beacon.morse.message[i] =
            (uint8_t)packet->data[IN_DATA_BYTE + (CHAR_LEN - 1) + CHAR_LEN * i];
        U_beacon.morse.message[i] =
            csp_ntoh32((uint32_t)U_beacon.morse.message[i]);
      }
      status = HAL_UHF_setMorse(U_beacon.morse);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      set_packet_length(packet, sizeof(int8_t) + 1);

      break;

    case UHF_SET_MIDI:
      uhf_struct_len = BEACON_MSG_LEN_MAX;
      U_beacon.MIDI.len = uhf_struct_len;
      for (i = 0; i < uhf_struct_len; i++) {
        U_beacon.MIDI.message[i] =
            (uint8_t)packet->data[IN_DATA_BYTE + (CHAR_LEN - 1) + CHAR_LEN * i];
        U_beacon.MIDI.message[i] =
            csp_ntoh32((uint32_t)U_beacon.MIDI.message[i]);
      }
      status = HAL_UHF_setMIDI(U_beacon.MIDI);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      set_packet_length(packet, sizeof(int8_t) + 1);

      break;

    case UHF_SET_BEACON_MSG:
      // uhf_struct_len = (uint8_t)packet->data[IN_DATA_BYTE]; Could be used if
      // GS did not require specified length for unicode strings.
      uhf_struct_len = BEACON_MSG_LEN_MAX;
      U_beacon.message.len = uhf_struct_len;
      for (i = 0; i < BEACON_MSG_LEN_MAX; i++) {
        U_beacon.message.message[i] =
            (uint8_t)
                packet->data[IN_DATA_BYTE + 1 + (CHAR_LEN - 1) + CHAR_LEN * i];
        U_beacon.message.message[i] =
            csp_ntoh32((uint32_t)U_beacon.message.message[i]);
      }
      status = HAL_UHF_setBeaconMsg(U_beacon.message);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      set_packet_length(packet, sizeof(int8_t) + 1);

      break;

    case UHF_SET_I2C:
      U_I2C_add.addr = (uint8_t)packet->data[IN_DATA_BYTE];
      U_I2C_add.addr = csp_ntoh32((uint32_t)U_I2C_add.addr);
      status = HAL_UHF_setI2C(U_I2C_add.addr);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      set_packet_length(packet, sizeof(int8_t) + 1);

      break;

    case UHF_WRITE_FRAM:
      cnv8_32(&packet->data[IN_DATA_BYTE], &U_FRAM.addr);
      for (i = 0; i < FRAM_SIZE; i++) {
        U_FRAM.data[i] =
            (uint8_t)packet->data[IN_DATA_BYTE + sizeof(U_FRAM.addr) +
                                  (CHAR_LEN - 1) + CHAR_LEN * i];
        U_FRAM.data[i] = csp_ntoh32((uint32_t)U_FRAM.data[i]);
      }
      status = HAL_UHF_setFRAM(U_FRAM);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      set_packet_length(packet, sizeof(int8_t) + 1);

      break;

    case UHF_SECURE:
      status = HAL_UHF_secure(&U_is_secure);
      if (sizeof(U_is_secure) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &U_is_secure, sizeof(U_is_secure));
      set_packet_length(packet, sizeof(int8_t) + sizeof(U_is_secure) + 1);

      break;

    case UHF_GET_FULL_STAT:
      status = HAL_UHF_getStatus(&U_stat.status_ctrl) +
               HAL_UHF_getFreq(&U_stat.set.freq) +
               HAL_UHF_getUptime(&U_stat.uptime) +
               HAL_UHF_getPcktsOut(&U_stat.pckts_out) +
               HAL_UHF_getPcktsIn(&U_stat.pckts_in) +
               HAL_UHF_getPcktsInCRC16(&U_stat.pckts_in_crc16) +
               HAL_UHF_getPipeT(&U_stat.set.pipe_t) +
               HAL_UHF_getBeaconT(&U_stat.set.beacon_t) +
               HAL_UHF_getAudioT(&U_stat.set.audio_t) +
               HAL_UHF_getTemp(&U_stat.temperature) +
               HAL_UHF_getLowPwr(&U_stat.low_pwr_stat) +
               HAL_UHF_getFV(&U_stat.firmware_ver) +
               HAL_UHF_getPayload(&U_stat.payload_size) +
               HAL_UHF_getSecureKey(&U_stat.secure_key);

      if (sizeof(U_stat) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      for (i = 0; i < STAT_WORD_LEN; i++) {
        U_stat.status_ctrl[i] = csp_hton32((uint32_t)U_stat.status_ctrl[i]);
      }
      U_stat.set.freq = csp_hton32(U_stat.set.freq);
      U_stat.uptime = csp_hton32(U_stat.uptime);
      U_stat.pckts_out = csp_hton32(U_stat.pckts_out);
      U_stat.pckts_in = csp_hton32(U_stat.pckts_in);
      U_stat.pckts_in_crc16 = csp_hton32(U_stat.pckts_in_crc16);
      U_stat.set.pipe_t = csp_hton16(U_stat.set.pipe_t);
      U_stat.set.beacon_t = csp_hton16(U_stat.set.beacon_t);
      U_stat.set.audio_t = csp_hton16(U_stat.set.audio_t);
      U_stat.temperature = csp_htonflt(U_stat.temperature);
      U_stat.low_pwr_stat = csp_hton32((uint32_t)U_stat.low_pwr_stat);
      U_stat.firmware_ver = csp_hton32((uint32_t)U_stat.firmware_ver);
      U_stat.payload_size = csp_hton16(U_stat.payload_size);
      U_stat.secure_key = csp_hton32(U_stat.secure_key);

      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &U_stat, sizeof(U_stat));
      set_packet_length(packet, sizeof(int8_t) + sizeof(U_stat) + 1);

      break;

    case UHF_GET_CALL_SIGN:
      status = HAL_UHF_getDestination(&U_callsign.dest) +
               HAL_UHF_getSource(&U_callsign.src);
      uhf_struct_len = CALLSIGN_LEN;
      uint8_t dst[CALLSIGN_LEN * CHAR_LEN];
      memset(dst, 0, CALLSIGN_LEN * CHAR_LEN);
      uint8_t src[CALLSIGN_LEN * CHAR_LEN];
      memset(src, 0, CALLSIGN_LEN * CHAR_LEN);
      if (sizeof(dst) + sizeof(src) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      for (i = 0; i < uhf_struct_len; i++) {
        dst[(CHAR_LEN - 1) + CHAR_LEN * i] =
            csp_hton32((uint32_t)U_callsign.dest.message[i]);
        src[(CHAR_LEN - 1) + CHAR_LEN * i] =
            csp_hton32((uint32_t)U_callsign.src.message[i]);
      }

      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &dst, sizeof(dst));
      memcpy(&packet->data[OUT_DATA_BYTE + sizeof(dst)], &src, sizeof(src));
      set_packet_length(packet, sizeof(int8_t) + sizeof(dst) + sizeof(src) + 1);

      break;

    case UHF_GET_MORSE:
      status = HAL_UHF_getMorse(&U_beacon.morse);
      uhf_struct_len = U_beacon.morse.len;
      uint8_t mrs[BEACON_MSG_LEN_MAX * CHAR_LEN];
      memset(mrs, 0, BEACON_MSG_LEN_MAX * CHAR_LEN);
      if (sizeof(mrs) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      for (i = 0; i < uhf_struct_len; i++) {
        mrs[(CHAR_LEN - 1) + CHAR_LEN * i] =
            csp_hton32((uint32_t)U_beacon.morse.message[i]);
      }

      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &mrs, sizeof(mrs));
      set_packet_length(packet, sizeof(int8_t) + sizeof(mrs) + 1);

      break;

    case UHF_GET_MIDI:
      status = HAL_UHF_getMIDI(&U_beacon.MIDI);
      uhf_struct_len = U_beacon.MIDI.len;
      uint8_t midi[BEACON_MSG_LEN_MAX * CHAR_LEN];
      memset(midi, 0, BEACON_MSG_LEN_MAX * CHAR_LEN);
      if (sizeof(midi) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      for (i = 0; i < uhf_struct_len; i++) {
        midi[(CHAR_LEN - 1) + CHAR_LEN * i] =
            csp_hton32((uint32_t)U_beacon.MIDI.message[i]);
      }

      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &midi, sizeof(midi));
      set_packet_length(packet, sizeof(int8_t) + sizeof(midi) + 1);

      break;

    case UHF_GET_BEACON_MSG:
      status = HAL_UHF_getBeaconMsg(&U_beacon.message);
      uhf_struct_len = U_beacon.message.len;
      // Switch BEACON_MSG_LEN_MAX to MAX_W_CMDLEN after packet configuration
      uint8_t beacon[BEACON_MSG_LEN_MAX * CHAR_LEN];
      memset(beacon, 0, BEACON_MSG_LEN_MAX * CHAR_LEN);
      if (sizeof(beacon) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      for (i = 0; i < uhf_struct_len; i++) {
        beacon[(CHAR_LEN - 1) + CHAR_LEN * i] =
            csp_hton32((uint32_t)U_beacon.message.message[i]);
      }

      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &beacon, sizeof(beacon));
      set_packet_length(packet, sizeof(int8_t) + sizeof(beacon) + 1);

      break;

    case UHF_GET_FRAM:  // doesn't need address?
      status = HAL_UHF_getFRAM(&U_FRAM);
      uint8_t fram[FRAM_SIZE * CHAR_LEN];
      memset(fram, 0, FRAM_SIZE * CHAR_LEN);
      if (sizeof(fram) + 1 > csp_buffer_data_size()) {
        return SATR_ERROR;
      }
      for (i = 0; i < FRAM_SIZE; i++) {
        fram[(CHAR_LEN - 1) + CHAR_LEN * i] =
            csp_hton32((uint32_t)U_FRAM.data[i]);
      }
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &fram, sizeof(fram));
      set_packet_length(packet, sizeof(int8_t) + sizeof(fram) + 1);

      break;

    default:
      ex2_log("No such subservice\n");
      return SATR_PKT_ILLEGAL_SUBSERVICE;
  }
  return SATR_OK;
}
