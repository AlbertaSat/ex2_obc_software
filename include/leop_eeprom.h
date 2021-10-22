/*
 * leop_eeprom.h
 *
 *  Created on: Oct, 2021
 *      Author: Grace Yi
 */

#ifndef LEOP_EEPROM_H_
#define LEOP_EEPROM_H_

#include <stdbool.h>
#include <stdint.h>
#include "HL_system.h"

#define EXISTS_FLAG 0xA5A5A5A5

#define LEOP_STATUS_BLOCKNUMBER 6 // status byte reports 1 if program present
#define LEOP_STATUS_OFFSET 0
#define BOOT_TYPE_LEN 1

bool eeprom_get_leop_status();
void eeprom_set_leop_status();
//bool leop_eeprom(uint leop_status);


#endif /* LEOP_EEPROM_H_ */
