// SPDX-License-Identifier: GPL-2.0-or-later
/* tmp421.c
 *
 * Copyright (C) 2009 Andre Prendel <andre.prendel@gmx.de>
 * Preliminary support by:
 * Melvin Rook, Raymond Ng
 */

/*
 * Driver for the Texas Instruments TMP421 SMBus temperature sensor IC.
 * Supported models: TMP421, TMP422, TMP423, TMP441, TMP442
 */

#include <stdint.h>


//CODE TO MAYBE BE ADDED TO I2C DRIVERS BELOW
void i2cSlaveWriteReg(uint8_t sadd, uint8_t reg, uint8_t data);

uint8_t i2cSlaveRead1ByteReg(uint8_t sadd, uint8_t reg);
uint16_t i2cSlaveRead2ByteReg(uint8_t sadd, uint8_t reg);

long calc_temp_pos(uint16_t reg);

long calc_temp_neg(uint16_t reg);

//int temp_from_uint16(uint16_t reg);

//int tmp421_update_device(uint8_t sadd, uint16_t * localtemp, uint16_t * remotetemp);

int tmp421_read(uint8_t sadd, int channel, long *val);



int tmp421_init_client(uint8_t sadd);

int tmp421_detect(uint8_t sadd);


void TMP421test(uint8_t sadd);


