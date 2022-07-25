/*
 * Copyright (C) 2015  University of Alberta
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
/*
 * iris_bootloader_cmds.c
 *
 *  Created on: May 25, 2022
 *      Author: jenish
 */

#include "iris_bootloader_cmds.h"
#include "iris.h"
#include "i2c_io.h"
#include "HL_gio.h"
#include "HL_reg_het.h"
#include "iris_i2c.h"
#include "iris_gio.h"
#include "FreeRTOS.h"
#include "redposix.h"
#include <string.h>

/* Optimization points
 * - As of now, the mass erase functionality is not verified and
 *   still WIP. We would like to have this functionality working in
 *   the future. But, we can manage with erasing individual pages for
 *   now
 * - When combining both i2c and spi drivers for Iris, it will nice to
 *   have all of the GIO outputs in a different file (e.g. iris_gio.c)
 * - Also instead of using ints for good or bad return we should integrate
 *   the Iris status flags. This is implemented in the spi feature. Once both
 *   features are merged, we can update this file
 */

/**
 * @brief
 *   Initialize i2c driver for Iris programming
 **/
void iris_i2c_init() {
    /* i2c initialization */
    init_i2c_driver();
}

/**
 * @brief
 *  GIO sequence to put Iris in boot mode
 **/
void iris_pre_sequence() {
    /* Start initialization sequence before I2C transaction */
    iris_reset_low();
    IRIS_PROGAMMING_DELAY;
    iris_boot_high();
    IRIS_PROGAMMING_DELAY;
    iris_reset_high();
    IRIS_PROGAMMING_DELAY;
}

/**
 * @brief
 *  GIO sequence to put Iris out of boot mode
 **/
void iris_post_sequence() {
    /* End I2C transaction by doing end sequence*/
    iris_boot_low();
    IRIS_PROGAMMING_DELAY;
    iris_reset_low();
    IRIS_PROGAMMING_DELAY;
    iris_reset_high();
}

/**
 * @brief
 *   Write page in flash memory on Iris
 *
 * @param[in] flash_addr
 *   Starting flash address from which writing is desired
 *
 * @param[in] buffer
 *   Pointer to write data
 *
 * @return
 *   Returns 0 data written, <0 if unable to write data.
 **/
int iris_write_page(uint32_t flash_addr, uint8_t *buffer) {
    /* number of bytes is equal to page size of flash memory on Iris */
    uint8_t num_bytes = FLASH_MEM_PAGE_SIZE;
    /*  Allocate bytes equal to page size of flash memory */
    static uint8_t packet[WRITE_PACKET_LENGTH];
    uint8_t flash_mem_checksum = 0x00;
    uint8_t data_checksum = 0x00;
    int ret = 0x00;
    uint8_t rx_data;

    /* First I2C transaction (2 bytes) */
    packet[0] = OPC_WRITE;
    packet[1] = N_OPC_WRITE;
    iris_write_packet(packet, 2);
    iris_read_packet(&rx_data, 1);
    memset(packet, 0, WRITE_PACKET_LENGTH * sizeof(uint8_t));

    /* Second I2C transaction (5 bytes) */
    packet[0] = (flash_addr >> (8 * 3)) & 0xff;
    packet[1] = (flash_addr >> (8 * 2)) & 0xff;
    packet[2] = (flash_addr >> (8 * 1)) & 0xff;
    packet[3] = (flash_addr >> (8 * 0)) & 0xff;
    flash_mem_checksum = packet[0] ^ packet[1] ^ packet[2] ^ packet[3];
    packet[4] = flash_mem_checksum;
    iris_write_packet(packet, 5);
    iris_read_packet(&rx_data, 1);
    memset(packet, 0, WRITE_PACKET_LENGTH * sizeof(uint8_t));

    /* Third I2C transaction (2 + num_bytes) */
    packet[0] = num_bytes - 1;
    data_checksum ^= packet[0];
    // transmit N+1 bytes of data
    for (int i = 0; i < FLASH_MEM_PAGE_SIZE; i++) {
        packet[i + 1] = buffer[i];
        data_checksum ^= *(packet + i + 1);
    }
    packet[num_bytes + 1] = data_checksum;
    iris_write_packet(packet, WRITE_PACKET_LENGTH);
    iris_read_packet(&rx_data, 1);
    memset(packet, 0, WRITE_PACKET_LENGTH * sizeof(uint8_t));

    return ret;
}

