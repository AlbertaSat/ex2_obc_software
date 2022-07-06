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

#include "communication/communication_service.h"

#include <FreeRTOS.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <main/system.h>

#include "services.h"
#include "task_manager/task_manager.h"
#include "uhf.h"
#include "sband.h"
#include "util/service_utilities.h"

#define CHAR_LEN 1 // If using Numpy unicode string, change to 4
#define CALLSIGN_LEN 6
#define MORSE_BEACON_MSG_LEN_MAX 36
#define BEACON_MSG_LEN_MAX 60
// Update this to 108 (MIDI) and 97 (Beacon msg) when packet configuration
// is changed.
#define FRAM_SIZE 16
#define SID_byte 1
#define SINGLE_NOTE_LEN 3 // For MIDI audio notes

SAT_returnState communication_service_app(csp_packet_t *packet);

static uint32_t svc_wdt_counter = 0;
static uint32_t get_svc_wdt_counter() { return svc_wdt_counter; }

/**
 * @brief
 *      FreeRTOS communication server task
 * @details
 *      Accepts incoming communication service packets and executes the
 * application
 * @param void* param
 * @return None
 */
void communication_service(void *param) {
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_HMACREQ);
    csp_bind(sock, TC_COMMUNICATION_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN);

    svc_wdt_counter++;

    for (;;) {
        csp_packet_t *packet;
        csp_conn_t *conn;
        if ((conn = csp_accept(sock, DELAY_WAIT_TIMEOUT)) == NULL) {
            svc_wdt_counter++;
            /* timeout */
            continue;
        }
        svc_wdt_counter++;

        while ((packet = csp_read(conn, 50)) != NULL) {
            if (communication_service_app(packet) != SATR_OK) {
                // something went wrong in the service
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
    TaskHandle_t svc_tsk;
    taskFunctions svc_funcs = {0};
    svc_funcs.getCounterFunction = get_svc_wdt_counter;

    if (xTaskCreate((TaskFunction_t)communication_service, "communication_service", 1024, NULL,
                    NORMAL_SERVICE_PRIO, &svc_tsk) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK start_communication_service\n");
        return SATR_ERROR;
    }
    ex2_register(svc_tsk, svc_funcs);
    ex2_log("Communication service started\n");
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
    int8_t status;                          // Status of HAL functions success
    SAT_returnState return_state = SATR_OK; // Remains OK unless changed by default or if statements.
    uint8_t uhf_struct_len;
    int i;           // For indexing in multiple loops
    int SID;         // The identifier in the packet
    uint8_t confirm; // For confirming the change

    switch (ser_subtype) {
    case S_GET_FREQ: {
        // Step 1: get the data
        float freq;
        status = HAL_S_getFreq(&freq);
        // Step 2: Check the size before proceeding
        if (sizeof(freq) + 1 > csp_buffer_data_size()) { // plus one for subservice
            return_state = SATR_ERROR;
        }
        // Step 3: convert to network order
        freq = csp_htonflt(freq);
        // step 4: copy data & status byte into packet
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &freq, sizeof(freq));
        // Step 5: set packet length
        set_packet_length(packet, sizeof(int8_t) + sizeof(freq) + 1);

        break;
    }

    case S_GET_CONTROL: {
        Sband_PowerAmplifier PA;
        status = HAL_S_getControl(&PA);
        if (sizeof(PA) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &PA, sizeof(PA));
        set_packet_length(packet, sizeof(int8_t) + sizeof(PA) + 1);

        break;
    }

    case S_GET_ENCODER: {
        Sband_Encoder enc;
        status = HAL_S_getEncoder(&enc);
        if (sizeof(enc) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &enc, sizeof(enc));
        set_packet_length(packet, sizeof(int8_t) + sizeof(enc) + 1);

        break;
    }

    case S_GET_PA_POWER: {
        uint8_t PA_Power;
        status = HAL_S_getPAPower(&PA_Power);
        if (sizeof(PA_Power) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &PA_Power, sizeof(PA_Power));
        set_packet_length(packet, sizeof(int8_t) + sizeof(PA_Power) + 1);

        break;
    }

    case S_GET_CONFIG: {
        Sband_config S_config;
        status = HAL_S_getFreq(&S_config.freq) + HAL_S_getPAPower(&S_config.PA_Power) +
                 HAL_S_getControl(&S_config.PA) + HAL_S_getEncoder(&S_config.enc);

        if (sizeof(S_config) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        S_config.freq = csp_htonflt(S_config.freq);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &S_config, sizeof(S_config));
        set_packet_length(packet, sizeof(int8_t) + sizeof(S_config) + 1);
        break;
    }

    case S_GET_STATUS: {
        Sband_Status S_status;
        status = HAL_S_getStatus(&S_status);
        if (sizeof(status) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &S_status, sizeof(S_status));
        set_packet_length(packet, sizeof(int8_t) + sizeof(S_status) + 1);

        break;
    }

    case S_GET_FW: {
      Sband_FirmwareV firmware;
      status = HAL_S_getFirmwareV(&firmware);
      if (sizeof(firmware) + 1 > csp_buffer_data_size()) {
        return_state = SATR_ERROR;
      }
      firmware.firmware = csp_hton16(firmware.firmware);
      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &firmware, sizeof(firmware));
      set_packet_length(packet, sizeof(int8_t) + sizeof(firmware) + 1);

      break;
    }

    case S_GET_TR: {
        Sband_TR transmit;
        status = HAL_S_getTR(&transmit);
        if (sizeof(transmit) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &transmit, sizeof(transmit));
        set_packet_length(packet, sizeof(int8_t) + sizeof(transmit) + 1);

        break;
    }

    case S_GET_HK: {
        Sband_Housekeeping HK;
        status = HAL_S_getHK(&HK);
        if (sizeof(HK) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        HK.Output_Power = csp_htonflt(HK.Output_Power);
        HK.PA_Temp = csp_htonflt(HK.PA_Temp);
        HK.Top_Temp = csp_htonflt(HK.Top_Temp);
        HK.Bottom_Temp = csp_htonflt(HK.Bottom_Temp);
        HK.Bat_Current = csp_htonflt(HK.Bat_Current);
        HK.Bat_Voltage = csp_htonflt(HK.Bat_Voltage);
        HK.PA_Current = csp_htonflt(HK.PA_Current);
        HK.PA_Voltage = csp_htonflt(HK.PA_Voltage);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &HK, sizeof(HK));
        set_packet_length(packet, sizeof(int8_t) + sizeof(HK) + 1);

        break;
    }

    case S_GET_BUFFER: {
        Sband_Buffer buffer;
        SID = packet->data[SID_byte];
        if (SID < 0 || SID > 2) {
            return_state = SATR_PKT_ILLEGAL_SUBSERVICE;
        } else {
            status = HAL_S_getBuffer(SID, &buffer);
            if (sizeof(buffer) + 1 > csp_buffer_data_size()) {
                return_state = SATR_ERROR;
            }
            buffer.pointer[SID] = csp_hton16(buffer.pointer[SID]);
            memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
            memcpy(&packet->data[OUT_DATA_BYTE], &buffer.pointer[SID], sizeof(buffer.pointer[SID]));
            set_packet_length(packet, sizeof(int8_t) + sizeof(buffer.pointer[SID]) + 1);
        }
        break;
    }

    case S_SOFT_RESET:
        status = HAL_S_softResetFPGA();
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;

    case S_GET_FULL_STATUS: {
      Sband_Full_Status S_FS;  // FS: Full Status
      status = HAL_S_getStatus(&S_FS.status) + HAL_S_getFirmwareV(&S_FS.firmware) +
              HAL_S_getTR(&S_FS.transmit) + HAL_S_getHK(&S_FS.HK);
      for (i = 0; i <= 2; i++) {
        status += HAL_S_getBuffer(i, &S_FS.buffer);
      }
      if (sizeof(S_FS) + 1 > csp_buffer_data_size()) {
        return_state = SATR_ERROR;
      }
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

      memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
      memcpy(&packet->data[OUT_DATA_BYTE], &S_FS, sizeof(S_FS));
      set_packet_length(packet, sizeof(int8_t) + sizeof(S_FS) + 1);
      break;
    }

    case S_SET_FREQ: {
        float freq;
        cnv8_F(&packet->data[IN_DATA_BYTE], &freq);
        freq = csp_ntohflt(freq);
        status = HAL_S_setFreq(freq);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case S_SET_PA_POWER: {
        uint8_t PA_Power;
        PA_Power = (uint8_t)packet->data[IN_DATA_BYTE];
        status = HAL_S_setPAPower(PA_Power);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case S_SET_CONTROL: {
        Sband_PowerAmplifier PA;
        PA.status = (Sband_PowerAmplifier_Status_t)packet->data[IN_DATA_BYTE];
        PA.mode = (Sband_Transmitter_Mode_t)packet->data[IN_DATA_BYTE + 1];
        status = HAL_S_setControl(PA);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case S_SET_ENCODER: {
        Sband_Encoder enc;
        enc.scrambler = (uint8_t)packet->data[IN_DATA_BYTE];
        enc.filter = (uint8_t)packet->data[IN_DATA_BYTE + 1];
        enc.modulation = (uint8_t)packet->data[IN_DATA_BYTE + 2];
        enc.rate = (uint8_t)packet->data[IN_DATA_BYTE + 3];
        status = HAL_S_setEncoder(enc);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case S_SET_CONFIG: {
        Sband_config S_config;
        cnv8_F(&packet->data[IN_DATA_BYTE], &S_config.freq);
        S_config.freq = csp_ntohflt(S_config.freq);
        S_config.PA_Power = (uint8_t)packet->data[IN_DATA_BYTE + 4]; // plus 4 because float takes 4B
        S_config.PA.status = (Sband_PowerAmplifier_Status_t)packet->data[IN_DATA_BYTE + 5];
        S_config.PA.mode = (Sband_Transmitter_Mode_t)packet->data[IN_DATA_BYTE + 6];
        S_config.enc.scrambler = (uint8_t)packet->data[IN_DATA_BYTE + 7];
        S_config.enc.filter = (uint8_t)packet->data[IN_DATA_BYTE + 8];
        S_config.enc.modulation = (uint8_t)packet->data[IN_DATA_BYTE + 9];
        S_config.enc.rate = (uint8_t)packet->data[IN_DATA_BYTE + 10];
        status = HAL_S_setFreq(S_config.freq) + HAL_S_setPAPower(S_config.PA_Power) +
                 HAL_S_setControl(S_config.PA) + HAL_S_setEncoder(S_config.enc);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

        /* UHF Subservices */
    case UHF_SET_SCW: {
        uint8_t scw[SCW_LEN];
        for (i = 0; i < SCW_LEN; i++) {
            scw[i] = (uint8_t)packet->data[IN_DATA_BYTE + i];
        }
        status = HAL_UHF_setSCW(scw);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    // read scw and change the respective bit and set again
    case UHF_SET_ECHO:
    case UHF_SET_BCN:
    case UHF_SET_PIPE: {
        uint8_t scw[SCW_LEN];
        status = HAL_UHF_getSCW(scw);
        if (status == U_GOOD_CONFIG) {
            scw[ser_subtype - UHF_SET_ECHO + 4] = 1;
            status = HAL_UHF_setSCW(scw);
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case UHF_SET_FREQ: {
        uint32_t freq;
        cnv8_32(&packet->data[IN_DATA_BYTE], &freq);
        freq = csp_ntoh32(freq);
        status = HAL_UHF_setFreq(freq);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case UHF_SET_PIPE_TIMEOUT: {
        uint32_t pipe_t;
        cnv8_32(&packet->data[IN_DATA_BYTE], &pipe_t);
        pipe_t = csp_ntoh32(pipe_t);
        status = HAL_UHF_setPipeT(pipe_t);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case UHF_SET_BEACON_T: {
        uint32_t beacon_t;
        cnv8_32(&packet->data[IN_DATA_BYTE], &beacon_t);
        beacon_t = csp_ntoh32(beacon_t);
        status = HAL_UHF_setBeaconT(beacon_t);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case UHF_SET_AUDIO_T: {
        uint32_t audio_t;
        cnv8_32(&packet->data[IN_DATA_BYTE], &audio_t);
        audio_t = csp_ntoh32(audio_t);
        status = HAL_UHF_setAudioT(audio_t);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case UHF_SET_PARAMS: {
        UHF_Settings set;
        cnv8_32(&packet->data[IN_DATA_BYTE], &set.freq);
        set.freq = csp_ntoh32(set.freq);
        cnv8_32(&packet->data[IN_DATA_BYTE + 4], &set.pipe_t);
        set.pipe_t = csp_ntoh32(set.pipe_t);
        cnv8_32(&packet->data[IN_DATA_BYTE + 8], &set.beacon_t);
        set.beacon_t = csp_ntoh32(set.beacon_t);
        cnv8_32(&packet->data[IN_DATA_BYTE + 12], &set.audio_t);
        set.audio_t = csp_ntoh32(set.audio_t);
        status = HAL_UHF_setFreq(set.freq) + HAL_UHF_setPipeT(set.pipe_t) + HAL_UHF_setBeaconT(set.beacon_t) +
                 HAL_UHF_setAudioT(set.audio_t);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case UHF_RESTORE_DEFAULT:
        confirm = (uint8_t)packet->data[IN_DATA_BYTE];
        status = HAL_UHF_restore(confirm);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;

    case UHF_LOW_PWR:
        confirm = (uint8_t)packet->data[IN_DATA_BYTE];
        status = HAL_UHF_lowPwr(confirm);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;

    case UHF_SET_DEST: {
        UHF_configStruct dest;
        uhf_struct_len = CALLSIGN_LEN;
        dest.len = uhf_struct_len;
        for (i = 0; i < uhf_struct_len && (uint8_t)packet->data[IN_DATA_BYTE + (CHAR_LEN - 1) + CHAR_LEN * i] != 0;
             i++) {
            dest.message[i] = (uint8_t)packet->data[IN_DATA_BYTE + (CHAR_LEN - 1) + CHAR_LEN * i];
        }
        status = HAL_UHF_setDestination(dest);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case UHF_SET_SRC: {
        UHF_configStruct src;
        uhf_struct_len = CALLSIGN_LEN;
        src.len = uhf_struct_len;
        for (i = 0; i < uhf_struct_len && (uint8_t)packet->data[IN_DATA_BYTE + (CHAR_LEN - 1) + CHAR_LEN * i] != 0;
             i++) {
            src.message[i] = (uint8_t)packet->data[IN_DATA_BYTE + (CHAR_LEN - 1) + CHAR_LEN * i];
        }
        status = HAL_UHF_setSource(src);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case UHF_SET_MORSE: {
        UHF_configStruct morse;
        uhf_struct_len = MORSE_BEACON_MSG_LEN_MAX;
        for (i = 0; i < uhf_struct_len && packet->data[IN_DATA_BYTE + (CHAR_LEN - 1) + CHAR_LEN * i] != 0; i++) {
            morse.message[i] = (uint8_t)packet->data[IN_DATA_BYTE + (CHAR_LEN - 1) + CHAR_LEN * i];
            if (morse.message[i] == '|') {
                morse.message[i] = ' ';
            }
        }
        morse.len = i;
        status = HAL_UHF_setMorse(morse);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case UHF_SET_MIDI: {
        UHF_configStruct MIDI;
        uhf_struct_len = BEACON_MSG_LEN_MAX;
        uint8_t M_char = (uint8_t)packet->data[IN_DATA_BYTE + (CHAR_LEN - 1)];
        if (M_char != 'M') { // To get around the parser, force a letter in the start
            status = U_BAD_PARAM;
        } else { // Now parse it
            for (i = 0; i < uhf_struct_len &&
                        packet->data[IN_DATA_BYTE + (CHAR_LEN - 1) + CHAR_LEN * (i + 1)] != 0; // +1 for M_char
                 i++) {
                MIDI.message[i] =
                    (uint8_t)packet->data[IN_DATA_BYTE + (CHAR_LEN - 1) + CHAR_LEN * (i + 1)]; // +1 for M_char
            }
            if (i % SINGLE_NOTE_LEN != 0) {
                status = U_BAD_PARAM;
            } else {
                MIDI.len = i / SINGLE_NOTE_LEN;
                status = HAL_UHF_setMIDI(MIDI);
            }
        }

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case UHF_SET_BEACON_MSG: {
        UHF_configStruct beacon;
        uhf_struct_len = BEACON_MSG_LEN_MAX;
        for (i = 0; i < uhf_struct_len && packet->data[IN_DATA_BYTE + (CHAR_LEN - 1) + CHAR_LEN * i] != 0; i++) {
            beacon.message[i] = (uint8_t)packet->data[IN_DATA_BYTE + (CHAR_LEN - 1) + CHAR_LEN * i];
        }
        beacon.len = i;
        status = HAL_UHF_setBeaconMsg(beacon);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case UHF_SET_I2C: {
        uint8_t I2C_address = (uint8_t)packet->data[IN_DATA_BYTE] + 12; // Hex to Dec (22 -> 0x22)
        I2C_address = csp_ntoh32((uint32_t)I2C_address);
        status = HAL_UHF_setI2C(I2C_address);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case UHF_WRITE_FRAM: {
        UHF_framStruct U_FRAM;
        cnv8_32(&packet->data[IN_DATA_BYTE], &U_FRAM.addr);
        for (i = 0; i < FRAM_SIZE; i++) {
            U_FRAM.data[i] =
                (uint8_t)packet->data[IN_DATA_BYTE + sizeof(U_FRAM.addr) + (CHAR_LEN - 1) + CHAR_LEN * i];
        }
        status = HAL_UHF_setFRAM(U_FRAM);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case UHF_SECURE:
        confirm = (uint8_t)packet->data[IN_DATA_BYTE];
        status = HAL_UHF_secure(confirm);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;

    case UHF_GET_FULL_STAT: {
        UHF_Status U_stat;
        status = HAL_UHF_getSCW(U_stat.scw) + HAL_UHF_getFreq(&U_stat.set.freq) +
                 HAL_UHF_getUptime(&U_stat.uptime) + HAL_UHF_getPcktsOut(&U_stat.pckts_out) +
                 HAL_UHF_getPcktsIn(&U_stat.pckts_in) + HAL_UHF_getPcktsInCRC16(&U_stat.pckts_in_crc16) +
                 HAL_UHF_getPipeT(&U_stat.set.pipe_t) + HAL_UHF_getBeaconT(&U_stat.set.beacon_t) +
                 HAL_UHF_getAudioT(&U_stat.set.audio_t) + HAL_UHF_getTemp(&U_stat.temperature);

        if (sizeof(U_stat) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        U_stat.set.freq = csp_hton32(U_stat.set.freq);
        U_stat.uptime = csp_hton32(U_stat.uptime);
        U_stat.pckts_out = csp_hton32(U_stat.pckts_out);
        U_stat.pckts_in = csp_hton32(U_stat.pckts_in);
        U_stat.pckts_in_crc16 = csp_hton32(U_stat.pckts_in_crc16);
        U_stat.set.pipe_t = csp_hton32(U_stat.set.pipe_t);
        U_stat.set.beacon_t = csp_hton32(U_stat.set.beacon_t);
        U_stat.set.audio_t = csp_hton32(U_stat.set.audio_t);
        U_stat.temperature = csp_htonflt(U_stat.temperature);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &U_stat, sizeof(U_stat));
        set_packet_length(packet, sizeof(int8_t) + sizeof(U_stat) + 1);
        break;
    }

    case UHF_GET_CALL_SIGN: {
        UHF_Call_Sign U_callsign;
        status = HAL_UHF_getDestination(&U_callsign.dest) + HAL_UHF_getSource(&U_callsign.src);
        uhf_struct_len = CALLSIGN_LEN;
        // dst, src are used for parsing in case of using unicode strings
        uint8_t dst[CALLSIGN_LEN * CHAR_LEN];
        memset(dst, 0, CALLSIGN_LEN * CHAR_LEN);
        uint8_t src[CALLSIGN_LEN * CHAR_LEN];
        memset(src, 0, CALLSIGN_LEN * CHAR_LEN);
        if (sizeof(dst) + sizeof(src) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        for (i = 0; i < uhf_struct_len; i++) {
            dst[(CHAR_LEN - 1) + CHAR_LEN * i] = U_callsign.dest.message[i];
            src[(CHAR_LEN - 1) + CHAR_LEN * i] = U_callsign.src.message[i];
        }

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &dst, sizeof(dst));
        memcpy(&packet->data[OUT_DATA_BYTE + sizeof(dst)], &src, sizeof(src));
        set_packet_length(packet, sizeof(int8_t) + sizeof(dst) + sizeof(src) + 1);
        break;
    }

    case UHF_GET_MORSE: {
        UHF_configStruct morse;
        status = HAL_UHF_getMorse(&morse);
        uhf_struct_len = morse.len;
        // mrs used in case of using unicode strings
        uint8_t mrs[MORSE_BEACON_MSG_LEN_MAX * CHAR_LEN];
        memset(mrs, 0, MORSE_BEACON_MSG_LEN_MAX * CHAR_LEN);
        if (sizeof(mrs) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        for (i = 0; i < uhf_struct_len; i++) {
            mrs[(CHAR_LEN - 1) + CHAR_LEN * i] = morse.message[i];
        }

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &mrs, sizeof(mrs));
        set_packet_length(packet, sizeof(int8_t) + sizeof(mrs) + 1);

        break;
    }

    case UHF_GET_MIDI: {
        UHF_configStruct MIDI_bcn;
        status = HAL_UHF_getMIDI(&MIDI_bcn);
        uhf_struct_len = MIDI_bcn.len * SINGLE_NOTE_LEN;
        uint8_t midi[BEACON_MSG_LEN_MAX * CHAR_LEN];
        memset(midi, 0, BEACON_MSG_LEN_MAX * CHAR_LEN);
        if (sizeof(midi) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        for (i = 0; i < uhf_struct_len; i++) {
            midi[(CHAR_LEN - 1) + CHAR_LEN * i] = MIDI_bcn.message[i];
        }

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &midi, sizeof(midi));
        set_packet_length(packet, sizeof(int8_t) + sizeof(midi) + 1);

        break;
    }

    case UHF_GET_BEACON_MSG: {
        UHF_configStruct beacon_msg;
        status = HAL_UHF_getBeaconMsg(&beacon_msg);
        uhf_struct_len = beacon_msg.len;
        // Switch BEACON_MSG_LEN_MAX to MAX_W_CMDLEN after packet configuration
        uint8_t beacon[BEACON_MSG_LEN_MAX * CHAR_LEN];
        memset(beacon, 0, BEACON_MSG_LEN_MAX * CHAR_LEN);
        if (sizeof(beacon) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        for (i = 0; i < uhf_struct_len; i++) {
            beacon[(CHAR_LEN - 1) + CHAR_LEN * i] = beacon_msg.message[i];
        }

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &beacon, sizeof(beacon));
        set_packet_length(packet, sizeof(int8_t) + sizeof(beacon) + 1);

        break;
    }

    case UHF_GET_FRAM: {
        UHF_framStruct U_FRAM;
        cnv8_32(&packet->data[IN_DATA_BYTE], &U_FRAM.addr);
        status = HAL_UHF_getFRAM(&U_FRAM);
        uint8_t fram[FRAM_SIZE * CHAR_LEN];
        memset(fram, 0, FRAM_SIZE * CHAR_LEN);
        if (sizeof(fram) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        for (i = 0; i < FRAM_SIZE; i++) {
            fram[(CHAR_LEN - 1) + CHAR_LEN * i] = U_FRAM.data[i];
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &fram, sizeof(fram));
        set_packet_length(packet, sizeof(int8_t) + sizeof(fram) + 1);
        break;
    }

    case UHF_GET_SECURE_KEY: {
        uint32_t key;
        status = HAL_UHF_getSecureKey(&key);
        key = csp_hton32(key);
        if (sizeof(key) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &key, sizeof(key));
        set_packet_length(packet, sizeof(int8_t) + sizeof(key) + 1);

        break;
    }
    case UHF_GET_SWVER: {
        uint16_t version;
        status = HAL_UHF_getSWVersion(&version);
        if (sizeof(version) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &version, sizeof(version));
        set_packet_length(packet, sizeof(int8_t) + sizeof(version) + 1);

        break;
    }
    case UHF_GET_PLDSZ: {
        uint16_t payload_size;
        status = HAL_UHF_getPayload(&payload_size);
        if (sizeof(payload_size) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &payload_size, sizeof(payload_size));
        set_packet_length(packet, sizeof(int8_t) + sizeof(payload_size) + 1);

        break;
    }


    default:
        ex2_log("No such subservice\n");
        return_state = SATR_PKT_ILLEGAL_SUBSERVICE;
    }

    return return_state;
}
