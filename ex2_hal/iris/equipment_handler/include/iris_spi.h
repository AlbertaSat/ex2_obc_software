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

#ifndef INCLUDE_IRIS_SPI_H_
#define INCLUDE_IRIS_SPI_H_

#include "FreeRTOS.h"

typedef enum {
    IRIS_ACK = 0,
    IRIS_NACK = 1,
    IRIS_LL_ERROR = 2,
} IrisLowLevelReturn;

#define DUMMY_BYTE 0xFF
#define IRIS_WAIT_FOR_ACK vTaskDelay(pdMS_TO_TICKS(20))

void iris_spi_init();
void iris_spi_send(uint16_t *tx_data, uint16_t data_length);
void iris_spi_get(uint16_t *rx_data, uint16_t data_length);
void iris_spi_send_and_get(uint16_t *tx_data, uint16_t *rx_data, uint16_t data_length);
void iris_spi_delay(uint32_t timeout);

IrisLowLevelReturn iris_send_command(uint16_t command);
IrisLowLevelReturn iris_send_data(uint16_t *tx_buffer, uint16_t data_length);
IrisLowLevelReturn iris_get_data(uint16_t *rx_buffer, uint16_t data_length); // Data length is obtained from IRIS

#endif /* INCLUDE_IRIS_SPI_H_ */
