/*
 * pcal9538.h
 *
 *  Created on: Sept. 10, 2021
 *      Author: Josh Lazaruk
 *
 *
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

#define PCAL9538A_ADDR              (0x70)
#ifdef IS_ATHENA
#define PCAL9538A_PORT              i2cREG2
#else
#define PCAL9538A_PORT              i2cREG1 // port used on dev board
#endif

typedef enum {
ADCS = 0,
DFGM = 1,
IRIS = 2,
OBC = 3,
CHARON = 4,
UHF = 5,
} Power_Channel;

uint8_t setuppcal9538a(void);
uint8_t triggerPowerChannelReset(Power_Channel channel);
