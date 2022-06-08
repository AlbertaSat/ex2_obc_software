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
  * @file    spi.h
  * @author  Jenish Patel
  * @date    2022-05-03
  */

#ifndef INCLUDE_IRIS_H_
#define INCLUDE_IRIS_H_
#endif /* INCLUDE_IRIS_H_ */

// Iris data transfer constants
#define IMAGE_TRANSFER_SIZE 512 // 512 data bytes + 1 start byte + 1 stop byte
#define START_FLAG 0xFF //TODO: What char?
#define STOP_FLAG 0xFF //TODO: What char?
#define MAX_IMAGE_LENGTH 16 // In bytes
#define MAX_IMAGE_COUNT 1 // In bytes
#define HOUSEKEEPING_SIZE 23 // In bytes

// Legal Iris commands
typedef enum {
    IRIS_TAKE_PIC = 0x10,
    IRIS_GET_IMAGE_LENGTH = 0x20,
    IRIS_TRANSFER_IMAGE = 0x31,
    IRIS_GET_IMAGE_COUNT = 0x30,
    IRIS_ON_SENSOR_IDLE = 0x40,
    IRIS_OFF_SENSOR_IDLE = 0x41,
    IRIS_SEND_HOUSEKEEPING = 0x51,
    IRIS_UPDATE_SENSOR_I2C_REG = 0x60,
    IRIS_UPDATE_CURRENT_LIMIT = 0x70,
} IRIS_COMMANDS;

enum {
    SEND_COMMAND,
    SEND_DATA,
    GET_DATA,
    FINISH,
    ERROR_STATE, // TODO: Potentially used for error handling
} controller_state;

// pre-defined SPI communication constants
#define ACK_FLAG 0xAA
#define NACK_FLAG 0x55

typedef struct __attribute__((__packed__)) {
    uint16_t vis_temp;
    uint16_t nir_temp;
    uint16_t flash_temp;
    uint16_t gate_temp;
    uint8_t imagenum;
    uint8_t software_version;
    uint8_t errornum;
    uint16_t MAX_5V_voltage;
    uint16_t MAX_5V_power;
    uint16_t MAX_3V_voltage;
    uint16_t MAX_3V_power;
    uint16_t MIN_5V_voltage;
    uint16_t MIN_3V_voltage;
} housekeeping_data;

typedef struct __attribute__((__packed__)) {
    uint16_t sensor_reg_addr;
    uint8_t sensor_data;
} sensor_reg;

// Command functions prototypes
void iris_take_pic();
uint32_t iris_get_image_length();
void iris_transfer_image();
uint8_t iris_get_image_count();
void iris_toggle_sensor_idle(uint8_t toggle);
housekeeping_data iris_get_housekeeping();
void iris_update_sensor_i2c_reg();
void iris_update_current_limit(uint16_t current_limit);