/**
 * @brief
 *   Erase page in flash memory on Iris
 *
 * @param[in] page_num
 *   Page index to be erased. Page number is from 0-511,
 *   starting from 0x0800 0000 - 0x0x0800 FFFF
 *
 * @param[in] buffer
 *   Pointer to write data
 *
 * @return
 *   Returns 0 data erased, <0 if unable to erase data.
 **/
int iris_erase_page(uint16_t page_num) {
    static uint8_t packet[ERASE_PACKET_LENGTH];
    uint8_t num_page_erased_checksum = 0x00;
    uint8_t page_num_checksum = 0x00;

    int ret = 0x00;
    uint8_t rx_data;

    /* First I2C transaction (2 bytes) */
    packet[0] = OPC_ERASE;
    packet[1] = N_OPC_ERASE;
    iris_write_packet(packet, 2);
    iris_read_packet(&rx_data, 1);
    memset(packet, 0, ERASE_PACKET_LENGTH);

    /* Second I2C transaction (3 bytes) */
    packet[0] = 0;
    packet[1] = NUM_PAGES_TO_ERASE - 1;
    num_page_erased_checksum = packet[0] ^ packet[1];
    packet[2] = num_page_erased_checksum;
    iris_write_packet(packet, 3);
    iris_read_packet(&rx_data, 1);
    memset(packet, 0, ERASE_PACKET_LENGTH);

    /* Third I2C transaction (2 + num_bytes) */
    packet[0] = (page_num >> (8 * 1)) & 0xff;
    packet[1] = (page_num >> (8 * 0)) & 0xff;
    page_num_checksum = packet[0] ^ packet[1];
    packet[2] = page_num_checksum;
    iris_write_packet(packet, 3);
    iris_read_packet(&rx_data, 1);
    memset(packet, 0, ERASE_PACKET_LENGTH);

    return ret;
}

/**
 * @brief
 *   Checks i2c bootloader version on Iris
 *
 * @return
 *   Returns 0 if version obtained, <0 if unable to get version.
 **/
int iris_check_bootloader_version(uint8_t *version) {
    static uint8_t packet[CHECK_VERSION_PACKET_LENGTH];

    int ret = 0x00;
    uint8_t rx_data;

    /* First I2C transaction (2 bytes) */
    packet[0] = OPC_CHECK_VERSION;
    packet[1] = N_OPC_CHECK_VERSION;
    iris_write_packet(packet, 2);
    iris_read_packet(&rx_data, 1);
    memset(packet, 0, 2);

    /* Read bootloader version */
    iris_read_packet(&version, 1);

    /* Wait for ACK/NACK */
    iris_read_packet(&rx_data, 1);

    return ret;
}

/**
 * @brief
 *   Jumps to user application code located in the internal Flash
 *   memory.
 *
 * @param[in] flash_addr
 *   Flash address to jump to
 *
 * @return
 *   Returns 0 jump is successfully, <0 if unable to jump.
 **/
