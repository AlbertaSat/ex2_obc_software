/*
 * iris_bootloader_cmds.c
 *
 *  Created on: May 25, 2022
 *      Author: jenish
 */

#include "iris_bootloader_cmds.h"
#include "common.h"
#include "i2c_io.h"
#include "HL_gio.h"
#include "HL_reg_het.h"
#include "iris_i2c.h"
#include "FreeRTOS.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Optimization points
 * - As of now, the mass erase functionality is not verified and
 *   still WIP. We would like to have this functionality working in
 *   the future. But, we can manage with erasing individual pages for
 *   now
 * - When combining both i2c and spi drivers for iris, it will nice to
 *   have all of the GIO outputs in a different file (e.g. iris_gio.c)
 * - Also instead of using ints for good or bad return we should integrate
 *   the iris status flags. This is implemented in the spi feature
 */

void iris_i2c_init() {
   /* i2c initialization */
    init_i2c_driver();
}

void BOOT_LOW() {
    gioSetBit(hetPORT1, 14, 0);
}

void BOOT_HIGH() {
    gioSetBit(hetPORT1, 14, 1);
}

void POWER_OFF() {
    gioSetBit(hetPORT1, 8, 0);
}

void POWER_ON() {
    gioSetBit(hetPORT1, 8, 1);
}

void iris_pre_sequence() {
    /* Start initialization sequence before I2C transaction */
    POWER_OFF();
    vTaskDelay(100);
    BOOT_HIGH();
    vTaskDelay(100);
    POWER_ON();
    vTaskDelay(100);
}

void iris_post_sequence() {
    /* End I2C transaction by doing end sequence*/
    BOOT_LOW();
    vTaskDelay(100);
    POWER_OFF();
    vTaskDelay(100);
    POWER_ON();
}

int iris_write_page(uint32_t flash_addr, uint8_t * buffer) {
    uint8_t num_bytes = FLASH_MEM_PAGE_SIZE;     /* number of bytes is equal to page size of flash memory on iris */
    uint8_t *packet = (uint8_t*)calloc(WRITE_PACKET_LENGTH, sizeof(uint8_t));           /*  Allocate bytes equal to page size of flash memory */
    uint8_t flash_mem_checksum = 0x00;
    uint8_t data_checksum = 0x00;

    int ret = 0x00;
    int i;
    uint8_t rx_data;

    /* First I2C transaction (2 bytes) */
    packet[0] = OPC_WRITE;
    packet[1] = N_OPC_WRITE;
    write_packet(packet, 2);
    read_packet(&rx_data, 1);
    memset(packet, 0, WRITE_PACKET_LENGTH*sizeof(uint8_t));


    /* Second I2C transaction (5 bytes) */
    packet[0] = (flash_addr >> (8*3)) & 0xff;
    packet[1] = (flash_addr >> (8*2)) & 0xff;
    packet[2] = (flash_addr >> (8*1)) & 0xff;
    packet[3] = (flash_addr >> (8*0)) & 0xff;
    flash_mem_checksum = packet[0] ^ packet[1] ^ packet[2] ^ packet[3];
    packet[4] = flash_mem_checksum;
    write_packet(packet, 5);
    read_packet(&rx_data, 1);
    memset(packet, 0, WRITE_PACKET_LENGTH*sizeof(uint8_t));


    /* Third I2C transaction (2 + num_bytes) */
    packet[0] = num_bytes - 1;
    data_checksum ^= packet[0];
    // transmit N+1 bytes of data
    for(i = 0; i < 128; i++){
        packet[i+1] = buffer[i];
        data_checksum ^= *(packet + i + 1);
    }
    packet[num_bytes + 1] = data_checksum;
    write_packet(packet, WRITE_PACKET_LENGTH);
    read_packet(&rx_data, 1);
    memset(packet, 0, WRITE_PACKET_LENGTH*sizeof(uint8_t));

    free(packet);

    return ret;
}

