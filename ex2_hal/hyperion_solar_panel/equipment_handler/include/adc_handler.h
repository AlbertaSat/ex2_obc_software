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
 * @file    adc_handler.h
 * @author  Vasu Gupta, Robert Taylor, Trung Tran, Nicholas Sorensen
 * @date    2020-06-15
 */

#ifndef ADC_HANDLER_H
#define ADC_HANDLER_H

#include "HL_i2c.h"
#include <common_defines.h>
#include <stdint.h>

/*
 * AD7291 registers definition
 */
#define AD7291_COMMAND 0x00
#define AD7291_VOLTAGE 0x01
#define AD7291_T_SENSE 0x02
#define AD7291_T_AVERAGE 0x03
#define AD7291_CH0_DATA_HIGH 0x04
#define AD7291_CH0_DATA_LOW 0x05
#define AD7291_CH0_HYST 0x06
#define AD7291_CH1_DATA_HIGH 0x07
#define AD7291_CH1_DATA_LOW 0x08
#define AD7291_CH1_HYST 0x09
#define AD7291_CH2_DATA_HIGH 0x0A
#define AD7291_CH2_DATA_LOW 0x0B
#define AD7291_CH2_HYST 0x0C
#define AD7291_CH3_DATA_HIGH 0x0D
#define AD7291_CH3_DATA_LOW 0x0E
#define AD7291_CH3_HYST 0x0F
#define AD7291_CH4_DATA_HIGH 0x10
#define AD7291_CH4_DATA_LOW 0x11
#define AD7291_CH4_HYST 0x12
#define AD7291_CH5_DATA_HIGH 0x13
#define AD7291_CH5_DATA_LOW 0x14
#define AD7291_CH5_HYST 0x15
#define AD7291_CH6_DATA_HIGH 0x16
#define AD7291_CH6_DATA_LOW 0x17
#define AD7291_CH6_HYST 0x18
#define AD7291_CH7_DATA_HIGH 0x19
#define AD7291_CH7_DATA_LOW 0x1A
#define AD7291_CH7_HYST 0x2B
#define AD7291_T_SENSE_HIGH 0x1C
#define AD7291_T_SENSE_LOW 0x1D
#define AD7291_T_SENSE_HYST 0x1E
#define AD7291_VOLTAGE_ALERT_STATUS 0x1F
#define AD7291_T_ALERT_STATUS 0x20

#define AD7291_VOLTAGE_LIMIT_COUNT 8

// AD7291 TSENSE Average setting
#define AD7291_TSENSE_AVG true

// AD7291 Control Register Map
#define AD7291_REPEAT_SET 1
#define AD7291_TSENSE_SET 1
#define AD7291_EXT_REF_SET 0
#define AD7291_NOISE_DELAY_SET 1
#define AD7291_RESET_SET 0

#define AD7291_REPEAT (1U << 0U)
#define AD7291_TSENSE (1U << 7U)
#define AD7291_EXT_REF (1U << 4U)
#define AD7291_NOISE_DELAY (1U << 5U)
#define AD7291_PPD (1U << 3U)
#define AD7291_CLEAR_ALERT (1U << 2U)
#define AD7291_RESET (1U << 1U)

#define AD7291_RES 4096

#define AD7291_INIT_DELAY_TICKS pdMS_TO_TICKS(0)

#define AD7291_BAD_TEMP -128
#define AD7291_BAD_PD 255

unsigned short control_reg_val;

// Initialize ADC defaults
unsigned char adc_init(uint8_t slave_addr, uint8_t channel);
int adc_set_command_reg(uint8_t slave_addr, uint8_t channel, uint8_t ext_ref, uint8_t tsense, uint8_t noise_delay,
                        uint8_t reset, uint8_t autocycle);

int adc_set_register_pointer(uint8_t slave_addr, uint8_t reg_sel);

// return the raw value from the adc
int adc_get_raw(uint8_t slave_addr, unsigned short *data, unsigned char *ch);

// calculate the vin voltage value
float adc_calculate_vin(unsigned short value, float vref);

// sensor calculations
float adc_calculate_sensor_temp(unsigned short value, float vref);
float adc_calculate_sensor_voltage(unsigned short value, float vref);
float adc_calculate_sensor_current(unsigned short value, float vref);
float adc_calculate_sensor_pd(unsigned short value, float vref);

// convert internal temp sensor value
float adc_get_tsense_temp(uint8_t slave_addr, float vref);
// convert all channels raw
void adc_get_all_raw(void);

// convert all channels vin
void adc_get_all_vin(void);

#endif
