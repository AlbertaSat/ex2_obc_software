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
 * @author Andrew Rooney, Arash Yazdani, Vasu Gupta
 * @date 2020-08-09
 */

#include "adcs_io.h"
#include "adcs_types.h"

#include "FreeRTOS.h"
#include "HL_sci.h"
#include "i2c_io.h"
#include "os_queue.h"
#include "os_semphr.h"
#include "os_task.h"
#include "system.h"
#include <stdbool.h>
#include <string.h>

#define ADCS_QUEUE_LENGTH 100
#define ITEM_SIZE 1

static QueueHandle_t adcsQueue;
static uint8_t adcsBuffer;
static SemaphoreHandle_t tx_semphr;
static SemaphoreHandle_t uart_mutex;

/**
 * @Brief
 *      Initialize ADCS driver
 */
void init_adcs_io() {
    tx_semphr = xSemaphoreCreateBinary();
    adcsQueue = xQueueCreate(ADCS_QUEUE_LENGTH, ITEM_SIZE);
    uart_mutex = xSemaphoreCreateMutex();
    adcsBuffer = 0;
    sciReceive(ADCS_SCI, 1, &adcsBuffer);
}

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
 * 		Send telecommand via UART protocol
 * @param command
 * 		Telecommand frame
 * @param length
 * 		Length of the data (in bytes)
 *
 */
ADCS_returnState send_uart_telecommand(uint8_t *command, uint32_t length) {
    if(xSemaphoreTake(uart_mutex, UART_TIMEOUT_MS) != pdTRUE) {
          return ADCS_UART_FAILED;
    } //  TODO: create response if it times out.

    uint8_t *frame = (uint8_t *)pvPortMalloc(sizeof(uint8_t)*(length+4));
    *frame = ADCS_ESC_CHAR;
    *(frame+1) = ADCS_SOM;
    memcpy((frame+2), command, length);
    *(frame+length+2) = ADCS_ESC_CHAR;
    *(frame+length+3) = ADCS_EOM;

    // Note TC_ID here is included in the command
    memcpy(&frame[2], &command, length);
    frame[length + 2] = ADCS_ESC_CHAR;
    frame[length + 3] = ADCS_EOM;
    sciSend(ADCS_SCI, frame, length+4);
    xSemaphoreTake(tx_semphr, portMAX_DELAY); // TODO: make a reasonable timeout

    int received = 0;
    uint8_t reply[6] = {1};

    xQueueReset(adcsQueue);

    while (received < 6) {
        xQueueReceive(adcsQueue, reply+received, UART_TIMEOUT_MS); // TODO: create response if it times out.
        received++;
    }
    ADCS_returnState TC_err_flag = reply[3];
    xSemaphoreGive(uart_mutex);
    return TC_err_flag;
}

/**
 * @brief
 * 		Send telecommand via I2C protocol
 * @param command
 * 		Telecommand frame
 * @param length
 * 		Length of the data (in bytes)
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
    ADCS_returnState TC_err_flag = tc_ack[2];

    return TC_err_flag;
}

/**
 * @brief
 * 		Request and receive telemetry via UART protocol
 * @param TM_ID
 * 		Telemetry ID byte
 * @param telemetry
 *    Received telemetry data
 * @param length
 * 		Length of the data (in bytes)
 *
 */
ADCS_returnState request_uart_telemetry(uint8_t TM_ID, uint8_t *telemetry, uint32_t length) {
    if(xSemaphoreTake(uart_mutex, UART_TIMEOUT_MS) != pdTRUE){
        return ADCS_UART_FAILED;
    }

    uint8_t frame[5];
    frame[0] = ADCS_ESC_CHAR;
    frame[1] = ADCS_SOM;
    frame[2] = TM_ID;
    frame[3] = ADCS_ESC_CHAR;
    frame[4] = ADCS_EOM;

    sciSend(ADCS_SCI, 5, frame);
    if(xSemaphoreTake(tx_semphr, UART_TIMEOUT_MS) != pdTRUE){
        return ADCS_UART_FAILED;
    }

    int received = 0;
    uint8_t reply[length + 5];

    xQueueReset(adcsQueue);

    while (received < length + 5) {
        xQueueReceive(adcsQueue, reply+received, UART_TIMEOUT_MS); //  TODO: add error handling
        received++;
    }

    for (int i = 0; i < length; i++) {
        *(telemetry + i) = reply[3 + i];
    }
    xSemaphoreGive(uart_mutex);

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
void receieve_uart_packet(uint8_t *hole_map, uint8_t *image_bytes) {
    int received = 0;
    uint16_t pixel = 0;
    uint8_t reply[22+5] = {0};

    while (received < (22+5)) {
        xQueueReceive(adcsQueue, &(reply[received]), UART_TIMEOUT_MS); // TODO: exit function once timeout finishes
        received++;
    }
    pixel = reply[2] << 8 | reply[3];
    *hole_map = *hole_map | 0x1 << pixel;
    for (int i = 0; i < 20; i++) {
        *(image_bytes + pixel + i) = reply[4 + i];
    }
    xSemaphoreGive(uart_mutex);
}

/**
 * @brief
 * 		Request and receive telemetry via I2C protocol
 * @param TM_ID
 * 		Telemetry ID byte
 * @param telemetry
 *    Received telemetry data
 * @param length
 * 		Length of the data (in bytes)
 *
 */
ADCS_returnState request_i2c_telemetry(uint8_t TM_ID, uint8_t *telemetry,
                                       uint32_t length)
{
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
