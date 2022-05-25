/*
 * i2c_bootloader_cmds.c
 *
 *  Created on: May 10, 2022
 *      Author: jenish
 */

#include "iris_i2c.h"
#include "FreeRTOS.h"
#include "i2c_io.h"
#include "HL_sys_common.h"
#include "system.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t rx_data;
uint8_t tx_data;
int ret; // TODO: Is this the "acknowledgment"

void iris_i2c_init() {
   /* i2c initialization */
    init_i2c_driver();
}

int write_packet(void *buf_tx_data, uint16_t data_length) {
    return i2c_Send(IRIS_I2C, SLAVE_ADDR, data_length, buf_tx_data); //Data size is constant as 7-bit data + 1-bit R/W (8 bits), should this be constant?
}

int read_packet(void *buf_rx_data, uint16_t data_length) {
    return i2c_Receive(IRIS_I2C, SLAVE_ADDR, data_length, &rx_data); //Data is stored in buf_tx_data
}
