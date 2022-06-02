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

#include "iris_spi.h"
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

void iris_init() {
    // Populate SPI config
    dataconfig.CS_HOLD = FALSE;
    dataconfig.WDEL = 0;
    dataconfig.DFSEL = SPI_FMT_0;
    dataconfig.CSNR = SPI_CS_1;

    gioSetDirection(hetPORT1, 0xFFFFFFFF);
}


void spi_send_and_get(uint16_t *tx_data, uint16_t *rx_data, uint16_t data_length) {
    spiSendAndGetData(IRIS_SPI, &dataconfig, data_length, tx_data, rx_data);
    while ((SpiTxStatus(IRIS_SPI) != SPI_COMPLETED) && (SpiRxStatus(IRIS_SPI) != SPI_COMPLETED));
}

void spi_send_byte(uint16_t *tx_data) {
    spiSendData(IRIS_SPI, &dataconfig, TRANSFER_SIZE, tx_data);
    while(SpiTxStatus(IRIS_SPI) != SPI_COMPLETED);
}

void spi_get_byte(uint16_t *rx_data) {
    spiGetData(IRIS_SPI, &dataconfig, TRANSFER_SIZE, rx_data);
    while(SpiRxStatus(IRIS_SPI) != SPI_COMPLETED);
}

/* Sudo delay created due to lack of TI's HAL Delay functionality
 * Each for-loop iteration is approximately 11 clock cycles:
 *  Given base clock speed to be 300 MHz, each second of delay will take
 *  (300M/11 = 27272727 iterations)
 */
void spi_delay(uint16_t timeout) {
    uint16_t i;
    for (i = 0; i < timeout; i++);
}

/* Sends two bytes:
 *  1st byte is the command
 *  2nd byte is a dummy byte to retrieve the ACK
 */
int send_command(uint16_t command) {
    uint16_t rx_data;
    uint16_t tx_dummy = DUMMY_BYTE;

    NSS_LOW();
    spi_delay(1000);
    spi_send_byte(&command);
    spi_delay(1000);
    spi_send_byte(&tx_dummy);
    spi_get_byte(&rx_data);
    spi_delay(1000);
    NSS_HIGH();

    if (rx_data == ACK_FLAG) {
        return 0;
    }
    return -1;
}

/* TODO: This functionality is still to be testes */
int send_data(uint16_t *tx_buffer, uint16_t data_length) {
    uint16_t rx_data = 0x00;
    if (rx_data != ACK_FLAG) {
        return -1; // Bad return
    }
    return 0; // Good return
}

uint16_t * get_data(uint16_t data_length) {
    // Transmit dummy data and receive incoming data
    uint16_t data_byte = 0xDD;
    uint16_t *dummy_buffer;
    uint16_t *rx_buffer;

    rx_buffer = (uint16_t*) calloc(data_length, sizeof(uint16_t));
    if (rx_buffer == NULL) {
        // TODO: Log error
        return;
    }
    memset(rx_buffer, 0, data_length);

    dummy_buffer = (uint16_t*) calloc(data_length, sizeof(uint16_t));
    if (dummy_buffer == NULL) {
        // TODO: Log error
        return;
    }
    for (int i = 0; i < data_length; i++) {
        dummy_buffer[i] = DUMMY_BYTE;
    }


    NSS_LOW();
    spi_delay(1000);
    spi_send_byte(&data_byte);
    spi_delay(1000);
    spi_send_and_get(dummy_buffer, rx_buffer, data_length);
    while ((SpiTxStatus(IRIS_SPI) != SPI_COMPLETED) && (SpiRxStatus(IRIS_SPI) != SPI_COMPLETED));
    spi_delay(1000);
    NSS_HIGH();

    /* Good amount of time for slave (iris) to finish and reset it's spi state */
    vTaskDelay(2000);

    return rx_buffer;

}
