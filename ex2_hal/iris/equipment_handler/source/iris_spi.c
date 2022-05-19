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

spiBASE_t * spi_handle;
spiDAT1_t dataconfig;

uint16_t tx_data;
uint16_t rx_data[];

/* TODO
 * - Add synchronization points (i.e. semaphores, mutex) once prelim testing completed
 */

void iris_init() {
    // Assign spi handle
    spi_handle = IRIS_SPI;

    // Populate SPI config
    dataconfig.CS_HOLD = FALSE;
    dataconfig.WDEL = FALSE; // not sure
    dataconfig.DFSEL = SPI_FMT_0;
    dataconfig.CSNR = SPI_CS_1;
}

void spi_write(uint16_t data_length, uint16_t *pTxData) { // TODO: Add data length param
    // SPI master write HAL function
    spiSendData(spi_handle, &dataconfig, data_length, pTxData);
}

void spi_read(uint16_t data_length, uint16_t pRxData[]) { // TODO: Add data length param
    // SPI master read HAL function
    spiGetData(spi_handle, &dataconfig, data_length, pRxData);
}

void spi_write_read(uint16_t data_length, uint16_t *pTxData, uint16_t pRxData[]) { // TODO: Add data length param
    // SPI master write and read HAL function
    spiSendAndGetData(spiREG3, &dataconfig, data_length, pTxData, pRxData);
}

/*  Simple verification test to ensure connection established (Optional)
 *  TODO: Possible name change: test_connection(), start_connection() ??*/
int verify_connection() {
    // Transmit verification byte
    tx_data = VERIFY_FLAG;
    spi_write_read(COMMAND_BLOCKSIZE, &tx_data, rx_data);

    // Check received byte
    if (rx_data[0] != VERIFY_FLAG) {
        return -1; // Bad return
    }
    return 0; // Good return

}

int send_command(uint8_t command) {
    // Transmit specified command
    tx_data = command;
    spi_write_read(COMMAND_BLOCKSIZE, &tx_data, rx_data);

    // Check acknowledgment
    if (rx_data[0] != ACK_FLAG) {
        return -1; // Bad return
    }
    return 0; // Good return
}

int send_data(uint16_t *tx_buffer, uint16_t data_length) {
    // Transmit data in tx_buffer
    spi_write_read(data_length, tx_buffer, rx_data);

    // Check acknowledgment
    if (rx_data[0] != ACK_FLAG) {
        return -1; // Bad return
    }
    return 0; // Good return
}

uint16_t * get_data(uint16_t data_length) {
    // Transmit dummy data and receive incoming data
    uint16_t dummy_data = 0xDD;
    uint16_t rx_buffer[4];
    uint16_t dummy_buffer[4] = {0xDD, 0xDD, 0xDD, 0xDD};
    uint16_t i = 0;

//    for (i = 0; i < data_length; i++) {
//        dummy_buffer[i] = dummy_data;
//    }

    for (i = 0; i < 16; i++) {
        spiSendAndGetData(spiREG3, &dataconfig, 1, dummy_buffer, rx_buffer);
        vTaskDelay(1);
        //while (SpiTxStatus(spi_handle) == SPI_PENDING);
    }

    //spiGetData(spiREG3, &dataconfig, 4, rx_buffer);

    // TODO: Problem; without delay the last byte going out is not the dummy byte
//    for (i = 0; i < data_length; i++) {
//        vTaskDelay(1);
//        spiSendAndGetData(spiREG3, &dataconfig, 1, &dummy_data, rx_data);
//    }

    //spi_write_read(data_length, dummy, rx_data); //TODO: Wrapper function


    return rx_data;
}
