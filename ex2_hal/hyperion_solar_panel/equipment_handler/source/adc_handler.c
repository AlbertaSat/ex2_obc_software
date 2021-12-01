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
 * @author  Quoc Trung Tran, Vasu Gupta, Nicholas Sorensen
 * @date    2020-06-15
 */

#include <ex2_hal/ex2_hyperion_solar_panel_software/equipment_handler/include/adc_handler.h>
#include <stdint.h>
#include "i2c_io.h"
#include <stdint.h>

/**
 * @brief
 * 		Initialize ADC_Handler for config 1 (include Port, Port Deployable, Starboard, StarBoard Deployable, Zenith)
 * @details
 * 		
 * @return
 * 		1 == success
 */
unsigned char adc_init(uint8_t slave_addr, uint8_t channel) {
    adc_set_command_reg(slave_addr, channel, 0, 1, 1, 0, 1);
    for(delay=0;delay<10000000;delay++);
    uint8_t reg_sel = 1; // select read register
    adc_set_register_pointer(slave_addr, reg_sel);
    for(delay=0;delay<1000000;delay++);

    return 1;
}

// return 0 on success; -1 on failure
int adc_write(uint8_t *buf, int size, uint8_t slave_addr) {
    // TODO: Make this use error code return
    return i2c_Send(ADC_i2c_PORT, slave_addr, size, buf);
}

// return 0 on success; -1 on failure
int adc_read(uint8_t *buf, uint32_t size, uint8_t slave_addr) {
    // TODO: make this use error code return
    return i2c_Receive(ADC_i2c_PORT, slave_addr, size, buf);
}

/**
 * @brief
 * 		Set and send control register bits to the ADC
 * @details
 * 		Refer to page 16 of AD7291 datasheet for more details
 * @param slave_addr
 *      Slave address of the panel we want to read data
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
 * @param noise_delay
 *      Enable/Disable the critical sampling intervals and bit trials when there is activity on I2C bus
 * 		1 = Enable
 *      0 = Disable
 * @param reset
 *      Enable/Disable reset the contents of all internal registers in the AD7291 to default states
 *      1 = Enable
 *      0 = Disable
 * @param autocycle
 *      Enable/Disable autocycle mode of operation
 * @return
 * 		0: success
 *      -1: fail
 */
int adc_set_command_reg(uint8_t slave_addr, uint8_t channel, uint8_t ext_ref, uint8_t tsense, uint8_t noise_delay,
                        uint8_t reset, uint8_t autocycle) {
    int return_val;
    uint8_t buffer[3] = {0, 0, 0};

    uint8_t control_reg_value = 0;
    buffer[1] = channel;

    control_reg_value = (ext_ref * AD7291_EXT_REF) | (tsense * AD7291_TSENSE) | (reset * AD7291_RESET) |
                        (noise_delay * AD7291_NOISE_DELAY) | (autocycle * AD7291_REPEAT);

    buffer[2] = control_reg_value;

    // i2c data send
    return_val = adc_write(buffer, 3, slave_addr);

    control_reg_val = control_reg_value;
    return return_val;
}

int adc_set_register_pointer(uint8_t slave_addr, uint8_t reg_sel) { return adc_write(&reg_sel, 1, slave_addr); }

/**
 * @brief
 * 		Returns the conversion results from ADC
 * @details
 * 		The ADC returns a 16-bit value, in which the 4 MSB are channel address bits,
 *      and the remaining 12 is the raw ADC value
 * @attention
 *      This function MUST be called twice (second time gives the correct value).
 *      According to the AD7291 datasheet, first read from DOUT will always be invalid.
 * @param data
 * 		Buffer for storing ADC raw value
 * @param ch
 * 		Buffer for storing which channel the value was received from.
 * @return
 * 		0: success
 *      -1: fail
 */
int adc_get_raw(uint8_t slave_addr, unsigned short *data, unsigned char *ch) {
    int ret;
    unsigned char buffer[2] = {0, 0};

    // i2c slave read
    ret = adc_read(buffer, 2, slave_addr);

    unsigned short value = (buffer[0] << 8) | buffer[1];

    // get current channel (first 4 bits)
    *ch = (value >> 12);

    // remove channel information from the 16 bit read.
    value = value - (*ch << 12);
    // get current data channel as well.

    *data = value;
    return ret;
}

/**
 * @brief
 * 		Converts the given raw ADC value to voltage (mV), relative to reference voltage.
 * @param value
 * 		The raw ADC value
 *      This value can be retrieved using adc_get_raw(..)
 * @param vref
 * 		The value of reference voltage (in mV) provided to the ADC module
 *      Refer to AD7291 datasheet.
 * @return
 * 		Value in mV.
 */
