/*
 * i2c_bootloader_cmds.c
 *
 *  Created on: May 10, 2022
 *      Author: jenish
 */

#include "FreeRTOS.h"
#include "i2c_bootloader_cmds.h"
#include "i2c_io.h"
#include "HL_sys_common.h"
#include "system.h"
#include <stdio.h>

i2cBASE_t * i2c_handle = IRIS_I2C; // temp
uint8_t slave_addr; // What is the slave (Iris-flash; where the user-code start) address?

uint8_t rx_data;
uint8_t tx_data;
int ret;

void i2c_bootloader_init() {
   /* i2c initialization */
    init_i2c_driver();
}

int write_packet(void *buf_tx_data) {
    return i2c_Send(i2c_handle, SLAVE_ADDR, TRANSFER_SIZE, buf_tx_data); //Data size is constant as 7-bit data + 1-bit R/W (8 bits), should this be constant?
}

int read_packet() {
    return i2c_Receive(i2c_handle, SLAVE_ADDR, TRANSFER_SIZE, &rx_data); //Data is stored in buf_tx_data
}

int verify_ack() { // Will not be void
    ret = read_packet();
    if (ret < 0); printf("Error");

    if (rx_data == NACK){
        printf("Error: NACK received\n");
        return -1;
    }
    else if (rx_data != ACK){    // if rx_data not ACK or NACK
        printf("Error: unexpected byte (%#x) received\n", rx_data);
        return -1;
    }
    else {
        printf("Acknowledgment accepted\n");
        return 0;
    }
}

void i2c_bootloader_write(uint32_t flash_addr, uint16_t num_bytes, void * bin_file) {
    // Preferred to use a write_state machine
    uint8_t i;

    write_state = WRITE_START_SEQ;
    switch (write_state) {
        case WRITE_START_SEQ: /* Send start bit, target address (slave address?), write bit     */
        {
            tx_data = START_FLAG;
            ret = write_packet(&tx_data);
            if (ret < 0); printf("Error");

            tx_data = SLAVE_ADDR;
            ret = write_packet(&tx_data);
            if (ret < 0); printf("Error");

            tx_data = WRITE_FLAG;
            ret = write_packet(&tx_data);
            if (ret < 0); printf("Error");

            /* Ack #1   */
            if (verify_ack() == 0) write_state = WRITE_OP_CODE;
            else write_state = WRITE_ERROR;
            break;
        }
        case WRITE_OP_CODE:  /* Send op code    */
        {
            tx_data = OPC_WREN;
            ret = write_packet(&tx_data);
            if (ret < 0); printf("Error");

            /* Ack #2   */
            if (verify_ack() == 0) write_state = WRITE_FLASH_ADDR;
            else write_state = WRITE_ERROR;
            break;
        }
        case WRITE_FLASH_ADDR: /* Send Addr_high/low_1/0      */
        {
            for (i = 0; i < num_bytes; i++) {
                tx_data = (flash_addr >> (8*i)) & 0xff; // Break flash address into 4 bytes
                ret = write_packet(&tx_data);
                if (ret < 0); printf("Error");

                /* Ack #3, #4, #5, #6   */
                if (verify_ack() == 0) write_state = WRITE_NUM_BYTES;
                else write_state = WRITE_ERROR;
            }
            break;
        }
        case WRITE_NUM_BYTES: /* Send number of bytes (Numbr_1/2)     */
        {
            for (i = 0; i < MAX_NUM_BYTE_TRANSFER; i++) {
                tx_data = (num_bytes >> (8*i)) & 0xff; // Break num_byte into 2 bytes
                ret = write_packet(&tx_data);
                if (ret < 0); printf("Error");
                /* Ack #7, #8   */
                if (verify_ack() == 0) write_state = WRITE_DUMMY;
                else write_state = WRITE_ERROR;
            }
            break;
        }
        case WRITE_DUMMY: /* Send dummy byte     */
        {
            tx_data = DUMMY_BYTE;
            ret = write_packet(&tx_data);
            if (ret < 0); printf("Error");
            if (verify_ack() == 0) write_state = WRITE_DATA;
            else write_state = WRITE_ERROR;
            break;
        }
        case WRITE_DATA: /* TODO: Send Data from source (e.g. .bin file byte-by-byte     */
        {
            /* TODO: Ack #10   */
            write_state = WRITE_STOP_SEQ;
            break;
        }
        case WRITE_STOP_SEQ: /* Send Stop bit     */
        {
            tx_data = STOP_FLAG;
            ret = write_packet(&tx_data);
            if (ret < 0); printf("Error");
            break;
        }
        case WRITE_ERROR:
        {
            printf("WRITE ERROR");
            break;
        }
    }
}

void i2c_bootloader_erase() {
    erase_state = ERASE_START_SEQ;

    switch (erase_state) {
        case ERASE_START_SEQ: /* Send start bit, target address (slave address?), write bit     */
        {
            tx_data = START_FLAG;
            ret = write_packet(&tx_data);
            if (ret < 0); printf("Error");

            tx_data = SLAVE_ADDR;
            ret = write_packet(&tx_data);
            if (ret < 0); printf("Error");

            tx_data = WRITE_FLAG;
            ret = write_packet(&tx_data);
            if (ret < 0); printf("Error");

            /* Ack #1   */
            if (verify_ack() == 0) erase_state = ERASE_OP_CODE;
            else erase_state = ERASE_ERROR;
            break;
        }
        case ERASE_OP_CODE: /* Send op code    */
        {
            tx_data = OPC_ERUSM;
            ret = write_packet(&tx_data);
            if (ret < 0); printf("Error");

            /* Ack #2   */
            if (verify_ack() == 0) erase_state = ERASE_STOP_SEQ;
            else erase_state = ERASE_ERROR;
            break;
        }
        case ERASE_STOP_SEQ: {
            tx_data = STOP_FLAG;
            ret = write_packet(&tx_data);
            if (ret < 0); printf("Error");
            break;
        }
        case ERASE_ERROR: {
            printf("ERASE ERROR");
            break;
        }
    }
}




