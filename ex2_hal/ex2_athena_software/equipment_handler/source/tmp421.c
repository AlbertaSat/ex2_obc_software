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

#include "HL_i2c.h"
#include "tmp421.h"
#include "stdio.h"

///* Addresses to scan */
//static const unsigned short normal_i2c[] = { 0x2a, 0x4c, 0x4d, 0x4e, 0x4f,
//					     I2C_CLIENT_END };

enum chips { tmp421, tmp422, tmp423, tmp441, tmp442 };

/* The TMP421 registers */
#define TMP421_STATUS_REG			0x08
#define TMP421_CONFIG_REG_1			0x09
#define TMP421_CONVERSION_RATE_REG		0x0B
#define TMP421_MANUFACTURER_ID_REG		0xFE
#define TMP421_DEVICE_ID_REG			0xFF
#define CHANNELS 2
#define CHANNEL_LOCAL 0
#define CHANNEL_REMOTE 1

static const uint8_t TMP421_TEMP_MSB[2]		= { 0x00, 0x01};
static const uint8_t TMP421_TEMP_LSB[2]		= { 0x10, 0x11};

/* Flags */
#define TMP421_CONFIG_SHUTDOWN			0x40
#define TMP421_CONFIG_RANGE			0x04

/* Manufacturer / Device ID's */
#define TMP421_MANUFACTURER_ID			0x55
#define TMP421_DEVICE_ID			0x21
#define TMP422_DEVICE_ID			0x22
#define TMP423_DEVICE_ID			0x23
#define TMP441_DEVICE_ID			0x41
#define TMP442_DEVICE_ID			0x42

// static const struct i2c_device_id tmp421_id[] = {
// 	{ "tmp421", 2 },
// 	{ "tmp422", 3 },
// 	{ "tmp423", 4 },
// 	{ "tmp441", 2 },
// 	{ "tmp442", 3 },
// 	{ }
// };
// MODULE_DEVICE_TABLE(i2c, tmp421_id);

//static const struct of_device_id __maybe_unused tmp421_of_match[] = {
//	{
//		.compatible = "ti,tmp421",
//		.data = (void *)2
//	},
//	{
//		.compatible = "ti,tmp422",
//		.data = (void *)3
//	},
//	{
//		.compatible = "ti,tmp423",
//		.data = (void *)4
//	},
//	{
//		.compatible = "ti,tmp441",
//		.data = (void *)2
//	},
//	{
//		.compatible = "ti,tmp442",
//		.data = (void *)3
//	},
//	{ },
//};
//MODULE_DEVICE_TABLE(of, tmp421_of_match);

//struct tmp421_data {
//	uint8_t sadd;
//	//struct mutex update_lock;
//	//u32 temp_config[5];
//	//struct hwmon_channel_info temp_info;
//	//const struct hwmon_channel_info *info[2];
//	//struct hwmon_chip_info chip;
//	//char valid;
//	//unsigned long last_updated;
//	//unsigned long channels;
//	uint8_t config;
//	uint16_t temp[4];
//};

//CODE TO MAYBE BE ADDED TO I2C DRIVERS BELOW
void i2cSlaveWriteReg(uint8_t sadd, uint8_t reg, uint8_t data) {
    i2cSetSlaveAdd(i2cREG2, sadd);
    i2cSetDirection(i2cREG2, I2C_TRANSMITTER);
    i2cSetCount(i2cREG2, 2);
    i2cSetMode(i2cREG2, I2C_MASTER);
    i2cSetStop(i2cREG2);
    i2cSetStart(i2cREG2);
    i2cSendByte(i2cREG2, reg);
    i2cSendByte(i2cREG2, data);
    while(i2cIsBusBusy(i2cREG2) == true);
    while(i2cIsStopDetected(i2cREG2) == 0);
    i2cClearSCD(i2cREG2);
}