/* Page number is from 0-511, starting from 0x0800 0000 - 0x0x0800 FFFF */
int iris_erase_page(uint16_t page_num) {
    uint8_t *packet = (uint8_t*)calloc(ERASE_PACKET_LENGTH, sizeof(uint8_t));
    uint8_t num_page_erased_checksum = 0x00;
    uint8_t page_num_checksum = 0x00;

    int ret = 0x00;
    uint8_t rx_data;

    /* First I2C transaction (2 bytes) */
    packet[0] = OPC_ERASE;
    packet[1] = N_OPC_ERASE;
    write_packet(packet, 2);
    read_packet(&rx_data, 1);
    memset(packet, 0, ERASE_PACKET_LENGTH);


    /* Second I2C transaction (3 bytes) */
    packet[0] = 0;
    packet[1] = NUM_PAGES_TO_ERASE - 1;
    num_page_erased_checksum = packet[0] ^ packet[1];
    packet[2] = num_page_erased_checksum;
    write_packet(packet, 3);
    read_packet(&rx_data, 1);
    memset(packet, 0, ERASE_PACKET_LENGTH);


    /* Third I2C transaction (2 + num_bytes) */
    packet[0] = (page_num >> (8*1)) & 0xff;
    packet[1] = (page_num >> (8*0)) & 0xff;
    page_num_checksum = packet[0] ^ packet[1];
    packet[2] = page_num_checksum;
    write_packet(packet, 3);
    read_packet(&rx_data, 1);
    memset(packet, 0, ERASE_PACKET_LENGTH);

    free(packet);

    return ret;
}


int iris_check_bootloader_version() {
    uint8_t *packet = (uint8_t*)calloc(2, sizeof(uint8_t));

    int ret = 0x00;
    uint8_t rx_data;

    /* First I2C transaction (2 bytes) */
    packet[0] = OPC_CHECK_VERSION;
    packet[1] = N_OPC_CHECK_VERSION;
    write_packet(packet, 2);
    read_packet(&rx_data, 1);
    memset(packet, 0, 2);

    /* Read bootloader version */
    read_packet(&rx_data, 1);

    /* Wait for ACK/NACK */
    read_packet(&rx_data, 1);

    free(packet);

    return ret;
}

int iris_go_to(uint32_t start_addr) {
    uint8_t *packet = (uint8_t*)calloc(GO_PACKET_LENGTH, sizeof(uint8_t));
    uint8_t start_addr_checksum = 0x00;

    int ret = 0x00;
    uint8_t rx_data;

    /* First I2C transaction (2 bytes) */
    packet[0] = OPC_GO;
    packet[1] = N_OPC_GO;
    write_packet(packet, 2);
    read_packet(&rx_data, 1);
    memset(packet, 0, GO_PACKET_LENGTH);


    /* Second I2C transaction (5 bytes) */
    packet[0] = (start_addr >> (8*3)) & 0xff;
    packet[1] = (start_addr >> (8*2)) & 0xff;
    packet[2] = (start_addr >> (8*1)) & 0xff;
    packet[3] = (start_addr >> (8*0)) & 0xff;
    start_addr_checksum = packet[0] ^ packet[1] ^ packet[2] ^ packet[3];
    packet[4] = start_addr_checksum;
    write_packet(packet, 5);
    read_packet(&rx_data, 1);
    memset(packet, 0, GO_PACKET_LENGTH);

    free(packet);

    return ret;
}


int iris_mass_erase_flash() {
    uint8_t packet[MASS_ERASE_PACKET_LENGTH];
    uint16_t num_pages_to_erase = 512;
    uint8_t checksum = 0x00;

    int ret = 0x00;
    uint8_t rx_data;

    /* First I2C transaction (2 bytes) */
    packet[0] = OPC_ERASE;
    packet[1] = N_OPC_ERASE;
    write_packet(packet, 2);
    read_packet(&rx_data, 1);
    memset(packet, 0, MASS_ERASE_PACKET_LENGTH);


    /* Second I2C transaction (3 bytes) */
    packet[0] = ((num_pages_to_erase - 1) >> (8*1)) & 0xff;
    packet[1] = ((num_pages_to_erase - 1) >> (8*0)) & 0xff;
    checksum = packet[0] ^ packet[1];
    packet[2] = checksum;
    write_packet(packet, 3);
    read_packet(&rx_data, 1);
    memset(packet, 0, MASS_ERASE_PACKET_LENGTH);

    /* Second I2C transaction (3 bytes) */
    uint16_t index;
    uint16_t page = 0x01;
    checksum = 0x00;
    for (index = 0; index < (num_pages_to_erase * 2); index+=2) {
        packet[index] = (page >> (8*1)) & 0xff;
        packet[index+1] = (page >> (8*0)) & 0xff;
        checksum ^= (packet[index] ^ packet[index+1]);

        page += 0x01;
    }
    packet[1024] = checksum;
    write_packet(packet, 1025);
    read_packet(&rx_data, 1);
    memset(packet, 0, MASS_ERASE_PACKET_LENGTH);

    return ret;
}

