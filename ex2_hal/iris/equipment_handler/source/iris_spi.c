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

/**
 * Optimization points:
 * - iris_spi_delay(timeout) is put in place for timing control although
 *   it is not tested for optimal control, rather it is set so that
 *   interfacing with iris can be more flexible. [removed spi delay]
 * - sending commands, data and getting data can be further modified
 *   for more robust communication protocol. Current state allows us
 *   to have multi-byte communication with Iris at a moderate speed
 *   and high data integrity. This include adding additional error
 *   handling to ensure that any unexpected behavior is logged, and
 *   both Athena and Iris returns to a stable state
 */

#include "FreeRTOS.h"
#include "os_semphr.h"
#include <stdlib.h>

#include "iris_spi.h"
#include "iris_gio.h"
#include "iris.h"
#include "HL_spi.h"
#include "system.h"

static SemaphoreHandle_t iris_spi_mutex;

spiDAT1_t dataconfig;

/**
 * @brief
 *   Initialize SPI data configurations (e.g. SPI data format)
 **/
IrisLowLevelReturn iris_spi_init() {
    // Populate SPI config
    dataconfig.CS_HOLD = FALSE;
    dataconfig.WDEL = 0;
#if IS_ATHENA == 1
    dataconfig.DFSEL = SPI_FMT_0; // spiREG4->FMT_0
#else
    dataconfig.DFSEL = SPI_FMT_2; // spiREG3->FMT_2
#endif
    dataconfig.CSNR = SPI_CS_1;

    iris_spi_mutex = xSemaphoreCreateMutex();
    if (iris_spi_mutex == NULL) {
        return IRIS_LL_ERROR;
    }
    return IRIS_LL_OK;
}

/**
 * @brief
 *   Transmits and receives bytes via the SPI bus
 *
 * @param[in] tx_data
 *   Pointer to transmit data buffer
 *
 * @param[in] rx_data
 *   Pointer to receive data buffer
 *
 * @param[in] data_length
 *   Number of bytes to transmit and receive
 *
 **/
void iris_spi_send_and_get(uint16_t *tx_data, uint16_t *rx_data, uint16_t data_length) {
    spiSendAndGetData(IRIS_SPI, &dataconfig, data_length, tx_data, rx_data);
    while ((SpiTxStatus(IRIS_SPI) != SPI_COMPLETED) && (SpiRxStatus(IRIS_SPI) != SPI_COMPLETED))
        ;
}

/**
 * @brief
 *   Transmits bytes via the SPI bus
 *
 * @param[in] tx_data
 *   Pointer to transmit data buffer
 *
 * @param[in] data_length
 *   Number of bytes to transmit
 *
 **/
void iris_spi_send(uint16_t *tx_data, uint16_t data_length) {
    spiSendData(IRIS_SPI, &dataconfig, data_length, tx_data);
    while (SpiTxStatus(IRIS_SPI) != SPI_COMPLETED)
        ;
}

/**
 * @brief
 *   Receives bytes via the SPI bus
 *
 * @param[in] rx_data
 *   Pointer to receive data buffer
 *
 * @param[in] data_length
 *   Number of bytes to receive
 *
 **/
void iris_spi_get(uint16_t *rx_data, uint16_t data_length) {
    spiGetData(IRIS_SPI, &dataconfig, data_length, rx_data);
    while (SpiRxStatus(IRIS_SPI) != SPI_COMPLETED)
        ;
}

/**
 * @brief
 *   Transmit a command packet of size 2:
 *      1st Byte: command byte
 *      2nd Byte: dummy byte to receive ACK/NACK
 *
 * @param[in] command
 *   Legal Iris command to start activity
 *
 * @return
 *   Returns IRIS_LL_OK if command was acknowledge, IRIS_LL_FAIL if not
 **/
IrisLowLevelReturn iris_send_command(uint16_t command) {
    if (xSemaphoreTake(iris_spi_mutex, IRIS_SPI_MUTEX_TIMEOUT) != pdTRUE) {
        return IRIS_SPI_BUSY;
    }

    uint16_t rx_data;
    uint16_t tx_dummy = DUMMY_BYTE;

    iris_nss_low();
    vTaskDelay(1);
    iris_spi_send(&command, 1);
    vTaskDelay(1);
    iris_spi_get(&rx_data, 1);
    /* This delay is modifiable and will depend on how fast Iris
     * can switch from running idle/background tasks to receiving
     * obc command and returning ACK
     */
    IRIS_WAIT_FOR_ACK;
    iris_spi_send(&tx_dummy, 1);
    vTaskDelay(1);
    iris_spi_get(&rx_data, 1);
    vTaskDelay(1);
    iris_nss_high();

    xSemaphoreGive(iris_spi_mutex);
    if (rx_data == ACK_FLAG) {
        return IRIS_LL_OK;
    } else if (rx_data == NACK_FLAG) {
        return IRIS_LL_FAIL;
    } else {
        return IRIS_LL_ERROR;
    }
}

/**
 * @brief
 *   Transmit data of size data_length:
 *      Byte N: Transmit data to Iris
 *
 * @param[in] tx_buffer
 *   Pointer to transmit data buffer
 *
 * @param[in] data_length
 *   Number of bytes to transmit
 *
 * @return
 *   Returns IRIS_LL_OK if ACK is recieved, IRIS_LL_FAIL if not
 **/
IrisLowLevelReturn iris_send_data(uint16_t *tx_buffer, uint16_t data_length) {
    if (xSemaphoreTake(iris_spi_mutex, IRIS_SPI_MUTEX_TIMEOUT) != pdTRUE) {
        return IRIS_SPI_BUSY;
    }

    iris_nss_low();
    vTaskDelay(1);
    iris_spi_send(tx_buffer, data_length);
    vTaskDelay(1);
    iris_nss_high();

    xSemaphoreGive(iris_spi_mutex);
    return IRIS_LL_OK;
}

/**
 * @brief
 *   Receives data of size data_length:
 *      Byte N: Transmit dummy data and receive Iris data
 *
 * @param[in] rx_buffer
 *   Pointer to receive data buffer
 *
 * @param[in] data_length
 *   Number of bytes to receive
 *
 * @return
 *   Returns TODO: Need to figure out a better return type
 **/
IrisLowLevelReturn iris_get_data(uint16_t *rx_buffer, uint16_t data_length) {
    if (xSemaphoreTake(iris_spi_mutex, IRIS_SPI_MUTEX_TIMEOUT) != pdTRUE) {
        return IRIS_SPI_BUSY;
    }

    uint16_t tx_dummy = 0xFF;

    iris_nss_low();
    vTaskDelay(1);
    iris_spi_send_and_get(&tx_dummy, rx_buffer, data_length);
    vTaskDelay(1);
    iris_nss_high();

    xSemaphoreGive(iris_spi_mutex);
    return IRIS_LL_OK;
}
