/*
 * iris_bootloader_cmds.h
 *
 *  Created on: May 25, 2022
 *      Author: jenish
 */


#ifndef INCLUDE_IRIS_BOOTLOADER_CMDS_H_
#define INCLUDE_IRIS_BOOTLOADER_CMDS_H_
#endif /* INCLUDE_IRIS_BOOTLOADER_CMDS_H_ */

#include "FreeRTOS.h"
#include "iris_i2c.h"
#include <stdlib.h>

#define OPC_WRITE 0x31 // Write opcode
#define N_OPC_WRITE 0xCE
#define OPC_ERASE 0x44 // Erase opcode
#define N_OPC_ERASE 0xBB
#define OPC_CHECK_VERSION 0x01 // Check version opcode
#define N_OPC_CHECK_VERSION 0xFE
#define OPC_GO 0x21 // Go opcode
#define N_OPC_GO 0xDE
#define DUMMY_BYTE 0xFF

#define FLASH_MEM_PAGE_SIZE 128
#define FLASH_NUM_PAGES 512

#define WRITE_PACKET_LENGTH 130
#define ERASE_PACKET_LENGTH 8
#define CHECK_VERSION_PACKET_LENGTH 2
#define GO_PACKET_LENGTH 7
#define NUM_PAGES_TO_ERASE 1
#define MAX_NUM_BYTE_TRANSFER 2

int iris_write_page(uint32_t flash_addr);
int iris_erase_page(uint16_t page_num);
int iris_check_bootloader_version();
int iris_go_to(uint32_t start_addr);
int iris_mass_erase_flash();
void i2c_send_test();
