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
 * @file    spi.h
 * @author  Jenish Patel
 * @date    2022-05-03
 */

 /**
  - spi_init()
  - spi_master_write() - MOSI
  - spi_master_read() - MISO
  - spi_set_data_length()
  - spi_set_cs_high() - Toggle CS (chip select) OFF
  - spi_set_cs_low() - Toggle CS (chip select) ON
 */
#ifndef INCLUDE_IRIS_SPI_H_
#define INCLUDE_IRIS_SPI_H_
#endif /* INCLUDE_IRIS_SPI_H_ */

#include "FreeRTOS.h"

typedef enum {
    /* Equipment handler level flags     */
    IRIS_OK = 0,
    IRIS_ACK = 1,
    IRIS_NACK = 2,
    IRIS_ERROR = 3,
} IRIS_return;

#define TRANSFER_SIZE 1 // In bytes
#define DUMMY_BYTE 0xFF

void iris_init();
static void spi_send(uint16_t *tx_data, uint16_t data_length);
static void spi_get(uint16_t *rx_data, uint16_t data_length);
static void spi_send_and_get(uint16_t *tx_data, uint16_t *rx_data, uint16_t data_length);
static void spi_delay(uint16_t timeout);

IRIS_return send_command(uint16_t command);
IRIS_return send_data(uint16_t *tx_buffer, uint16_t data_length);
IRIS_return get_data(uint16_t *rx_buffer, uint16_t data_length); // Data length is obtained from IRIS
