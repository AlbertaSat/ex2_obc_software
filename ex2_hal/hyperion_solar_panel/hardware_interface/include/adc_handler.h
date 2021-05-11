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
 * @author  Vasu Gupta, Robert Taylor
 * @date    2020-06-15
 */


#ifndef ADC_HANDLER_H
#define ADC_HANDLER_H

#include "common_defines.h"
#include <stdint.h>
#include "HL_i2c.h"


// AD7298 Control Register Map
#define AD7298_REPEAT       (1U << 0U)
#define AD7298_TSENSE       (1U << 7U)
#define AD7298_EXT_REF      (1U << 4U)
#define AD7298_NOISE_DELAY  (1U << 5U)
#define AD7298_PPD          (1U << 3U)
#define AD7298_CLEAR_ALERT  (1U << 2U)
#define AD7298_RESET        (1U << 1U)

#define AD7298_RES          4096


// typedef struct adc_handler_t ADC_Handler;

// struct adc_handler_t{
//     //Current value of the ADC control register
//     uint16_t  control_reg_val;   

//     // SPI data register configuration 
//     spiDAT1_t spi_dat_conf;

//     // Select Panel
//     enum panel_t panel;
// };


unsigned short control_reg_val;

// Initialize ADC defaults
unsigned char adc_init(void);
void adc_set_command_reg(uint8_t channel,
                                 uint8_t ext_ref,
                                 uint8_t tsense,
                                 uint8_t noise_delay,
                                 uint8_t reset,
                                 uint8_t autocycle);

void adc_set_register_pointer(uint8_t reg_sel);

//return the raw value from the adc
void adc_get_raw(unsigned short *data, unsigned char *ch);

//calculate the vin voltage value
float adc_calculate_vin(unsigned short value, float vref);

//sensor calculations
float adc_calculate_sensor_temp(unsigned short value, float vref);
float adc_calculate_sensor_voltage(unsigned short value, float vref);
float adc_calculate_sensor_current(unsigned short value, float vref);
float adc_calculate_sensor_pd(unsigned short value, float vref);

// convert internal temp sensor value
float adc_get_tsense_temp(unsigned short value, float vref);

#endif
