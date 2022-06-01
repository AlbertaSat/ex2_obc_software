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


void i2c_send_test() {

    iris_i2c_init();

    uint32_t flash_addr = 0x08001000;
    uint16_t data_length = 0x000F;
    iris_write_page(flash_addr);
    //i2c_erase_memory();

    vTaskDelay(100);

}

int iris_write_page(uint32_t flash_addr) {
    uint8_t num_bytes = FLASH_MEM_PAGE_SIZE;     /* number of bytes is equal to page size of flash memory on iris */
    uint8_t opc_wren = OPC_WREN;                /* write command opcode */
    uint8_t n_opc_wren = N_OPC_WREN;              /* command after the write command opcode */

    uint8_t *packet = (uint8_t*)calloc(WRITE_PACKET_LENGTH, sizeof(uint8_t));           /*  Allocate bytes equal to page size of flash memory */
    uint8_t flash_mem_checksum = 0x00;
    uint8_t data_checksum = 0x00;

    int ret = 0x00;
    int i;
    uint8_t rx_data;

    /* Start initialization sequence before I2C transaction */
    POWER_OFF();
    vTaskDelay(100);
    BOOT_HIGH();
    vTaskDelay(100);
    POWER_ON();
    vTaskDelay(100);

    /* First I2C transaction (2 bytes) */
    packet[0] = opc_wren;
    packet[1] = n_opc_wren;
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
        packet[i+1] = i;
        data_checksum ^= *(packet + i + 1);
    }
    packet[num_bytes + 1] = data_checksum;
    write_packet(packet, WRITE_PACKET_LENGTH);
    read_packet(&rx_data, 1);
    memset(packet, 0, WRITE_PACKET_LENGTH*sizeof(uint8_t));
    free(packet);

    /* End I2C transaction by doing end sequence*/
    BOOT_LOW();
    vTaskDelay(100);
    POWER_OFF();
    vTaskDelay(100);
    POWER_ON();

    return ret;
}

/* Page number is from 0-511, starting from 0x0800 0000 - 0x0x0800 FFFF */
int iris_erase_page(uint16_t page_num) {
    uint8_t opc_erusm = OPC_ERUSM;            /* erase command opcode */
    uint8_t n_opc_erusm = N_OPC_ERUSM;          /* command after the erase command opcode */

    uint8_t *packet = (uint8_t*)calloc(ERASE_PACKET_LENGTH, sizeof(uint8_t));
    uint8_t num_page_erased_checksum = 0x00;
    uint8_t page_num_checksum = 0x00;

    int ret = 0x00;
    int i;
    uint8_t rx_data;

    /* Start initialization sequence before I2C transaction */
    POWER_OFF();
    vTaskDelay(100);
    BOOT_HIGH();
    vTaskDelay(100);
    POWER_ON();
    vTaskDelay(100);

    /* First I2C transaction (2 bytes) */
    packet[0] = opc_erusm;
    packet[1] = n_opc_erusm;
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

    /* End I2C transaction by doing end sequence*/
    BOOT_LOW();
    vTaskDelay(100);
    POWER_OFF();
    vTaskDelay(100);
    POWER_ON();

    return ret;
}

int iris_mass_erase_flash() {
    int ret = 0;

    for (int page_num = 0; page_num < 512; page_num++) {
        iris_erase_page(page_num);
        // TODO: Verify return
    }
}