int iris_go_to(uint32_t start_addr) {
    static uint8_t packet[GO_PACKET_LENGTH];
    uint8_t start_addr_checksum = 0x00;

    int ret = 0x00;
    uint8_t rx_data;

    /* First I2C transaction (2 bytes) */
    packet[0] = OPC_GO;
    packet[1] = N_OPC_GO;
    iris_write_packet(packet, 2);
    iris_read_packet(&rx_data, 1);
    memset(packet, 0, GO_PACKET_LENGTH);

    /* Second I2C transaction (5 bytes) */
    packet[0] = (start_addr >> (8 * 3)) & 0xff;
    packet[1] = (start_addr >> (8 * 2)) & 0xff;
    packet[2] = (start_addr >> (8 * 1)) & 0xff;
    packet[3] = (start_addr >> (8 * 0)) & 0xff;
    start_addr_checksum = packet[0] ^ packet[1] ^ packet[2] ^ packet[3];
    packet[4] = start_addr_checksum;
    iris_write_packet(packet, 5);
    iris_read_packet(&rx_data, 1);
    memset(packet, 0, GO_PACKET_LENGTH);

    return ret;
}

/**
 * @brief
 *   Erase entire flash program memory. TODO: Not yet being verified
 *
 * @return
 *   Returns 0 flash memory is erased, <0 if unable to mass erase.
 **/
int iris_mass_erase_flash() {
    static uint8_t packet[MASS_ERASE_PACKET_LENGTH];
    uint16_t num_pages_to_erase = 512;
    uint8_t checksum = 0x00;

    int ret = 0x00;
    uint8_t rx_data;

    /* First I2C transaction (2 bytes) */
    packet[0] = OPC_ERASE;
    packet[1] = N_OPC_ERASE;
    iris_write_packet(packet, 2);
    iris_read_packet(&rx_data, 1);
    memset(packet, 0, MASS_ERASE_PACKET_LENGTH);

    /* Second I2C transaction (3 bytes) */
    packet[0] = ((num_pages_to_erase - 1) >> (8 * 1)) & 0xff;
    packet[1] = ((num_pages_to_erase - 1) >> (8 * 0)) & 0xff;
    checksum = packet[0] ^ packet[1];
    packet[2] = checksum;
    iris_write_packet(packet, 3);
    iris_read_packet(&rx_data, 1);
    memset(packet, 0, MASS_ERASE_PACKET_LENGTH);

    /* Second I2C transaction (3 bytes) */
    uint16_t index;
    uint16_t page = 0x01;
    checksum = 0x00;
    for (index = 0; index < (num_pages_to_erase * 2); index += 2) {
        packet[index] = (page >> (8 * 1)) & 0xff;
        packet[index + 1] = (page >> (8 * 0)) & 0xff;
        checksum ^= (packet[index] ^ packet[index + 1]);

        page += 0x01;
    }
    packet[1024] = checksum;
    iris_write_packet(packet, 1025);
    iris_read_packet(&rx_data, 1);
    memset(packet, 0, MASS_ERASE_PACKET_LENGTH);

    return ret;
}

uint32_t get_file_size(int32_t fptr) {
    REDSTAT file_stat;

    red_fstat(fptr, &file_stat);

    return (uint32_t)file_stat.st_size;
}

uint32_t get_num_pages(uint32_t fsize) { return (fsize + PAGE_SIZE - 1) / PAGE_SIZE; }

Iris_HAL_return iris_program() {
    uint32_t flash_addr = FLASH_MEM_BASE_ADDR;
    uint8_t buffer[128];

    int32_t fptr;
    fptr = red_open("ex2_Iris_MCU_Software.bin", RED_O_RDONLY);

    if (fptr == -1) {
        return NULL;
    }

    uint32_t fsize = get_file_size(fptr);
    uint32_t num_pages = get_num_pages(fsize);

    iris_pre_sequence();

    for (uint32_t page = 0; page < num_pages; page++) {
        iris_erase_page(page);
        red_read(fptr, buffer, 128);
        iris_write_page(flash_addr, buffer);

        flash_addr += FLASH_MEM_PAGE_SIZE;
    }

    flash_addr = FLASH_MEM_BASE_ADDR;

    iris_go_to(flash_addr);
    iris_post_sequence();

    red_close(fptr);

    return IRIS_HAL_OK;
}
