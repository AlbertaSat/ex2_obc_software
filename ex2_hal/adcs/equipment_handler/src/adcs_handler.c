/*
 * Copyright (C) 2020  University of Alberta
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
 * @file adcs_commands.c
 * @author Andrew Rooney, Vasu Gupta, Arash Yazdani
 * @date 2020-08-09
 */

#include "adcs_handler.h"

#include <string.h>

#include "adcs_io.h"
#include "adcs_types.h"

#define USE_UART
//#define USE_I2C

/*************************** General functions ***************************/
/**
 * @brief
 *		send and receive acknowledgment via selected data protocol (i2c,
 *SPI, UART)
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState adcs_telecommand(uint8_t *command, uint32_t length) {
    ADCS_returnState ack = ADCS_OK;

#if defined(USE_UART)
    ack = send_uart_telecommand(command, length);
#elif defined(USE_I2C)
    ack = send_i2c_telecommand(command, length);
#endif

    return ack;
}

/**
 * @brief
 *		request telemetry and receive data via selected data protocol
 *(i2c, SPI, UART)
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState adcs_telemetry(uint8_t TM_ID, uint8_t *reply, uint32_t length) {
    ADCS_returnState ack = ADCS_OK;
#if defined(USE_UART)
    ack = request_uart_telemetry(TM_ID, reply, length);
#elif defined(USE_I2C)
    ack = request_i2c_telemetry(TM_ID, reply, length);
#endif

    return ack;
}

// To Do: We should put these functions into a new file so we can use them in
// test, too. A lot of bitwise operations in this file can be replaced with this
// function (probably with a better name!).
/**
 * @brief
 * 		append two bytes of a int16_t
 * @param b1 LSB
 * @param b2 MSB
 * @return
 * 		the appended int16_t
 */
int16_t uint82int16(uint8_t b1, uint8_t b2) {
    int16_t b;
    if (b2 >> 7) { // negative
        b = -1 * ((~((b2 << 8) | b1)) + 1);
    } else
        b = (b2 << 8) | b1; // positive
    return b;
}

/**
 * @brief
 * 		append four bytes of a int32_t
 * @param address
 * 		the address of the 4 bytes
 * @return
 * 		the appended int32_t
 */
int32_t uint82int32(uint8_t *address) {
    int32_t b;
    if (*(address + 3) >> 7) { // negative
        b = -1 * ((~((*(address + 3) << 24) | (*(address + 2) << 16) | (*(address + 1) << 8) | *address)) + 1);
    } else
        b = (*(address + 3) << 24) | (*(address + 2) << 16) | (*(address + 1) << 8) | *address; // positive
    return b;
}

/**
 * @brief
 * 		append two bytes of a uint16_t to a float
 * @param b1 LSB
 * @param b2 MSB
 * @return
 * 		the appended float
 */
uint16_t uint82uint16(uint8_t b1, uint8_t b2) { //* improve
    uint16_t b;
    b = (b2 << 8) | b1;
    return b;
}

/**
 * @brief
 * 		a supplementary function for many ACP TMs
 * @detail
 * 		Converts the correct value from telemetry bytes with the
 * coefficient factor
 * @param measurement
 * 		the measured parameter
 * @param address
 * 		the position in the telemetry frame where the data is located
 * @param coef
 * 		formatted value = rawval * coef;
 */
void get_xyz(xyz *measurement, uint8_t *address, float coef) {
    measurement->x = coef * uint82int16(*address, *(address + 1));
    measurement->y = coef * uint82int16(*(address + 2), *(address + 3));
    measurement->z = coef * uint82int16(*(address + 4), *(address + 5));
}

/**
 * @brief
 * 		a supplementary function for many ACP TMs
 * @detail
 * 		Converts the correct value from telemetry bytes
 * @param measurement
 * 		the measured parameter
 * @param address
 * 		the position in the telemetry frame where the data is located
 */
void get_xyz16(xyz16 *measurement, uint8_t *address) {
    measurement->x = uint82int16(*address, *(address + 1));
    measurement->y = uint82int16(*(address + 2), *(address + 3));
    measurement->z = uint82int16(*(address + 4), *(address + 5));
}

/**
 * @brief
 * 		A supplementary function for ADCS_get_full_config
 * @detail
 * 		Converts the correct value from telemetry bytes with the
 * coefficient factor
 * @param matrix
 * 		A 3*3 matrix
 * @param address
 * 		the position in the telemetry frame where the data is located
 * @param coef
 * 		formatted value = rawval * coef;
 */
void get_3x3(float *matrix, uint8_t *address, float coef) {
    for (int i = 0; i < 3; i++) {
        matrix[4 * i] = coef * uint82int16(*(address + 2 * i), *(address + 2 * i + 1));
    }
    for (int i = 0; i < 3; i++) {
        matrix[1 + i] = coef * uint82int16(*(address + 2 * (i + 3)), *(address + 2 * (i + 1) + 1));
        matrix[5 + i] = coef * uint82int16(*(address + 2 * (i + 6)), *(address + 2 * (i + 5) + 1));
    }
}

