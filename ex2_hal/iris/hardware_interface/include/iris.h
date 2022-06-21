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

// Iris data transfer constants
#define IMAGE_TRANSFER_SIZE 512 // 512 data bytes
#define MAX_IMAGE_LENGTH 3 // In bytes
#define MAX_IMAGE_COUNT 1 // In bytes
#define HOUSEKEEPING_SIZE 23 // In bytes

typedef enum {
    // TODO: Add more meaningful return types
    IRIS_HAL_OK = 0,
    IRIS_HAL_FAILURE = 1,
    IRIS_HAL_ERROR = 2,
} Iris_HAL_return;

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

typedef enum {
    IRIS_SENSOR_OFF = 0,
    IRIS_SENSOR_ON = 1,
} IRIS_SENSOR_TOGGLE;

enum {
    SEND_COMMAND,
    SEND_DATA,
    GET_DATA,
    FINISH,
    ERROR_STATE, // TODO: Potentially used for error handling
} controller_state;

// pre-defined SPI communication constants
#define ACK_FLAG 0xAA
#define NACK_FLAG 0x0F

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
} IRIS_Housekeeping;

typedef struct __attribute__((__packed__)) {
    uint16_t sensor_reg_addr;
    uint8_t sensor_data;
} sensor_reg;

// Command functions prototypes
Iris_HAL_return iris_take_pic();
Iris_HAL_return iris_get_image_length(uint32_t *image_length);
Iris_HAL_return iris_transfer_image(uint32_t image_length);
Iris_HAL_return iris_get_image_count(uint16_t *image_count);
Iris_HAL_return iris_toggle_sensor_idle(uint8_t toggle);
Iris_HAL_return iris_get_housekeeping(IRIS_Housekeeping *hk_data);
Iris_HAL_return iris_update_sensor_i2c_reg();
Iris_HAL_return iris_update_current_limit(uint16_t current_limit);

#endif /* INCLUDE_IRIS_H_ */
