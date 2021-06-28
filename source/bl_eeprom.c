/*
 * bl_eeprom.c
 *
 *  Created on: May 17, 2021
 *      Author: Robert Taylor
 */

#include "bl_eeprom.h"
#include "ti_fee.h"

// Returns false on failure
bool eeprom_init() {
    int delayCount = 0;
    TI_Fee_Init();
    while (TI_Fee_GetStatus(0) != IDLE) {
        delayCount++;
        if (delayCount > 10000) { // timeout after trying this many times
            return false;
        }
    }
    return true;
}

void eeprom_shutdown() {
    TI_Fee_Shutdown();
}

char eeprom_get_boot_type() {
    char bootType;
    TI_Fee_ReadSync(BOOT_TYPE_BLOCK, BOOT_TYPE_OFFSET, (uint8_t *)(&bootType), BOOT_TYPE_LEN);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
    return bootType;
}

void eeprom_set_boot_type(char boot) {
    TI_Fee_WriteSync(BOOT_TYPE_BLOCK, (uint8_t *)&boot);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
}

void eeprom_set_app_info(image_info i) {
    TI_Fee_WriteSync(APP_STATUS_BLOCKNUMBER, (uint8_t *)(&i));
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
}

image_info eeprom_get_app_info() {
    image_info out;
    TI_Fee_ReadSync(APP_STATUS_BLOCKNUMBER, APP_STATUS_OFFSET, (uint8_t *)(&out), APP_STATUS_LEN);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
    return out;
}

void eeprom_set_golden_info(image_info i) {
    TI_Fee_WriteSync(GOLD_STATUS_BLOCKNUMBER, (uint8_t *)&i);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
}

image_info eeprom_get_golden_info() {
    image_info out = {0};
    TI_Fee_ReadSync(GOLD_STATUS_BLOCKNUMBER, GOLD_STATUS_OFFSET, (uint8_t *)(&out), GOLD_STATUS_LEN);
    TI_FeeJobResultType res = TI_Fee_GetJobResult(0);
    return out;
}


