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
 * @file uart_i2c.c
 * @author Arash Yazdani, Vasu Gupta
 * @date 2021-02-19
 */

#include "uart_i2c.h"
#include "HL_i2c.h" // HalCoGen generated i2c driver

#include <stdint.h>

// TODO: Define these values before testing
#define I2C_BASE_ADDR       // i2c module base address
#define ADCS_I2C_SLAVE_ADDR // slave address of ADCS module

const uint8_t ADCS_I2C_WRITE_NODE = 0xAE;
const uint8_t ADCS_I2C_READ_NODE = 0xAF;

// TODO: implement uart_send() and uart_receive()

/**
 * @brief
 * 		Write data to ADCS i2c slave address
 *      Refer to CubeADCS Firmware Manual Section 5.3
 * @param data
 * 		Data bytes to write
 * @param length
 *      Length of the data (in bytes)
 * @attention
 *      This function has not been tested and may need modifications
 *      when testing on hardware is done.
 *
 */
void i2c_send(uint8_t *data, uint32_t length) {
    i2cSetSlaveAdd(I2C_BASE_ADDR, ADCS_I2C_SLAVE_ADDR);
    i2cSetDirection(I2C_BASE_ADDR, I2C_TRANSMITTER);
    i2cSetCount(I2C_BASE_ADDR, length + 1);
    i2cSetMode(I2C_BASE_ADDR, I2C_MASTER);

    i2cSetStop(I2C_BASE_ADDR);
    i2cSetStart(I2C_BASE_ADDR);
    i2cSendByte(I2C_BASE_ADDR, ADCS_I2C_WRITE_NODE);
    i2cSend(I2C_BASE_ADDR, length, data);
    while (i2cIsBusBusy(I2C_BASE_ADDR))
        ;
    while (i2cIsStopDetected(I2C_BASE_ADDR) == 0)
        ;
    i2cClearSCD(I2C_BASE_ADDR);
}

/**
 * @brief
 * 		Read data from ADCS i2c slave address, using "repeated start condition".
 *      Refer to CubeADCS Firmware Manual Section 5.3
 * @param data
 * 		Data received
 * @param reg
 *      the ADCS register to read data from (also the TLM ID)
 * @param length
 * 		Length of the data (in bytes)
 * @attention
 *      This function has not been tested and may need modifications
 *      when testing on hardware is done.
 *
 */
void i2c_receive(uint8_t *data, uint8_t reg, uint32_t length) {
    // Select TLM register
    i2cSetSlaveAdd(I2C_BASE_ADDR, ADCS_I2C_SLAVE_ADDR);
    i2cSetDirection(I2C_BASE_ADDR, I2C_TRANSMITTER);
    i2cSetCount(I2C_BASE_ADDR, length + 3);
    i2cSetMode(I2C_BASE_ADDR, I2C_MASTER);

    i2cSetStop(I2C_BASE_ADDR);
    i2cSetStart(I2C_BASE_ADDR);
    i2cSendByte(I2C_BASE_ADDR, ADCS_I2C_WRITE_NODE);
    i2cSendByte(I2C_BASE_ADDR, reg);

    // send read node address
    i2cSetStart(I2C_BASE_ADDR);
    i2cSendByte(I2C_BASE_ADDR, ADCS_I2C_READ_NODE);
    while (i2cIsTxReady(I2C_BASE_ADDR) == 0)
        ; // make sure all bytes are sent

    // recevie data
    i2cSetSlaveAdd(I2C_BASE_ADDR, ADCS_I2C_SLAVE_ADDR);
    i2cSetDirection(I2C_BASE_ADDR, I2C_RECEIVER);
    i2cSetMode(I2C_BASE_ADDR, I2C_MASTER);
    i2cReceive(I2C_BASE_ADDR, length, data);

    while (i2cIsBusBusy(I2C_BASE_ADDR))
        ;
    while (i2cIsStopDetected() == 0)
        ;
    i2cClearSCD();
}
