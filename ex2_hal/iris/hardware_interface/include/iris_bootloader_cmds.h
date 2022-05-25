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

#define OPC_WREN 0x04 // Write opcode
#define OPC_ERUSM 0x0B // Erase opcode
#define DUMMY_BYTE 0xFF

#define WRITE_METADATA_LENGTH 8
#define ERASE_METADATA_LENGTH 1
#define MAX_NUM_BYTE_TRANSFER 2

#define FLASH_MEM_SIZE 128
#define FLASH_MEM_ADDR_SIZE 0x80001000

int i2c_write_memory(uint32_t flash_addr, uint16_t num_bytes);
int i2c_erase_memory();
void i2c_send_test();
