/*
 * common.c
 *
 *  Created on: May 11, 2022
 *      Author: jenish
 */

#include "FreeRTOS.h"
#include <stdio.h>
#include <stdlib.h>

uint16_t * read_bin_file(const char *filepath) {
    uint8_t *buffer;
    uint32_t fsize = 0;

    FILE *fptr;
    fptr = fopen(filepath, "rb");

    if (fptr == NULL) {
        printf("HELLO");
    }

    fseek(fptr, 0L, SEEK_END);
    fsize = ftell(fptr);
    fseek(fptr, 0L, SEEK_SET);

    buffer = (uint8_t*) calloc(fsize, sizeof(uint8_t));

    fread(buffer, sizeof(buffer), 1, fptr);

    fclose(fptr);
    free(buffer);

    fseek(fptr, 0L, SEEK_END);
    fseek(fptr, 0L, SEEK_SET);

    return buffer;
}

