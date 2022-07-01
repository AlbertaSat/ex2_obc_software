/*
 * Copyright (C) 2021  University of Alberta
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
 * @file northern_spirit_io.c
 * @author Thomas Ganley
 * @date 2021-10-22
 */

/**
 * @brief
 *      Sends a command to the Northern SPIRIT payload over UART
 * @param command
 *      Pointer to command array
 * @param length
 *      Length of the command array
 * @return
 *      NS_return
 */

#include "northern_spirit_io.h"

#define NS_QUEUE_LENGTH 100
#define ITEM_SIZE 1

static QueueHandle_t nsQueue;
static uint8_t nsBuffer;
static SemaphoreHandle_t tx_semphr;
static SemaphoreHandle_t uart_mutex;

void init_ns_io() {
    sciSetBaudrate(PAYLOAD_SCI, 9600);
    tx_semphr = xSemaphoreCreateBinary();
    nsQueue = xQueueCreate(NS_QUEUE_LENGTH, ITEM_SIZE);
    uart_mutex = xSemaphoreCreateMutex();
    nsBuffer = 0;
    sciReceive(PAYLOAD_SCI, 1, &nsBuffer);
}

void ns_sciNotification(sciBASE_t *sci, int flags) {
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    switch (flags) {
    case SCI_RX_INT:
        xQueueSendToBackFromISR(nsQueue, &nsBuffer, &xHigherPriorityTaskWoken);
        sciReceive(sci, 1, &nsBuffer);
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

NS_return send_NS_command(uint8_t* command, uint32_t command_length, uint8_t* answer, uint8_t answer_length) {
    if(xSemaphoreTake(uart_mutex, NS_SEMAPHORE_TIMEOUT_MS) != pdTRUE){
        return NS_UART_FAIL;
    }

    sciSend(PAYLOAD_SCI, command_length, command);

    if(xSemaphoreTake(tx_semphr, NS_SEMAPHORE_TIMEOUT_MS) != pdTRUE){
        return NS_UART_FAIL;
    }

    int received = 0;
    uint8_t* reply = (uint8_t *)pvPortMalloc(answer_length*sizeof(uint8_t));

    if (reply == NULL){
        return NS_UART_FAIL;
    }

    while (received < answer_length) {
        if(!xQueueReceive(nsQueue, (reply+received), NS_UART_TIMEOUT_MS))return NS_UART_FAIL;
        received++;
    }

    memcpy(answer, reply, answer_length);

    vPortFree(reply);

    xSemaphoreGive(uart_mutex);
    return NS_OK;
}
