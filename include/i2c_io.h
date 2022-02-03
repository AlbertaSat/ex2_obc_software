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
 * @file    i2c_io.h
 * @author  Robert Taylor
 * @date    2021-08-04
 */

#include "FreeRTOS.h"
#include "HL_i2c.h"
#include "os_semphr.h"

#define I2C_TIMEOUT_MS 1000
#define I2C_POLLING_TIMEOUT 1000
#define MAX_I2C_RECV_LEN 20

void init_i2c_driver();

int i2c_Send(i2cBASE_t *i2c, uint8_t addr, uint16_t size, void *buf);

int i2c_Receive(i2cBASE_t *i2c, uint8_t addr, uint16_t size, void *buf);
