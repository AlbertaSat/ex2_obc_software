/*
 * Copyright (C) 2021  University of Alberta
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

/*
 * privileged_functions.h
 *
 *  Created on: Jun. 14, 2021
 *      Author: Robert Taylor
 */

#ifndef PRIVILEGED_FUNCTIONS_H_
#define PRIVILEGED_FUNCTIONS_H_

/*
 * This file works as a wrapper to allow including privileged functions in multiple files
 * Since privileged functions must be implemented in the header per FreeRTOS rules it would
 * fail to compile if they were included in more than one place.
 */
#include "bl_eeprom.h"
#include <stdbool.h>

inline void reboot_system(char reboot_type);
inline bool init_eeprom();
inline void shutdown_eeprom();
inline image_info priv_eeprom_get_app_info();
inline image_info priv_eeprom_get_golden_info();
inline void priv_eeprom_set_app_info(image_info app_info);
inline void priv_eeprom_set_golden_info(image_info app_info);
inline uint32_t priv_Fapi_BlockErase(uint32_t ulAddr, uint32_t Size);
inline uint32_t priv_Fapi_BlockProgram( uint32_t Bank, uint32_t Flash_Address, uint32_t Data_Address, uint32_t SizeInBytes);
#endif /* INCLUDE_PRIVILEGED_FUNCTIONS_H_ */
