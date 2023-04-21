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

//#include <linux/module.h>
//#include <linux/init.h>
//#include <linux/slab.h>
//#include <linux/jiffies.h>
//#include <linux/i2c.h>
//#include <linux/hwmon.h>
//#include <linux/hwmon-sysfs.h>
//#include <linux/err.h>
//#include <linux/mutex.h>
//#include <linux/of_device.h>
//#include <linux/sysfs.h>

#include "tmp421.h"
#include "HL_i2c.h"
#include "i2c_io.h"
#include "stdio.h"

///* Addresses to scan */
// static const unsigned short normal_i2c[] = { 0x2a, 0x4c, 0x4d, 0x4e, 0x4f,
//					     I2C_CLIENT_END };

enum chips { tmp421, tmp422, tmp423, tmp441, tmp442 };

/* The TMP421 registers */
#define TMP421_STATUS_REG 0x08
#define TMP421_CONFIG_REG_1 0x09
#define TMP421_CONVERSION_RATE_REG 0x0B
#define TMP421_MANUFACTURER_ID_REG 0xFE
#define TMP421_DEVICE_ID_REG 0xFF
#define CHANNELS 2
#define CHANNEL_LOCAL 0
#define CHANNEL_REMOTE 1

static const uint8_t TMP421_TEMP_MSB[2] = {0x00, 0x01};
static const uint8_t TMP421_TEMP_LSB[2] = {0x10, 0x11};

/* Flags */
#define TMP421_CONFIG_SHUTDOWN 0x40
#define TMP421_CONFIG_RANGE 0x04

/* Manufacturer / Device ID's */
#define TMP421_MANUFACTURER_ID 0x55
#define TMP421_DEVICE_ID 0x21
#define TMP422_DEVICE_ID 0x22
#define TMP423_DEVICE_ID 0x23
#define TMP441_DEVICE_ID 0x41
#define TMP442_DEVICE_ID 0x42

int tmp421_Write1ByteReg(uint8_t addr, uint8_t reg_addr, uint8_t data) {
    uint8_t buf[2];
    buf[0] = reg_addr;
    buf[1] = data;
    return i2c_Send(i2cREG2, addr, 2, &buf);
}

int tmp421_Write2ByteReg(uint8_t addr, uint8_t reg_addr, uint16_t data) {
    uint8_t buf[3];
    buf[0] = reg_addr;
    buf[1] = data & 0x00FF;
    buf[2] = data & 0xFF00;
    return i2c_Send(i2cREG2, addr, 3, &buf);
}

int tmp421_Read1ByteReg(uint8_t addr, uint8_t reg_addr, uint8_t *val) {
    uint8_t value = 0;

    i2c_Send(i2cREG2, addr, 1, &reg_addr);

    i2c_Receive(i2cREG2, addr, 1, &value);

    return value;
}

int tmp421_Read2ByteReg(uint8_t addr, uint8_t reg_addr, uint16_t *val) {
    uint8_t data[2] = {0};

    if (i2c_Send(i2cREG2, addr, 1, &reg_addr) == -1) {
        return -1;
    }

    if (i2c_Receive(i2cREG2, addr, 2, &data) == -1) {
        return -1;
    }

    *val = (((uint16_t)(data[0])) << 8) | data[1];

    return 0;
}

long calc_temp_pos(uint16_t reg) {
    /* Mask out status bits */
    int temp = reg & ~0xf;

    return (temp * 10000 + 128) / 256;
}

long calc_temp_neg(uint16_t reg) { return (((~(reg >> 8) + 1) & 0x7F) * -10000) + ((reg >> 4) & 0xF) * 625; }

// int temp_from_u16(uint16_t reg) {
//	/* Mask out status bits */
//	int temp = reg & ~0xf;
//
//	/* Add offset for extended temperature range. */
//	temp -= 64 * 256;
//
//	return (temp * 1000 + 128) / 256;
//}

// Note that because val is stored as an integer, the actual temperature is the decimal integer value shifted 4
// digits to the right Ex: val == 276875 means a measurement of 27.6875 degrees C
int tmp421_read(uint8_t sadd, int channel, long *val) {

    uint8_t temp[2] = {0};
    uint16_t temp16 = 0;

    if (channel == CHANNEL_LOCAL) {
        tmp421_Read1ByteReg(sadd, TMP421_TEMP_MSB[0], &(temp[0]));
        int delay;
        for (delay = 0; delay < 0x1000; delay++)
            ; // temporary fix... don't want delay down the road
        tmp421_Read1ByteReg(sadd, TMP421_TEMP_LSB[0], &(temp[1]));
        temp16 = (((uint16_t)(temp[0])) << 8) | temp[1];
    } else if (channel == CHANNEL_REMOTE) {
        tmp421_Read1ByteReg(sadd, TMP421_TEMP_MSB[1], &(temp[0]));
        int delay;
        for (delay = 0; delay < 0x1000; delay++)
            ; // temporary fix... don't want delay down the road
        tmp421_Read1ByteReg(sadd, TMP421_TEMP_LSB[1], &(temp[1]));
        temp16 = (((uint16_t)(temp[0])) << 8) | temp[1];
    } else {
        return 1;
    }

    // switch (attr) {
    //	// case hwmon_temp_input:
    //		if (tmp421->config & TMP421_CONFIG_RANGE)
    //			*val = temp_from_u16(tmp421->temp[channel]);
    //		else

    if (temp16 & 0x8000 == 0x8000) {
        *val = calc_temp_neg(temp16);
    } else {
        *val = calc_temp_pos(temp16);
    }

    return 0;
    // case hwmon_temp_fault:

    // 	 * The OPEN bit signals a fault. This is bit 0 of the temperature
    // 	 * register (low byte).

    // 	*val = tmp421->temp[channel] & 0x01;
    // 	return 0;
    //	default:
    //		return 1;
    //	}
}

