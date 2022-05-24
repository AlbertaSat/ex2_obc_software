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

/*
 * We can defined acknowledgments as:
 * GOOD: If ACK FLAG is successfully returned
 * BAD: If nothing (wait X ms) or other than ACK_FLAG is returned
 */

#include "FreeRTOS.h"
#include "os_timer.h"

#include "iris_spi.h"
#include "HL_spi.h"
#include "system.h"

#define TIMER_ID 1

spiDAT1_t dataconfig;

static TimerHandle_t xCommandTimer = NULL;
static BaseType_t is_timer_expired(TimerHandle_t xTimer);
static void command_timer_callback(TimerHandle_t pxTimer);
const TickType_t x1second = pdMS_TO_TICKS( 1000UL );

/* TODO
 * - Add synchronization points (i.e. semaphores, mutex) once prelim testing completed
 */

void iris_init() {
    // Initialize timer CHANGED TIMER PRIORITY IN FREERTOS CONFIG FILE!!!!!!!!
    xCommandTimer = xTimerCreate( (const char *) "Command_Timer",
                            x1second,
                            pdFALSE,
                            (void *) TIMER_ID,
                            command_timer_callback);

    configASSERT(xCommandTimer);

    // Populate SPI config
    dataconfig.CS_HOLD = FALSE;
    dataconfig.WDEL = 2;
    dataconfig.DFSEL = SPI_FMT_0;
    dataconfig.CSNR = SPI_CS_1;
}

void spi_send_and_get(uint16_t *tx_data, uint16_t *rx_data) {
    // SPI master send 1BYTE and read 1BYTE HAL function
    spiSendAndGetData(IRIS_SPI, &dataconfig, TRANSFER_SIZE, tx_data, rx_data);
    while(SpiTxStatus(IRIS_SPI) != SPI_COMPLETED);
}

int send_command(uint16_t command) {
    // Transmit specified command
    uint16_t rx_data;

    xTimerStart(xCommandTimer, 0);

    while (is_timer_expired(xCommandTimer) != pdTRUE) {
        spi_send_and_get(&command, &rx_data);

        // Check acknowledgment
        if (rx_data == ACK_FLAG) {
            xTimerStop(xCommandTimer, 0);
            return 0;
        }

        vTaskDelay(10);
    }

    xTimerReset(xCommandTimer, 0);
    return -1;
}

int send_data(uint16_t *tx_buffer, uint16_t data_length) {
    uint16_t rx_data = 0x00;
    // Transmit data in tx_buffer
    //spi_write_read(tx_buffer, rx_data);

    // Check acknowledgment
    if (rx_data != ACK_FLAG) {
        return -1; // Bad return
    }
    return 0; // Good return
}

uint16_t * get_data(uint16_t data_length) {
    // Transmit dummy data and receive incoming data
    uint16_t i;
    uint16_t dummy = 0xDD;
    uint16_t rx_data;
    uint16_t *rx_buf;

    rx_buf = (uint16_t*) calloc(data_length, sizeof(uint16_t));

    if (rx_buf == NULL) {
        // TODO: Log error
    }

    for (i = 0; i < data_length; i++) {
        spi_send_and_get(&dummy, &rx_data);
        while(SpiTxStatus(spiREG3) != SPI_COMPLETED);

        *(rx_buf + i) = rx_data;
        vTaskDelay(100);
    }

    return rx_buf;
}

static BaseType_t is_timer_expired(TimerHandle_t xTimer) {
     if( xTimerIsTimerActive( xTimer ) != pdFALSE ) {
         return pdFALSE;
     }
     return pdTRUE;
}

static void command_timer_callback( TimerHandle_t xTimer ) {
    return; // TODO: Log athena failed to communicate with iris
}
