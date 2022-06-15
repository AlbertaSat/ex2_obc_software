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
 * i2c_bootloader_cmds.c
 *
 *  Created on: May 10, 2022
 *      Author: jenish
 */

#include "iris_i2c.h"
#include "FreeRTOS.h"
#include "i2c_io.h"
#include "system.h"

/* Optimization points
 * - Although it is not mission critical, we could read back from
 *   iris for debugging purpose. As of now we have tested and verified
 *   that we can load .bin file accurately to iris.
 */

/**
 * @brief
 *   Transmits bytes via the I2C bus
 *
 * @param[in] buf_tx_data
 *   Pointer to transmit data buffer
 *
 * @param[in] data_length
 *   Number of bytes to transmit

 * @return
 *   Returns 0 data written, <0 if unable to write data.
 **/
int iris_write_packet(void *buf_tx_data, uint16_t data_length) {
    return i2c_Send(IRIS_I2C, IRIS_SLAVE_ADDR, data_length, buf_tx_data);
}

/**
 * @brief
 *   Receives bytes via the I2C bus
 *
 * @param[in] buf_tx_data
 *   Pointer to receive data buffer
 *
 * @param[in] data_length
 *   Number of bytes to receive

 * @return
 *   Returns 0 data read, <0 if unable to read data.
 **/
int iris_read_packet(void *buf_rx_data, uint16_t data_length) {
    return i2c_Receive(IRIS_I2C, IRIS_SLAVE_ADDR, data_length, &buf_rx_data);
}