/*************************** Common TCs ***************************/
/**
 * @brief
 * 		Perform a reset.
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_reset(void) {
    uint8_t command[2];
    command[0] = RESET_ID;
    command[1] = ADCS_MAGIC_NUMBER; // Magic number 0x5A
    return adcs_telecommand(command, 2);
}

/**
 * @brief
 * 		Resets pointer to log buffer (from where LastLogEvent TLM is
 returned)
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_reset_log_pointer(void) {
    uint8_t command = RESET_LOG_POINTER_ID;
    return adcs_telecommand(&command, 1);
}

/**
 * @brief
 * 		Advance pointer to log buffer (from where LastLogEvent TLM is
 returned)
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_advance_log_pointer(void) {
    uint8_t command = ADVANCE_LOG_POINTER_ID;
    return adcs_telecommand(&command, 1);
}

/**
 * @brief
 * 		Resets Boot Registers
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_reset_boot_registers(void) {
    uint8_t command = RESET_BOOT_REGISTERS_ID;
    return adcs_telecommand(&command, 1);
}

/**
 * @brief
 * 		Formats SD Card
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_format_sd_card(void) {
    uint8_t command[2];
    command[0] = FORMAT_SD_CARD_ID;
    command[1] = ADCS_MAGIC_NUMBER; // magic number 0x5A
    return adcs_telecommand(command, 2);
}

/**
 * @brief
 * 		Erases file
 * @param file_type
 * 		Accepted parameters (Table 16):
 * 		telemetry log = 2
 * 		JPG image = 3
 * 		BMP image = 4
 * 		index = 15
 * @param file_counter
 * @param erase_all
 * 		if erase all
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_erase_file(uint8_t file_type, uint8_t file_counter, bool erase_all) {
    uint8_t command[4];
    command[0] = ERASE_FILE_ID;
    command[1] = file_type;
    command[2] = file_counter;
    command[3] = erase_all;
    return adcs_telecommand(command, 4);
}

/**
 * @brief
 * 		Fills download with file contents
 * @param file_type
 * 		Accepted parameters (Table 16):
 * 		telemetry log = 2
 * 		JPG image = 3
 * 		BMP image = 4
 * 		index = 15
 * @param counter
 * @param offset
 * @param block_length
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_load_file_download_block(uint8_t file_type, uint8_t counter, uint32_t offset,
                                               uint16_t block_length) {
    uint8_t command[9];
    command[0] = LOAD_FILE_DOWNLOAD_BLOCK_ID;
    command[1] = file_type;
    command[2] = counter;
    memcpy(&command[3], &offset, 4);
    command[7] = block_length & 0xFF;
    command[8] = block_length >> 8;

    return adcs_telecommand(command, 5); //* tested + (command[6] * 256 * 256 * 256 + command[5] *
                                         // 256 * 256 + command[4] * 256 + command[3])
}

/**
 * @brief
 * 		Advance File List Read Pointer
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_advance_file_list_read_pointer(void) {
    uint8_t command = ADVANCE_FILE_LIST_READ_POINTER_ID;
    return adcs_telecommand(&command, 1);
}

/**
 * @brief
 * 		Initiates File Upload
 * @param file_dest
 * 		Accepted parameters (Table 20):
 * 		EEPROM = 2
 * 		FLASH_PROGRAM_1 - FLASH_PROGRAM_7 = 3-10
 * 		SD_USER_FILE_1 - SD_USER_FILE_8 = 11-17
 * @param block_size
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_initiate_file_upload(uint8_t file_dest, uint8_t block_size) {
    uint8_t command[3];
    command[0] = INITIATE_FILE_UPLOAD_ID;
    command[1] = file_dest;
    command[2] = block_size;
    return adcs_telecommand(command, 3);
}

/**
 * @brief
 * 		File Upload Packet
 * @param packet_number
 * 		Packet Number
 * @param file_bytes
 * 		File Bytes
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_file_upload_packet(uint16_t packet_number, char *file_bytes) {
    uint8_t command[23];
    command[0] = FILE_UPLOAD_PACKET_ID;
    command[1] = packet_number & 0xFF;
    command[2] = packet_number >> 8;
    memcpy(&command[3], file_bytes, 22);
    return adcs_telecommand(command,
                            3); //* Tested. returns ascii value of the string test: +command[6]
}

/**
 * @brief
 * 		Finalizes Uploaded File Block
 * @param file_dest
 * 		File Destination
 * @param offset
 * 		Offset into file
 * @param block_length
 * 		Length of block
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_finalize_upload_block(uint8_t file_dest, uint32_t offset, uint16_t block_length) {
    uint8_t command[8];
    command[0] = FINALIZE_UPLOAD_BLOCK_ID;
    command[1] = file_dest;
    memcpy(&command[2], &offset, 4);
    command[6] = block_length & 0xFF;
    command[7] = block_length >> 8;
    return adcs_telecommand(command, 8);
}

/**
 * @brief
 * 		Resets HoleMap for Upload Block
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_reset_upload_block(void) {
    uint8_t command = RESET_UPLOAD_BLOCK_ID;
    return adcs_telecommand(&command, 1);
}

/**
 * @brief
 * 		Resets File List Read Pointer
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_reset_file_list_read_pointer(void) {
    uint8_t command = RESET_FILE_LIST_READ_POINTER_ID;
    return adcs_telecommand(&command, 1);
}

/**
 * @brief
 * 		Initiates Download Burst
 * @param msg_length
 * 		Message Length
 * @param ignore_hole_map
 * 		Ignore Hole Map
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_initiate_download_burst(uint8_t msg_length, bool ignore_hole_map) {
    uint8_t command[3];
    command[0] = INITIATE_DOWNLOAD_BURST_ID;
    command[1] = msg_length;
    command[2] = ignore_hole_map;
    return adcs_telecommand(command, 3);
}

void ADCS_receive_download_burst(uint8_t *hole_map, uint8_t *image_bytes, uint16_t length_bytes) {
#if defined(USE_UART)
    for(int i = 0; i < length_bytes/20; i++) {
        receieve_uart_packet(hole_map, image_bytes);
    }
#elif defined(USE_I2C)
    //TODO: write receive function for I2C
#endif

}

/*************************** Common TMs ***************************/
/**
 * @brief
 * 		Gets identification information for the node. Refer to table 27.
 * @param interface_ver
 * 		Should have a value of 1
 * @param time_s
 * 		number of seconds since processor start-up
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_node_identification(uint8_t *node_type, uint8_t *interface_ver, uint8_t *major_firm_ver,
                                              uint8_t *minor_firm_ver, uint16_t *runtime_s, uint16_t *runtime_ms) {
    uint8_t telemetry[8];
    ADCS_returnState state;
    state = adcs_telemetry(NODE_IDENTIFICATION_ID, telemetry, 8);
    *node_type = telemetry[0];
    *interface_ver = telemetry[1];
    *major_firm_ver = telemetry[2];
    *minor_firm_ver = telemetry[3];
    *runtime_s = (telemetry[5] << 8) | telemetry[4];  // [s]
    *runtime_ms = (telemetry[7] << 8) | telemetry[6]; // [ms]
    return state;
}

/**
 * @brief
 * 		Gets Boot And Running Program Status. Refer to table 28.
 * @param mcu_reset_cause
 * 		Possible values: 0-15. Refer to Table 29
 * @param boot_cause
 * 		Possible values: 0-5. Refer to Table 30
 * @param boot_cause
 * 		Possible values: 1,2. Refer to Table 31
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_boot_program_stat(uint8_t* mcu_reset_cause,
                                            uint8_t* boot_cause,
                                            uint16_t* boot_count,
                                            uint8_t* boot_idx,
                                            uint8_t* major_firm_ver,
                                            uint8_t* minor_firm_ver) {
  uint8_t telemetry[6];
  ADCS_returnState state;
  state = adcs_telemetry(BOOT_RUNNING_STAT, telemetry, 6);
  *mcu_reset_cause = telemetry[0] & 0xF;
  *boot_cause = (telemetry[0] >> 4) & 0xF;
  *boot_count = telemetry[2] << 8 | telemetry[1];
  *boot_idx = telemetry[3];
  *major_firm_ver = telemetry[4];
  *minor_firm_ver = telemetry[5];
  return state;
}

/**
 * @brief
 * 		Gets current selected boot index and status of last boot. Refer
 * to table 32.
 * @param program_idx
 * 		Possible values: 1,2. Refer to Table 31
 * @param boot_stat
 * 		Possible values: 0-4. Refer to Table 33
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_boot_index(uint8_t *program_idx, uint8_t *boot_stat) {
    uint8_t telemetry[2];
    ADCS_returnState state;
    state = adcs_telemetry(BOOT_IDX_STAT, telemetry, 2);
    *program_idx = telemetry[0];
    *boot_stat = telemetry[1];
    return state;
}

/**
 * @brief
 * 		Get Last Logged Event (relative to pointer - adjusted via
 * Advance and Reset TCs (3 & 4). Refer to table 34.
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_last_logged_event(uint32_t *time, uint8_t *event_id, uint8_t *event_param) {
    uint8_t telemetry[6];
    ADCS_returnState state;
    state = adcs_telemetry(LAST_LOGGED_EVENT_ID, telemetry, 6);
    *time = (telemetry[3] << 24) | (telemetry[2] << 16) | (telemetry[1] << 8) | telemetry[0];
    *event_id = telemetry[4];
    *event_param = telemetry[5];
    return state;
}

/**
 * @brief
 * 		Gets SD card format or erase progress. Refer to table 38.
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_SD_format_progress(bool *format_busy, bool *erase_all_busy) {
    uint8_t telemetry[1];
    ADCS_returnState state;
    state = adcs_telemetry(SD_FORMAT_PROGRESS, telemetry, 1);
    *format_busy = telemetry[0] & 0x1;
    *erase_all_busy = telemetry[0] & 0x2;
    return state;
}

/**
 * @brief
 * 		Gets the acknowledge status of the previously sent command.
 * Refer to table 39.
 * @param tc_err_stat
 * 		Status of last processed TC. Possible values in Table 40
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_TC_ack(uint8_t *last_tc_id, bool *tc_processed, ADCS_returnState *tc_err_stat,
                                 uint8_t *tc_err_idx) {
    uint8_t telemetry[4];
    ADCS_returnState state;
    state = adcs_telemetry(LAST_TC_ACK_ID, telemetry, 4);
    *last_tc_id = telemetry[0];
    *tc_processed = telemetry[1] & 1;
    *tc_err_stat = telemetry[2];
    *tc_err_idx = telemetry[3];
    return state;
}

/**
 * @brief
 * 		Gets file download buffer 20-byte packet.
 * @param file
 * 		A 20-byte file
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_file_download_buffer(uint16_t *packet_count, uint8_t *file[20]) {
    uint8_t telemetry[22];
    ADCS_returnState state;
    state = adcs_telemetry(FILE_DL_BUFFER_ID, telemetry, 22);
    *packet_count = (telemetry[1] << 8) | telemetry[0];
    memcpy(&file, &telemetry[2], 20);
    return state;
}

/**
 * @brief
 * 		Gets the status about download block preparation. Refer to
 * table 42.
 * @param param_err
 * 		The combination of message length and hole map resulted in
 * invalid array lengths
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_file_download_block_stat(bool *ready, bool *param_err, uint16_t *crc16_checksum,
                                                   uint16_t *length) {
    uint8_t telemetry[5];
    ADCS_returnState state;
    state = adcs_telemetry(DL_BLOCK_STAT_ID, telemetry, 5);
    *ready = telemetry[0] & 0x1;
    *param_err = telemetry[0] & 0x2;
    *crc16_checksum = (telemetry[2] << 8) + telemetry[1];
    *length = (telemetry[4] << 8) | telemetry[3];
    return state;
}

/**
 * @brief
 * 		Gets the file information. Refer to table 43.
 * @param type
 * 		File_type. Possible values: 2,3,4,15. Refer to table 16
 * @param time
 * 		File Date and Time (in MS-DOS format) [s]
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_file_info(uint8_t *type, bool *updating, uint8_t *counter, uint32_t *size,
                                    uint32_t *time, uint16_t *crc16_checksum) {
    uint8_t telemetry[12];
    ADCS_returnState state;
    state = adcs_telemetry(FILE_INFO_ID, telemetry, 12);
    *type = telemetry[0] & 0xF;
    *updating = (telemetry[0] >> 4) & 1;
    *counter = telemetry[1];
    *size = (telemetry[5] << 24) | (telemetry[4] << 16) | (telemetry[3] << 8) | telemetry[2];
    *time = (telemetry[9] << 24) | (telemetry[8] << 16) | (telemetry[7] << 8) | telemetry[6];
    *crc16_checksum = (telemetry[11] << 8) | telemetry[10];
    return state;
}

/**
 * @brief
 * 		Gets the upload initialization status.
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_init_upload_stat(bool *busy) {
    uint8_t telemetry[1];
    ADCS_returnState state;
    state = adcs_telemetry(INIT_UPLOAD_STAT_ID, telemetry, 1);
    *busy = telemetry[0] & 1;
    return state;
}

/**
 * @brief
 * 		Gets the block finalization status.
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_finalize_upload_stat(bool *busy, bool *err) {
    uint8_t telemetry[1];
    ADCS_returnState state;
    state = adcs_telemetry(FINIALIZE_UPLOAD_STAT_ID, telemetry, 1);
    *busy = telemetry[0] & 0x1;
    *busy = telemetry[0] & 0x2;
    return state;
}

/**
 * @brief
 * 		Gets the file upload Block CRC16 Checksum.
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_upload_crc16_checksum(uint16_t *checksum) {
    uint8_t telemetry[2];
    ADCS_returnState state;
    state = adcs_telemetry(UPLOAD_CRC16_ID, telemetry, 2);
    *checksum = (telemetry[1] << 8) | telemetry[0];
    return state;
}

/**
 * @brief
 * 		Gets the number of SRAM Latchups. Refer to table 35.
 * @attention
 * 		The size of TM is more than requested data
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_SRAM_latchup_count(uint16_t *sram1, uint16_t *sram2) {
    uint8_t telemetry[6];
    ADCS_returnState state;
    state = adcs_telemetry(SRAM_LATCHUP_COUNT_ID, telemetry, 6);
    *sram1 = (telemetry[1] << 8) | telemetry[0];
    *sram2 = (telemetry[3] << 8) | telemetry[2];
    return state;
}

/**
 * @brief
 * 		Gets the number of EDAC errors.
 * @details
 * 		Refer to table 36
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_EDAC_err_count(uint16_t *single_sram, uint16_t *double_sram, uint16_t *multi_sram) {
    uint8_t telemetry[6];
    ADCS_returnState state;
    state = adcs_telemetry(EDAC_ERR_COUNT_ID, telemetry, 6);
    *single_sram = (telemetry[1] << 8) | telemetry[0];
    *double_sram = (telemetry[3] << 8) | telemetry[2];
    *multi_sram = (telemetry[5] << 8) | telemetry[4];
    return state;
}

/**
 * @brief
 * 		Gets the communication status - includes TC and TM counters and
 * error. flags
 * @param flags_arr
 * 		An array of flags. Refer to table 37
 * @attention
 * 		The size of TM is more than requested data
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_comms_stat(uint16_t *TC_num, uint16_t *TM_num,
                                     uint8_t *flags_arr) {
    ADCS_returnState state;
    uint8_t telemetry[6];
    state = adcs_telemetry(COMMS_STAT_ID, telemetry, 6);
    *TC_num = (telemetry[1] << 8) | telemetry[0];
    *TM_num = (telemetry[3] << 8) | telemetry[2];
    for (int i = 0; i < 6; i++) {
        *(flags_arr + i) = (telemetry[4] >> i) & 1;
    }
    return state;
}

/************************* Common Config Msgs *************************/
/**
 * @brief
 * 		Sets the cache enabled state.
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_cache_en_state(bool en_state) {
    uint8_t command[2];
    command[0] = SET_CACHE_EN_STATE_ID;
    command[1] = en_state;
    return adcs_telecommand(command, 2);
}

/**
 * @brief
 * 		Sets the SRAM scrubbing size.
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_sram_scrub_size(uint16_t size) {
    uint8_t command[3];
    command[0] = SET_SRAM_SCRUB_PARAM_ID;
    memcpy(&command[1], &size, 2);
    return adcs_telecommand(command, 3);
}

/**
 * @brief
 * 		Sets configuration settings for Unix time flash memory
 * persistence.
 * @param when
 * 		Specifies when the time is to be saved:
 * 		1(001) : now
 * 		2(010) : on update
 * 		4(100) : periodically
 * @attention
 * 		The description implies the bools cannot be true at the same
 * time. (Table 51)
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_UnixTime_save_config(uint8_t when, uint8_t period) {
    uint8_t command[3];
    command[0] = SET_UNIX_TIME_SAVE_ID;
    command[1] = when;
    command[2] = period; // [s]
    return adcs_telecommand(command, 3);
}

/**
 * @brief
 * 		Sets File Upload Hole Map.
 * @param num
 * 		The hole map number: 1-8
 * @param hole_map
 * 		An array of 16 bytes
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_hole_map(uint8_t *hole_map, uint8_t num) {
    uint8_t command[17];
    command[0] = SET_HOLE_MAP_ID + num;
    memcpy(&command[1], hole_map, 16);
    return adcs_telecommand(command, 17); //* + command[9]. Tested
}

/**
 * @brief
 * 		Sets the current Unix Time.
 * @param unix_t
 * 		Time since 01/01/1970, 00:00. [s]
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_unix_t(uint32_t unix_t, uint16_t count_ms) {
    uint8_t command[7] = {0};
    //switch the endianness of the two input variables
    uint32_t unix_t_le = ((unix_t << 24) & 0xFF000000) | ((unix_t << 8) & 0x00FF0000) | ((unix_t >> 8) & 0x0000FF00) | ((unix_t >> 24) & 0x000000FF);
    uint16_t count_ms_le = ((count_ms << 8) & 0xFF00) | ((count_ms >> 8) & 0x00FF);
    command[0] = SET_CURRENT_UNIX_TIME;

    memcpy(&command[1], &unix_t_le, 4);
    memcpy(&command[5], &count_ms_le, 2); // [ms]

    return adcs_telecommand(command, 7);
}

/**
 * @brief
 * 		Gets the cache enabled state.
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_cache_en_state(bool *en_state) {
    uint8_t telemetry[1];
    ADCS_returnState state;
    state = adcs_telemetry(GET_CACHE_EN_STATE_ID, telemetry, 1);
    *en_state = telemetry[0] & 1;
    return state;
}

/**
 * @brief
 * 		Gets the SRAM scrubbing size.
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_sram_scrub_size(uint16_t *size) {
    uint8_t telemetry[2];
    ADCS_returnState state;
    state = adcs_telemetry(GET_SRAM_SCRUB_PARAM_ID, telemetry, 2);
    *size = (telemetry[1] >> 8) | telemetry[0];
    return state;
}

/**
 * @brief
 * 		Gets configuration settings for unix time flash memory
 * persistence.
 * @param when
 * 		Specifies when the time is to be saved:
 * 		1(001) : now
 * 		2(010) : on update
 * 		4(100) : periodically
 * @attention
 * 		The description implies the bools cannot be true at the same
 * time. (Table 51)
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_UnixTime_save_config(uint8_t *when, uint8_t *period) {
    uint8_t telemetry[2];
    ADCS_returnState state;
    state = adcs_telemetry(GET_UNIX_TIME_SAVE_ID, telemetry, 2);
    *when = telemetry[0];
    *period = telemetry[1];
    return state;
}

/**
 * @brief
 * 		Gets File Upload Hole Map.
 * @param num
 * 		The hole map number: 1-8
 * @param hole_map
 * 		An array of 16 bytes
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_hole_map(uint8_t *hole_map, uint8_t num) {
    uint8_t telemetry[16];
    ADCS_returnState state;
    uint8_t TM_ID = GET_HOLE_MAP_ID + num;
    state = adcs_telemetry(TM_ID, telemetry, 16);
    memcpy(&hole_map, &telemetry[0], 16);
    return state;
}

/**
 * @brief
 * 		Gets the current Unix Time.
 * @param unix_t
 * 		Time since 01/01/1970, 00:00. [s]
 * @return
 * 		Success of function defined in adcs_types.h
 */

//ADCS_returnState ADCS_get_unix_t(uint32_t *unix_t, uint16_t *count_ms) {
//    uint8_t telemetry[6];
//    uint8_t telemetry_little[6];
//    ADCS_returnState state;
//    //The endianness of the memory read is little-endian. This is done manually.
//    state = adcs_telemetry(GET_CURRENT_UNIX_TIME, telemetry, 6);
//
//    int i=0;
//
//    for (i=0; i<4; ++i){
//        telemetry_little[i] = telemetry[3-i];
//    }
//    for (i=4; i<6; ++i){
//        telemetry_little[i] = telemetry[5-i+4];
//    }
//
//    memcpy(unix_t, &telemetry_little[0], 4);
//    memcpy(count_ms, &telemetry_little[4], 2); // [ms]
//    return state;
//}

ADCS_returnState ADCS_get_unix_t(uint32_t *unix_t, uint16_t *count_ms) {
    ADCS_returnState state;
    uint8_t telemetry[6];
    state = adcs_telemetry(GET_CURRENT_UNIX_TIME, telemetry, 6);
    *unix_t = (telemetry[3] << 24) | (telemetry[2] << 16) | (telemetry[1] << 8) | telemetry[0];
    *count_ms = (telemetry[5] << 8) | telemetry[4];

    return state;
}

