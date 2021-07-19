/*
 * privileged_functions.c
 *
 *  Created on: Jun. 14, 2021
 *      Author: Robert Taylor
 */

#include "application_defined_privileged_functions.h"

void reboot_system(char reboot_type) {
    reboot_system_(reboot_type);
}
bool init_eeprom() {
    return init_eeprom_();
}

void shutdown_eeprom() {
    shutdown_eeprom_();
}

image_info priv_eeprom_get_app_info() {
    return priv_eeprom_get_app_info_();
}
image_info priv_eeprom_get_golden_info() {
    return priv_eeprom_get_golden_info_();
}
void priv_eeprom_set_app_info(image_info app_info) {
    priv_eeprom_set_app_info_(app_info);
}

void priv_eeprom_set_golden_info(image_info app_info) {
    priv_eeprom_set_golden_info_(app_info);
}

bool priv_verify_application() {
    return priv_verify_application_();
}

bool priv_verify_golden() {
    return priv_verify_golden_();
}

uint32_t priv_Fapi_BlockErase(uint32_t ulAddr, uint32_t Size) {
    return priv_Fapi_BlockErase_(ulAddr, Size);
}

uint32_t priv_Fapi_BlockProgram( uint32_t Bank, uint32_t Flash_Address, uint32_t Data_Address, uint32_t SizeInBytes){
    return priv_Fapi_BlockProgram_(Bank, Flash_Address, Data_Address, SizeInBytes);
}
