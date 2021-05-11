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
 * @file    adc_handler.c
 * @author  Vasu Gupta
 * @date    2020-06-15
 */

#include "adc_handler.h"
#include <stdint.h>
#include "printf.h"

/**
 * @brief
 * 		Initialize ADC_Handler
 * @details
 * 		
 * @return
 * 		1 == success
 */
unsigned char adc_init(void) {
    int delay;
    adc_set_command_reg(1<<7,0,0,1,0,1);
    for(delay=0;delay<10000000;delay++);
    uint8_t reg_sel = 1;
    adc_set_register_pointer(reg_sel);
    for(delay=0;delay<1000000;delay++);

    unsigned short data = 0;
    unsigned char ch = 0;
    adc_get_raw(&data, &ch);
    printf("Data: %x, ch: %x\r\n", ch, data);

    return 1;
}

void adc_write(uint8_t *buf, int size, uint8_t slave_addr) {
    i2cSetSlaveAdd(ADC_i2c_PORT, slave_addr);
    i2cSetDirection(i2cREG1, I2C_TRANSMITTER);
    i2cSetCount(ADC_i2c_PORT, size);
    /* Set mode as Master */
    i2cSetMode(i2cREG1, I2C_MASTER);


    i2cSetStop(ADC_i2c_PORT);
    /* Transmit Start Condition */
    i2cSetStart(ADC_i2c_PORT);
    i2cSend(ADC_i2c_PORT, size, buf);

    /* Wait until Bus Busy is cleared */
    while(i2cIsBusBusy(ADC_i2c_PORT) == true);

    /* Wait until Stop is detected */
    //while(i2cIsStopDetected(ADC_i2c_PORT) == 0);
    /* Clear the Stop condition */
    i2cClearSCD(ADC_i2c_PORT);
}

void adc_request_from(uint16_t slave_addr) {

}

void adc_read(uint8_t *data, uint32_t length, uint8_t slave_addr) {
    i2cSetSlaveAdd(ADC_i2c_PORT, slave_addr);
    i2cSetDirection(i2cREG1, I2C_RECEIVER);
    i2cSetCount(i2cREG1, length);
    i2cSetMode(i2cREG1, I2C_MASTER);
    /* Set Stop after programmed Count */
    i2cSetStop(i2cREG1);
    /* Transmit Start Condition */
    i2cSetStart(ADC_i2c_PORT);

    i2cReceive(ADC_i2c_PORT,length,data);

    /* Wait until Bus Busy is cleared */
    while(i2cIsBusBusy(i2cREG1) == true);

    /* Wait until Stop is detected */
    //while(i2cIsStopDetected(i2cREG1) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(i2cREG1);

}

void adc_end_request() {

}

/**
 * @brief
 * 		Set and send control register bits to the ADC
 * @details
 * 		Refer to page 16 of AD7298 datasheet for more details
 * @param handl
 * 		Pointer to ADC_Handler struct
 * @param repeat
 * 		1 = Enable
 *      0 = Disable
 * @param channel
 * 		Select which channel to enable (0-7)
 *      Accepted values: 0-7
 * @param ext_ref
 *      Enable/Disable the use of external reference.
 * 		1 = Enable
 *      0 = Disable
 * @param tsense
 *      Enable/Disable (internal) temperature conversion
 * 		1 = Enable
 *      0 = Disable
 * @param tsense_avg
 *      Enable/Disable temperature sensor averaging function
 * 		1 = Enable
 *      0 = Disable
 * @return
 * 		None
 */
void adc_set_command_reg(uint8_t channel,
                                 uint8_t ext_ref,
                                 uint8_t tsense,
                                 uint8_t noise_delay,
                                 uint8_t reset,
                                 uint8_t autocycle)
{
    uint8_t buffer[3] = {0,0,0};

    uint8_t control_reg_value = 0;
    buffer[1] = channel;

    control_reg_value = (ext_ref * AD7298_EXT_REF) |
                        (tsense * AD7298_TSENSE)   |
                        (reset * AD7298_RESET)     |
                        (noise_delay * AD7298_NOISE_DELAY) |
                        (autocycle * AD7298_REPEAT);

    buffer[2] = control_reg_value;
  
    //i2c data send
    adc_write(buffer, 3, ADC_SLAVE_ADDR);


    control_reg_val = control_reg_value;
}

void adc_set_register_pointer(uint8_t reg_sel) {
    adc_write(&reg_sel, 1, ADC_SLAVE_ADDR);
}

/**
 * @brief
 * 		Returns the conversion results from ADC
 * @details
 * 		The ADC returns a 16-bit value, in which the 4 MSB are channel address bits,
 *      and the remaining 12 is the raw ADC value
 * @attention
 *      This function MUST be called twice (second time gives the correct value).
 *      According to the AD7298 datasheet, first read from DOUT will always be invalid.
 * @param data
 * 		Buffer for storing ADC raw value
 * @param ch
 * 		Buffer for storing which channel the value was received from.
 * @return
 * 		None
 */