/*************************** BootLoader TCs ***************************/
/**
 * @brief
 * 		Clears the error flags.
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_clear_err_flags(void) {
    uint8_t command = CLEAR_ERR_FLAGS_ID;
    return adcs_telecommand(&command, 1);
}

/**
 * @brief
 * 		Sets the boot index.
 * @param index
 * 		one valid value: 1 = internal flash program
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_boot_index(uint8_t index) {
    uint8_t command[2];
    command[0] = SET_BOOT_INDEX_ID;
    if (index != 1) {
        return ADCS_INVALID_PARAMETERS;
    } else {
        command[1] = 1;
        return adcs_telecommand(command, 2);
    }
}

/**
 * @brief
 * 		Runs the selected program.
 * @attention
 * 		could be merged with the previous one in services
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_run_selected_program(void) {
    uint8_t command = RUN_SELECTED_PROGRAM_ID;
    return adcs_telecommand(&command, 1);
}

/**
 * @brief
 * 		Reads the program information.
 * @param index
 * 		Table 66 - Program index
 * 		0 : BootLoader, 1 : Internal flash program, 2 : EEPROM,
 * 		3-9: External flash program 1-7,
 * 		10-17: SD user file 1-8
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_read_program_info(uint8_t index) {
    uint8_t command[2];
    command[0] = READ_PROGRAM_INFO_ID;
    if (index < 0 || index > 18) {
        return ADCS_INVALID_PARAMETERS;
    } else {
        command[1] = index;
        return adcs_telecommand(command, 2);
    }
}

/**
 * @brief
 * 		Copies the program to internal flash.
 * @param index
 * 		Table 66 - Source Program index
 * 		0 : BootLoader, 1 : Internal flash program, 2 : EEPROM,
 * 3-9: External flash program 1-7, 10-17: SD user file 1-8
 * //* not good for services
 * @attention
 * 		flag = 0x5A overwrites the boot segment
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_copy_program_internal_flash(uint8_t index, uint8_t overwrite_flag) {
    uint8_t command[3];
    command[0] = COPY_PROGRAM_INTERNAL_FLASH_ID;
    if (index < 0 || index > 18) {
        return ADCS_INVALID_PARAMETERS;
    } else {
        command[1] = index;
        command[2] = overwrite_flag;
        return adcs_telecommand(command, 3);
    }
}

/*************************** BootLoader TMs ***************************/

/**
 * @brief
 * 		Gets the BootLoader state.
 * @param uptime
 * @param flags_arr
 * 		12 boolean flags from Table 69
 * 		sram1, sram2, sram_latch_not_recovered, sram_latch_recovered,
 * 		sd_initial_err, sd_read_err, sd_write_err, external_flash_err,
 * 		internal_flash_err, eeprom_err, bad_boot_reg, comms_radio_err
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_bootloader_state(uint16_t* uptime,
                                           uint8_t* flags_arr) {
  uint8_t telemetry[6];
  ADCS_returnState state;
  state = adcs_telemetry(GET_BOOTLOADER_STATE_ID, telemetry, 6);
  *uptime = (telemetry[1] << 8) + telemetry[0];
  uint32_t flags;
  memcpy(&flags, &telemetry[2], 4);
  for (int i = 0; i < 12; i++) {
    *(flags_arr + i) = (flags >> i) & 1;
  }
  return state;
}

/**
 * @brief
 * 		Gets the program information. Refer to table 70.
 * @param index
 * 		Table 66 - Source Program index
 * @param busy
 * 		program busy reading
 * @param file_size
 * 		File size in bytes
 * @param crc16_checksum
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_program_info(uint8_t *index, bool *busy, uint32_t *file_size, uint16_t *crc16_checksum) {
    uint8_t telemetry[8];
    ADCS_returnState state;
    state = adcs_telemetry(GET_PROGRAM_INFO_ID, telemetry, 8);
    *index = telemetry[0];
    *busy = telemetry[1] & 1;
    *file_size = (telemetry[5] << 24) + (telemetry[4] << 16) + (telemetry[3] << 8) + telemetry[2];
    *crc16_checksum = (telemetry[7] << 8) + telemetry[6];
    return state;
}

/**
 * @brief
 * 		Gets the progress of copy to internal flash operation
 * @param busy
 * 		busy copying
 * @param err
 * 		error in copying
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_copy_internal_flash_progress(bool *busy, bool *err) {
    uint8_t telemetry;
    ADCS_returnState state;
    state = adcs_telemetry(COPY_INTERNAL_FLASH_PROGRESS_ID, &telemetry, 1);
    *busy = telemetry & 1;
    *err = telemetry & 2;
    return state;
}

/*************************** ACP TCs ***************************/
/**
 * @brief
 * 		Deploy MTM boom (sets the deployment actuation timeout value).
 * @param actuation_timeout
 * 		in seconds
 * @attention
 * 		uses a TC_ID that has been used before!
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_deploy_magnetometer_boom(uint8_t actuation_timeout) {
    uint8_t command[2];
    command[0] = DEPLOY_MAGNETOMETER_BOOM_ID;
    command[1] = actuation_timeout;
    return adcs_telecommand(command, 2);
}

/**
 * @brief
 * 		Sets the enabled state and control loop behavior
 * @param state : ADCS loop state
 * 		0 : inactive
 * 		1 : 1Hz loop active
 * 		2 : execute when triggered
 * 		3 : in simulation mode
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_enabled_state(uint8_t state) {
    uint8_t command[2];
    command[0] = ADCS_RUN_MODE_ID;
    command[1] = state;
    return adcs_telecommand(command, 2);
}

/**
 * @brief
 * 		Clears latched error flags.
 * @param adcs_flag: ADCS error flags
 * @param hk_flag: Housekeeping error flags
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_clear_latched_errs(bool adcs_flag, bool hk_flag) {
    uint8_t command[2];
    command[0] = CLEAR_LATCHED_ERRS_ID;
    command[1] = adcs_flag + 2 * hk_flag;
    return adcs_telecommand(command,
                            2); //* add "+ command[1]" for test and let it fail and check the value
}

/**
 * @brief
 * 		Sets the attitude control mode.
 * @param ctrl_mode: 0-15: Refer to Table 78 in the manual
 * @param timeout: control timeout duration in seconds (0XFFFF: infinity)
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_attitude_ctrl_mode(uint8_t ctrl_mode, uint16_t timeout) {
    uint8_t command[4];
    command[0] = SET_ATT_CONTROL_MODE_ID;
    command[1] = ctrl_mode;
    command[2] = timeout & 0xFF;
    command[3] = timeout >> 8;
    return adcs_telecommand(command, 4); //*  + (256*command[3]+command[2])
}

/**
 * @brief
 * 		Sets the attitude estimation mode.
 * @param mode: 0-7 : Refer to Table 80 in the manual
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_attitude_estimate_mode(uint8_t mode) {
    uint8_t command[2];
    command[0] = SET_ATT_ESTIMATE_MODE_ID;
    command[1] = mode;
    return adcs_telecommand(command, 2);
}

/**
 * @brief
 * 		Triggers the ADCS loop.
 * 	@attention
 * 		ADCS_set_enabled_state(2) must have been called in order for
 * this to operate
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_trigger_adcs_loop(void) {
    uint8_t command = TRIGGER_ADCS_LOOP_ID;
    return adcs_telecommand(&command, 1);
}

/**
 * @brief
 * 		Triggers the ADCS loop with simulated sensor data.
 * 	@parameter sim_data
 * 		127 bytes of data. Refer to table 84
 * 	@attention
 * 		ADCS_set_enabled_state(2) must have been called in order for
 * this to operate
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_trigger_adcs_loop_sim(sim_sensor_data sim_data) {
    uint8_t command[128];
    command[0] = TRIGGER_ADCS_LOOP_SIM_ID;
    memcpy(&command[1], &sim_data, 121);
    command[122] = sim_data.pos_std_dev.x * 10;
    command[123] = sim_data.pos_std_dev.y * 10;
    command[124] = sim_data.pos_std_dev.z * 10;
    memcpy(&command[124], &sim_data.vel_std_dev, 3);
    return adcs_telecommand(command, 128);
    /* test cases:
     + command[16] >> 8 | command[15]
     + command[123]
     */
}

/**
 * @brief
 * 		Sets the ASGP4 run mode.
 * @param mode (Table 87)
 * 		0 : off
 * 		1 : waiting for trigger
 * 		2 : background
 * 		3 : augment
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_ASGP4_rune_mode(uint8_t mode) {
    uint8_t command[2];
    command[0] = ASGP4_RUN_MODE_ID;
    command[1] = mode;
    return adcs_telecommand(command, 2);
}

/**
 * @brief
 * 		Triggers start of the ASGP4 process.
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_trigger_ASGP4(void) {
    uint8_t command = ASGP4_TRIGGER_ID;
    return adcs_telecommand(&command, 1);
}

/**
 * @brief
 * 		Sets the MTM operation mode.
 * @param mode (Table 90)
 * 		0 : Main MTM through signal
 * 		1 : Redundant MTM through signal
 * 		2 : Main MTM through motor
 * 		3 : None
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_MTM_op_mode(uint8_t mode) {
    uint8_t command[2];
    command[0] = SET_MTM_OP_MODE_ID;
    command[1] = mode;
    return adcs_telecommand(command, 2);
}

/**
 * @brief
 * 		Converts raw or bmp files to JPG file.
 * @param source
 * 		source file counter
 * @param QF
 * 		quality factor
 * @param white_balance
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_cnv2jpg(uint8_t source, uint8_t QF, uint8_t white_balance) {
    uint8_t command[4];
    command[0] = CNV2JPG_ID;
    command[1] = source;
    command[1] = QF;
    command[1] = white_balance;
    return adcs_telecommand(command, 4);
}

/**
 * @brief
 * 		Saves image from one of the cameras to SD.
 * @param camera
 * 		Camera selection:
 * 		0 : CubeSense Cam1
 * 		1 : CubeSense Cam2
 * 		2 : CubeStar
 * @param img_size
 * 		0 : 1024 * 1024
 * 		1 : 512*512
 * 		2 : 256*256
 * 		3 : 128*128
 * 		4 : 64*64
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_save_img(uint8_t camera, uint8_t img_size) {
    uint8_t command[3];
    command[0] = SAVE_IMG_ID;
    command[1] = camera;
    command[2] = img_size;
    return adcs_telecommand(command, 3);
}

/**
 * @brief
 * 		Sets MTQ output.
 * @attention
 * 		ADCS_set_attitude_ctrl_mode(0, ) must be called. (Control mode
 * None)
 * @attention
 * 		If using the raw value, perform /1000.
 * @param x,y,z
 * 		Commanded x,y,z-torquer duty cycle
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_magnetorquer_output(xyz16 duty_cycle) {
    uint8_t command[7];
    command[0] = SET_MAGNETORQUER_OUTPUT_ID;
    memcpy(&command[1], &duty_cycle, 6);
    return adcs_telecommand(command, 7); //* + (256*command[6] + command[5])
}

/**
 * @brief
 * 		Sets the wheel speed.
 * @attention
 * 		ADCS_set_attitude_ctrl_mode(0, ) must be called. (Control mode
 * None)
 * @param x,y,z
 * 		Commanded x,y,z-wheel speed in rpm
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_wheel_speed(xyz16 speed) {
    uint8_t command[7];
    command[0] = SET_WHEEL_SPEED_ID;
    memcpy(&command[1], &speed, 6);
    return adcs_telecommand(command, 7);
}

/**
 * @brief
 * 		Saves the current configuration to flash memory.
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_save_config(void) {
    uint8_t command = SAVE_CONFIG_ID;
    return adcs_telecommand(&command, 1);
}

/**
 * @brief
 * 		Saves the current orbit parameters to flash memory.
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_save_orbit_params(void) {
    uint8_t command = SAVE_ORBIT_PARAMS;
    return adcs_telecommand(&command, 1);
}

/*************************** ACP TMs ***************************/
/************************* ADCS State **************************/
/**
 * @brief
 * 		Gets ADCS current full state.
 * @param data
 * 		A struct of floats defined in adcs_handler.h
 * 		Refer to table 149
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_current_state(adcs_state *data) {
    uint8_t telemetry[54];
    ADCS_returnState state;
    state = adcs_telemetry(ADCS_STATE, telemetry, 54);
    data->att_estimate_mode = telemetry[0] & 0xF;    // Refer to table 80
    data->att_ctrl_mode = (telemetry[0] >> 4) & 0xF; // Refer to table 78
    data->run_mode = telemetry[1] & 0x3;             // Refer to table 75
    data->ASGP4_mode = (telemetry[1] >> 2) & 0x3;    // Refer to table 87
    for (int i = 0; i < 4; i++) {
        *(data->flags_arr + i) = (telemetry[1] >> (i + 4)) & 1;
    }
    uint32_t flags1;
    memcpy(&flags1, &telemetry[2], 4);
    for (int i = 0; i < 32; i++) {
        *(data->flags_arr + i + 4) = (flags1 >> i) & 1;
    }
    uint32_t flags2;
    memcpy(&flags2, &telemetry[6], 4);
    for (int i = 0; i < 3; i++) {
        *(data->flags_arr + i + 36) = (flags2 >> i) & 1;
    }
    // position:52
    data->MTM_sample_mode = (flags2 >> 4) & 0x3; // Refer to table 90
    for (int i = 6; i < 18; i++) {
        *(data->flags_arr + i + 34) = (flags2 >> i) & 1; // 34 because 2 were for sample mode
    }
    get_xyz(&data->est_angle, &telemetry[12], 0.01); // [deg]
    get_xyz16(&data->est_quaternion, &telemetry[18]);
    get_xyz(&data->est_angular_rate, &telemetry[24], 0.01); // [deg/s]
    get_xyz(&data->ECI_pos, &telemetry[30], 0.25);          // [km]
    get_xyz(&data->ECI_vel, &telemetry[36], 0.25);          // [m/s]
    get_xyz(&data->longlatalt, &telemetry[42], 0.01);       // [deg, deg, km]
    if (telemetry[47] >> 7) {                               // since it is uint16 and has been treated as int16
        data->longlatalt.z = 0.01 * (telemetry[47] << 8 | telemetry[46]);
    }
    get_xyz16(&data->ecef_pos, &telemetry[48]); // [m]
    return state;
}

/************************* General **************************/
/**
 * @brief
 * 		Gets JPG conversion progress.
 * @param percentage
 * 		progress in percentage
 * @param result
 * 		conversion result:
 * 		0 : nothing converted
 * 		1 : success
 * 		2 : file load error
 * 		3 : busy
 * @param file_counter
 * 		Output file counter
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_jpg_cnv_progress(uint8_t *percentage, uint8_t *result, uint8_t *file_counter) {
    uint8_t telemetry[3];
    ADCS_returnState state;
    state = adcs_telemetry(JPG_CNV_PROGRESS_ID, telemetry, 3);
    *percentage = telemetry[0];
    *result = telemetry[1];
    *file_counter = telemetry[2];
    return state;
}

/**
 * @brief
 * 		Gets flags regarding CubeACP state.
 * @param flags_arr
 * 		6 boolean flags from Table 101
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_cubeACP_state(uint8_t *flags_arr) {
    uint8_t telemetry;
    ADCS_returnState state;
    state = adcs_telemetry(CUBEACP_STATE_FLAGS_ID, &telemetry, 1);
    for (int i = 0; i < 6; i++) {
        *(flags_arr + i) = (telemetry >> i) & 1;
    }
    return state;
}

/**
 * @brief
 * 		Gets the satellite position in WGS-84 coordinate (Table 106).
 * @param target
 * 		longitude, latitude, angle
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_sat_pos_LLH(xyz *target) {
    uint8_t telemetry[6];
    ADCS_returnState state;
    state = adcs_telemetry(SATELLITE_POSITION_LLH_ID, telemetry, 6);
    memcpy(&target->x, &telemetry[0], 2);
    memcpy(&target->y, &telemetry[2], 2);
    memcpy(&target->z, &telemetry[4], 2);
    return state;
}

/**
 * @brief
 * 		Gets the execution times of ACP functions.
 * @param adcs_update
 * 		time to perform complete adcs update [ms]
 * @param sensor_comms
 * 		time to perform sensor/actuator communications [ms]
 * @param sgp4_propag
 * 		time to perform SGP4 propagator [ms]
 * @param igrf_model
 * 		time to execute IGRF computation [ms]
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_execution_times(uint16_t *adcs_update, uint16_t *sensor_comms, uint16_t *sgp4_propag,
                                          uint16_t *igrf_model) {
    uint8_t telemetry[8];
    ADCS_returnState state;
    state = adcs_telemetry(ADCS_EXE_TIMES_ID, telemetry, 8);
    *adcs_update = (telemetry[1] << 8) + telemetry[0];
    *sensor_comms = (telemetry[3] << 8) + telemetry[2];
    *sgp4_propag = (telemetry[5] << 8) + telemetry[4];
    *igrf_model = (telemetry[7] << 8) + telemetry[6];
    return state;
}

/**
 * @brief
 * 		Returns the information about the ACP loop.
 * @param time
 * 		 Time since the start of the current loop iteration [ms]
 * @param execution_point
 * 		Indicates which part of the loop is currently executing
 * 		13 possible values. Refer to Table 168
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_ACP_loop_stat(uint16_t *time, uint8_t *execution_point) {
    uint8_t telemetry[3];
    ADCS_returnState state;
    state = adcs_telemetry(ACP_EXE_STATE_ID, telemetry, 3);
    *time = (telemetry[1] << 8) + telemetry[0];
    *execution_point = telemetry[2];
    return state;
}

/**
 * @brief
 * 		Gets the status of Image Capture and Save Operation.
 * @param percentage
 * 		progress in percentage
 * @param status
 * 		conversion result:
 * 		0 : no error
 * 		1 : timeout waiting for sensor
 * 		2 : timeout waiting for frame
 * 		3 : checksum mismatch between downloaded and unit frame
 * 		4 : error writing to SD
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_img_save_progress(uint8_t *percentage, uint8_t *status) {
    uint8_t telemetry[2];
    ADCS_returnState state;
    state = adcs_telemetry(IMG_CAPTURE_SAVE_OP_STAT, telemetry, 2);
    *percentage = telemetry[0];
    *status = telemetry[1];
    return state;
}

/*********************** ADCS Measurement ************************/

