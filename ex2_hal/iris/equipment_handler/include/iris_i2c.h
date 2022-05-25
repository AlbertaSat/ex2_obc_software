/*
 * i2c_driver.h
 *
 *  Created on: May 10, 2022
 *      Author: jenish
 */

#ifndef INCLUDE_IRIS_I2C_H_
#define INCLUDE_IRIS_I2C_H_
#endif /* INCLUDE_IRIS_I2C_H_ */

#include "FreeRTOS.h"

#define SLAVE_ADDR 0x20

/* TODO: Function prototypes    */
void iris_i2c_init();
int send_packet(void *buf_tx_data, uint16_t data_length);
int read_packet(void *buf_rx_data, uint16_t data_length);
