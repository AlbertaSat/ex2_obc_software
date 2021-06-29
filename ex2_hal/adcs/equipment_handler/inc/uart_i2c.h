/*
 * Copyright (C) 2020  University of Alberta
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

#ifndef UART_I2C_H
#define UART_I2C_H

void uart_send(uint8_t* data, uint32_t length);
void uart_receive(uint8_t* data, uint32_t length);

void i2c_send(uint8_t *data, uint32_t length);
void i2c_receive(uint8_t *data, uint8_t reg, uint32_t length); // reg: the register to read (TLM ID)

#endif