/**
 * @brief
 * 		Gets the calibrated sensor measurements.
 * @param measurements
 * 		A struct of floats defined in adcs_handler.h
 * 		Refer to table 150
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_measurements(adcs_measures *measurements) {
    uint8_t telemetry[72];
    ADCS_returnState state;
    state = adcs_telemetry(ADCS_MEASUREMENTS_ID, telemetry, 72);
    get_xyz(&measurements->magnetic_field, &telemetry[0], 0.01); // [uT]
    get_xyz(&measurements->coarse_sun, &telemetry[6], 0.0001);
    get_xyz(&measurements->sun, &telemetry[12], 0.0001);
    get_xyz(&measurements->nadir, &telemetry[18], 0.0001);
    get_xyz(&measurements->angular_rate, &telemetry[24], 0.01); // [deg/s]
    get_xyz(&measurements->wheel_speed, &telemetry[30], 1);     // [rpm]
    get_xyz(&measurements->star1b, &telemetry[36], 0.0001);
    get_xyz(&measurements->star1o, &telemetry[42], 0.0001);
    get_xyz(&measurements->star2b, &telemetry[48], 0.0001);
    get_xyz(&measurements->star2o, &telemetry[54], 0.0001);
    get_xyz(&measurements->star3b, &telemetry[60], 0.0001);
    get_xyz(&measurements->star3o, &telemetry[66], 0.0001);
    return state;
}

/*********************** ADCS Actuator ************************/
/**
 * @brief
 * 		Gets the actuator commands.
 * @param measurements
 * 		A struct of floats defined in adcs_handler.h
 * 		Refer to table 151
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_actuator(adcs_actuator *commands) {
    uint8_t telemetry[12];
    ADCS_returnState state;
    state = adcs_telemetry(ACTUATOR_ID, telemetry, 12);
    get_xyz(&commands->magnetorquer, &telemetry[0], 100); // [s]
    get_xyz(&commands->wheel_speed, &telemetry[6], 1);    // [rpm]
    return state;
}

/*********************** ADCS Estimation ************************/
/**
 * @brief
 * 		Gets the estimation meta-data.
 * @param data
 * 		A struct of floats defined in adcs_handler.h
 * 		Refer to table 152
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_estimation(adcs_estimate *data) {
    uint8_t telemetry[42];
    ADCS_returnState state;
    state = adcs_telemetry(ESTIMATION_ID, telemetry, 42);
    get_xyz(&data->igrf_magnetic_field, &telemetry[0], 0.01); // [uT]
    get_xyz(&data->sun, &telemetry[6], 0.0001);
    get_xyz(&data->gyro_bias, &telemetry[12], 0.001); // [deg/s]
    get_xyz(&data->innovation, &telemetry[18], 0.0001);
    get_xyz(&data->quaternion_err, &telemetry[24], 0.0001);
    get_xyz(&data->quaternion_covar, &telemetry[30], 0.001);
    get_xyz(&data->angular_rate_covar, &telemetry[36], 0.001);
    return state;
}

/**
 * @brief
 * 		Gets the ASGP4 TLEs.
 * @param complete
 * 		ASGP4 process complete
 * @param err
 * 		The error state that ASGP4 module is in (Table 171)
 * 		0 : no error
 * 		1 : timeout
 * 		2 : position error exceeding Th
 * 		3 : overflow
 * @param asgp4
 * 		A struct of floats defined in adcs_handler.h
 * 		Refer to table 170
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_ASGP4(bool *complete, uint8_t *err, adcs_asgp4 *asgp4) {
    uint8_t telemetry[33];
    ADCS_returnState state;
    state = adcs_telemetry(ASGP4_TLEs_ID, telemetry, 33);
    *complete = telemetry[0] & 1;
    *err = telemetry[0] >> 1;
    memcpy(&asgp4->epoch, &telemetry[1], 4);
    memcpy(&asgp4->inclination, &telemetry[5], 4);
    memcpy(&asgp4->RAAN, &telemetry[9], 4);
    memcpy(&asgp4->ECC, &telemetry[13], 4);
    memcpy(&asgp4->AOP, &telemetry[17], 4);
    memcpy(&asgp4->MA, &telemetry[21], 4);
    memcpy(&asgp4->MM, &telemetry[25], 4);
    memcpy(&asgp4->Bstar, &telemetry[29], 4);
    return state;
}

/********************* ADCS Raw Sensor Measurements **********************/
/**
 * @brief
 * 		Camera sensor capture and detection result
 * @param centroid_x
 * 		azimuth angle
 * @param centroid_y
 * 		elevation angle
 * @param capture_stat
 * 		Camera capture status: 0-5 Refer to table 123
 * @param detect_result
 * 		Camera detection result: 0-7 Refer to table 124
 * @param address
 * 		the position in the telemetry frame where the data is located
 */
void get_cam_sensor(cam_sensor *cam, uint8_t *address) {
    cam->centroid_x = uint82int16(*address, *(address + 1));
    cam->centroid_y = uint82int16(*(address + 2), *(address + 3));
    cam->capture_stat = *(address + 4);
    cam->detect_result = *(address + 5);
}

/**
 * @brief
 * 		Gets the raw sensor measurements.
 * @param measurements
 * 		A struct of floats defined in adcs_handler.h
 * 		Refer to table 153
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_raw_sensor(adcs_raw_sensor *measurements) {
    uint8_t telemetry[34];
    ADCS_returnState state;
    state = adcs_telemetry(RAW_SENSOR_MEASUREMENTS_ID, telemetry, 34);
    get_cam_sensor(&measurements->cam2, &telemetry[0]);
    get_cam_sensor(&measurements->cam1, &telemetry[6]);
    for (int i = 0; i < 10; i++) {
        *(measurements->css + i) = telemetry[i + 12];
    }
    get_xyz16(&measurements->MTM, &telemetry[22]);
    get_xyz16(&measurements->rate, &telemetry[28]);
    return state;
}

/**
 * @brief
 * 		Camera sensor capture and detection result
 * @param pos
 * 		ECEF position [m]
 * @param vel
 * 		ECEF velocity [m/s]
 * @param address
 * 		the position in the telemetry frame where the data is located
 */
void get_ecef(ecef *coordinate, uint8_t *address) {
    coordinate->pos = uint82int32(address);
    coordinate->vel = uint82int16(*(address + 4), *(address + 5));
}

/**
 * @brief
 * 		Gets the raw GPS measurements.
 * @param measurements
 * 		A struct of floats defined in adcs_handler.h
 * 		Refer to table 158
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_raw_GPS(adcs_raw_gps *measurements) {
    uint8_t telemetry[36];
    ADCS_returnState state;
    state = adcs_telemetry(RAW_GPS_MEASUREMENTS_ID, telemetry, 36);
    measurements->sol_stat = telemetry[0];
    measurements->tracked_sats = telemetry[1];
    measurements->usedInSol_sats = telemetry[2];
    measurements->xyz_lof_count = telemetry[3];
    measurements->range_log_count = telemetry[4];
    measurements->response_msg = telemetry[5];
    measurements->reference_week = telemetry[7] << 8 | telemetry[6];
    measurements->time = telemetry[11] << 24 | telemetry[10] << 16 | telemetry[9] << 8 | telemetry[8];
    get_ecef(&measurements->x, &telemetry[12]);
    get_ecef(&measurements->y, &telemetry[18]);
    get_ecef(&measurements->z, &telemetry[24]);
    measurements->pos_std_dev.x = telemetry[30] * 0.1;
    measurements->pos_std_dev.y = telemetry[31] * 0.1;
    measurements->pos_std_dev.z = telemetry[32] * 0.1;
    measurements->vel_std_dev.x = telemetry[33];
    measurements->vel_std_dev.y = telemetry[34];
    measurements->vel_std_dev.z = telemetry[35];
    return state;
}

/**
 * @brief
 * 		Gets raw data for stars.
 * @param address
 * 		the position in the telemetry frame where the data is located
 * @param i
 * 		star index
 */
void get_star_data(star_data *coordinate, uint8_t *address, uint8_t i) {
    coordinate->confidence = *(address + i); // percent
    coordinate->magnitude = *(address + 4 + 2 * i) << 8 | *(address + 3 + 2 * i);
    coordinate->catalouge_num = *(address + 10 + 6 * i) << 8 | *(address + 9 + 6 * i);
    if ((*(address + 12 + 6 * i)) >> 7) {
        coordinate->centroid_x = -1 * (~(*(address + 12 + 6 * i) << 8 | *(address + 11 + 6 * i)) + 1);
    } else {
        coordinate->centroid_x = *(address + 12 + 6 * i) << 8 | *(address + 11 + 6 * i);
    }
    if ((*(address + 14 + 6 * i)) >> 7) {
        coordinate->centroid_y = -1 * (~(*(address + 14 + 6 * i) << 8 | *(address + 13 + 6 * i)) + 1);
    } else {
        coordinate->centroid_y = *(address + 14 + 6 * i) << 8 | *(address + 13 + 6 * i);
    }
}

/**
 * @brief
 * 		Gets the raw star tracker measurements.
 * @param measurements
 * 		A struct of floats defined in adcs_handler.h
 * 		Refer to table 159
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_star_tracker(adcs_star_track *measurements) {
    uint8_t telemetry[54];
    ADCS_returnState state;
    state = adcs_telemetry(RAW_STAR_TRACKER_ID, telemetry, 54);
    measurements->detected_stars = telemetry[0];
    measurements->img_noise = telemetry[1];
    measurements->invalid_stars = telemetry[2];
    measurements->identified_stars = telemetry[3];
    measurements->identification_mode = telemetry[4];
    measurements->img_dark_val = telemetry[5];
    for (int i = 0; i < 8; i++) {
        *(measurements->flags_arr + i) = (telemetry[6] >> i) & 1;
    }
    measurements->sample_T = telemetry[8] << 8 | telemetry[7];
    get_star_data(&measurements->star1, &telemetry[9], 0);
    get_star_data(&measurements->star2, &telemetry[9], 1);
    get_star_data(&measurements->star3, &telemetry[9], 2);
    measurements->capture_t = telemetry[37] << 8 | telemetry[36];        // [ms]
    measurements->detect_t = telemetry[39] << 8 | telemetry[38];         // [ms]
    measurements->identification_t = telemetry[41] << 8 | telemetry[40]; // [ms]
    get_xyz(&measurements->estimated_rate, &telemetry[42], 0.0001);
    get_xyz(&measurements->estimated_att, &telemetry[48], 0.0001);
    return state;
}

/**
 * @brief
 * 		Gets the secondary Magnetometer raw measurements.
 * @param Mag
 * 		2nd MTM sampled A/D value
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_MTM2_measurements(xyz16 *Mag) {
    uint8_t telemetry[6];
    ADCS_returnState state;
    state = adcs_telemetry(MTM2_MEASUREMENTS_ID, telemetry, 6);
    get_xyz16(Mag, &telemetry[0]);
    return state;
}

/******************* ADCS Power & Temperature ********************/
/**
 * @brief
 * 		A supplementary function for the ADCS power measurements
 * @param measurement
 * 		the measured parameter (Table 154)
 * @param address
 * 		the position in the telemetry frame where the data is located
 * @param coef
 * 		formatted value = rawval * coef;
 */
