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
 * @file adcs_io.c
 * @author Andrew Rooney, Arash Yazdani, Vasu Gupta, Thomas Ganley
 * @date 2020-08-09
 */
#include <stdlib.h>
#include "adcs_io.h"
#include "adcs_types.h"

static QueueHandle_t adcsQueue;
static uint8_t adcsBuffer;
static SemaphoreHandle_t tx_semphr;
static SemaphoreHandle_t uart_mutex;

static void adcs_byte_stuff(uint8_t *thin_cmd, uint8_t *stuffed_cmd, uint8_t thin_length, uint8_t *stuffed_length);
static void adcs_byte_destuff(uint8_t *stuffed_reply, uint8_t *thin_reply, uint8_t stuffed_length, uint8_t *thin_length);

/**
 * @Brief
 *      Initialize ADCS driver
 * @return ADCS_returnState
 *      Whether or not the driver initialized correctly
 */
ADCS_returnState init_adcs_io() {
    tx_semphr = xSemaphoreCreateBinary();

    if (tx_semphr == NULL) {
        return ADCS_UART_FAILED;
    }

    adcsQueue = xQueueCreate(ADCS_QUEUE_LENGTH, ADCS_QUEUE_ITEM_SIZE);
    if(adcsQueue == NULL){
        return ADCS_UART_FAILED;
    }

    uart_mutex = xSemaphoreCreateMutex();
    if (uart_mutex == NULL) {
        return ADCS_UART_FAILED;
    }
    adcsBuffer = 0;
    sciReceive(ADCS_SCI, 1, &adcsBuffer);
}

/**
 * @Brief
 *      sciNotification for ADCS_SCI. Handles adding to receive queue
 */
