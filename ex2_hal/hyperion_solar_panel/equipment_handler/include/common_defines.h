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
 * @file    common_defines.h
 * @author  Vasu Gupta, Trung Tran
 * @date    2020-06-17
 */

#ifndef COMMON_DEFINES_H
#define COMMON_DEFINES_H

#define ADC_i2c_PORT            i2cREG1

#define HIGH                    1
#define LOW                     0
 
#define ADC_VREF                2.5f       //V


// Temperature Sensor (LMT70) constants
#define TEMP_VOLT_MAX           1.375f      //V
#define TEMP_VOLT_MIN           0.302f      //V
#define TEMP_VAL_MAX            150         //celsius
#define TEMP_VAL_MIN            -55         //celsius

// Voltage/Current Sensor Constants
#define ADC_VOLT_MAX            2.5f        //V
#define ADC_VOLT_MIN            0           //V
#define VOLT_MAX                16.32f       //V
#define VOLT_MIN                0           //V
#define CURR_MAX                0.6f      //A
#define CURR_MIN                0           //A

// Slave Address for each panel of the hyperion board
#define PANEL_SLAVE_ADDR_PORT                       0x20
#define PANEL_SLAVE_ADDR_PORT_DEPLOYABLE            0x22
#define PANEL_SLAVE_ADDR_STARBOARD                  0x23
#define PANEL_SLAVE_ADDR_STARBOARD_DEPLOYABLE       0x28
#define PANEL_SLAVE_ADDR_ZENITH                     0x2E
#define PANEL_SLAVE_ADDR_PORT2U                     0x20
#define PANEL_SLAVE_ADDR_PORT_DEPLOYABLE2U          0x22
#define PANEL_SLAVE_ADDR_STARBOARD2U                0x23
#define PANEL_SLAVE_ADDR_STARBOARD_DEPLOYABLE2U     0x28
#define PANEL_SLAVE_ADDR_ZENITH2U                   0x2E
#define PANEL_SLAVE_ADDR_ARKE                       0x2C
#define PANEL_SLAVE_ADDR_NADIR                      0x2F
#define PANEL_SLAVE_ADDR_NADIR2U                    0x2F


// VIN channels on ADC for sensors in configuration 1: all 3U panels - see datasheet
typedef enum config_1_channel_type_t{
    CONFIG_1_CHANNEL_TEMP_1 = 0,
    CONFIG_1_CHANNEL_TEMP_2,
    CONFIG_1_CHANNEL_TEMP_3,
    CONFIG_1_CHANNEL_PD_1,
    CONFIG_1_CHANNEL_PD_2,
    CONFIG_1_CHANNEL_PD_3,
    CONFIG_1_CHANNEL_VOLT,
    CONFIG_1_CHANNEL_CURR,
    CONFIG_1_CHANNEL_NUM,         // Number of ADC channels
    CONFIG_1_ADC_TEMP
}config_1_channel_type_t;

typedef enum config_1_panel_t{
    CONFIG_1_PANEL_P = 0,        
    CONFIG_1_PANEL_PD,
    CONFIG_1_PANEL_S,
    CONFIG_1_PANEL_SD,
    CONFIG_1_PANEL_Z,
    CONFIG_1_PANEL_NUM           // Number of solar panels
}config_1_panel_t;



// VIN channels on ADC for sensors in configuration 2: ARKE, NADIR, NADIR 2U
typedef enum config_2_channel_type_t{
    CONFIG_2_CHANNEL_TEMP_1 = 0,
    CONFIG_2_CHANNEL_PD_1 = 1,
    CONFIG_2_ADC_TEMP
}config_2_channel_type_t;

typedef enum config_2_panel_t{
    CONFIG_2_PANEL_ARKE = 0,        
    CONFIG_2_PANEL_NADIR,
    CONFIG_2_PANEL_NADIR2U
}config_2_panel_t;

// VIN channels on ADC for sensors in configuration 3: 2U PANELS
typedef enum config_3_channel_type_t{
    CONFIG_3_CHANNEL_TEMP_1 = 0,
    CONFIG_3_CHANNEL_TEMP_2,
    CONFIG_3_CHANNEL_PD_1,
    CONFIG_3_CHANNEL_PD_2,
    CONFIG_3_CHANNEL_VOLT,
    CONFIG_3_CHANNEL_CURR,
    CONFIG_3_ADC_TEMP
}config_3_channel_type_t;

typedef enum config_3_panel_t{
    CONFIG_3_PANEL_P2U,
    CONFIG_3_PANEL_PD2U,
    CONFIG_3_PANEL_S2U,
    CONFIG_3_PANEL_SD2U,
    CONFIG_3_PANEL_Z2U
}config_3_panel_t;


#endif