void get_current(float *measurement, uint8_t *address, float coef) {
    *measurement = coef * uint82uint16(*address, *(address+1));
}

/**
 * @brief
 * 		A supplementary function for the ADCS temperature measurements
 * @param measurement
 * 		the measured parameter (Table 154)
 * @param address
 * 		the position in the telemetry frame where the data is located
 * @param coef
 * 		formatted value = rawval * coef;
 */
void get_temp(float *measurement, uint8_t *address, float coef) {
    *measurement = coef * uint82int16(*address, *(address + 1));
}

/**
 * @brief
 * 		Gets the Power & Temperature measurements.
 * @param measurements
 * 		A struct of floats defined in adcs_handler.h
 * 		Refer to table 154
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_power_temp(adcs_pwr_temp *measurements) {
    uint8_t telemetry[38];
    ADCS_returnState state;
    state = adcs_telemetry(POWER_TEMP_ID, telemetry, 38);

    get_current(&measurements->cubesense1_3v3_I, &telemetry[0], 0.1);     // [mA]
    get_current(&measurements->cubesense1_camSram_I, &telemetry[2], 0.1); // [mA]
    get_current(&measurements->cubesense2_3v3_I, &telemetry[4], 0.1);     // [mA]
    get_current(&measurements->cubesense2_camSram_I, &telemetry[6], 0.1); // [mA]
    get_current(&measurements->cubecontrol_3v3_I, &telemetry[8],
                0.48828125); // [mA]
    get_current(&measurements->cubecontrol_5v_I, &telemetry[10],
                0.48828125); // [mA]
    get_current(&measurements->cubecontrol_vBat_I, &telemetry[12],
                0.48828125);                                         // [mA]
    get_current(&measurements->wheel1_I, &telemetry[14], 0.01);      // [mA]
    get_current(&measurements->wheel2_I, &telemetry[16], 0.01);      // [mA]
    get_current(&measurements->wheel3_I, &telemetry[18], 0.01);      // [mA]
    get_current(&measurements->cubestar_I, &telemetry[20], 0.01);    // [mA]
    get_current(&measurements->magnetorquer_I, &telemetry[20], 0.1); // [mA]

    get_temp(&measurements->cubestar_temp, &telemetry[22], 0.01); // [C]
    get_temp(&measurements->MCU_temp, &telemetry[24], 1);         // [C]
    get_temp(&measurements->MTM_temp, &telemetry[26], 0.1);       // [C]
    get_temp(&measurements->MTM2_temp, &telemetry[28], 0.1);      // [C]
    get_xyz16(&measurements->rate_sensor_temp, &telemetry[30]);   // [C]

    return state;
}

/************************* ACP Config Msgs *************************/
/***************************** General *****************************/
/**
 * @brief
 * 		Controls the power state of some components (Table 184).
 * @param control
 * 		an array with the values defined in Table 185:
 * 		0 : off
 * 		1 : on
 * 		2 : keep the same
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_power_control(uint8_t *control) {
    uint8_t command[4] = {0}; //TODO: FIX power control setting bytes. Right now it only works for cubesense 1
    command[0] = SET_POWER_CONTROL_ID;
    //command[1] = 0x10; //sets the cubesense 1 on
    for (int i = 0; i < 4; i++) {
        command[1] = command[1] | (*(control + i) << 2*i);
    }
    for (int i = 0; i < 4; i++) {
        command[2] = command[2] | (*(control + 4 + i) << 2*i);
    }
    for (int i = 0; i < 2; i++) {
        command[3] = command[3] | (*(control + 8 + i) << 2*i);
    }
    return adcs_telecommand(command, 4); //* + command[1] and  + command[3]. Tested
}

/**
 * @brief
 * 		Gets the power state of some components (Table 184).
 * @param control
 * 		a 10-value array with the values defined in Table 185:
 * 		0 : off
 * 		1 : on
 * 		2 : keep the same
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_power_control(uint8_t *control) {
    uint8_t telemetry[3];
    ADCS_returnState state;
    state = adcs_telemetry(GET_POWER_CONTROL_ID, telemetry, 3);

    for (int i = 0; i < 4; i++) {
        *(control + i) = (telemetry[0] >> 2 * i) & 0x3;
    }
    for (int i = 0; i < 4; i++) {
        *(control + i + 4) = (telemetry[1] >> 2 * i) & 0x3;
    }
    for (int i = 0; i < 2; i++) {
        *(control + i + 8) = (telemetry[2] >> 2 * i) & 0x3;
    }
    return state;
}

/**
 * @brief
 * 		Sets the commanded attitude angles (Table 186).
 * @param att_angle
 * 		roll, pitch, yaw angle
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_attitude_angle(xyz att_angle) {
    uint8_t command[7];
    command[0] = SET_ATT_ANGLE_ID;
    float coef = 0.01;
    xyz16 raw_val;
    raw_val.x = att_angle.x / coef;
    raw_val.y = att_angle.y / coef;
    raw_val.z = att_angle.z / coef;
    memcpy(&command[1], &raw_val, 6);
    return adcs_telecommand(command,
                            7); //* Tested. + (command[2]<<8 | command[1])
}

/**
 * @brief
 * 		Gets the commanded attitude angles (Table 186).
 * @param att_angle
 * 		roll, pitch, yaw angle
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_attitude_angle(xyz *att_angle) {
    uint8_t telemetry[6];
    ADCS_returnState state;
    state = adcs_telemetry(GET_ATT_ANGLE_ID, telemetry, 6);
    float coef = 0.01;
    get_xyz(att_angle, &telemetry[0], coef);
    return state;
}

/**
 * @brief
 * 		Sets the target reference for tracking control mode (Table 187).
 * @param target
 * 		longitude, latitude, angle
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_track_controller(xyz target) {
    uint8_t command[13];
    command[0] = SET_TRACK_CTRLER_TARGET_REF_ID;
    memcpy(&command[1], &target, 6);
    return adcs_telecommand(command, 13);
}

/**
 * @brief
 * 		Gets the target reference for tracking control mode (Table 187).
 * @param target
 * 		longitude, latitude, angle
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_track_controller(xyz *target) {
    uint8_t telemetry[12];
    ADCS_returnState state;
    state = adcs_telemetry(GET_TRACK_CTRLER_TARGET_REF_ID, telemetry, 12);
    memcpy(&target->x, &telemetry[0], 4);
    memcpy(&target->y, &telemetry[4], 4);
    memcpy(&target->z, &telemetry[8], 4);
    return state;
}

/**
 * @brief
 * 		Log selection and period for SD log 1,2 & UART
 * @param flags_arr
 * 		Up to 80 flags indicating which telemetry frames should be
 * logged
 * @param period
 * 		log period (0 for stop)
 * @param dest
 * 		log destination (Table 211)
 * 		0 : primary SD card
 * 		1 : secondary SD card
 * @param log
 * 		1 : log1
 * 		2 : log2
 * 		3 : UART
 * @attention
 * 		dest for UART is not needed.
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_log_config(uint8_t *flags_arr, uint16_t period, uint8_t dest, uint8_t log) {
    uint8_t command[14] = {0};
    command[0] = SET_SD_LOG1_CONFIG_ID + (log - 1);
    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < 8; i++) {
            command[j + 1] = command[j + 1] | (*(flags_arr + (8 * j) + i) << i);
        }
    }
    memcpy(&command[11], &period, 2);
    command[13] = dest;
    ADCS_returnState state;
    if (log == 3) {
        state = adcs_telecommand(command, 13);
    } else {
        state = adcs_telecommand(command, 14);
    }
    return state; //* Tested. + command[8] or + command[13] or (+ command[12] <<
                  // 8) | command[11]
}

/**
 * @brief
 * 		Gets the log selection and period for LOG 1,2 & UART.
 * @param flags_arr
 * 		Up to 80 flags indicating which telemetry frames should be
 * logged
 * @param period
 * 		log period (0 for stop)
 * @param dest
 * 		log destination (Table 211)
 * 		0 : primary SD card
 * 		1 : secondary SD card
 * @param log
 * 		1 : log1
 * 		2 : log2
 * 		3 : UART
 * @attention
 * 		dest for UART is not used.
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_log_config(uint8_t *flags_arr, uint16_t *period, uint8_t *dest, uint8_t log) {
    uint8_t telemetry[13];
    ADCS_returnState state;
    uint8_t TM_ID = GET_SD_LOG1_CONFIG_ID + (log - 1);
    if (TM_ID == GET_UART_LOG_CONFIG_ID) {
        state = adcs_telemetry(TM_ID, telemetry, 12);
    } else {
        state = adcs_telemetry(TM_ID, telemetry, 13);
    }

    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < 8; i++) {
            *(flags_arr + (8 * j) + i) = (telemetry[j] >> i) & 1;
        }
    }
    *period = telemetry[11] << 8 | telemetry[10];
    if (TM_ID == GET_UART_LOG_CONFIG_ID) {
        *dest = 2;
    } else {
        *dest = telemetry[12];
    }

    return state;
}

/**
 * @brief
 * 		Sets the reference unit vector for inertial pointing control
 * mode. (Table 214)
 * @param inter_ref
 * 		Inertial reference
 * @param coef
 * 		formatted value = rawval * coef;
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_inertial_ref(xyz iner_ref) {
    uint8_t command[7];
    command[0] = SET_INERTIAL_POINT_ID;
    float coef = 0.0001;
    xyz16 raw_val;
    raw_val.x = iner_ref.x / coef;
    raw_val.y = iner_ref.y / coef;
    raw_val.z = iner_ref.z / coef;
    memcpy(&command[1], &raw_val, 6);
    return adcs_telecommand(command, 7);
}

/**
 * @brief
 * 		Gets the reference unit vector for inertial pointing control
 * mode. (Table 214)
 * @param inter_ref
 * 		Inertial reference
 * @param coef
 * 		formatted value = rawval * coef;
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_inertial_ref(xyz *iner_ref) {
    uint8_t telemetry[6];
    ADCS_returnState state;
    state = adcs_telemetry(GET_INERTIAL_POINT_ID, telemetry, 6);
    float coef = 0.0001;
    get_xyz(iner_ref, &telemetry[0], coef);
    return state;
}

/************************* Configuration *************************/
/**
 * @brief
 * 		Sets the SGP4 orbit parameter.
 * @param params
 * 		Refer to table 194
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_sgp4_orbit_params(adcs_sgp4 params) {
    uint8_t command[65];
    command[0] = SET_SGP4_ORBIT_PARAMS_ID;
    memcpy(&command[1], &params, 64);
    return adcs_telecommand(command, 65); //* Tested  + params.epoch*10
}

/**
 * @brief
 * 		Gets the SGP4 orbit parameter.
 * @param params
 * 		Refer to table 194
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_sgp4_orbit_params(adcs_sgp4 *params) {
    uint8_t telemetry[64];
    ADCS_returnState state;
    state = adcs_telemetry(GET_SGP4_ORBIT_PARAMS_ID, telemetry, 64);
    memcpy(&params->inclination, &telemetry[0], 8);
    memcpy(&params->ECC, &telemetry[8], 8);
    memcpy(&params->RAAN, &telemetry[16], 8);
    memcpy(&params->AOP, &telemetry[24], 8);
    memcpy(&params->Bstar, &telemetry[32], 8);
    memcpy(&params->MM, &telemetry[40], 8);
    memcpy(&params->MA, &telemetry[48], 8);
    memcpy(&params->epoch, &telemetry[56], 8);
    return state;
}

/**
 * @brief
 * 		Sets the current hard-coded system configuration.
 * @param config
 * 		Refer to table 201-207
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_system_config(adcs_sysConfig config) {
    uint8_t command[174];
    command[0] = SET_SYSTEM_CONFIG_ID;
    command[1] = (config.special_ctrl_sel << 4) | config.acp_type;
    command[2] = config.CC_sig_ver;
    command[3] = config.CC_motor_ver;
    command[4] = config.CS1_ver;
    command[5] = config.CS2_ver;
    command[6] = (config.CS2_cam << 4) | config.CS1_cam;
    command[7] = config.cubeStar_ver;
    command[8] = (config.include_MTM2 << 4) | config.GPS;
    memcpy(&command[9], &config.MTQ_max_dipole, 12);
    memcpy(&command[21], &config.MTQ_ontime_res, 4);
    memcpy(&command[25], &config.MTQ_max_ontime, 4);
    memcpy(&command[29], &config.RW_max_torque, 12);
    memcpy(&command[41], &config.RW_max_moment, 12);
    memcpy(&command[53], &config.RW_inertia, 12);
    memcpy(&command[65], &config.RW_torque_inc, 4);
    memcpy(&command[69], &config.MTM1, 48);
    memcpy(&command[117], &config.MTM2, 48);
    command[165] = (config.CC_signal.pin << 4) | config.CC_signal.port;
    command[166] = (config.CC_motor.pin << 4) | config.CC_motor.port;
    command[167] = (config.CC_common.pin << 4) | config.CC_common.port;
    command[168] = (config.CS1.pin << 4) | config.CS1.port;
    command[169] = (config.CS2.pin << 4) | config.CS2.port;
    command[170] = (config.cubeStar.pin << 4) | config.cubeStar.port;
    command[171] = (config.CW1.pin << 4) | config.CW1.port;
    command[172] = (config.CW2.pin << 4) | config.CW2.port;
    command[173] = (config.CW3.pin << 4) | config.CW3.port;
    return adcs_telecommand(command,
                            174); //* Tested + command[8] and +command[165]
}

/**
 * @brief
 * 		Gets the current hard-coded system configuration.
 * @param config
 * 		Refer to table 201-207
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_system_config(adcs_sysConfig *config) {
    uint8_t telemetry[173];
    ADCS_returnState state;
    state = adcs_telemetry(GET_SYSTEM_CONFIG_ID, telemetry, 173);
    config->acp_type = telemetry[0] & 0xF;
    config->special_ctrl_sel = (telemetry[0] >> 4) & 0xF;
    config->CC_sig_ver = telemetry[1];
    config->CC_motor_ver = telemetry[2];
    config->CS1_ver = telemetry[3];
    config->CS2_ver = telemetry[4];
    config->CS1_cam = telemetry[5] & 0xF;
    config->CS2_cam = (telemetry[5] >> 4) & 0xF;
    config->cubeStar_ver = telemetry[6];
    config->GPS = telemetry[7] & 0xF;
    config->include_MTM2 = (telemetry[7] >> 4) & 1;
    memcpy(&config->MTQ_max_dipole, &telemetry[8], 12);
    memcpy(&config->MTQ_ontime_res, &telemetry[20], 4);
    memcpy(&config->MTQ_max_ontime, &telemetry[24], 4);
    memcpy(&config->RW_max_torque, &telemetry[28], 12);
    memcpy(&config->RW_max_moment, &telemetry[40], 12);
    memcpy(&config->RW_inertia, &telemetry[52], 12);
    memcpy(&config->RW_torque_inc, &telemetry[64], 4);
    memcpy(&config->MTM1, &telemetry[68], 48);
    memcpy(&config->MTM2, &telemetry[116], 48);
    config->CC_signal.port = telemetry[164] & 0xF;
    config->CC_signal.pin = (telemetry[164] >> 4) & 0xF;
    config->CC_motor.port = telemetry[165] & 0xF;
    config->CC_motor.pin = (telemetry[165] >> 4) & 0xF;
    config->CC_common.port = telemetry[166] & 0xF;
    config->CC_common.pin = (telemetry[166] >> 4) & 0xF;
    config->CS1.port = telemetry[167] & 0xF;
    config->CS1.pin = (telemetry[167] >> 4) & 0xF;
    config->CS2.port = telemetry[168] & 0xF;
    config->CS2.pin = (telemetry[168] >> 4) & 0xF;
    config->cubeStar.port = telemetry[169] & 0xF;
    config->cubeStar.pin = (telemetry[169] >> 4) & 0xF;
    config->CW1.port = telemetry[170] & 0xF;
    config->CW1.pin = (telemetry[170] >> 4) & 0xF;
    config->CW2.port = telemetry[171] & 0xF;
    config->CW2.pin = (telemetry[171] >> 4) & 0xF;
    config->CW3.port = telemetry[172] & 0xF;
    config->CW3.pin = (telemetry[172] >> 4) & 0xF;
    return state;
}

/**
 * @brief
 * 		Sets the magnetorquer configuration parameters.
 * 		(Table 179)
 * @param params
 * 		axis selection : Refer to table 180
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_MTQ_config(xyzu8 params) {
    uint8_t command[4];
    command[0] = SET_MTQ_CONFIG_ID;
    memcpy(&command[1], &params, 3);
    return adcs_telecommand(command, 4);
}

/**
 * @brief
 * 		Sets the wheel configuration parameters.
 * 		(Table 181)
 * @param RW
 * 		An array for the 4 wheels' axis selection (Table 180)
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_RW_config(uint8_t *RW) {
    uint8_t command[5];
    command[0] = SET_WHEEL_CONFIG_ID;
    memcpy(&command[1], &RW[0], 4);
    return adcs_telecommand(command, 5);
}

/**
 * @brief
 * 		Sets the rate gyro configuration parameters.
 * 		(Table 182)
 * @param params.sensor_offset
 * 		rate sensor offset [deg/s]
 * @param coef
 * 		formatted_value = coef * raw_value
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_rate_gyro(rate_gyro_config params) {
    uint8_t command[11];
    command[0] = SET_RATE_GYRO_CONFIG_ID;
    memcpy(&command[1], &params.gyro, 3);
    float coef = 0.001;
    xyz16 raw_val;
    raw_val.x = params.sensor_offset.x / coef;
    raw_val.y = params.sensor_offset.y / coef;
    raw_val.z = params.sensor_offset.z / coef;
    memcpy(&command[4], &raw_val, 6);
    command[10] = params.rate_sensor_mult;
    return adcs_telecommand(command, 11);
}

/**
 * @brief
 * 		Sets the photodiode pointing directions and scale factors.
 * 		(Table 183)
 * @param config
 * 		css axis selection : Refer to table 180
 * @param rel_scale
 * 		relative scaling factor
 * @param coef
 * 		formatted_value = coef * raw_value
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_css_config(css_config config) {
    uint8_t command[22];
    command[0] = SET_CSS_CONFIG_ID;
    memcpy(&command[1], &config.config[0], 10);
    uint8_t raw_val[10];
    float coef = 0.01;
    for (int i = 0; i < 10; i++) {
        raw_val[i] = config.rel_scale[i] / coef;
    }
    memcpy(&command[11], &raw_val[0], 10);
    command[21] = config.threshold;
    return adcs_telecommand(command, 22);
}

/**
 * @brief
 * 		Sets configurations of CubeStar.
 * 		(Table 188)
 * @param coef
 * 		formatted_value = coef * raw_value
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_star_track_config(cubestar_config config) {
    uint8_t command[54];
    command[0] = SET_STAR_TRACK_CONFIG_ID;
    xyz16 raw_val;
    float coef = 0.01;
    raw_val.x = config.mounting_angle.x / coef;
    raw_val.y = config.mounting_angle.y / coef;
    raw_val.z = config.mounting_angle.z / coef;
    memcpy(&command[1], &raw_val, 6);
    memcpy(&command[7], &config.exposure_t, 45);
    command[52] = (config.loc_predict_en >> 1) | config.module_en;
    uint8_t search_wid = config.search_wid * 5;
    command[53] = search_wid;
    return adcs_telecommand(command, 54);
}

/**
 * @brief
 *      Gets the CubeSense configuration parameters.
 *      (Table 189)
 * @return
 *      Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_cubesense_config(cubesense_config *config) {

    uint8_t *telemetry = (uint8_t*)pvPortMalloc(112);
    if (telemetry == NULL) {
        return ADCS_MALLOC_FAILED;
    }

    float coef = 0.01;
    xyz16 raw_val_angle1, raw_val_angle2;
    uint16_t raw_boresight_x1, raw_boresight_y1;
    uint16_t raw_boresight_x2, raw_boresight_y2;
    //request cubsense configuration and store in telemetry
    ADCS_returnState state;
    state = adcs_telemetry(GET_CUBESENSE_CONFIG_ID, telemetry, 112);


    //point these values to the pre-function structure
    raw_val_angle1.x = ((telemetry[1] << 8) & 0xFF00) | telemetry[0];
    raw_val_angle1.y = ((telemetry[3] << 8) & 0xFF00) | telemetry[2];
    raw_val_angle1.z = ((telemetry[5] << 8) & 0xFF00) | telemetry[4];
    config->cam1_sense.detect_th = telemetry[6];
    config->cam1_sense.auto_adjust = telemetry[7];
    config->cam1_sense.exposure_t = ((telemetry[9] << 8) & 0xFF00) | telemetry[8];
    raw_boresight_x1 = ((telemetry[11] << 8) & 0xFF00) | telemetry[10];

    //The following code does not set the endianness right
    /*
    memcpy(& raw_boresight_y1, &telemetry[12], 2);
    memcpy(& raw_val_angle2.x, &telemetry[14], 2);
    memcpy(& raw_val_angle2.y, &telemetry[16], 2);
    memcpy(& raw_val_angle2.z, &telemetry[18], 2);
    */
    //This code does set the endianness right (le)
    raw_boresight_y1 = ((telemetry[13] << 8) & 0xFF00) | telemetry[12];
    raw_val_angle2.x = ((telemetry[15] << 8) & 0xFF00) | telemetry[14];
    raw_val_angle2.y = ((telemetry[17] << 8) & 0xFF00) | telemetry[16];
    raw_val_angle2.z = ((telemetry[19] << 8) & 0xFF00) | telemetry[18];

    config->cam2_sense.detect_th = telemetry[20];
    config->cam2_sense.auto_adjust = telemetry[21];

    /*
    memcpy(& config->cam2_sense.exposure_t, &telemetry[22], 2);
    memcpy(& raw_boresight_x2, &telemetry[24], 2);
    memcpy(& raw_boresight_y2, &telemetry[26], 2);
    */
    config->cam2_sense.exposure_t = ((telemetry[23] << 8) & 0xFF00) | telemetry[22];
    raw_boresight_x2 = ((telemetry[25] << 8) & 0xFF00) | telemetry[24];
    raw_boresight_y2 = ((telemetry[27] << 8) & 0xFF00) | telemetry[26];

    //convert data as per the ADCS datasheet
    config->cam1_sense.mounting_angle.x = raw_val_angle1.x * coef;
    config->cam1_sense.mounting_angle.y = raw_val_angle1.y * coef;
    config->cam1_sense.mounting_angle.z = raw_val_angle1.z * coef;
    config->cam1_sense.boresight_x = raw_boresight_x1 * coef;
    config->cam1_sense.boresight_y = raw_boresight_y1 * coef;
    config->cam2_sense.mounting_angle.x = raw_val_angle2.x * coef;
    config->cam2_sense.mounting_angle.y = raw_val_angle2.y * coef;
    config->cam2_sense.mounting_angle.z = raw_val_angle2.z * coef;
    config->cam2_sense.boresight_x = raw_boresight_x2 * coef;
    config->cam2_sense.boresight_y = raw_boresight_y2 * coef;

    //no conversion necessary for the following parameters
    config->nadir_max_deviate = telemetry[28];
    config->nadir_max_bad_edge = telemetry[29];
    config->nadir_max_radius = telemetry[30];
    config->nadir_min_radius = telemetry[31];
    /*
    memcpy(& config->cam1_area.area1.x.min, &telemetry[32], 2);
    memcpy(& config->cam1_area.area1.x.max, &telemetry[36], 2);
    memcpy(& config->cam1_area.area1.y.min, &telemetry[38], 2);
    memcpy(& config->cam1_area.area1.y.max, &telemetry[40], 2);
    memcpy(& config->cam1_area.area2.x.min, &telemetry[42], 2);
    memcpy(& config->cam1_area.area2.x.max, &telemetry[44], 2);
    memcpy(& config->cam1_area.area2.y.min, &telemetry[46], 2);
    memcpy(& config->cam1_area.area2.y.max, &telemetry[48], 2);
    memcpy(& config->cam1_area.area3.x.min, &telemetry[50], 2);
    memcpy(& config->cam1_area.area3.x.max, &telemetry[52], 2);
    memcpy(& config->cam1_area.area3.y.min, &telemetry[54], 2);
    memcpy(& config->cam1_area.area3.y.max, &telemetry[56], 2);
    memcpy(& config->cam1_area.area4.x.min, &telemetry[58], 2);
    memcpy(& config->cam1_area.area4.x.max, &telemetry[60], 2);
    memcpy(& config->cam1_area.area4.y.min, &telemetry[62], 2);
    memcpy(& config->cam1_area.area4.y.max, &telemetry[62], 2);
    memcpy(& config->cam1_area.area5.x.min, &telemetry[64], 2);
    memcpy(& config->cam1_area.area5.x.max, &telemetry[66], 2);
    memcpy(& config->cam1_area.area5.y.min, &telemetry[68], 2);
    memcpy(& config->cam1_area.area5.y.max, &telemetry[72], 2);

    memcpy(& config->cam2_area.area1.x.min, &telemetry[74], 2);
    memcpy(& config->cam2_area.area1.x.max, &telemetry[76], 2);
    memcpy(& config->cam2_area.area1.y.min, &telemetry[78], 2);
    memcpy(& config->cam2_area.area1.y.max, &telemetry[80], 2);
    memcpy(& config->cam2_area.area2.x.min, &telemetry[82], 2);
    memcpy(& config->cam2_area.area2.x.max, &telemetry[84], 2);
    memcpy(& config->cam2_area.area2.y.min, &telemetry[86], 2);
    memcpy(& config->cam2_area.area2.y.max, &telemetry[88], 2);
    memcpy(& config->cam2_area.area3.x.min, &telemetry[90], 2);
    memcpy(& config->cam2_area.area3.x.max, &telemetry[92], 2);
    memcpy(& config->cam2_area.area3.y.min, &telemetry[94], 2);
    memcpy(& config->cam2_area.area3.y.max, &telemetry[96], 2);
    memcpy(& config->cam2_area.area4.x.min, &telemetry[98], 2);
    memcpy(& config->cam2_area.area4.x.max, &telemetry[100], 2);
    memcpy(& config->cam2_area.area4.y.min, &telemetry[102], 2);
    memcpy(& config->cam2_area.area4.y.max, &telemetry[104], 2);
    memcpy(& config->cam2_area.area5.x.min, &telemetry[106], 2);
    memcpy(& config->cam2_area.area5.x.max, &telemetry[108], 2);
    memcpy(& config->cam2_area.area5.y.min, &telemetry[110], 2);
    memcpy(& config->cam2_area.area5.y.max, &telemetry[111], 2);
    */
    config->cam1_area.area1.x.min = ((telemetry[33] << 8) & 0xFF00) | telemetry[32];
    config->cam1_area.area1.x.max = ((telemetry[35] << 8) & 0xFF00) | telemetry[34];
    config->cam1_area.area1.y.min = ((telemetry[37] << 8) & 0xFF00) | telemetry[36];
    config->cam1_area.area1.y.max = ((telemetry[39] << 8) & 0xFF00) | telemetry[38];
    config->cam1_area.area2.x.min = ((telemetry[41] << 8) & 0xFF00) | telemetry[40];
    config->cam1_area.area2.x.max = ((telemetry[43] << 8) & 0xFF00) | telemetry[42];
    config->cam1_area.area2.y.min = ((telemetry[45] << 8) & 0xFF00) | telemetry[44];
    config->cam1_area.area2.y.max = ((telemetry[47] << 8) & 0xFF00) | telemetry[46];
    config->cam1_area.area3.x.min = ((telemetry[49] << 8) & 0xFF00) | telemetry[48];
    config->cam1_area.area3.x.max = ((telemetry[51] << 8) & 0xFF00) | telemetry[50];
    config->cam1_area.area3.y.min = ((telemetry[53] << 8) & 0xFF00) | telemetry[52];
    config->cam1_area.area3.y.max = ((telemetry[55] << 8) & 0xFF00) | telemetry[54];
    config->cam1_area.area4.x.min = ((telemetry[57] << 8) & 0xFF00) | telemetry[56];
    config->cam1_area.area4.x.max = ((telemetry[59] << 8) & 0xFF00) | telemetry[58];
    config->cam1_area.area4.y.min = ((telemetry[61] << 8) & 0xFF00) | telemetry[60];
    config->cam1_area.area4.y.max = ((telemetry[63] << 8) & 0xFF00) | telemetry[62];
    config->cam1_area.area5.x.min = ((telemetry[65] << 8) & 0xFF00) | telemetry[64];
    config->cam1_area.area5.x.max = ((telemetry[67] << 8) & 0xFF00) | telemetry[66];
    config->cam1_area.area5.y.min = ((telemetry[69] << 8) & 0xFF00) | telemetry[68];
    config->cam1_area.area5.y.max = ((telemetry[71] << 8) & 0xFF00) | telemetry[70];
    config->cam2_area.area1.x.min = ((telemetry[73] << 8) & 0xFF00) | telemetry[72];
    config->cam2_area.area1.x.max = ((telemetry[75] << 8) & 0xFF00) | telemetry[74];
    config->cam2_area.area1.y.min = ((telemetry[77] << 8) & 0xFF00) | telemetry[76];
    config->cam2_area.area1.y.max = ((telemetry[79] << 8) & 0xFF00) | telemetry[78];
    config->cam2_area.area2.x.min = ((telemetry[81] << 8) & 0xFF00) | telemetry[80];
    config->cam2_area.area2.x.max = ((telemetry[83] << 8) & 0xFF00) | telemetry[82];
    config->cam2_area.area2.y.min = ((telemetry[85] << 8) & 0xFF00) | telemetry[84];
    config->cam2_area.area2.y.max = ((telemetry[87] << 8) & 0xFF00) | telemetry[86];
    config->cam2_area.area3.x.min = ((telemetry[89] << 8) & 0xFF00) | telemetry[88];
    config->cam2_area.area3.x.max = ((telemetry[91] << 8) & 0xFF00) | telemetry[90];
    config->cam2_area.area3.y.min = ((telemetry[93] << 8) & 0xFF00) | telemetry[92];
    config->cam2_area.area3.y.max = ((telemetry[95] << 8) & 0xFF00) | telemetry[94];
    config->cam2_area.area4.x.min = ((telemetry[97] << 8) & 0xFF00) | telemetry[96];
    config->cam2_area.area4.x.max = ((telemetry[99] << 8) & 0xFF00) | telemetry[98];
    config->cam2_area.area4.y.min = ((telemetry[101] << 8) & 0xFF00) | telemetry[100];
    config->cam2_area.area4.y.max = ((telemetry[103] << 8) & 0xFF00) | telemetry[102];
    config->cam2_area.area5.x.min = ((telemetry[105] << 8) & 0xFF00) | telemetry[104];
    config->cam2_area.area5.x.max = ((telemetry[107] << 8) & 0xFF00) | telemetry[106];
    config->cam2_area.area5.y.min = ((telemetry[109] << 8) & 0xFF00) | telemetry[108];
    config->cam2_area.area5.y.max = ((telemetry[111] << 8) & 0xFF00) | telemetry[110];

    vPortFree(telemetry);

    return state;
}

