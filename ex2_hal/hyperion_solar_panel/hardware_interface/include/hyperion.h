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
 * @date    2020-06-04
 */

#ifndef EX2_HAL_EX2_HYPERION_SOLAR_PANEL_SOFTWARE_HARDWARE_INTERFACE_INCLUDE_HYPERION_H_
#define EX2_HAL_EX2_HYPERION_SOLAR_PANEL_SOFTWARE_HARDWARE_INTERFACE_INCLUDE_HYPERION_H_

#include <ex2_hal/ex2_hyperion_solar_panel_software/equipment_handler/include/adc_handler.h>

void hyperion_config_1_value(enum config_1_panel_t panel, enum config_1_channel_type_t channel, float* temp, float* pd, float* voltage, float* current);
void hyperion_config_2_value(config_2_panel_t panel, config_2_channel_type_t channel, float* temp, float* pd);
void hyperion_config_3_value(config_3_panel_t panel, config_3_channel_type_t channel, float* temp, float* pd, float* voltage, float* current);
#endif /* EX2_HAL_EX2_HYPERION_SOLAR_PANEL_SOFTWARE_HARDWARE_INTERFACE_INCLUDE_HYPERION_H_ */
