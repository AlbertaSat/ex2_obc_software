/*
 * privileged_functions.c
 *
 *  Created on: Jun. 14, 2021
 *      Author: Robert Taylor
 */

#include "application_defined_privileged_functions.h"

inline void reboot_system(char reboot_type) {
    reboot_system_(reboot_type);
}
inline bool init_eeprom() {
    return init_eeprom_();
}

inline void shutdown_eeprom() {
    shutdown_eeprom_();
}

inline image_info priv_eeprom_get_app_info() {
    return priv_eeprom_get_app_info_();
}
inline image_info priv_eeprom_get_golden_info() {
    return priv_eeprom_get_golden_info_();
}
inline void priv_eeprom_set_app_info(image_info app_info) {
    priv_eeprom_set_app_info_(app_info);
}

inline void priv_eeprom_set_golden_info(image_info app_info) {
    priv_eeprom_set_golden_info_(app_info);
}