uint8_t i2cSlaveRead1ByteReg(uint8_t sadd, uint8_t reg) {
	uint8_t value = 0;

	i2cSetSlaveAdd(i2cREG2, sadd);

    i2cSetDirection(i2cREG2, I2C_TRANSMITTER);
    i2cSetCount(i2cREG2, 1);
    i2cSetMode(i2cREG2, I2C_MASTER);
    i2cSetStop(i2cREG2);
    i2cSetStart(i2cREG2);
    i2cSend(i2cREG2, 1, &reg);

    while(i2cIsBusBusy(i2cREG2) == true);
    while(i2cIsStopDetected(i2cREG2) == 0);
    i2cClearSCD(i2cREG2);

    int temp;
    for (temp = 0; temp < 0x1000; temp++);//temporary fix... don't want delay down the road

    i2cSetSlaveAdd(i2cREG2, sadd);
    /* Set direction to receiver */
    i2cSetDirection(i2cREG2, I2C_RECEIVER);
    i2cSetCount(i2cREG2, 1);
    /* Set mode as Master */
    i2cSetMode(i2cREG2, I2C_MASTER);
    i2cSetStop(i2cREG2);
    /* Transmit Start Condition */
    i2cSetStart(i2cREG2);
    value = i2cReceiveByte(i2cREG2);

    /* Wait until Bus Busy is cleared */
    while(i2cIsBusBusy(i2cREG2) == true);

    /* Wait until Stop is detected */
    while(i2cIsStopDetected(i2cREG2) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(i2cREG2);

	return value;
}

uint16_t i2cSlaveRead2ByteReg(uint8_t sadd, uint8_t reg) {
	uint8_t data[2] = {0};
	uint16_t value = 0;

    i2cSetSlaveAdd(i2cREG2, sadd);

    i2cSetDirection(i2cREG2, I2C_TRANSMITTER);
    i2cSetCount(i2cREG2, 1);
    i2cSetMode(i2cREG2, I2C_MASTER);
    i2cSetStop(i2cREG2);
    i2cSetStart(i2cREG2);
    i2cSend(i2cREG2, 1, &reg);

    while(i2cIsBusBusy(i2cREG2) == true);
    while(i2cIsStopDetected(i2cREG2) == 0);
    i2cClearSCD(i2cREG2);

    int temp;
    for (temp = 0; temp < 0x10000; temp++);//temporary fix... don't want delay down the road

    i2cSetSlaveAdd(i2cREG2, sadd);
    /* Set direction to receiver */
    i2cSetDirection(i2cREG2, I2C_RECEIVER);
    i2cSetCount(i2cREG2, 2);
    /* Set mode as Master */
    i2cSetMode(i2cREG2, I2C_MASTER);
    i2cSetStop(i2cREG2);
    /* Transmit Start Condition */
    i2cSetStart(i2cREG2);
    i2cReceive(i2cREG2, 2, data);


    /* Wait until Bus Busy is cleared */
    while(i2cIsBusBusy(i2cREG2) == true);

    /* Wait until Stop is detected */
    while(i2cIsStopDetected(i2cREG2) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(i2cREG2);

    value = (((uint16_t)(data[0])) << 8) | data[1];

	return value;
}

long calc_temp_pos(uint16_t reg) {
	/* Mask out status bits */
	int temp = reg & ~0xf;

	return (temp * 10000 + 128) / 256;
}

long calc_temp_neg(uint16_t reg){

    return (((~(reg>>8) + 1) & 0x7F)*-10000) + ((reg>>4) & 0xF)*625;
}

//int temp_from_u16(uint16_t reg) {
//	/* Mask out status bits */
//	int temp = reg & ~0xf;
//
//	/* Add offset for extended temperature range. */
//	temp -= 64 * 256;
//
//	return (temp * 1000 + 128) / 256;
//}

//int tmp421_update_device(uint8_t sadd, uint16_t * localtemp, uint16_t * remotetemp) {
//	// struct tmp421_data *data = dev_get_drvdata(dev);
//    uint8_t temp[4] - {0};
// 	// struct i2c_client *client = data->client;
//	int i;
//
//	//mutex_lock(&data->update_lock);
//
//	// if (time_after(jiffies, data->last_updated + (HZ / 2)) ||
//	//     !data->valid) {
//		//data->config = i2c_smbus_read_byte_data(client,
//		//	TMP421_CONFIG_REG_1);
//		//data->config = i2cSlaveRead1ByteReg(sadd, TMP421_CONFIG_REG_1);
//
////		for (i = 0; i < CHANNELS; i++) {
//			//data->temp[i] = i2c_smbus_read_byte_data(client,
//			//	TMP421_TEMP_MSB[i]) << 8;
////			data->temp[i] = i2cSlaveRead1ByteReg(sadd, TMP421_TEMP_MSB[i]) << 8;
////			//data->temp[i] |= i2c_smbus_read_byte_data(client,
////			//	TMP421_TEMP_LSB[i]);
////			data->temp[i] = i2cSlaveRead1ByteReg(sadd, TMP421_TEMP_LSB[i]);
//	  	}
//	// 	data->last_updated = jiffies;
//	// 	data->valid = 1;
//	// }
//
//	//mutex_unlock(&data->update_lock);
//
//
//
//	return 0;//check this
//}


//Note that because val is stored as an integer, the actual temperature is the decimal integer value shifted 4 digits to the right
//Ex: val == 276875 means a measurement of 27.6875 degrees C
int tmp421_read(uint8_t sadd, int channel, long *val) {

    uint8_t temp[2] = {0};
    uint16_t temp16 = 0;

    if(channel == CHANNEL_LOCAL){
        temp[0] = i2cSlaveRead1ByteReg(sadd, TMP421_TEMP_MSB[0]);
        int delay;
        for (delay = 0; delay < 0x1000; delay++);//temporary fix... don't want delay down the road
        temp[1] = i2cSlaveRead1ByteReg(sadd, TMP421_TEMP_LSB[0]);
        temp16 = (((uint16_t)(temp[0])) << 8) | temp[1];
    }
    else if (channel == CHANNEL_REMOTE){
        temp[0] = i2cSlaveRead1ByteReg(sadd, TMP421_TEMP_MSB[1]);
        int delay;
        for (delay = 0; delay < 0x1000; delay++);//temporary fix... don't want delay down the road
        temp[1] = i2cSlaveRead1ByteReg(sadd, TMP421_TEMP_LSB[1]);
        temp16 = (((uint16_t)(temp[0])) << 8) | temp[1];
    }
    else{
        return 1;
    }

	// switch (attr) {
//	// case hwmon_temp_input:
//		if (tmp421->config & TMP421_CONFIG_RANGE)
//			*val = temp_from_u16(tmp421->temp[channel]);
//		else

    if(temp16 & 0x8000 == 0x8000){
        *val = calc_temp_neg(temp16);
    }
    else{
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

// static umode_t tmp421_is_visible(const void *data, enum hwmon_sensor_types type,
// 				 u32 attr, int channel)
// {
// 	switch (attr) {
// 	case hwmon_temp_fault:
// 		if (channel == 0)
// 			return 0;
// 		return 0444;
// 	case hwmon_temp_input:
// 		return 0444;
// 	default:
// 		return 0;
// 	}
// }

int tmp421_init_client(uint8_t sadd) {
	int config, config_orig;

	/* Set the conversion rate to 2 Hz */
	//i2c_smbus_write_byte_data(client, TMP421_CONVERSION_RATE_REG, 0x05);
	i2cSlaveWriteReg(sadd, TMP421_CONVERSION_RATE_REG, 0x05);

	int temp;
	for (temp = 0; temp < 0x1000; temp++);


	/* Start conversions (disable shutdown if necessary) */
	//config = i2c_smbus_read_byte_data(client, TMP421_CONFIG_REG_1);
	config = i2cSlaveRead1ByteReg(sadd, TMP421_CONFIG_REG_1);
	if (config < 0) {
		fprintf(stderr, "Could not read configuration register (%d)\n", config);
		return config;
	}

	for (temp = 0; temp < 0x1000; temp++);

	config_orig = config;
	config &= ~TMP421_CONFIG_SHUTDOWN;

	if (config != config_orig) {
		fprintf(stderr, "Enable monitoring chip\n");
		//i2c_smbus_write_byte_data(client, TMP421_CONFIG_REG_1, config);
		i2cSlaveWriteReg(sadd, TMP421_CONFIG_REG_1, config);
	}

	for (temp = 0; temp < 0x1000; temp++);

	return 0;
}

int tmp421_detect(uint8_t sadd) {
	enum chips kind;
//	struct i2c_adapter *adapter = client->adapter;
	static const char * const names[] = {
		"TMP421", "TMP422", "TMP423",
		"TMP441", "TMP442"
	};
//	int addr = client->addr;
	uint8_t reg;

//	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA))
//		return 1;

	//reg = i2c_smbus_read_byte_data(client, TMP421_MANUFACTURER_ID_REG);
	reg = i2cSlaveRead1ByteReg(sadd, TMP421_MANUFACTURER_ID_REG);
	if (reg != TMP421_MANUFACTURER_ID)
		return 1;

    int temp;
    for (temp = 0; temp < 0x1000; temp++);


	//reg = i2c_smbus_read_byte_data(client, TMP421_CONVERSION_RATE_REG);
	reg = i2cSlaveRead1ByteReg(sadd, TMP421_CONVERSION_RATE_REG);
	if (reg & 0xf8)
		return 1;

	for (temp = 0; temp < 0x1000; temp++);

	//reg = i2c_smbus_read_byte_data(client, TMP421_STATUS_REG);
	reg = i2cSlaveRead1ByteReg(sadd, TMP421_STATUS_REG);
	if (reg & 0x7f)
		return 1;

	for (temp = 0; temp < 0x1000; temp++);

	//reg = i2c_smbus_read_byte_data(client, TMP421_DEVICE_ID_REG);
	reg = i2cSlaveRead1ByteReg(sadd, TMP421_DEVICE_ID_REG);
	switch (reg) {
	case TMP421_DEVICE_ID:
		kind = tmp421;
		break;
	case TMP422_DEVICE_ID:
//		if (addr == 0x2a)
			return 1;
		kind = tmp422;
		break;
	case TMP423_DEVICE_ID:
//		if (addr != 0x4c && addr != 0x4d)
			return 1;
		kind = tmp423;
		break;
	case TMP441_DEVICE_ID:
		kind = tmp441;
		break;
	case TMP442_DEVICE_ID:
//		if (addr != 0x4c && addr != 0x4d)
			return 1;
		kind = tmp442;
		break;
	default:
		return 1;
	}

	//strlcpy(info->type, tmp421_id[kind].name, I2C_NAME_SIZE);
	fprintf(stderr, "Detected %s\n",names[kind]);

	return 0;
}

// static const struct hwmon_ops tmp421_ops = {
// 	.is_visible = tmp421_is_visible,
// 	.read = tmp421_read,
// };

// static int tmp421_probe(struct i2c_client *client,
// 			const struct i2c_device_id *id)
// {
// 	struct device *dev = &client->dev;
// 	struct device *hwmon_dev;
// 	struct tmp421_data *data;
// 	int i, err;

// 	data = devm_kzalloc(dev, sizeof(struct tmp421_data), GFP_KERNEL);
// 	if (!data)
// 		return -ENOMEM;

// 	mutex_init(&data->update_lock);
// 	if (client->dev.of_node)
// 		data->channels = (unsigned long)
// 			of_device_get_match_data(&client->dev);
// 	else
// 		data->channels = id->driver_data;
// 	data->client = client;

// 	err = tmp421_init_client(client);
// 	if (err)
// 		return err;

// 	for (i = 0; i < data->channels; i++)
// 		data->temp_config[i] = HWMON_T_INPUT | HWMON_T_FAULT;

// 	data->chip.ops = &tmp421_ops;
// 	data->chip.info = data->info;

// 	data->info[0] = &data->temp_info;

// 	data->temp_info.type = hwmon_temp;
// 	data->temp_info.config = data->temp_config;

// 	hwmon_dev = devm_hwmon_device_register_with_info(dev, client->name,
// 							 data,
// 							 &data->chip,
// 							 NULL);
// 	return PTR_ERR_OR_ZERO(hwmon_dev);
// }

// static struct i2c_driver tmp421_driver = {
// 	.class = I2C_CLASS_HWMON,
// 	.driver = {
// 		.name	= "tmp421",
// 		.of_match_table = of_match_ptr(tmp421_of_match),
// 	},
// 	.probe = tmp421_probe,
// 	.id_table = tmp421_id,
// 	.detect = tmp421_detect,
// 	.address_list = normal_i2c,
// };

// module_i2c_driver(tmp421_driver);

// MODULE_AUTHOR("Andre Prendel <andre.prendel@gmx.de>");
// MODULE_DESCRIPTION("Texas Instruments TMP421/422/423/441/442 temperature sensor driver");
// MODULE_LICENSE("GPL");

void TMP421test(uint8_t sadd){
	long temp_val = 0;

//	fprintf(stderr, "testing chip with address %d", sadd);

	if(tmp421_detect(sadd)){
	    fprintf(stderr, "tmp421_detect failed\n");
	}



	tmp421_init_client(sadd);

	if(tmp421_read(sadd, CHANNEL_LOCAL, &temp_val)){
	    fprintf(stderr, "local tmp421_read failed\n");
	}
	fprintf(stderr, "Local t = %d\n", temp_val);

	if(tmp421_read(sadd, CHANNEL_REMOTE, &temp_val)){
	    fprintf(stderr, "remote tmp421_read failed\n");
	}
	fprintf(stderr, "Remote t = %d\n", temp_val);
}

//Add the following to a new file: athena_temp.c
/*
int init_temp(void){
	return 0;
}


int get_all_temp(){
	
	if(tmp421_read(0x4c, CHANNEL_REMOTE, )){
		return 1;
	}
	if(tmp421_read(0x4f, CHANNEL_REMOTE, )){
		return 1;
	}
	if(tmp421_read(0x1e, CHANNEL_REMOTE, )){
		return 1;
	}
	if(tmp421_read(0x1d, CHANNEL_REMOTE, )){
		return 1;
	}
	if(tmp421_read(0x45, CHANNEL_REMOTE, )){
		return 1;
	}
	if(tmp421_read(0x4d, CHANNEL_REMOTE, )){
		return 1;
	}

	return 0;
}
*/
