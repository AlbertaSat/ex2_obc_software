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
#include "HL_gio.h"
#include "HL_reg_het.h"

#define TIMER_ID 1

spiDAT1_t dataconfig;

static TimerHandle_t xCommandTimer = NULL;
static BaseType_t is_timer_expired(TimerHandle_t xTimer);
static void command_timer_callback(TimerHandle_t pxTimer);
const TickType_t x1second = pdMS_TO_TICKS( 1000UL );

/* TODO
 * - Add synchronization points (i.e. semaphores, mutex) once prelim testing completed
 */

void NSS_LOW() {
    gioSetBit(hetPORT1, 8, 0);
}

void NSS_HIGH() {
    gioSetBit(hetPORT1, 8, 1);
}

void iris_init() {

    // Populate SPI config
    dataconfig.CS_HOLD = FALSE;
    dataconfig.WDEL = 2;
    dataconfig.DFSEL = SPI_FMT_0;
    dataconfig.CSNR = SPI_CS_1;

    gioSetDirection(hetPORT1, 0xFFFFFFFF);

    int i = 0;
    int j = 0;
    uint16_t rx_data;

    vTaskDelay(100);

        //xTimerStart(xCommandTimer, 0);

//        // TODO: Send pre-amble
//        uint16_t preamble = 0xAB;
//        for (i = 0; i < 30; i++) {
//            NSS_LOW();
//
//            for (j = 0; j < 1000; j++);
//            spi_send_and_get(&preamble, &rx_data);
//            vTaskDelay(1);
//
//            NSS_HIGH();
//        }
}

void spi_send_and_get(uint16_t *tx_data, uint16_t *rx_data) {
    spi_send(tx_data);
    spi_get(rx_data);
}

void spi_send(uint16_t *tx_data) {
    spiSendData(IRIS_SPI, &dataconfig, TRANSFER_SIZE, tx_data);
    while(SpiTxStatus(IRIS_SPI) != SPI_COMPLETED);
}

void spi_get(uint16_t *rx_data) {
    spiGetData(IRIS_SPI, &dataconfig, TRANSFER_SIZE, rx_data);
    while(SpiRxStatus(IRIS_SPI) != SPI_COMPLETED);
}

void spi_delay(uint16_t timeout) {
    uint16_t i;
    for (i = 0; i < timeout; i++);
}

int send_command(uint16_t command) {
    uint16_t rx_data;
    uint16_t tx_dummy = 0xFF;

    NSS_LOW();
    spi_delay(10000);
    spi_send(&command);
    spi_delay(10000);
    spi_send_and_get(&tx_dummy, &rx_data);
    NSS_HIGH();

    if (rx_data == ACK_FLAG) {
        return 0;
    }
    return -1;
}

//int send_command_1(uint16_t command) {
//    // Transmit specified command
//    uint16_t rx_data;
//    uint16_t rx_dummy;
//    uint16_t rx_buffer[256];
//    uint16_t dummy = 0xFF;
//    uint16_t count = 0;
//    uint16_t count_1 = 0;
//    uint16_t com = 0x20;
//    int ret = -1;
//    int i = 0;
//
//
//    NSS_LOW();
//    while (count < 100) {
//        //NSS_LOW();
//
//        //NSS_HIGH();
//
//        //if (count % 2 == 0) {
//            rx_buffer[count] = rx_data;
////            count_1++;
////        }
//
//        count += 1;
//
//        // Check acknowledgment
//        if (rx_data == ACK_FLAG) {
//            ret = 0;
//            break;
//        }
//
//    }
//    NSS_HIGH();
//
//    //xTimerReset(xCommandTimer, 0);
//    return ret;
//}

int send_data(uint16_t *tx_buffer, uint16_t data_length) {
    uint16_t rx_data = 0x00;


    // Check acknowledgment
    if (rx_data != ACK_FLAG) {
        return -1; // Bad return
    }
    return 0; // Good return
}

uint16_t * get_data(uint16_t data_length) {
    // Transmit dummy data and receive incoming data
    uint16_t i;

    uint16_t start_byte = 0xF0;
    uint16_t data_byte = 0xDD;
    uint16_t dummy = 0xFF;
    uint16_t stop_byte = 0x0F;

    uint16_t rx_data;
    uint16_t *rx_buffer;


    rx_buffer = (uint16_t*) calloc(data_length, sizeof(uint16_t));

    if (rx_buffer == NULL) {
        // TODO: Log error
    }

    NSS_LOW();
    spi_delay(10000);
    spi_send(&start_byte);
    spi_delay(10000);
    spi_send_and_get(&dummy, &rx_data);

    if (rx_data == ACK_FLAG) {
        spi_delay(1000);
        spi_send(&data_byte);
        for (i = 0; i < data_length; i++) {
            spi_delay(1000);
            spi_send_and_get(&data_byte, &rx_data);
            rx_buffer[i] = rx_data;
        }
    }


    spi_delay(10000);
    spi_send(&stop_byte);
    spi_delay(10000);
    spi_send_and_get(&dummy, &rx_data);

    NSS_HIGH();

    if (rx_data == ACK_FLAG) {
        return rx_buffer;
    }
    return NULL;

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
