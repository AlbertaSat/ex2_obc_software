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

#ifndef ADCS_IO_H
#define ADCS_IO_H

#include <stdint.h>
#include "adcs_types.h"

#define ADCS_I2C_ADDR 0x57
#define UART_TIMEOUT_MS 300

void init_adcs_io();

// sends telecommand over uart/i2c, and return acknowledgment
ADCS_returnState send_uart_telecommand(uint8_t *command, uint32_t length);
ADCS_returnState send_i2c_telecommand(uint8_t *command, uint32_t length);

// request telemetry over uart/i2c, and return acknowledgment
ADCS_returnState request_uart_telemetry(uint8_t TM_ID, uint8_t *telemetry, uint32_t length);
ADCS_returnState request_i2c_telemetry(uint8_t TM_ID, uint8_t *telemetry, uint32_t length);

// receive downloaded packets over uart
ADCS_returnState receive_uart_packet(uint8_t *hole_map, uint8_t *image_bytes);

#endif /* ADCS_IO_H */
