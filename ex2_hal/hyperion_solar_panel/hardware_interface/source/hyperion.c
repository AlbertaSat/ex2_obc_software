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
 * @file    hyperion.c
 * @author  Trung Tran
 * @date    2021-06-04
 */
#include "hyperion.h"


/**
 * @brief
 * 		Get the data from a panel and its corresponding channel
 * @details
 * 		Temperature, photodiodes, voltage, and current values of a panel can be obtained via this function 
 *      Refer to Hyperion Detailed Design document
 * @attention
 * 		This function is used for config 1, which only includes Port, Port Deployable, Starboard, Startboard Deployable and Zenith
 * @param panel
 * 		The panel that we want to retrieve data from 
 * @param channel
 * 		voltage channel corresponding to that panel
 * @param param
 * 		The value that corresponds to each panel and channel
 * @return
 * 		void
 */
void hyperion_config_1_value(config_1_panel_t panel, config_1_channel_type_t channel, float* param) {
    unsigned short data = 0;
    unsigned char ch = 0; // channel
    uint8_t slave_addr = 0;

    switch (panel)
    {
    case CONFIG_1_PANEL_P:
        slave_addr = PANEL_SLAVE_ADDR_PORT;
        break;

    case CONFIG_1_PANEL_PD:
        slave_addr = PANEL_SLAVE_ADDR_PORT_DEPLOYABLE;
        break;

    case CONFIG_1_PANEL_S:
        slave_addr = PANEL_SLAVE_ADDR_STARBOARD;
        break;

    case CONFIG_1_PANEL_SD:
        slave_addr = PANEL_SLAVE_ADDR_STARBOARD_DEPLOYABLE;
        break;

    case CONFIG_1_PANEL_Z:
        slave_addr = PANEL_SLAVE_ADDR_ZENITH;
        break;

    default:
        break;
    }


    switch (channel)
    {
    /* Channel 1 to 3 handle temp sensor */

    case CONFIG_1_CHANNEL_TEMP_1:
        adc_init(slave_addr, 1 << 7);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_temp(data, ADC_VREF);
        break;

    case CONFIG_1_CHANNEL_TEMP_2:
        adc_init(slave_addr, 1 << 6);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_temp(data, ADC_VREF);
        break;

    case CONFIG_1_CHANNEL_TEMP_3:
        adc_init(slave_addr, 1 << 5);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_temp(data, ADC_VREF);
        break;

    case CONFIG_1_CHANNEL_PD_1:
        adc_init(slave_addr, 1 << 4);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_pd(data, ADC_VREF);
        break;

    case CONFIG_1_CHANNEL_PD_2:
        adc_init(slave_addr, 1 << 3);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_pd(data, ADC_VREF);
        break;

    case CONFIG_1_CHANNEL_PD_3:
        adc_init(slave_addr, 1 << 2);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_pd(data, ADC_VREF);
        break;

    case CONFIG_1_CHANNEL_VOLT:
        adc_init(slave_addr, 1 << 1);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_voltage(data, ADC_VREF);
        break;

    case CONFIG_1_CHANNEL_CURR:
        adc_init(slave_addr, 1 << 0);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_current(data, ADC_VREF);
        break;

    case CONFIG_1_ADC_TEMP:
        adc_init(slave_addr, 1 << 7);
        *param = adc_get_tsense_temp(slave_addr, ADC_VREF);
         break;

    default:
        break;
    }
}


/**
 * @brief
 * 		Get the data from a panel and its corresponding channel
 * @details
 * 		Temperature, photodiodes, voltage, and current values of a panel can be obtained via this function 
 *      Refer to Hyperion Detailed Design document
 * @attention
 * 		This function is used for config 2, which only includes Arke, Nadir, and Nadir 2U
 * @param panel
 * 		The panel that we want to retrieve data from 
 * @param channel
 * 		voltage channel corresponding to that panel
 * @param param
 * 		The value that corresponds to each panel and channel
 * @return
 * 		void
 */
