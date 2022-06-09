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
#include <stdlib.h>

#include "iris_spi.h"
#include "iris.h"
#include "HL_spi.h"
#include "system.h"
#include "HL_gio.h"
#include "HL_reg_het.h"

spiDAT1_t dataconfig;

void NSS_LOW() {
    gioSetBit(hetPORT1, 8, 0);
}

void NSS_HIGH() {
    gioSetBit(hetPORT1, 8, 1);
}

void RESET_IRIS() {
    gioSetBit(hetPORT1, 14, 0);
    vTaskDelay(10);
    gioSetBit(hetPORT1, 14, 1);
}

void iris_init() {
    // Populate SPI config
    dataconfig.CS_HOLD = FALSE;
    dataconfig.WDEL = 0;
    dataconfig.DFSEL = SPI_FMT_0;
    dataconfig.CSNR = SPI_CS_1;

    gioSetDirection(hetPORT1, 0xFFFFFFFF);
}


static void spi_send_and_get(uint16_t *tx_data, uint16_t *rx_data, uint16_t data_length) {
    spiSendAndGetData(IRIS_SPI, &dataconfig, data_length, tx_data, rx_data);
    while ((SpiTxStatus(IRIS_SPI) != SPI_COMPLETED) && (SpiRxStatus(IRIS_SPI) != SPI_COMPLETED));
}

static void spi_send(uint16_t *tx_data, uint16_t data_length) {
    spiSendData(IRIS_SPI, &dataconfig, data_length, tx_data);
    while(SpiTxStatus(IRIS_SPI) != SPI_COMPLETED);
}

static void spi_get(uint16_t *rx_data, uint16_t data_length) {
    spiGetData(IRIS_SPI, &dataconfig, data_length, rx_data);
    while(SpiRxStatus(IRIS_SPI) != SPI_COMPLETED);
}

/* Sudo delay created due to lack of TI's HAL Delay functionality
 * Each for-loop iteration is approximately 11 clock cycles:
 *  Given base clock speed to be 300 MHz, each second of delay will take
 *  (300M/11 = 27272727 iterations)
 */
static void spi_delay(uint16_t timeout) {
    uint16_t i;
    for (i = 0; i < timeout; i++);
}

/* Sends two bytes:
 *  1st byte is the command
 *  2nd byte is a dummy byte to retrieve the ACK
 */
IRIS_return send_command(uint16_t command) {
    uint16_t rx_data;
    uint16_t tx_dummy = DUMMY_BYTE;

    NSS_LOW();
    spi_delay(2727);
    spi_send(&command, 1);
    spi_delay(2727);
    spi_send(&tx_dummy, 1);
    spi_get(&rx_data, 1);
    spi_delay(2722);
    NSS_HIGH();

    if (rx_data == ACK_FLAG) {
        return IRIS_ACK;
    } else if (rx_data == NACK_FLAG) {
        return IRIS_NACK;
    } else {
        return IRIS_ERROR;
    }
}

IRIS_return send_data(uint16_t *tx_buffer, uint16_t data_length) {
    uint16_t tx_dummy = DUMMY_BYTE;
    uint16_t rx_data;

    NSS_LOW();
    spi_delay(1000);
    spi_send_and_get(tx_buffer, &rx_data, data_length);
    spi_delay(1000);
    spi_send(&tx_dummy, 1);
    spi_get(&rx_data, 1);
    spi_delay(1000);
    NSS_HIGH();

    if (rx_data == ACK_FLAG) {
        return IRIS_ACK;
    } else if (rx_data == NACK_FLAG) {
        return IRIS_NACK;
    } else {
        return IRIS_ERROR;
    }
}

IRIS_return get_data(uint16_t *rx_buffer, uint16_t data_length) {
    // Transmit dummy data and receive incoming data
//    uint16_t get_data_byte = 0xDD;
    uint16_t tx_dummy = 0xFF;
    uint16_t *dummy_buffer;

//    dummy_buffer = (uint16_t*) calloc(data_length + 1, sizeof(uint16_t));
//    if (dummy_buffer == NULL) {
//        return IRIS_ERROR;
//    }
//    for (int i = 0; i < (data_length + 1); i++) {
//        dummy_buffer[i] = DUMMY_BYTE;
//    }

    NSS_LOW();
//    spi_delay(1000);
//    spi_send(&get_data_byte, 1);
    spi_delay(10000);
    spi_send_and_get(&tx_dummy, rx_buffer, data_length);
    spi_delay(1000);
    NSS_HIGH();
    spi_delay(1000);

//    free(dummy_buffer);

    return IRIS_OK;
}

//IRIS_return get_image_data(uint16_t *rx_buffer, uint16_t data_length, uint16_t index) {
//    // Transmit dummy data and receive incoming data
//    uint16_t tx_dummy = 0xFF;
//    uint16_t *dummy_buffer;
//
//    NSS_LOW();
//    spi_delay(1000);
//    spi_send(&index, 1); // index is a 16 bit number
//    spi_delay(10000);
//    spi_send_and_get(&tx_dummy, rx_buffer, data_length);
//    spi_delay(1000);
//    NSS_HIGH();
//
////    free(dummy_buffer);
//
//    return IRIS_OK;
//}
