
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
 * @file spi.c
 * @author Thomas Ganley
 * @date 2021-06-23
 */

#include "spi.h"

void SPIMasterTx(spiBASE_t *regset, uint16_t *data, uint32_t length, uint8_t format) {
    spiDAT1_t dataconfig1_t;

    dataconfig1_t.CS_HOLD = FALSE;
    dataconfig1_t.WDEL = FALSE; // not sure
    dataconfig1_t.DFSEL = (SPIDATAFMT_t) format;
    dataconfig1_t.CSNR = 0x00; // no CS lines

    spiTransmitData(regset, &dataconfig1_t, length, data);
}

void SPISbandTx(uint8_t *data8, uint32_t length) {
    /* Convert bytes to shorts. This should work as is on a big-endian machine.
     * Note: spiTransmitData appears hard coded to a big-endian machine anyway.
     */
    uint16_t *data16 = (uint16_t *) data8;
    SPIMasterTx(SBAND_SPI, data16, length/sizeof(uint16_t), SPI_SBAND_DEF_FMT);
}
