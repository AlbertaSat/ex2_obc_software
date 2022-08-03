/*
 * pcal9538.c
 *
 *  Created on: Sept. 10, 2021
 *      Author: Josh Lazaruk
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
#include "pcal9538a.h"
#include "system.h"
#include <i2c_io.h>

/*
 * @brief
 *      Configures device for current implementation (all pins outputs with value = 0).
 *      Needs to occur after every reset of chip.
 *      Note that chip is reset every time triggerPowerChannelReset() is called.
 * @return
 *      1 if error, 0 if successful.
 */
uint8_t setuppcal9538a(void) {

    uint16_t data = 0x0100;

    if (i2c_Send(PCAL9538A_PORT, PCAL9538A_ADDR, 2, &data) !=
        0) { // set output value register (0x01) to all be logic low = 0
        return 1;
    }

    data = 0x0300;

    if (i2c_Send(PCAL9538A_PORT, PCAL9538A_ADDR, 2, &data) !=
        0) { // set config register (0x03) to make all pins outputs
        return 1;
    }

    return 0;
}

/*
 * @brief
 *      Trigger 1 chip output (set output high) to reset power channel using on-board circuitry that
 *      triggers over-current condition on EPS.
 * @param channel
 *      The power channel that is to be reset.
 * @return
 *      1 if error, 0 if successful.
 */
uint8_t triggerPowerChannelReset(Power_Channel channel) {

    uint8_t data[2] = {0b1, 0b0};
    data[1] = 0b1 << channel;

    if (i2c_Send(PCAL9538A_PORT, PCAL9538A_ADDR, 2, data) != 0) {
        return 1;
    }
    return 0;
}
