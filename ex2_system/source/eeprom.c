/*
 * eeprom.c
 *
 *  Created on: Jun. 1, 2022
 *      Author: Robert Taylor
 */

#include "FreeRTOSConfig.h"
#include "privileged_functions.h"
#include "flash_defines.h"
#include "F021.h"
#include "bl_flash.h"

static const SECTORS *eeprom_get_sector_by_block(uint8_t block) {
    const SECTORS *sector = 0;
    for (int i = 0; i < NUMBEROFSECTORS; i++) {
        if (flash_sector[i].bankNumber != 7) {
            continue;
        }
        if (flash_sector[i].sectorNumber == block) {
            sector = &flash_sector[i];
            break;
        }
    }
    return sector;
}

Fapi_StatusType eeprom_write(void *dat, uint8_t block, uint32_t size) {
    // find address of block
    const SECTORS *sector = eeprom_get_sector_by_block(block);
    if (sector == 0) {
        return Fapi_Error_InvalidAddress;
    }

    void *addr = sector->start;
    uint32_t sector_size = sector->length;

    if (size > sector_size) {
        return Fapi_Error_AsyncIncorrectDataBufferLength;
    }
    raise_privilege();
    uint32_t status = Fapi_BlockErase((uint32_t)addr, size);
    status = Fapi_BlockProgram(7, (uint32_t)addr, (uint32_t)dat, size);
    reset_privilege();
    return (Fapi_StatusType)status;
}

Fapi_StatusType eeprom_read(void *dat, uint8_t block, uint32_t size) {
    const SECTORS *sector = eeprom_get_sector_by_block(block);
    if (sector == 0) {
        return Fapi_Error_InvalidAddress;
    }
    void *addr = sector->start;
    uint32_t sector_size = sector->length;

    if (sector == 0) {
        return Fapi_Error_InvalidAddress;
    }

    if (size > sector_size) {
        return Fapi_Error_AsyncIncorrectDataBufferLength;
    }
    memcpy(dat, addr, size);
    return Fapi_Status_Success;
}
