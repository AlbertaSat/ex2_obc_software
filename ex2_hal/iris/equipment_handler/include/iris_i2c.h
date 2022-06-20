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
 * i2c_driver.h
 *
 *  Created on: May 10, 2022
 *      Author: jenish
 */

#ifndef INCLUDE_IRIS_I2C_H_
#define INCLUDE_IRIS_I2C_H_

#include "FreeRTOS.h"

#define IRIS_SLAVE_ADDR 0x42

int iris_write_packet(void *buf_tx_data, uint16_t data_length);
int iris_read_packet(void *buf_rx_data, uint16_t data_length);

#endif /* INCLUDE_IRIS_I2C_H_ */