/**
 * @brief
 * 		Sets the CubeSense configuration parameters.
 * 		(Table 189)
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_cubesense_config(cubesense_config params) {
    uint8_t command[113];
    if (command == NULL) {
        return ADCS_MALLOC_FAILED;
    }

    command[0] = SET_CUBESENSE_CONFIG_ID;

    xyz16 raw_val_angle1, raw_val_angle2;
    uint16_t raw_boresight_x1, raw_boresight_y1;
    uint16_t raw_boresight_x2, raw_boresight_y2;
    float coef = 0.01;

    raw_val_angle1.x = params.cam1_sense.mounting_angle.x / coef;
    raw_val_angle1.y = params.cam1_sense.mounting_angle.y / coef;
    raw_val_angle1.z = params.cam1_sense.mounting_angle.z / coef;
    raw_boresight_x1 = params.cam1_sense.boresight_x / coef;
    raw_boresight_y1 = params.cam1_sense.boresight_y / coef;
    raw_val_angle2.x = params.cam2_sense.mounting_angle.x / coef;
    raw_val_angle2.y = params.cam2_sense.mounting_angle.y / coef;
    raw_val_angle2.z = params.cam2_sense.mounting_angle.z / coef;
    raw_boresight_x2 = params.cam2_sense.boresight_x / coef;
    raw_boresight_y2 = params.cam2_sense.boresight_y / coef;

    command[1] = (raw_val_angle1.x) & 0x00FF;
    command[2] = (raw_val_angle1.x >> 8) & 0x00FF;
    command[3] = (raw_val_angle1.y) & 0x00FF;
    command[4] = (raw_val_angle1.y >> 8) & 0x00FF;
    command[5] = (raw_val_angle1.z) & 0x00FF;
    command[6] = (raw_val_angle1.z >> 8) & 0x00FF;
    command[7] = params.cam1_sense.detect_th;
    command[8] = params.cam1_sense.auto_adjust;
    command[9] = (params.cam1_sense.exposure_t) & 0x00FF;
    command[10] = (params.cam1_sense.exposure_t >> 8) & 0x00FF;
    command[11] = (raw_boresight_x1) & 0x00FF;
    command[12] = (raw_boresight_x1 >> 8) & 0x00FF;
    command[13] = (raw_boresight_y1) & 0x00FF;
    command[14] = (raw_boresight_y1 >> 8) & 0x00FF;
    command[15] = (raw_val_angle2.x) & 0x00FF;
    command[16] = (raw_val_angle2.x >> 8) & 0x00FF;
    command[17] = (raw_val_angle2.y) & 0x00FF;
    command[18] = (raw_val_angle2.y >> 8) & 0x00FF;
    command[19] = (raw_val_angle2.z) & 0x00FF;
    command[20] = (raw_val_angle2.z >> 8) & 0x00FF;
    command[21] = params.cam2_sense.detect_th;
    command[22] = params.cam2_sense.auto_adjust;
    command[23] = (params.cam2_sense.exposure_t) & 0x00FF;
    command[24] = (params.cam2_sense.exposure_t >> 8) & 0x00FF;
    command[25] = (raw_boresight_x2) & 0x00FF;
    command[26] = (raw_boresight_x2 >> 8) & 0x00FF;
    command[27] = (raw_boresight_y2) & 0x00FF;
    command[28] = (raw_boresight_y2 >> 8) & 0x00FF;
    command[29] = params.nadir_max_deviate;
    command[30] = params.nadir_max_bad_edge;
    command[31] = params.nadir_max_radius;
    command[32] = params.nadir_min_radius;
    command[33] = (params.cam1_area.area1.x.min) & 0x00FF;
    command[34] = (params.cam1_area.area1.x.min >> 8) & 0x00FF;
    command[35] = (params.cam1_area.area1.x.max) & 0x00FF;
    command[36] = (params.cam1_area.area1.x.max >> 8) & 0x00FF;
    command[37] = (params.cam1_area.area1.y.min ) & 0x00FF;
    command[38] = (params.cam1_area.area1.y.min >> 8) & 0x00FF;
    command[39] = (params.cam1_area.area1.y.max) & 0x00FF;
    command[40] = (params.cam1_area.area1.y.max >> 8) & 0x00FF;
    command[41] = (params.cam1_area.area2.x.min ) & 0x00FF;
    command[42] = (params.cam1_area.area2.x.min >> 8) & 0x00FF;
    command[43] = (params.cam1_area.area2.x.max ) & 0x00FF;
    command[44] = (params.cam1_area.area2.x.max >> 8) & 0x00FF;
    command[45] = (params.cam1_area.area2.y.min ) & 0x00FF;
    command[46] = (params.cam1_area.area2.y.min >> 8) & 0x00FF;
    command[47] = (params.cam1_area.area2.y.max ) & 0x00FF;
    command[48] = (params.cam1_area.area2.y.max >> 8) & 0x00FF;
    command[49] = (params.cam1_area.area3.x.min ) & 0x00FF;
    command[50] = (params.cam1_area.area3.x.min >> 8) & 0x00FF;
    command[51] = (params.cam1_area.area3.x.max  ) & 0x00FF;
    command[52] = (params.cam1_area.area3.x.max >> 8) & 0x00FF;
    command[53] = (params.cam1_area.area3.y.min ) & 0x00FF;
    command[54] = (params.cam1_area.area3.y.min >> 8) & 0x00FF;
    command[55] = (params.cam1_area.area3.y.max  ) & 0x00FF;
    command[56] = (params.cam1_area.area3.y.max >> 8) & 0x00FF;
    command[57] = (params.cam1_area.area4.x.min ) & 0x00FF;
    command[58] = (params.cam1_area.area4.x.min >> 8) & 0x00FF;
    command[59] = (params.cam1_area.area4.x.max ) & 0x00FF;
    command[60] = (params.cam1_area.area4.x.max >> 8) & 0x00FF;
    command[61] = (params.cam1_area.area4.y.min) & 0x00FF;
    command[62] = (params.cam1_area.area4.y.min >> 8) & 0x00FF;
    command[63] = (params.cam1_area.area4.y.max ) & 0x00FF;
    command[64] = (params.cam1_area.area4.y.max >> 8) & 0x00FF;
    command[65] = (params.cam1_area.area5.x.min ) & 0x00FF;
    command[66] = (params.cam1_area.area5.x.min >> 8) & 0x00FF;
    command[67] = (params.cam1_area.area5.x.max ) & 0x00FF;
    command[68] = (params.cam1_area.area5.x.max >> 8) & 0x00FF;
    command[69] = (params.cam1_area.area5.y.min ) & 0x00FF;
    command[70] = (params.cam1_area.area5.y.min >> 8) & 0x00FF;
    command[71] = (params.cam1_area.area5.y.max ) & 0x00FF;
    command[72] = (params.cam1_area.area5.y.max >> 8) & 0x00FF;
    command[73] = (params.cam2_area.area1.x.min ) & 0x00FF;
    command[74] = (params.cam2_area.area1.x.min >> 8) & 0x00FF;
    command[75] = (params.cam2_area.area1.x.max ) & 0x00FF;
    command[76] = (params.cam2_area.area1.x.max >> 8) & 0x00FF;
    command[77] = (params.cam2_area.area1.y.min ) & 0x00FF;
    command[78] = (params.cam2_area.area1.y.min >> 8) & 0x00FF;
    command[79] = (params.cam2_area.area1.y.max ) & 0x00FF;
    command[80] = (params.cam2_area.area1.y.max >> 8) & 0x00FF;
    command[81] = (params.cam2_area.area2.x.min ) & 0x00FF;
    command[82] = (params.cam2_area.area2.x.min >> 8) & 0x00FF;
    command[83] = (params.cam2_area.area2.x.max ) & 0x00FF;
    command[84] = (params.cam2_area.area2.x.max >> 8) & 0x00FF;
    command[85] = (params.cam2_area.area2.y.min ) & 0x00FF;
    command[86] = (params.cam2_area.area2.y.min >> 8) & 0x00FF;
    command[87] = (params.cam2_area.area2.y.max ) & 0x00FF;
    command[88] = (params.cam2_area.area2.y.max >> 8) & 0x00FF;
    command[89] = (params.cam2_area.area3.x.min ) & 0x00FF;
    command[90] = (params.cam2_area.area3.x.min >> 8) & 0x00FF;
    command[91] = (params.cam2_area.area3.x.max ) & 0x00FF;
    command[92] = (params.cam2_area.area3.x.max >> 8) & 0x00FF;
    command[93] = (params.cam2_area.area3.y.min ) & 0x00FF;
    command[94] = (params.cam2_area.area3.y.min >> 8) & 0x00FF;
    command[95] = (params.cam2_area.area3.y.max ) & 0x00FF;
    command[96] = (params.cam2_area.area3.y.max >> 8) & 0x00FF;
    command[97] = (params.cam2_area.area4.x.min ) & 0x00FF;
    command[98] = (params.cam2_area.area4.x.min >> 8) & 0x00FF;
    command[99] = (params.cam2_area.area4.x.max ) & 0x00FF;
    command[100] = (params.cam2_area.area4.x.max >> 8) & 0x00FF;
    command[101] = (params.cam2_area.area4.y.min ) & 0x00FF;
    command[102] = (params.cam2_area.area4.y.min >> 8) & 0x00FF;
    command[103] = (params.cam2_area.area4.y.max ) & 0x00FF;
    command[104] = (params.cam2_area.area4.y.max >> 8) & 0x00FF;
    command[105] = (params.cam2_area.area5.x.min ) & 0x00FF;
    command[106] = (params.cam2_area.area5.x.min >> 8) & 0x00FF;
    command[107] = (params.cam2_area.area5.x.max ) & 0x00FF;
    command[108] = (params.cam2_area.area5.x.max >> 8) & 0x00FF;
    command[109] = (params.cam2_area.area5.y.min ) & 0x00FF;
    command[110] = (params.cam2_area.area5.y.min >> 8) & 0x00FF;
    command[111] = (params.cam2_area.area5.y.max ) & 0x00FF;
    command[112] = (params.cam2_area.area5.y.max >> 8) & 0x00FF;

    ADCS_returnState result = adcs_telecommand(command, 113);
    return result;
}

/**
 * @brief
 * 		Sets the Magnetometers configuration parameters.
 * 		(Table 190, 191)
 * @param mtm
 * 		Select primary (1) or secondary(2) Magnetometer
 * @attention
 * 		The order of input matrix is s11, s12, s13, s21, ...
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_mtm_config(mtm_config params, uint8_t mtm) {
    uint8_t command[31];
    if (mtm == 1) {
        command[0] = SET_MTM_CONFIG_ID;
    } else if (mtm == 2) {
        command[0] = SET_MTM2_CONFIG_ID;
    } else {
        return ADCS_INVALID_PARAMETERS;
    }
    xyz16 raw_val_angle, raw_val_offset;
    float coef = 0.01;
    raw_val_angle.x = params.mounting_angle.x / coef;
    raw_val_angle.y = params.mounting_angle.y / coef;
    raw_val_angle.z = params.mounting_angle.z / coef;
    memcpy(&command[1], &raw_val_angle, 6);
    coef = 0.001;
    raw_val_offset.x = params.channel_offset.x / coef;
    raw_val_offset.y = params.channel_offset.y / coef;
    raw_val_offset.z = params.channel_offset.z / coef;
    memcpy(&command[7], &raw_val_offset, 6);
    int16_t cell[9];
    int j = 0;
    for (int i = 0; i < 3; i++) {
        cell[i] = params.sensitivity_mat[4 * i] / coef; // diagonal
    }
    for (int i = 0; i < 3; i++) {
        cell[3 + i] = params.sensitivity_mat[1 + i] / coef;
        cell[6 + i] = params.sensitivity_mat[5 + i] / coef;
    }
    memcpy(&command[13], &cell[0], 18);
    return adcs_telecommand(command, 31); //* Tested but not completely exact!  +
                                          //(command[16] << 8 | command[15])
}

/**
 * @brief
 * 		Sets the controller gains and reference values for Detumbling
 * control. mode (Table 195)
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_detumble_config(detumble_config config) {
    uint8_t command[15];
    command[0] = SET_DETUMBLE_PARAM_ID;
    memcpy(&command[1], &config, 8);
    int16_t raw_spin_rate;
    float coef = 0.001;
    raw_spin_rate = config.spin_rate / coef;
    memcpy(&command[9], &raw_spin_rate, 2);
    memcpy(&command[11], &config.fast_bDot, 4);
    return adcs_telecommand(command, 15);
}

/**
 * @brief
 * 		Sets the controller gains and reference value for Y-wheel
 * control. mode (Table 196)
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_ywheel_config(ywheel_ctrl_config params) {
    uint8_t command[21];
    command[0] = SET_YWHEEL_CTRL_PARAM_ID;
    memcpy(&command[1], &params, 20);
    return adcs_telecommand(command, 21);
}

/**
 * @brief
 * 		Sets the controller gains and reference value for reaction wheel
 * control mode (Table 197)
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_rwheel_config(rwheel_ctrl_config params) {
    uint8_t command[14];
    command[0] = SET_RWHEEL_CTRL_PARAM_ID;
    memcpy(&command[1], &params, 12);
    command[13] = (params.auto_transit << 7) | params.sun_point_facet;
    return adcs_telecommand(command, 14);
}

/**
 * @brief
 * 		Sets the controller gains for tracking control mode.
 * 		(Table 198)
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_tracking_config(track_ctrl_config params) {
    uint8_t command[14];
    command[0] = SET_TRACK_CTRL_ID;
    memcpy(&command[1], &params, 13);
    return adcs_telecommand(command, 14);
}

/**
 * @brief
 * 		Sets the satellite moment of inertia matrix.
 * 		(Table 199)
 * @param cell
 * 		diag: Ixx, Iyy, Izz
 * 		nondiag: Ixy, Ixz, Iyz
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_MoI_mat(moment_inertia_config cell) {
    uint8_t command[25];
    command[0] = SET_MOMENT_INERTIA_MAT_ID;
    memcpy(&command[1], &cell, 24);
    return adcs_telecommand(command, 25);
}

/**
 * @brief
 * 		Sets the estimation noise covariance and sensor mask.
 * 		(Table 200)
 * @param config.select_arr
 * 		All the bool "use" and auto-transit selects in Table 200
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_estimation_config(estimation_config config) {
    uint8_t command[32];
    command[0] = SET_ESTIMATE_PARAM;
    memcpy(&command[1], &config, 28);
    command[29] = 0;
    for (int i = 0; i < 6; i++) {
        command[29] |= (config.select_arr[i] << i);
    }
    command[29] |= (config.MTM_mode << 6);
    command[30] = config.MTM_select | (config.select_arr[7] << 2);
    command[31] = config.cam_sample_period;
    return adcs_telecommand(command,
                            32); //* Tested.  + command[29] and  + command[30]
}

/**
 * @brief
 * 		Sets the settings for user-coded estimation and control modes.
 * 		(Table 208)
 * @param setting
 * 		Two arrays of length 48
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_usercoded_setting(usercoded_setting setting) {
    uint8_t command[97];
    command[0] = SET_USERCODED_PARAM_ID;
    memcpy(&command[1], &setting, 96);
    return adcs_telecommand(command, 97);
}

/**
 * @brief
 * 		Sets the settings for GPS augmented SGP4.
 * 		(Table 209)
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_set_asgp4_setting(aspg4_setting setting) {
    if ((setting.inclination < 0) | (setting.RAAN < 0) | (setting.ECC < 0) | (setting.AoP < 0) |
        (setting.time < 0) | (setting.pos < 0) | (setting.max_pos_err < 0) | (setting.pos_sd < 0) |
        (setting.vel_sd < 0) | (setting.time_gain < 0) | (setting.max_lag < 0)) {
        return ADCS_INVALID_PARAMETERS;
    }
    uint8_t command[31];
    command[0] = SET_ASGP4_PARAM_ID;
    float coef = 0.001;
    uint16_t inclination = setting.inclination / coef;
    memcpy(&command[1], &inclination, 2);
    uint16_t RAAN = setting.RAAN / coef;
    memcpy(&command[3], &RAAN, 2);
    uint16_t ECC = setting.ECC / coef;
    memcpy(&command[5], &ECC, 2);
    uint16_t AoP = setting.AoP / coef;
    memcpy(&command[7], &AoP, 2);
    uint16_t time = setting.time / coef;
    memcpy(&command[9], &time, 2);
    uint16_t pos = setting.pos / coef;
    memcpy(&command[11], &pos, 2);
    coef = 0.1;
    uint8_t max_pos_err = setting.max_pos_err / coef;
    command[13] = max_pos_err;
    command[14] = setting.asgp4_filter;
    coef = 0.0000001;
    int32_t xp = setting.xp / coef;
    memcpy(&command[15], &xp, 4);
    int32_t yp = setting.yp / coef;
    memcpy(&command[19], &yp, 4);
    command[23] = setting.gps_rollover;
    coef = 0.1;
    uint8_t pos_sd = setting.pos_sd / coef;
    command[24] = pos_sd;
    coef = 0.01;
    uint8_t vel_sd = setting.vel_sd / coef;
    command[25] = vel_sd;
    command[26] = setting.min_sat;
    uint8_t time_gain = setting.time_gain / coef;
    command[27] = time_gain;
    uint8_t max_lag = setting.max_lag / coef;
    command[28] = max_lag;
    memcpy(&command[29], &setting.min_samples, 2);
    return adcs_telecommand(command, 31);
}

/**
 * @brief
 * 		Gets the current full configuration.
 * @param config
 * 		Refer to table 192
 * @return
 * 		Success of function defined in adcs_types.h
 */
