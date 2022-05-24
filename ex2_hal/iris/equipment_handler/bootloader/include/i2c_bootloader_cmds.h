/*
 * i2c_driver.h
 *
 *  Created on: May 10, 2022
 *      Author: jenish
 */

#ifndef INCLUDE_I2C_BOOTLOADER_CMDS_H_
#define INCLUDE_I2C_BOOTLOADER_CMDS_H_
#endif /* INCLUDE_I2C_BOOTLOADER_CMDS_H_ */

#include "FreeRTOS.h"

/* TODO: Define bytes used in i2c communication
 * #define BASE_ADDRESS 0x1FF00000 // Base address of the the flash memory
 */


#define START_FLAG 0x01
#define STOP_FLAG 0x02
#define WRITE_FLAG 0x03
#define ACK 0x0A
#define NACK 0x06
#define DUMMY_BYTE 0xFF

#define OPC_WREN 0x04 //0100
#define OPC_ERUSM 0x0B //1011

// Define number of byte during one i2c transaction (Number of bytes will
// correspond the page size in the flash memory
#define TRANSFER_SIZE 8
#define MAX_NUM_BYTE_TRANSFER 2

// Define size of STM32 Flash memory (total number of pages)
#define FLASH_MEM_SIZE 128
#define FLASH_MEM_ADDR_SIZE 4

// TODO: (Optional) The maximum of pages erased in one i2c transaction

enum write_states {
    WRITE_START_SEQ,
    WRITE_OP_CODE,
    WRITE_FLASH_ADDR,
    WRITE_NUM_BYTES,
    WRITE_DUMMY,
    WRITE_DATA,
    WRITE_STOP_SEQ,
    WRITE_ERROR,
} write_state;

// TODO: states for erase flash memory
enum erase_states {
    ERASE_START_SEQ,
    ERASE_OP_CODE,
    ERASE_STOP_SEQ,
    ERASE_ERROR,
} erase_state;

/* TODO: Function prototypes    */
void i2c_bootloader_init();
int write_packet(void *buf_tx_data);
int read_packet();
int verify_ack();
void i2c_bootloader_write(uint32_t flash_addr, uint16_t num_bytes, void * bin_file);
void i2c_bootloader_erase();
void i2c_send_test();
