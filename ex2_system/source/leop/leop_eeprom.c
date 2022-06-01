/*
 * leop_eeprom.c
 *
 *  Created on: Oct, 2021
 *      Author: Grace Yi
 */
#include "eeprom.h"
#include "leop_eeprom.h"
#include "F021.h"

bool eeprom_get_leop_status() {
    bool out;
    if (eeprom_read(&out, LEOP_INFO_BLOCKNUMBER, sizeof(out)) == Fapi_Status_Success) {
        return true;
    }
    else {
        return false;
    }
}

bool eeprom_set_leop_status() {
    bool leop_status = true;
    eeprom_write(&leop_status, LEOP_INFO_BLOCKNUMBER, sizeof(leop_status));
    return true;
}
