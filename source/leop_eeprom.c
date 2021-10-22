/*
 * leop_eeprom.c
 *
 *  Created on: Oct, 2021
 *      Author: Grace Yi
 */

#include "leop_eeprom.h"


bool eeprom_get_leop_status() {
    int out;
    if ((TI_Fee_ReadSync(LEOP_STATUS_BLOCKNUMBER, LEOP_STATUS_OFFSET, (uint8_t *)(&out), BOOT_TYPE_LEN)) == 1) {
        return true;
    }
    else {
        return false;
    }
}

void eeprom_set_leop_status() {
    bool leop_status = true;
    TI_Fee_WriteSync(LEOP_STATUS_BLOCKNUMBER, (uint8_t *)&leop_status);
}