ADCS_returnState ADCS_get_full_config(adcs_config *config) {
    uint8_t telemetry[504];
    float coef;
    ADCS_returnState state;
    state = adcs_telemetry(GET_FULL_CONFIG_ID, telemetry, 504);
    memcpy(&config->MTQ, &telemetry[0], 3);
    memcpy(&config->RW[0], &telemetry[3], 4);
    memcpy(&config->rate_gyro, &telemetry[7], 3);
    get_xyz(&config->rate_gyro.sensor_offset, &telemetry[10], 0.001);
    config->rate_gyro.rate_sensor_mult = telemetry[16];
    memcpy(&config->css, &telemetry[17], 10);
    coef = 0.01;
    for (int i = 0; i < 10; i++) {
        config->css.rel_scale[i] = telemetry[27 + i] * coef;
    }
    config->css.threshold = telemetry[37];
    get_xyz(&config->cubesense.cam1_sense.mounting_angle, &telemetry[38], 0.01);
    config->cubesense.cam1_sense.detect_th = telemetry[44];
    config->cubesense.cam1_sense.auto_adjust = telemetry[45] & 1;
    memcpy(&config->cubesense.cam1_sense.exposure_t, &telemetry[46], 2);
    config->cubesense.cam1_sense.boresight_x = ((telemetry[49] << 8) | telemetry[48]) * coef;
    config->cubesense.cam1_sense.boresight_y = ((telemetry[51] << 8) | telemetry[50]) * coef;
    get_xyz(&config->cubesense.cam2_sense.mounting_angle, &telemetry[52], 0.01);
    config->cubesense.cam2_sense.detect_th = telemetry[58];
    config->cubesense.cam2_sense.auto_adjust = telemetry[59] & 1;
    memcpy(&config->cubesense.cam2_sense.exposure_t, &telemetry[60], 2);
    config->cubesense.cam2_sense.boresight_x = ((telemetry[63] << 8) | telemetry[62]) * coef;
    config->cubesense.cam2_sense.boresight_y = ((telemetry[65] << 8) | telemetry[64]) * coef;
    memcpy(&config->cubesense.nadir_max_deviate, &telemetry[66], 84);
    get_xyz(&config->MTM1.mounting_angle, &telemetry[150], 0.01);
    get_xyz(&config->MTM1.channel_offset, &telemetry[156], 0.001);
    get_3x3(config->MTM1.sensitivity_mat, &telemetry[162], 0.001);
    get_xyz(&config->MTM2.mounting_angle, &telemetry[180], 0.01);
    get_xyz(&config->MTM2.channel_offset, &telemetry[186], 0.001);
    get_3x3(config->MTM2.sensitivity_mat, &telemetry[192], 0.001);
    get_xyz(&config->star_tracker.mounting_angle, &telemetry[210], 0.01);
    memcpy(&config->star_tracker.exposure_t, &telemetry[216], 45);
    config->star_tracker.module_en = telemetry[261] & 0x1;
    config->star_tracker.loc_predict_en = telemetry[261] & 0x2; // second bit
    config->star_tracker.search_wid = telemetry[262] / 5;
    memcpy(&config->detumble, &telemetry[263], 8);
    coef = 0.001;
    config->detumble.spin_rate = uint82int16(telemetry[271], telemetry[272]) * coef;
    memcpy(&config->detumble.fast_bDot, &telemetry[273], 4);
    memcpy(&config->ywheel, &telemetry[277], 20);
    memcpy(&config->rwheel, &telemetry[297], 12);
    config->rwheel.sun_point_facet = telemetry[309] & 0x7F; // 7 bits
    config->rwheel.auto_transit = telemetry[309] & 0x80;    // 8th bit
    memcpy(&config->tracking, &telemetry[310],
           65); // tracking + MoI + partially estimation
    for (int i = 0; i < 6; i++) {
        config->estimation.select_arr[i] = (telemetry[375] >> i) & 1;
    }
    config->estimation.MTM_mode = (telemetry[375] >> 6) & 0x3;
    config->estimation.MTM_select = telemetry[376] & 0x3;
    config->estimation.select_arr[7] = (telemetry[376] >> 2) & 1;
    config->estimation.cam_sample_period = telemetry[377];
    coef = 0.001;
    config->aspg4.inclination = ((telemetry[379] << 8) | telemetry[378]) * coef;
    config->aspg4.RAAN = ((telemetry[381] << 8) | telemetry[380]) * coef;
    config->aspg4.ECC = ((telemetry[383] << 8) | telemetry[382]) * coef;
    config->aspg4.AoP = ((telemetry[385] << 8) | telemetry[384]) * coef;
    config->aspg4.time = ((telemetry[387] << 8) | telemetry[386]) * coef;
    config->aspg4.pos = ((telemetry[389] << 8) | telemetry[388]) * coef;
    coef = 0.1;
    config->aspg4.max_pos_err = telemetry[390] * coef;
    config->aspg4.asgp4_filter = telemetry[391];
    coef = 0.0000001;
    config->aspg4.xp = uint82int32(&telemetry[392]) * coef;
    config->aspg4.yp = uint82int32(&telemetry[396]) * coef;
    config->aspg4.gps_rollover = telemetry[400];
    coef = 0.1;
    config->aspg4.pos_sd = telemetry[401] * coef;
    coef = 0.01;
    config->aspg4.vel_sd = telemetry[402] * coef;
    config->aspg4.min_sat = telemetry[403];
    config->aspg4.time_gain = telemetry[404] * coef;
    config->aspg4.max_lag = telemetry[405] * coef;
    config->aspg4.min_samples = (telemetry[407] << 8) | telemetry[406];
    memcpy(&config->usercoded, &telemetry[408], 96);
    return state;
}
