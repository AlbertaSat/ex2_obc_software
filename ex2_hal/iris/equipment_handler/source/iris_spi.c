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
 * - spi_delay(timeout) is put in place for timing control although
 *   it is not tested for optimal control, rather it is set so that
 *   interfacing with iris can be more flexible
 * - sending commands, data and getting data can be further modified
 *   for more robust communication protocol. Current state allows us
 *   to have multi-byte communication with Iris at a moderate speed
 *   and high data integrity. This include adding additional error
 *   handling to ensure that any unexpected behavior is logged, and
 *   both Athena and Iris returns to a stable state
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

/**
 * @brief
 *   Pull slave select low via GPIO pin
 *   TODO: Specify pin #8 in system.h (separate commit)
 **/
void NSS_LOW() {
    gioSetBit(hetPORT1, 8, 0);
}

/**
 * @brief
 *   Pull slave select high via GPIO pin
 **/
void NSS_HIGH() {
    gioSetBit(hetPORT1, 8, 1);
}

/**
 * @brief
 *   Initialize SPI data configurations (e.g. SPI data format)
 **/
void iris_spi_init() {
    // Populate SPI config
    dataconfig.CS_HOLD = FALSE;
    dataconfig.WDEL = 0;
    dataconfig.DFSEL = SPI_FMT_0;
    dataconfig.CSNR = SPI_CS_1;

    gioSetDirection(hetPORT1, 0xFFFFFFFF);
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
static void spi_send_and_get(uint16_t *tx_data, uint16_t *rx_data, uint16_t data_length) {
    spiSendAndGetData(IRIS_SPI, &dataconfig, data_length, tx_data, rx_data);
    while ((SpiTxStatus(IRIS_SPI) != SPI_COMPLETED) && (SpiRxStatus(IRIS_SPI) != SPI_COMPLETED));
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
static void spi_send(uint16_t *tx_data, uint16_t data_length) {
    spiSendData(IRIS_SPI, &dataconfig, data_length, tx_data);
    while(SpiTxStatus(IRIS_SPI) != SPI_COMPLETED);
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
static void spi_get(uint16_t *rx_data, uint16_t data_length) {
    spiGetData(IRIS_SPI, &dataconfig, data_length, rx_data);
    while(SpiRxStatus(IRIS_SPI) != SPI_COMPLETED);
}

/*
 */
/**
 * @brief
 *   Sudo delay created due to lack of TI's HAL Delay functionality
 *   Each for-loop iteration is approximately 11 clock cycles:
 *   Given base clock speed to be 300 MHz, each second of delay will take
 *   (300M/11 = 27272727 iterations)
 *
 *   Minimum delay vTaskDelay supports is 1 millisecond, for finer
 *   timing control it is efficient to use microsecond using for loops
 *
 * @param[in] ticks
 *   Number of ticks for non FreeRTOS delay
 *
 **/
static void spi_delay(uint16_t ticks) {
    uint16_t i;
    for (i = 0; i < ticks; i++);
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
 *   Returns IRIS_ACK if command was acknowledge, IRIS_NACK if not
 **/
IrisLowLevelReturn send_command(uint16_t command) {
    uint16_t rx_data;
    uint16_t tx_dummy = DUMMY_BYTE;

    NSS_LOW();
    spi_delay(10000);
    spi_send(&command, 1);
    spi_delay(10000);
    spi_send(&tx_dummy, 1);
    spi_get(&rx_data, 1);
    spi_delay(10000);
    NSS_HIGH();

    if (rx_data == ACK_FLAG) {
        return IRIS_ACK;
    } else if (rx_data == NACK_FLAG) {
        return IRIS_NACK;
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
 *   Returns IRIS_ACK if ACK is recieved, IRIS_NACK if not
 **/
IrisLowLevelReturn send_data(uint16_t *tx_buffer, uint16_t data_length) {
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
        return IRIS_LL_ERROR;
    }
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
IrisLowLevelReturn get_data(uint16_t *rx_buffer, uint16_t data_length) {
    uint16_t tx_dummy = 0xFF;

    NSS_LOW();
    spi_delay(10000);
    spi_send_and_get(&tx_dummy, rx_buffer, data_length);
    spi_delay(1000);
    NSS_HIGH();
    spi_delay(1000);

    return IRIS_ACK;
}
