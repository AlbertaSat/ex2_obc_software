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
 * @file    hyperion.h
 * @author  Trung Tran, Nicholas Sorensen
 * @date    2021-06-04
 */

#ifndef EX2_HAL_EX2_HYPERION_SOLAR_PANEL_SOFTWARE_HARDWARE_INTERFACE_INCLUDE_HYPERION_H_
#define EX2_HAL_EX2_HYPERION_SOLAR_PANEL_SOFTWARE_HARDWARE_INTERFACE_INCLUDE_HYPERION_H_

#include "adc_handler.h"

#define HYPERION_2U_TEMP_PLACEHOLDER -128
#define HYPERION_2U_PD_PLACEHOLDER 255

typedef struct __attribute__((packed)) {
    int8_t Nadir_Temp1;
    int8_t Nadir_Temp_Adc;
    int8_t Port_Temp1;
    int8_t Port_Temp2;
    int8_t Port_Temp3;
    int8_t Port_Temp_Adc;
    int8_t Port_Dep_Temp1;
    int8_t Port_Dep_Temp2;
    int8_t Port_Dep_Temp3;
    int8_t Port_Dep_Temp_Adc;
    int8_t Star_Temp1;
    int8_t Star_Temp2;
    int8_t Star_Temp3;
    int8_t Star_Temp_Adc;
    int8_t Star_Dep_Temp1;
    int8_t Star_Dep_Temp2;
    int8_t Star_Dep_Temp3;
    int8_t Star_Dep_Temp_Adc;
    int8_t Zenith_Temp1;
    int8_t Zenith_Temp2;
    int8_t Zenith_Temp3;
    int8_t Zenith_Temp_Adc;
    uint8_t Nadir_Pd1;
    uint8_t Port_Pd1;
    uint8_t Port_Pd2;
    uint8_t Port_Pd3;
    uint8_t Port_Dep_Pd1;
    uint8_t Port_Dep_Pd2;
    uint8_t Port_Dep_Pd3;
    uint8_t Star_Pd1;
    uint8_t Star_Pd2;
    uint8_t Star_Pd3;
    uint8_t Star_Dep_Pd1;
    uint8_t Star_Dep_Pd2;
    uint8_t Star_Dep_Pd3;
    uint8_t Zenith_Pd1;
    uint8_t Zenith_Pd2;
    uint8_t Zenith_Pd3;
    uint16_t Port_Voltage;
    uint16_t Port_Dep_Voltage;
    uint16_t Star_Voltage;
    uint16_t Star_Dep_Voltage;
    uint16_t Zenith_Voltage;
    uint16_t Port_Current;
    uint16_t Port_Dep_Current;
    uint16_t Star_Current;
    uint16_t Star_Dep_Current;
    uint16_t Zenith_Current;
} Hyperion_HouseKeeping;

void hyperion_config_1_value(enum config_1_panel_t panel, enum config_1_channel_type_t channel, void *param);
void hyperion_config_2_value(enum config_2_panel_t panel, enum config_2_channel_type_t channel, void *param);
void hyperion_config_3_value(enum config_3_panel_t panel, enum config_3_channel_type_t channel, void *param);

void Hyperion_config1_getHK(Hyperion_HouseKeeping *hyperion_hk);
void Hyperion_config3_getHK(Hyperion_HouseKeeping *hyperion_hk);

#endif /* EX2_HAL_EX2_HYPERION_SOLAR_PANEL_SOFTWARE_HARDWARE_INTERFACE_INCLUDE_HYPERION_H_ */
