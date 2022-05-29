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
 * @file adcs_io.h
 * @author Andrew Rooney, Arash Yazdani, Vasu Gupta, Thomas Ganley
 * @date 2020-08-09
 */

#ifndef ADCS_IO_H
#define ADCS_IO_H

#include <stdint.h>
#include <stdio.h>
#include "adcs_types.h"
#include "adcs_handler.h"
#include "FreeRTOS.h"
#include "HL_sci.h"
#include "i2c_io.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_task.h"
#include "system.h"
#include <stdbool.h>
#include <string.h>

#define ADCS_QUEUE_LENGTH 600
#define ADCS_FILE_DOWNLOAD_QUEUE_LENGTH (10 * ADCS_UART_FILE_DOWNLOAD_PKT_LEN)
#define ADCS_QUEUE_ITEM_SIZE 1

#define ADCS_I2C_ADDR 0x57
#define UART_TIMEOUT_MS 1500
#define ADCS_FILE_DOWNLOAD_QUEUE_TIMEOUT pdMS_TO_TICKS(500)

#define ADCS_TC_HEADER_SZ 4 // Does not include TC ID
#define ADCS_TC_ANS_LEN 6
#define ADCS_TM_HEADER_SZ 5  // Includes TM ID
#define ADCS_TM_DATA_INDEX 3 // Data excludes TM ID
#define ADCS_EXTRA_SZ_FOR_STUFFING 6
#define ADCS_PARSING_BYTE 0x1F
#define ADCS_ENDING_BYTE 0xFF
#define ADCS_NUM_ENDING_BYTES 2

ADCS_returnState init_adcs_io();

// sends telecommand over uart/i2c, and return acknowledgment
ADCS_returnState send_uart_telecommand(uint8_t *command, uint32_t length);
ADCS_returnState send_i2c_telecommand(uint8_t *command, uint32_t length);
ADCS_returnState send_uart_telecommand_no_reply(uint8_t *command, uint32_t length);

// request telemetry over uart/i2c, and return acknowledgment
ADCS_returnState request_uart_telemetry(uint8_t TM_ID, uint8_t *telemetry, uint32_t length);
ADCS_returnState request_i2c_telemetry(uint8_t TM_ID, uint8_t *telemetry, uint32_t length);

// receive downloaded packets over uart
ADCS_returnState receive_file_download_uart_packet(uint8_t *packet, uint16_t *packet_counter);
void write_packet_to_file(uint32_t file_des, uint8_t *packet_data, uint8_t length);
ADCS_returnState adcs_io_exit_file_download_state();
ADCS_returnState adcs_io_enter_file_download_state();

#endif /* ADCS_IO_H */
