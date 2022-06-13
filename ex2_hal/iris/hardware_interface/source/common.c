/*
 * common.c
 *
 *  Created on: May 11, 2022
 *      Author: jenish
 */

#include "FreeRTOS.h"
#include "common.h"
#include "iris_bootloader_cmds.h"
#include <stdio.h>
#include <stdlib.h>

uint16_t get_file_size(FILE *fptr) {
    uint16_t fsize = 0; // Max .bin file size should be 64KB

    fseek(fptr, 0L, SEEK_END);
    fsize = ftell(fptr);
    fseek(fptr, 0L, SEEK_SET);

    return fsize;
}

uint16_t get_num_pages(uint16 fsize) {
    return (fsize + PAGE_SIZE - 1) / PAGE_SIZE;
}

uint8_t * read_bin_file(FILE *fptr, uint8_t *buffer) {
    fread(buffer, PAGE_SIZE*sizeof(uint8_t), 1, fptr);
}