float adc_calculate_vin(unsigned short value, float vref) {
    float volts = 0;

    // from AD7291 datasheet
    volts = ((float)value * vref * 1000) / AD7298_RES;

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
 *      Refer to AD7291 datasheet.
 * @return
 * 		Value in celsius.
 */
float adc_calculate_sensor_temp(unsigned short value, float vref) {
    float celsius = 0;
    int i = 0;

    // change into mv
    float temp_voltage = adc_calculate_vin(value, vref);

    // Conversion parameters from temperature sensor datasheet
    // https://www.ti.com/lit/ds/symlink/lmt70.pdf?HQS=dis-dk-null-digikeymode-dsf-pf-null-wwe&ts=1622648303670
//    float voltage_vect[22] = {1375.219, 1350.441,1300.593,1250.398,1199.884,1149.070,1097.987,1046.647,995.050,943.227,891.178,838.882,786.36,733.608,680.654,627.49,574.117,520.551,466.76,412.739,358.164,302.785};
//    float slope[22] = {4.958,4.976,5.002,5.036,5.066,5.108,5.121,5.134,5.171,5.194,5.217,5.241,5.264,5.285,5.306,5.327,5.347,5.368,5.391,5.43,5.498,5.538};
//    float temps[22] = {-55,-50,-40,-30,-20,-10,0,10,20,30,40,50,60,70,80,90,100,110,120,130,140,150};

    // Conversion parameters from new temperature sensor datasheet
    // https://www.ti.com/lit/ds/symlink/tmp235.pdf?HQS=dis-dk-null-digikeymode-dsf-pf-null-wwe&ts=1627422300911&ref_url=https%253A%252F%252Fwww.digikey.ca%252F
    float voltage_vect[39] = {100,150,200,250,300,350,400,450,500,550,600,650,700,750,800,850,900,950,1000,1050,1100,1150,1200,1250,1300,1350,1400,1450,1500,1550.5,1601,1651.5,1702,1752.5,1805.5,1858.5,1911.5,1964.5,2017.5};
    float temps[39] = {-40,-35,-30,-35,-20,-15,-10,-5,0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100,105,110,115,120,125,130,135,140,145,150};

    //interpolates between the previously defined voltage to temperature conversions for the first temperature sensor.

//    if (temp_voltage >= voltage_vect[0] + (temps[1]-temps[0])/2*slope[0]) {
//        celsius = temps[0]+ (temp_voltage-voltage_vect[0])/slope[0];
//    } else if (temp_voltage <= voltage_vect[21] -(10)/2 * slope[21]) {
//        celsius = temps[21]+ (temp_voltage-voltage_vect[21])/slope[21];
//    } else {
//        for (i=1; i<20; i++) {
//            float highvolt = voltage_vect[i] + (temps[i+1]-temps[i])/2 * slope[i];
//            float lowvolt = voltage_vect[i] -(temps[i]-temps[i-1])/2 * slope[i];
//            if ((temp_voltage <= highvolt) && (temp_voltage > lowvolt)) {
//                celsius =  temps[i] - (temp_voltage-voltage_vect[i])/slope[i];
//                break;
//            }
//        }
//    }

    //interpolates between the previously defined voltage to temperature conversions for the second temperature sensor.

    if (temp_voltage >= voltage_vect[38]) { // If above the highest piecewise voltage
        celsius = temps[38] + (temp_voltage - voltage_vect[38]) / 10;
    } else if (temp_voltage <= voltage_vect[0]) { // If beneath the lowest piecewise voltage
        celsius = temps[0] + (temp_voltage - voltage_vect[0]) / 10;
    } else {
        for (i = 1; i < 37; i++) {
            if ((temp_voltage <= voltage_vect[i + 1]) && (temp_voltage > voltage_vect[i])) {
                celsius = temps[i] + (temp_voltage - voltage_vect[i]) / 10;
                break;
            }
        }
    }
    
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
 *      Refer to AD7291 datasheet.
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
 *      Refer to AD7291 datasheet.
 * @return
 * 		Value in mA.
 */
float adc_calculate_sensor_current(unsigned short value, float vref) {
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
 *      Refer to AD7291 datasheet.
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
 *      Refer to AD7291 datasheet.
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
 *      Refer to AD7291 datasheet.
 * @return
 * 		Temperature value in celsius
 */
float adc_get_tsense_temp(uint8_t slave_addr, float vref) {
    int delay;
    unsigned short data = 0;
    unsigned char ch = 0;
    // printf("\n ADC TEMP RESULTS: \r\n Channel    Result \r\n");
    // loops through and requests conversion results from all channels
    uint8_t reg_sel = 2;
    adc_set_register_pointer(slave_addr, reg_sel);
    for (delay = 0; delay < 200000; delay++)
        ;
    adc_get_raw(slave_addr, &data, &ch);
    float temp_celsius = 0;
    unsigned short value = data;
    temp_celsius =
        (float)(value >> 11) * (float)(-512) + (float)((value - ((value >> 11) << 11)) >> 10) * (float)(256) +
        (float)((value - ((value >> 10) << 10)) >> 9) * (128) +
        (float)((value - ((value >> 9) << 9)) >> 8) * (64) + (float)((value - ((value >> 8) << 8)) >> 7) * (32) +
        (float)((value - ((value >> 7) << 7)) >> 6) * (16) + (float)((value - ((value >> 6) << 6)) >> 5) * (8) +
        (float)((value - ((value >> 5) << 5)) >> 4) * (4) + (float)((value - ((value >> 4) << 4)) >> 3) * (2) +
        (float)((value - ((value >> 3) << 3)) >> 2) * (1) + (float)((value - ((value >> 2) << 2)) >> 1) * (0.5) +
        (float)((value - ((value >> 1) << 1)) >> 0) * (0.25);
    // printf("%d          %.2f \r\n", ch, temp_celsius);
    return temp_celsius;
}