void adc_get_raw(unsigned short *data, unsigned char *ch)
{  
    unsigned char buffer[2] = {0,0};
    //unsigned short buffer_H = 0;
    //unsigned short buffer_L = 0;

    unsigned short value = 0;

    //i2c slave read
    adc_read(buffer, 2, ADC_SLAVE_ADDR);

    value = (buffer[1] << 8) | buffer[0];
    
    *data = value & 0x0FFF;
    
    //get current data channel as well.
    *ch   = (value >> 12);
}

/**
 * @brief
 * 		Converts the given raw ADC value to voltage (mV), relative to reference voltage.
 * @param value
 * 		The raw ADC value
 *      This value can be retrieved using adc_get_raw(..)
 * @param vref
 * 		The value of reference voltage (in mV) provided to the ADC module
 *      Refer to AD7298 datasheet.
 * @return
 * 		Value in mV.
 */
float adc_calculate_vin(unsigned short value, float vref) {
    float volts = 0;

    // from AD7298 datasheet
    volts = ((float)value * vref) / AD7298_RES;

    return volts;
}

/**
 * @brief
 * 		Calculates temperature in celsius from temperature sensor output voltage.
 * @param value
 * 		The raw ADC value
 *      This value can be retrieved using adc_get_raw(..)
 * @param vref
 * 		The value of reference voltage (in mV) provided to the ADC module
 *      Refer to AD7298 datasheet.
 * @return
 * 		Value in celsius.
 */
float adc_calculate_sensor_temp(unsigned short value, float vref) {
    float temp_voltage = adc_calculate_vin(value, vref);

    float celsius = 0;

    celsius = temp_voltage - TEMP_VOLT_MIN;
    celsius = (celsius * (TEMP_VAL_MAX - TEMP_VAL_MIN)) 
                / (TEMP_VOLT_MAX - TEMP_VOLT_MIN);
    celsius = TEMP_VAL_MAX - celsius;
    
    return celsius;
}

/**
 * @brief
 * 		Converts the given raw ADC value to voltage (mV), relative to reference voltage.
 * @param value
 * 		The raw ADC value
 *      This value can be retrieved using adc_get_raw(..)
 * @param vref
 * 		The value of reference voltage (in mV) provided to the ADC module
 *      Refer to AD7298 datasheet.
 * @return
 * 		Value in mV.
 */
float adc_calculate_sensor_voltage(unsigned short value, float vref) {
    float val = adc_calculate_vin(value, vref);

    val = val * ((VOLT_MAX - VOLT_MIN) / (ADC_VOLT_MAX - ADC_VOLT_MIN)); 

    return val;
}

/**
 * @brief
 * 		Converts the given raw ADC value to current (mA), relative to reference voltage.
 * @param value
 * 		The raw ADC value
 *      This value can be retrieved using adc_get_raw(..)
 * @param vref
 * 		The value of reference voltage (in mV) provided to the ADC module
 *      Refer to AD7298 datasheet.
 * @return
 * 		Value in mA.
 */
float adc_calculate_sensor_current(unsigned short value, float vref)
{
    float val = adc_calculate_vin(value, vref);

    val = val * ((CURR_MAX - CURR_MIN) / (ADC_VOLT_MAX - ADC_VOLT_MIN)); 

    return val;
}

/**
 * @brief
 * 		Calculates temperature in celsius from temperature sensor output voltage.
 * @param value
 * 		The raw ADC value
 *      This value can be retrieved using adc_get_raw(..)
 * @param vref
 * 		The value of reference voltage (in mV) provided to the ADC module
 *      Refer to AD7298 datasheet.
 * @return
 * 		Value in celsius.
 */
float adc_calculate_sensor_pd(unsigned short value, float vref)
{
    float val = adc_calculate_vin(value, vref);

    return val;
}

/**
 * @brief
 * 		Converts given raw ADC temperature value (from internal temp sensor) to celsius 
 *      (relative to reference voltage)
 * @details
 * 		The raw ADC value is retrieved from the TSENSE channel on the ADC
 *      Refer to AD7298 datasheet.
 * @attention
 * 		This function is to be used only if 'TSENSE' on the ADC control reg. is enabled.
 * @param handl
 * 		Pointer to ADC_Handler struct
 * @param value
 * 		The raw ADC temperature value
 *      This value can be retrieved usig adc_get_raw(..) and when 'TSENSE' is enabled
 *      by setting the control register
 * @param vref
 * 		The value of reference voltage (in mV) provided to the ADC module
 *      Refer to AD7298 datasheet.
 * @return
 * 		Temperature value in celsius
 */
float adc_get_tsense_temp(unsigned short value, float vref) {
    float temp_celsius = 0;

    if(value >= 0x800) {
        value  = AD7298_RES - value;
        value *= -1;
    }

    // from AD7298 datasheet
    temp_celsius = vref * (((float)value / 10) + 109.3) - 273.15; 

    return temp_celsius;
}

