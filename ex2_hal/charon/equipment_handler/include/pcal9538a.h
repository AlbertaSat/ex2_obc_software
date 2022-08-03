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
#include <stdint.h>

#define PCAL9538A_ADDR (0x70)

typedef enum {
    ADCS_CHANNEL = 0,
    DFGM_CHANNEL = 1,
    IRIS_CHANNEL = 2,
    OBC_CHANNEL = 3,
    CHARON_CHANNEL = 4,
    UHF_CHANNEL = 5,
} Power_Channel;

uint8_t setuppcal9538a(void);
uint8_t triggerPowerChannelReset(Power_Channel channel);
