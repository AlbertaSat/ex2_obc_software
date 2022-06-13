/*
 * i2c_bootloader_cmds.c
 *
 *  Created on: May 10, 2022
 *      Author: jenish
 */

#include "iris_i2c.h"
#include "FreeRTOS.h"
#include "i2c_io.h"
#include "system.h"
#include <stdio.h>

/* Optimization points
 * - Although it is not mission critical, we could read back from
 *   iris for debugging purpose. As of now we have tested and verified
 *   that we can load .bin file accurately to iris.
 */

int write_packet(void *buf_tx_data, uint16_t data_length) {
    return i2c_Send(IRIS_I2C, SLAVE_ADDR, data_length, buf_tx_data); //Data size is constant as 7-bit data + 1-bit R/W (8 bits), should this be constant?
}

int read_packet(void *buf_rx_data, uint16_t data_length) {
    return i2c_Receive(IRIS_I2C, SLAVE_ADDR, data_length, &buf_rx_data); //Data is stored in buf_tx_data
}
