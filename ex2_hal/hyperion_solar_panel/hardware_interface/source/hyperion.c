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
 * @author  Trung Tran, Nicholas Sorensen
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
 * 		This function is used for config 1, which only includes Port, Port Deployable, Starboard, Startboard
 * Deployable and Zenith
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

    switch (panel) {
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
    }
    case CONFIG_1_CHANNEL_TEMP_2: {
        adc_init(slave_addr, ADC_CHANNEL_1);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_temp(data, ADC_VREF);
        break;
    }
    case CONFIG_1_CHANNEL_TEMP_3: {
        adc_init(slave_addr, ADC_CHANNEL_2);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_temp(data, ADC_VREF);
        break;
    }
    case CONFIG_1_CHANNEL_PD_1: {
        adc_init(slave_addr, ADC_CHANNEL_3);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_pd(data, ADC_VREF);
        break;
    }
    case CONFIG_1_CHANNEL_PD_2: {
        adc_init(slave_addr, ADC_CHANNEL_4);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_pd(data, ADC_VREF);
        break;
    }
    case CONFIG_1_CHANNEL_PD_3: {
        adc_init(slave_addr, ADC_CHANNEL_5);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_pd(data, ADC_VREF);
        break;
    }
    case CONFIG_1_CHANNEL_VOLT: {
        adc_init(slave_addr, ADC_CHANNEL_6);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_voltage(data, ADC_VREF);
        break;
    }
    case CONFIG_1_CHANNEL_CURR: {
        adc_init(slave_addr, ADC_CHANNEL_7);
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

    switch (panel) {
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
    case CONFIG_2_CHANNEL_TEMP_1:{
        adc_init(slave_addr, 1 << 7);
        adc_get_raw(slave_addr, &data, &ch);
        *param = (float) adc_calculate_sensor_temp(data, ADC_VREF);
        break;
    }
    case CONFIG_2_CHANNEL_PD_1: {
        adc_init(slave_addr, ADC_CHANNEL_1);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_pd(data, ADC_VREF);
        break;

    case CONFIG_2_ADC_TEMP:
        adc_init(slave_addr, 1 << 7);
        int8_t *result = (int8_t *)param;
        *result = (int8_t) adc_get_tsense_temp(slave_addr, ADC_VREF);
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
 * 		This function is used for config 3, which only includes Port2U, Port Deployable 2U, Starboard 2U,
 * Startboard Deployable 2U and Zenith 2U
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

    switch (panel) {
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
    case CONFIG_3_CHANNEL_TEMP_1:{
        adc_init(slave_addr, 1 << 7);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_temp(data, ADC_VREF);
        break;
    }
    case CONFIG_3_CHANNEL_TEMP_2: {
        adc_init(slave_addr, ADC_CHANNEL_1);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_temp(data, ADC_VREF);
        break;
    }
    case CONFIG_3_CHANNEL_PD_1: {
        adc_init(slave_addr, ADC_CHANNEL_2);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_pd(data, ADC_VREF);
        break;
    }
    case CONFIG_3_CHANNEL_PD_2: {
        adc_init(slave_addr, ADC_CHANNEL_3);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_pd(data, ADC_VREF);
        break;
    }
    case CONFIG_3_CHANNEL_VOLT: {
        adc_init(slave_addr, ADC_CHANNEL_4);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_voltage(data, ADC_VREF);
        break;
    }
    case CONFIG_3_CHANNEL_CURR: {
        adc_init(slave_addr, ADC_CHANNEL_5);
        adc_get_raw(slave_addr, &data, &ch);
        *param = adc_calculate_sensor_current(data, ADC_VREF);
        break;
    }
    case CONFIG_3_ADC_TEMP: {
        adc_init(slave_addr, ADC_CHANNEL_0);
        int8_t *result = (int8_t *)param;
        *result = (int8_t) adc_get_tsense_temp(slave_addr, ADC_VREF);
         break;
    
    default:
        break;
    }
}

void Hyperion_config1_getHK(Hyperion_HouseKeeping *hyperion_hk) {
    // NADIR TEMP 1
    hyperion_config_2_value(CONFIG_2_PANEL_NADIR, CONFIG_2_CHANNEL_PD_1, &hyperion_hk->Nadir_Temp1);

    // NADIR Temp Adc
    hyperion_hk->Nadir_Temp_Adc = adc_get_tsense_temp(PANEL_SLAVE_ADDR_NADIR, ADC_VREF);

    // Port TEMP 1 2 3
    hyperion_config_1_value(CONFIG_1_PANEL_P, CONFIG_1_CHANNEL_TEMP_1, &hyperion_hk->Port_Temp1);
    hyperion_config_1_value(CONFIG_1_PANEL_P, CONFIG_1_CHANNEL_TEMP_2, &hyperion_hk->Port_Temp2);
    hyperion_config_1_value(CONFIG_1_PANEL_P, CONFIG_1_CHANNEL_TEMP_3, &hyperion_hk->Port_Temp3);

    // Port Temp Adc
    hyperion_hk->Port_Temp_Adc = adc_get_tsense_temp(PANEL_SLAVE_ADDR_PORT, ADC_VREF);

    // Port Dep Temp 1 2 3
    hyperion_config_1_value(CONFIG_1_PANEL_PD, CONFIG_1_CHANNEL_TEMP_1, &hyperion_hk->Port_Dep_Temp1);
    hyperion_config_1_value(CONFIG_1_PANEL_PD, CONFIG_1_CHANNEL_TEMP_2, &hyperion_hk->Port_Dep_Temp2);
    hyperion_config_1_value(CONFIG_1_PANEL_PD, CONFIG_1_CHANNEL_TEMP_3, &hyperion_hk->Port_Dep_Temp3);

    // Port Dep Temp Adc
    hyperion_hk->Port_Dep_Temp_Adc = adc_get_tsense_temp(PANEL_SLAVE_ADDR_PORT_DEPLOYABLE, ADC_VREF);

    // Star Temp 1 2 3
    hyperion_config_1_value(CONFIG_1_PANEL_S, CONFIG_1_CHANNEL_TEMP_1, &hyperion_hk->Star_Temp1);
    hyperion_config_1_value(CONFIG_1_PANEL_S, CONFIG_1_CHANNEL_TEMP_2, &hyperion_hk->Star_Temp2);
    hyperion_config_1_value(CONFIG_1_PANEL_S, CONFIG_1_CHANNEL_TEMP_3, &hyperion_hk->Star_Temp3);

    // Star Temp Adc
    hyperion_hk->Star_Temp_Adc = adc_get_tsense_temp(PANEL_SLAVE_ADDR_STARBOARD, ADC_VREF);

    // Star Dep Temp 1 2 3
    hyperion_config_1_value(CONFIG_1_PANEL_SD, CONFIG_1_CHANNEL_TEMP_1, &hyperion_hk->Star_Dep_Temp1);
    hyperion_config_1_value(CONFIG_1_PANEL_SD, CONFIG_1_CHANNEL_TEMP_2, &hyperion_hk->Star_Dep_Temp2);
    hyperion_config_1_value(CONFIG_1_PANEL_SD, CONFIG_1_CHANNEL_TEMP_3, &hyperion_hk->Star_Dep_Temp3);

    // Star Dep Adc
    hyperion_hk->Star_Dep_Temp_Adc = adc_get_tsense_temp(PANEL_SLAVE_ADDR_STARBOARD_DEPLOYABLE, ADC_VREF);

    // Zenith Temp 1 2 3
    hyperion_config_1_value(CONFIG_1_PANEL_Z, CONFIG_1_CHANNEL_TEMP_1, &hyperion_hk->Zenith_Temp1);
    hyperion_config_1_value(CONFIG_1_PANEL_Z, CONFIG_1_CHANNEL_TEMP_2, &hyperion_hk->Zenith_Temp2);
    hyperion_config_1_value(CONFIG_1_PANEL_Z, CONFIG_1_CHANNEL_TEMP_3, &hyperion_hk->Zenith_Temp3);

    // Zenith Temp Adc
    hyperion_hk->Zenith_Temp_Adc = adc_get_tsense_temp(PANEL_SLAVE_ADDR_ZENITH, ADC_VREF);

    // Nadir Pd 1
    hyperion_config_2_value(CONFIG_2_PANEL_NADIR, CONFIG_2_CHANNEL_PD_1, &hyperion_hk->Nadir_Pd1);

    // Port Pd 1 2 3
    hyperion_config_1_value(CONFIG_1_PANEL_P, CONFIG_1_CHANNEL_PD_1, &hyperion_hk->Port_Pd1);
    hyperion_config_1_value(CONFIG_1_PANEL_P, CONFIG_1_CHANNEL_PD_2, &hyperion_hk->Port_Pd2);
    hyperion_config_1_value(CONFIG_1_PANEL_P, CONFIG_1_CHANNEL_PD_3, &hyperion_hk->Port_Pd3);

    // Port Dep Pd 1 2 3
    hyperion_config_1_value(CONFIG_1_PANEL_PD, CONFIG_1_CHANNEL_PD_1, &hyperion_hk->Port_Dep_Pd1);
    hyperion_config_1_value(CONFIG_1_PANEL_PD, CONFIG_1_CHANNEL_PD_2, &hyperion_hk->Port_Dep_Pd2);
    hyperion_config_1_value(CONFIG_1_PANEL_PD, CONFIG_1_CHANNEL_PD_3, &hyperion_hk->Port_Dep_Pd3);

    // Star Pd 1 2 3
    hyperion_config_1_value(CONFIG_1_PANEL_S, CONFIG_1_CHANNEL_PD_1, &hyperion_hk->Star_Pd1);
    hyperion_config_1_value(CONFIG_1_PANEL_S, CONFIG_1_CHANNEL_PD_2, &hyperion_hk->Star_Pd2);
    hyperion_config_1_value(CONFIG_1_PANEL_S, CONFIG_1_CHANNEL_PD_3, &hyperion_hk->Star_Pd3);

    // Star Dep Pd 1 2 3
    hyperion_config_1_value(CONFIG_1_PANEL_SD, CONFIG_1_CHANNEL_PD_1, &hyperion_hk->Star_Dep_Pd1);
    hyperion_config_1_value(CONFIG_1_PANEL_SD, CONFIG_1_CHANNEL_PD_2, &hyperion_hk->Star_Dep_Pd2);
    hyperion_config_1_value(CONFIG_1_PANEL_SD, CONFIG_1_CHANNEL_PD_3, &hyperion_hk->Star_Dep_Pd3);

    // Zenith Pd 1 2 3
    hyperion_config_1_value(CONFIG_1_PANEL_Z, CONFIG_1_CHANNEL_PD_1, &hyperion_hk->Zenith_Pd1);
    hyperion_config_1_value(CONFIG_1_PANEL_Z, CONFIG_1_CHANNEL_PD_2, &hyperion_hk->Zenith_Pd2);
    hyperion_config_1_value(CONFIG_1_PANEL_Z, CONFIG_1_CHANNEL_PD_3, &hyperion_hk->Zenith_Pd3);

    // Port Voltage
    hyperion_config_1_value(CONFIG_1_PANEL_P, CONFIG_1_CHANNEL_VOLT, &hyperion_hk->Port_Voltage);

    // Port Dep Voltage
    hyperion_config_1_value(CONFIG_1_PANEL_PD, CONFIG_1_CHANNEL_VOLT, &hyperion_hk->Port_Dep_Voltage);

    // Star Voltage
    hyperion_config_1_value(CONFIG_1_PANEL_S, CONFIG_1_CHANNEL_VOLT, &hyperion_hk->Star_Voltage);

    // Star Dep Voltage
    hyperion_config_1_value(CONFIG_1_PANEL_S, CONFIG_1_CHANNEL_VOLT, &hyperion_hk->Star_Dep_Voltage);

    // Zenith Voltage
    hyperion_config_1_value(CONFIG_1_PANEL_Z, CONFIG_1_CHANNEL_VOLT, &hyperion_hk->Zenith_Voltage);

    // Port Current
    hyperion_config_1_value(CONFIG_1_PANEL_P, CONFIG_1_CHANNEL_CURR, &hyperion_hk->Port_Current);

    // Port Dep Current
    hyperion_config_1_value(CONFIG_1_PANEL_PD, CONFIG_1_CHANNEL_CURR, &hyperion_hk->Port_Dep_Current);

    // Star Current
    hyperion_config_1_value(CONFIG_1_PANEL_S, CONFIG_1_CHANNEL_CURR, &hyperion_hk->Star_Current);

    // Star Dep Current
    hyperion_config_1_value(CONFIG_1_PANEL_S, CONFIG_1_CHANNEL_CURR, &hyperion_hk->Star_Dep_Current);

    // Zenith Current
    hyperion_config_1_value(CONFIG_1_PANEL_Z, CONFIG_1_CHANNEL_CURR, &hyperion_hk->Zenith_Current);
}

void Hyperion_config3_getHK(Hyperion_HouseKeeping *hyperion_hk) {
    // NADIR TEMP 1
    hyperion_config_2_value(CONFIG_2_PANEL_NADIR, CONFIG_2_CHANNEL_PD_1, &hyperion_hk->Nadir_Pd1);

    // NADIR Temp Adc
    hyperion_hk->Nadir_Temp_Adc = adc_get_tsense_temp(PANEL_SLAVE_ADDR_NADIR2U, ADC_VREF);

    // Port 2U TEMP 1 2
    hyperion_config_3_value(CONFIG_3_PANEL_P2U, CONFIG_3_CHANNEL_TEMP_1, &hyperion_hk->Port_Temp1);
    hyperion_config_3_value(CONFIG_3_PANEL_P2U, CONFIG_3_CHANNEL_TEMP_2, &hyperion_hk->Port_Temp2);
    hyperion_hk->Port_Temp3 = HYPERION_2U_TEMP_PLACEHOLDER;

    // Port 2U Temp Adc
    hyperion_config_3_value(CONFIG_3_PANEL_P2U, CONFIG_3_ADC_TEMP, &hyperion_hk->Port_Temp_Adc);

    // Port Dep Temp 1 2
    hyperion_config_3_value(CONFIG_3_PANEL_PD2U, CONFIG_3_CHANNEL_TEMP_1, &hyperion_hk->Port_Dep_Temp1);
    hyperion_config_3_value(CONFIG_3_PANEL_PD2U, CONFIG_3_CHANNEL_TEMP_2, &hyperion_hk->Port_Dep_Temp2);
    hyperion_hk->Port_Dep_Temp3 = HYPERION_2U_TEMP_PLACEHOLDER;

    // Port Dep Temp Adc
    hyperion_config_3_value(CONFIG_3_PANEL_PD2U, CONFIG_3_ADC_TEMP, &hyperion_hk->Port_Dep_Temp_Adc);

    // Star Temp 1 2
    hyperion_config_3_value(CONFIG_3_PANEL_S2U, CONFIG_3_CHANNEL_TEMP_1, &hyperion_hk->Star_Temp1);
    hyperion_config_3_value(CONFIG_3_PANEL_S2U, CONFIG_3_CHANNEL_TEMP_2, &hyperion_hk->Star_Temp2);
    hyperion_hk->Star_Temp3 = HYPERION_2U_TEMP_PLACEHOLDER;

    // Star Temp Adc
    hyperion_config_3_value(CONFIG_3_PANEL_S2U, CONFIG_3_ADC_TEMP, &hyperion_hk->Star_Temp_Adc);

    // Star Dep Temp 1 2
    hyperion_config_3_value(CONFIG_3_PANEL_SD2U, CONFIG_3_CHANNEL_TEMP_1, &hyperion_hk->Star_Dep_Temp1);
    hyperion_config_3_value(CONFIG_3_PANEL_SD2U, CONFIG_3_CHANNEL_TEMP_2, &hyperion_hk->Star_Dep_Temp2);
    hyperion_hk->Star_Dep_Temp3 = HYPERION_2U_TEMP_PLACEHOLDER;

    // Star Dep Adc
    hyperion_config_3_value(CONFIG_3_PANEL_SD2U, CONFIG_3_ADC_TEMP, &hyperion_hk->Star_Dep_Temp_Adc);

    // Zenith Temp 1 2
    hyperion_config_3_value(CONFIG_3_PANEL_Z2U, CONFIG_3_CHANNEL_TEMP_1, &hyperion_hk->Zenith_Temp1);
    hyperion_config_3_value(CONFIG_3_PANEL_Z2U, CONFIG_3_CHANNEL_TEMP_2, &hyperion_hk->Zenith_Temp2);
    hyperion_hk->Zenith_Temp3 = HYPERION_2U_TEMP_PLACEHOLDER;

    // Zenith Temp Adc
    hyperion_config_3_value(CONFIG_3_PANEL_Z2U, CONFIG_3_ADC_TEMP, &hyperion_hk->Zenith_Temp_Adc);

    // Nadir Pd 1
    hyperion_config_2_value(CONFIG_2_PANEL_NADIR, CONFIG_2_CHANNEL_PD_1, &hyperion_hk->Nadir_Pd1);

    // Port 2U Pd 1 2
    hyperion_config_3_value(CONFIG_3_PANEL_P2U, CONFIG_3_CHANNEL_PD_1, &hyperion_hk->Port_Pd1);
    hyperion_config_3_value(CONFIG_3_PANEL_P2U, CONFIG_3_CHANNEL_PD_2, &hyperion_hk->Port_Pd2);
    hyperion_hk->Port_Pd3 = HYPERION_2U_PD_PLACEHOLDER;

    // Port 2U Dep Pd 1 2 3
    hyperion_config_3_value(CONFIG_3_PANEL_PD2U, CONFIG_3_CHANNEL_PD_1, &hyperion_hk->Port_Dep_Pd1);
    hyperion_config_3_value(CONFIG_3_PANEL_PD2U, CONFIG_3_CHANNEL_PD_2, &hyperion_hk->Port_Dep_Pd2);
    hyperion_hk->Port_Dep_Pd3 = HYPERION_2U_PD_PLACEHOLDER;

    // Star 2U Pd 1 2
    hyperion_config_3_value(CONFIG_3_PANEL_S2U, CONFIG_3_CHANNEL_PD_1, &hyperion_hk->Star_Pd1);
    hyperion_config_3_value(CONFIG_3_PANEL_S2U, CONFIG_3_CHANNEL_PD_2, &hyperion_hk->Star_Pd2);
    hyperion_hk->Star_Pd3 = HYPERION_2U_PD_PLACEHOLDER;

    // Star Dep 2U Pd 1 2
    hyperion_config_3_value(CONFIG_3_PANEL_SD2U, CONFIG_3_CHANNEL_PD_1, &hyperion_hk->Star_Dep_Pd1);
    hyperion_config_3_value(CONFIG_3_PANEL_SD2U, CONFIG_3_CHANNEL_PD_2, &hyperion_hk->Star_Dep_Pd2);
    hyperion_hk->Star_Dep_Pd3 = HYPERION_2U_PD_PLACEHOLDER;

    // Zenith 2U Pd 1 2
    hyperion_config_3_value(CONFIG_3_PANEL_Z2U, CONFIG_3_CHANNEL_PD_1, &hyperion_hk->Zenith_Pd1);
    hyperion_config_3_value(CONFIG_3_PANEL_Z2U, CONFIG_3_CHANNEL_PD_2, &hyperion_hk->Zenith_Pd2);
    hyperion_hk->Zenith_Pd3 = HYPERION_2U_PD_PLACEHOLDER;

    // Port 2U Voltage
    hyperion_config_3_value(CONFIG_3_PANEL_P2U, CONFIG_3_CHANNEL_VOLT, &hyperion_hk->Port_Voltage);

    // Port Dep 2U Voltage
    hyperion_config_3_value(CONFIG_3_PANEL_PD2U, CONFIG_3_CHANNEL_VOLT, &hyperion_hk->Port_Dep_Voltage);

    // Star 2U Voltage
    hyperion_config_3_value(CONFIG_3_PANEL_S2U, CONFIG_3_CHANNEL_VOLT, &hyperion_hk->Star_Voltage);

    // Star Dep 2U Voltage
    hyperion_config_3_value(CONFIG_3_PANEL_S2U, CONFIG_3_CHANNEL_VOLT, &hyperion_hk->Star_Dep_Voltage);

    // Zenith 2U Voltage
    hyperion_config_3_value(CONFIG_3_PANEL_Z2U, CONFIG_3_CHANNEL_VOLT, &hyperion_hk->Zenith_Voltage);

    // Port 2U Current
    hyperion_config_3_value(CONFIG_3_PANEL_P2U, CONFIG_3_CHANNEL_CURR, &hyperion_hk->Port_Current);

    // Port Dep 2U Current
    hyperion_config_3_value(CONFIG_3_PANEL_PD2U, CONFIG_3_CHANNEL_CURR, &hyperion_hk->Port_Dep_Current);

    // Star 2U Current
    hyperion_config_3_value(CONFIG_3_PANEL_S2U, CONFIG_3_CHANNEL_CURR, &hyperion_hk->Star_Current);

    // Star Dep 2U Current
    hyperion_config_3_value(CONFIG_3_PANEL_S2U, CONFIG_3_CHANNEL_CURR, &hyperion_hk->Star_Dep_Current);

    // Zenith 2U Current
    hyperion_config_3_value(CONFIG_3_PANEL_Z2U, CONFIG_3_CHANNEL_CURR, &hyperion_hk->Zenith_Current);
}
>>>>>>> 974f3c7 (Clean code, add error handling code, add gethk for config3)
