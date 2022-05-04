/*
 * bl_eeprom.h
 *
 *  Created on: May 17, 2021
 *      Author: Robert Taylor
 */

#ifndef BL_EEPROM_H_
#define BL_EEPROM_H_

#include <stdbool.h>
#include <stdint.h>
#include "HL_system.h"
#include <F021.h>
#include "privileged_functions.h"

#define EXISTS_FLAG 0x5A5A5A5A

#define APP_STATUS_BLOCKNUMBER 1 // status byte reports 1 if program present
#define APP_STATUS_LEN sizeof(image_info)

#define GOLD_STATUS_BLOCKNUMBER 2
#define GOLD_STATUS_LEN sizeof(image_info)

#define GOLD_MINIMUM_ADDR 0x00020000
#define GOLD_DEFAULT_ADDR 0x00020000
#define GOLD_START_BANK 0

#define APP_MINIMUM_ADDR 0x00200000
#define APP_DEFAULT_ADDR 0x00200000
#define APP_START_BANK 1

#define BOOTLOADER_MAX_ADDR 0x0017FFF

#define BOOT_INFO_BLOCKNUMBER 0
#define BOOT_INFO_LEN sizeof(boot_info)

#define UPDATE_INFO_BLOCKNUMBER 3
#define UPDATE_INFO_LEN sizeof(update_info)

// Representation of data which will be stored in FEE flash
typedef struct __attribute__((packed)) {
    uint32_t exists; // EXISTS_FLAG for exists, else does not exist
    uint32_t size;
    uint32_t addr;
    uint16_t crc;
} image_info;

typedef enum {
    NONE,
    UNDEF,
    DABORT,
    PREFETCH,
    REQUESTED
} SW_RESET_REASON;

typedef enum {
    BOOTLOADER = 'B',
    GOLDEN = 'G',
    APPLICATION = 'A'
} SYSTEM_TYPE;

typedef struct __attribute__((packed)) {
    resetSource_t rstsrc;
    SW_RESET_REASON swr_reason;
} boot_reason;

typedef struct __attribute__((packed)) {
    uint32_t start_address;
    uint32_t size;
    uint32_t next_address;
    uint32_t initialized;
    uint16_t crc;
} update_info;

typedef struct __attribute__((packed)) {
    char type;
    uint32_t count; // total number of boot attempts
    uint32_t attempts; // total attempts since last failure
    boot_reason reason;
} boot_info;

void sw_reset(char reboot_type, SW_RESET_REASON reason);

bool eeprom_init();

void eeprom_shutdown();

Fapi_StatusType eeprom_set_app_info(image_info *i);

Fapi_StatusType eeprom_get_app_info(image_info *i);

Fapi_StatusType eeprom_set_golden_info(image_info *i);

Fapi_StatusType eeprom_get_golden_info(image_info *i);

Fapi_StatusType eeprom_get_boot_info(boot_info *b);

Fapi_StatusType eeprom_set_boot_info(boot_info *b);

Fapi_StatusType eeprom_set_update_info(update_info *u);

Fapi_StatusType eeprom_get_update_info(update_info *u);

bool verify_application();

bool verify_golden();

#endif /* BL_EEPROM_H_ */