int tmp421_init_client(uint8_t sadd) {
    uint8_t config, config_orig;

    /* Set the conversion rate to 2 Hz */
    // i2c_smbus_write_byte_data(client, TMP421_CONVERSION_RATE_REG, 0x05);
    tmp421_Write1ByteReg(sadd, TMP421_CONVERSION_RATE_REG, 0x05);

    int temp;
    for (temp = 0; temp < 0x1000; temp++)
        ;

    /* Start conversions (disable shutdown if necessary) */
    // config = i2c_smbus_read_byte_data(client, TMP421_CONFIG_REG_1);
    tmp421_Read1ByteReg(sadd, (uint8_t)TMP421_CONFIG_REG_1, &config);

    for (temp = 0; temp < 0x1000; temp++)
        ;

    config_orig = config;
    config &= ~TMP421_CONFIG_SHUTDOWN;

    if (config != config_orig) {
        fprintf(stderr, "Enable monitoring chip\n");
        // i2c_smbus_write_byte_data(client, TMP421_CONFIG_REG_1, config);
        tmp421_Write1ByteReg(sadd, TMP421_CONFIG_REG_1, config);
    }

    for (temp = 0; temp < 0x1000; temp++)
        ;

    return 0;
}

int tmp421_detect(uint8_t sadd) {
    enum chips kind;
    //	struct i2c_adapter *adapter = client->adapter;
    static const char *const names[] = {"TMP421", "TMP422", "TMP423", "TMP441", "TMP442"};
    //	int addr = client->addr;
    uint8_t reg;

    tmp421_Read1ByteReg(sadd, TMP421_MANUFACTURER_ID_REG, &reg);
    if (reg != TMP421_MANUFACTURER_ID)
        return 1;

    int temp;
    for (temp = 0; temp < 0x1000; temp++)
        ;

    tmp421_Read1ByteReg(sadd, TMP421_CONVERSION_RATE_REG, &reg);
    if (reg & 0xf8)
        return 1;

    for (temp = 0; temp < 0x1000; temp++)
        ;

    tmp421_Read1ByteReg(sadd, TMP421_STATUS_REG, &reg);
    if (reg & 0x7f)
        return 1;

    for (temp = 0; temp < 0x1000; temp++)
        ;

    tmp421_Read1ByteReg(sadd, TMP421_DEVICE_ID_REG, &reg);
    switch (reg) {
    case TMP421_DEVICE_ID:
        kind = tmp421;
        break;
    case TMP422_DEVICE_ID:
#if 0
        if (addr == 0x2a)
            kind = tmp422;
#endif
        return 1;
    case TMP423_DEVICE_ID:
#if 0
        if (addr != 0x4c && addr != 0x4d)
            kind = tmp423;
#endif
        return 1;
    case TMP441_DEVICE_ID:
        kind = tmp441;
        break;
    case TMP442_DEVICE_ID:
#if 0
        if (addr != 0x4c && addr != 0x4d)
            kind = tmp442;
#endif
        return 1;
    default:
        return 1;
    }

    // strlcpy(info->type, tmp421_id[kind].name, I2C_NAME_SIZE);
    fprintf(stderr, "Detected %s\n", names[kind]);

    return 0;
}

void TMP421test(uint8_t sadd) {
    long temp_val = 0;

    //	fprintf(stderr, "testing chip with address %d", sadd);

    if (tmp421_detect(sadd)) {
        fprintf(stderr, "tmp421_detect failed\n");
    }

    tmp421_init_client(sadd);

    if (tmp421_read(sadd, CHANNEL_LOCAL, &temp_val)) {
        fprintf(stderr, "local tmp421_read failed\n");
    }
    fprintf(stderr, "Local t = %d\n", temp_val);

    if (tmp421_read(sadd, CHANNEL_REMOTE, &temp_val)) {
        fprintf(stderr, "remote tmp421_read failed\n");
    }
    fprintf(stderr, "Remote t = %d\n", temp_val);
}
