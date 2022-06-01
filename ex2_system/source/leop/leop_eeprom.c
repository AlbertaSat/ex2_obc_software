/*
 * leop_eeprom.c
 *
 *  Created on: Oct, 2021
 *      Author: Grace Yi
 */
#include "eeprom.h"
#include "leop_eeprom.h"
#include "F021.h"

typedef struct __attribute__((packed)){
    uint32_t exists_flag;
    bool status;
} leop_status_t;

bool eeprom_get_leop_status() {
    leop_status_t state = {0};
    if (eeprom_read(&state, LEOP_INFO_BLOCKNUMBER, sizeof(state)) == Fapi_Status_Success) {
        if (state.exists_flag == EXISTS_FLAG) {
            return state.status;
        } else {
            memset(&state, 0, sizeof(state));
            state.exists_flag = EXISTS_FLAG;
            eeprom_write(&state, LEOP_INFO_BLOCKNUMBER, sizeof(state));
            return state.status;
        }
    }
    else {
        return false;
    }
}

bool eeprom_set_leop_status() {
    leop_status_t state = {0};
    state.exists_flag = EXISTS_FLAG;
    state.status = true;
    eeprom_write(&state, LEOP_INFO_BLOCKNUMBER, sizeof(state));
    return true;
}
