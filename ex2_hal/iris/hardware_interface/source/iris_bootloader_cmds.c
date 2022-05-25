/*
 * iris_bootloader_cmds.c
 *
 *  Created on: May 25, 2022
 *      Author: jenish
 */

#include "iris_bootloader_cmds.h"
#include "common.h"
#include "iris_i2c.h"
#include "FreeRTOS.h"
#include <stdlib.h>
#include <stdio.h>

void i2c_send_test() {

    iris_i2c_init();

    uint32_t flash_addr = 0x80001000;
    uint16_t data_length = 0x0200;
    i2c_write_memory(flash_addr, data_length);
    i2c_erase_memory();

    vTaskDelay(100);

}

int i2c_write_memory(uint32_t flash_addr, uint16_t num_bytes) {
    uint16_t data_length = WRITE_METADATA_LENGTH + num_bytes;
    uint8_t *packet = (uint8_t*)calloc(data_length, sizeof(uint8_t));
    int ret;

    *(packet + 0) = OPC_WREN;
    *(packet + 1) = (flash_addr >> (8*3)) & 0xff;
    *(packet + 2) = (flash_addr >> (8*2)) & 0xff;
    *(packet + 3) = (flash_addr >> (8*1)) & 0xff;
    *(packet + 4) = (flash_addr >> (8*0)) & 0xff;
    *(packet + 5) = (num_bytes >> (8*1)) & 0xff;
    *(packet + 6) = (num_bytes >> (8*0)) & 0xff;
    *(packet + 7) = DUMMY_BYTE;

    // TESTING PURPOSE
    int i;
    for (i = WRITE_METADATA_LENGTH; i < data_length; i++) {
        packet[i] = i;
    }

    ret = write_packet(packet, data_length);
    free(packet);

    return ret;
}

int i2c_erase_memory() {
    uint8_t packet[ERASE_METADATA_LENGTH] = {OPC_ERUSM};
    uint16_t data_length = 1;
    int ret;

    ret = write_packet(packet, data_length);

    return ret;
}