void adcs_sciNotification(sciBASE_t *sci, int flags) {
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    switch (flags) {
    case SCI_RX_INT:
        xQueueSendToBackFromISR(adcsQueue, &adcsBuffer, &xHigherPriorityTaskWoken);
        sciReceive(sci, 1, &adcsBuffer);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        break;
    case SCI_TX_INT:
        xSemaphoreGiveFromISR(tx_semphr, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        break;
    default:
        break;
    }
}

/**
 * @brief
 *      Send telecommand via UART protocol
 * @param command
 *      Telecommand frame
 * @param length
 *      Length of the data (in bytes)
 *
 */
ADCS_returnState send_uart_telecommand(uint8_t *command, uint32_t length) {
    if (xSemaphoreTake(uart_mutex, UART_TIMEOUT_MS) != pdTRUE) {
        return ADCS_UART_BUSY;
    } //  TODO: create response if it times out.

    //Stuff the command
    uint8_t stuffed_length = length;
    uint8_t *stuffed_command;
    adcs_byte_stuff(command, stuffed_command, length, &stuffed_length);

    // Form the command frame
    uint8_t *frame = (uint8_t *)pvPortMalloc((stuffed_length + ADCS_TC_HEADER_SZ) * sizeof(uint8_t));
    if (frame == NULL) return ADCS_MALLOC_FAILED;
    *frame = ADCS_ESC_CHAR;
    *(frame + 1) = ADCS_SOM;
    memcpy((frame + 2), stuffed_command, stuffed_length);
    *(frame + stuffed_length + 2) = ADCS_ESC_CHAR;
    *(frame + stuffed_length + 3) = ADCS_EOM;
    vPortFree(stuffed_command);

    // Send the command frame
    sciSend(ADCS_SCI, stuffed_length + ADCS_TC_HEADER_SZ, frame);
    vPortFree(frame);

    if (xSemaphoreTake(tx_semphr, UART_TIMEOUT_MS) != pdTRUE) {
        xSemaphoreGive(uart_mutex);
        return ADCS_UART_FAILED;
    } // TODO: create response if it times out.

    // Receive the reply
    int received = 0;
    uint8_t reply[ADCS_TC_ANS_LEN];

    while (received < ADCS_TC_ANS_LEN) {
        if (xQueueReceive(adcsQueue, reply + received, UART_TIMEOUT_MS) == pdFAIL) {
            xSemaphoreGive(uart_mutex);
            return ADCS_UART_FAILED;
        } else {
            received++;
        }
    }

    ADCS_returnState TC_err_flag = (ADCS_returnState) reply[3];
    xSemaphoreGive(uart_mutex);
    xQueueReset(adcsQueue);
    return TC_err_flag;
}

/**
 * @brief
 *      Send telecommand via I2C protocol
 * @param command
 *      Telecommand frame
 * @param length
 *      Length of the data (in bytes)
 *
 */
ADCS_returnState send_i2c_telecommand(uint8_t *command, uint32_t length) {
    // Send telecommand
    i2c_Send(ADCS_I2C, ADCS_I2C_ADDR, length, command);

    // Poll TC Acknowledge Telemetry Format until the Processed flag equals 1.
    bool processed = false;
    uint8_t tc_ack[4];
    while (!processed) {
        request_i2c_telemetry(LAST_TC_ACK_ID, tc_ack, 4);
        processed = tc_ack[1] & 1;
    }

    // Confirm telecommand validity by checking the TC Error flag of the last read TC Acknowledge Telemetry Format.
    request_i2c_telemetry(LAST_TC_ACK_ID, tc_ack, 4);
    ADCS_returnState TC_err_flag = (ADCS_returnState) tc_ack[2];

    return TC_err_flag;
}

/**
 * @brief
 *      Request and receive telemetry via UART protocol
 * @param TM_ID
 *      Telemetry ID byte
 * @param telemetry
 *    Received telemetry data
 * @param expected_len
 *      Expected length of the data (in bytes)
 *
 */
ADCS_returnState request_uart_telemetry(uint8_t TM_ID, uint8_t *telemetry, uint32_t length) {
    if (xSemaphoreTake(uart_mutex, UART_TIMEOUT_MS) != pdTRUE) {
        return ADCS_UART_BUSY;
    }

    // Form the command frame
    uint8_t frame[ADCS_TM_HEADER_SZ];
    frame[0] = ADCS_ESC_CHAR;
    frame[1] = ADCS_SOM;
    frame[2] = TM_ID;
    frame[3] = ADCS_ESC_CHAR;
    frame[4] = ADCS_EOM;

    // Send the command frame
    sciSend(ADCS_SCI, ADCS_TM_HEADER_SZ, frame);
    if (xSemaphoreTake(tx_semphr, UART_TIMEOUT_MS) != pdTRUE) {
        xSemaphoreGive(uart_mutex);
        return ADCS_UART_FAILED;
    }

    int received = 0;

    uint8_t *reply = (uint8_t *)pvPortMalloc(length + ADCS_TM_HEADER_SZ);
    if (reply == NULL){
        xSemaphoreGive(uart_mutex);
        return ADCS_MALLOC_FAILED;
    }

    bool end_of_message = false;
    const uint8_t ending_bytes[ADCS_NUM_ENDING_BYTES] = {ADCS_PARSING_BYTE, ADCS_ENDING_BYTE};
    uint8_t ending_bytes_index = 0;

    while (!end_of_message) {
        if (xQueueReceive(adcsQueue, reply + received, UART_TIMEOUT_MS) == pdFAIL) {
            xSemaphoreGive(uart_mutex);
            return ADCS_UART_FAILED;
        } else {
            // Check for EOM
            if(*(reply + received) = ending_bytes[ending_bytes_index]){
                ending_bytes_index++;

                if(ending_bytes_index == ADCS_NUM_ENDING_BYTES) end_of_message = true;
            }
            received++;
        }
    }

    // Destuff the reply
    uint8_t *thin_reply = (uint8_t *)pvPortMalloc((received - ADCS_TM_HEADER_SZ) * sizeof(uint8_t));
    uint8_t thin_length;
    adcs_byte_destuff((reply + ADCS_TM_DATA_INDEX), thin_reply, received - ADCS_TM_HEADER_SZ, &thin_length);

    for (int i = 0; i < thin_length; i++) {
        *(telemetry + i) = *(thin_reply);
    }
    vPortFree(reply);
    vPortFree(thin_reply);
    xSemaphoreGive(uart_mutex);
    xQueueReset(adcsQueue);
    return ADCS_OK;
}

/**
 * @brief
 *      Receive packet sent by ADCS from file download request
 * @param hole_map
 *      Map that captures which packets have been sent, and which have not
 * @param image_bytes
 *    the actual image data
 *
 */
ADCS_returnState receive_file_download_uart_packet(uint8_t *packet, uint16_t *packet_counter) {
    if (xSemaphoreTake(uart_mutex, UART_TIMEOUT_MS) != pdTRUE) {
        return ADCS_UART_BUSY;
    }

    int received = 0;
    uint8_t reply[ADCS_UART_FILE_DOWNLOAD_PKT_LEN] = {0};

    // Receive a UART file download packet
    while (received < (ADCS_UART_FILE_DOWNLOAD_PKT_LEN)) {
        uint8_t retries = 0;
        if (xQueueReceive(adcsQueue, reply + received, 500) == pdFAIL) {
            retries++;
            if (retries >= ADCS_UART_FILE_DOWNLOAD_PKT_RETRIES) {
                xSemaphoreGive(uart_mutex);
                return ADCS_UART_FAILED;
            }
        } else {
            received++;
        }
    }
    // First byte in packet is file download burst ID = 119
    // Second and third bytes are the packet counter
    *packet_counter = (reply[4] << 8) | reply[3];

    memcpy(packet, &reply[5], ADCS_UART_FILE_DOWNLOAD_PKT_DATA_LEN);

    xSemaphoreGive(uart_mutex);
    return ADCS_OK;
}

/**
 * @brief
 *      Stuffs 0x1F data bytes
 * @param thin_cmd
 *      Command before byte stuffing (input)
 * @param stuffed_cmd
 *      Command after byte stuffing (output)
 * @param thin_length
 *      Length of original command
 * @param stuffed_length
 *      Length of output command
 *
 */
static void adcs_byte_stuff(uint8_t *thin_cmd, uint8_t *stuffed_cmd, uint8_t thin_length, uint8_t *stuffed_length){
    *stuffed_length = thin_length;
    for(int i = 0; i < thin_length; i++){
        if(*(thin_cmd + i) == ADCS_PARSING_BYTE) stuffed_length++;
    }

    stuffed_cmd = (uint8_t *)pvPortMalloc(*stuffed_length * sizeof(uint8_t));

    uint8_t stuffed_index = 0;
    for(uint8_t thin_index = 0; thin_index < thin_length; thin_index++){

        *(stuffed_cmd + stuffed_index) = *(thin_cmd + thin_index);
        stuffed_index++;

        if(*(thin_cmd + thin_index) == ADCS_PARSING_BYTE){
            /* Byte needs to be stuffed */
            *(stuffed_cmd + stuffed_index) = ADCS_PARSING_BYTE;
            stuffed_index++;
        }
    }
    *stuffed_length = stuffed_index;
}

/**
 * @brief
 *      Destuffs 0x1F data bytes
 * @param stuffed reply
 *      Reply directly from the ADCS (input)
 * @param thin_reply
 *      Reply after byte destuffing (output)
 * @param stuffed_length
 *      Length of original reply
 * @param thin_length
 *      Length of destuffed reply
 *
 */
static void adcs_byte_destuff(uint8_t *stuffed_reply, uint8_t *thin_reply, uint8_t stuffed_length, uint8_t *thin_length){
    uint8_t thin_index = 0;
    for(uint8_t stuffed_index = 0; stuffed_index < stuffed_length; stuffed_index++){
        *(thin_reply + thin_index) = *(stuffed_reply + stuffed_index);
        thin_index++;

        if(*(stuffed_reply + stuffed_index) == ADCS_PARSING_BYTE){
            /* Stuffed byte present. Skip the next byte */
            stuffed_index++;
        }
    }
    *thin_length = thin_index;
}

void write_packet_to_file(uint32_t file_des, uint8_t *packet_data, uint8_t length) {
    // Write data to file
    int32_t iErr = red_write(file_des, packet_data, length);
    if (iErr == -1) {
        printf("Unexpected error %d from red_write() in write_pkt_to_file()\r\n", (int)red_errno);
        exit(red_errno);
    }
}

/**
 * @brief
 *      Request and receive telemetry via I2C protocol
 * @param TM_ID
 *      Telemetry ID byte
 * @param telemetry
 *    Received telemetry data
 * @param length
 *      Length of the data (in bytes)
 *
 */
ADCS_returnState request_i2c_telemetry(uint8_t TM_ID, uint8_t *telemetry, uint32_t length) {
    i2c_Receive(ADCS_I2C, TM_ID, length, telemetry);

    // Read error flag from Communication Status telemetry frame
    // to determine if an incorrect number of bytes are read.
    // Should this check be done here?
    // (Refer to CubeADCS Firmware Manual section 5.3.1)

    // uint8_t err_reply[6];
    // i2c_receive(err_reply, COMMS_STAT_ID, 6);
    // uint8_t TL_err_flag = (err_reply[4] >> 3) & 1;

    return ADCS_OK;
}
