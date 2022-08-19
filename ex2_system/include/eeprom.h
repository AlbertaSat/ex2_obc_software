/*
 * eeprom.h
 *
 *  Created on: Jun. 1, 2022
 *      Author: Robert Taylor
 */

#ifndef EX2_SYSTEM_INCLUDE_EEPROM_H_
#define EX2_SYSTEM_INCLUDE_EEPROM_H_

#include "FreeRTOSConfig.h"
#include "privileged_functions.h"
#include "flash_defines.h"
#include "F021.h"

#define EXISTS_FLAG 0x5A5A5A5A

#define APP_STATUS_BLOCKNUMBER 1 // status byte reports 1 if program present

#define GOLD_STATUS_BLOCKNUMBER 2

#define BOOT_INFO_BLOCKNUMBER 0

#define UPDATE_INFO_BLOCKNUMBER 3

#define LEOP_INFO_BLOCKNUMBER 4

#define KEY_STORE_BLOCKNUMBER 5

Fapi_StatusType eeprom_write(void *dat, uint8_t block, uint32_t size);
Fapi_StatusType eeprom_read(void *dat, uint8_t block, uint32_t size);

#endif /* EX2_SYSTEM_INCLUDE_EEPROM_H_ */