void hyperion_config_2_value(config_2_panel_t panel, config_2_channel_type_t channel, float* param) {
    unsigned short data = 0;
    unsigned char ch = 0; // channel
    int i=0;
    uint8_t slave_addr = 0;

    switch (panel)
    {
    case CONFIG_2_PANEL_ARKE:
        slave_addr = PANEL_SLAVE_ADDR_ARKE;
        break;

    case CONFIG_2_PANEL_NADIR:
        slave_addr = PANEL_SLAVE_ADDR_NADIR;
        break;

    case CONFIG_2_PANEL_NADIR2U:
        slave_addr = PANEL_SLAVE_ADDR_NADIR2U;
        break;

    default:
        break;
    }

    switch (channel)
    {
    case CONFIG_2_CHANNEL_TEMP_1:
        adc_init(slave_addr, 1 << 7);
        adc_get_raw(slave_addr, &data, &ch);
        *param = (float) adc_calculate_sensor_temp(data, ADC_VREF);
        break;

    case CONFIG_2_CHANNEL_PD_1:
        adc_init(slave_addr, 1 << 6);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_pd(data, ADC_VREF);
        break;

    case CONFIG_2_ADC_TEMP:
        adc_init(slave_addr, 1 << 7);
        *param = adc_get_tsense_temp(slave_addr, ADC_VREF);
         break;

    default:
        break;
    }
    uint8_t test=0;
}

/**
 * @brief
 * 		Get the data from a panel and its corresponding channel
 * @details
 * 		Temperature, photodiodes, voltage, and current values of a panel can be obtained via this function 
 *      Refer to Hyperion Detailed Design document
 * @attention
 * 		This function is used for config 3, which only includes Port2U, Port Deployable 2U, Starboard 2U, Startboard Deployable 2U and Zenith 2U
 * @param panel
 * 		The panel that we want to retrieve data from 
 * @param channel
 * 		voltage channel corresponding to that panel
 * @param param
 * 		The value that corresponds to each panel and channel
 * @return
 * 		void
 */
 void hyperion_config_3_value(config_3_panel_t panel, config_3_channel_type_t channel, float* param) {
    unsigned short data = 0;
    unsigned char ch = 0; // channel
    uint8_t slave_addr = 0;

    switch (panel)
    {
    case CONFIG_3_PANEL_P2U:
        slave_addr = PANEL_SLAVE_ADDR_PORT2U;
        break;
    
    case CONFIG_3_PANEL_PD2U:
        slave_addr = PANEL_SLAVE_ADDR_PORT_DEPLOYABLE2U;
        break;
    
    case CONFIG_3_PANEL_S2U:
        slave_addr = PANEL_SLAVE_ADDR_STARBOARD2U;
        break;

    case CONFIG_3_PANEL_SD2U:
        slave_addr = PANEL_SLAVE_ADDR_STARBOARD_DEPLOYABLE2U;
        break;
    
    case CONFIG_3_PANEL_Z2U:
        slave_addr = PANEL_SLAVE_ADDR_ZENITH2U;
        break;

    default:
        break;
    }

    switch (channel)
    {
    case CONFIG_3_CHANNEL_TEMP_1:
        adc_init(slave_addr, 1 << 7);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_temp(data, ADC_VREF);
        break;

    case CONFIG_3_CHANNEL_TEMP_2:
        adc_init(slave_addr, 1 << 6);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_temp(data, ADC_VREF);
        break;

    case CONFIG_3_CHANNEL_PD_1:
        adc_init(slave_addr, 1 << 5);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_pd(data, ADC_VREF);
        break;

    case CONFIG_3_CHANNEL_PD_2:
        adc_init(slave_addr, 1 << 4);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_pd(data, ADC_VREF);
        break;

    case CONFIG_3_CHANNEL_VOLT:
        adc_init(slave_addr, 1 << 3);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_voltage(data, ADC_VREF);
        break;

    case CONFIG_3_CHANNEL_CURR:
        adc_init(slave_addr, 1 << 2);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_current(data, ADC_VREF);
        break;

    case CONFIG_3_ADC_TEMP:
        adc_init(slave_addr, 1 << 7);
        *param = adc_get_tsense_temp(slave_addr, ADC_VREF);
         break;
    
    default:
        break;
    }
 }
