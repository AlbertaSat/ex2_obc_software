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
/**
 * @file ads7128.h
 * @author Josh Lazaruk
 * @date 2021-09-10
 */
#include <stdint.h>
#include <stdbool.h>

#define ADS7128_ADDR (0x11)
#if IS_ATHENA == 1
#define ADS7128_PORT i2cREG1
#else
#define ADS7128_PORT i2cREG1 // port used on dev board
#endif

bool ads7128Init(void);
void voltageToTemperature(uint16_t voltage, int8_t *temperature);
uint8_t readSingleTemp(uint8_t channel, int8_t *temperature);
uint8_t readAllTemps(int8_t *temperatures);
