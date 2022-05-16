/*
 * common.c
 *
 *  Created on: May 11, 2022
 *      Author: jenish
 */

#include "FreeRTOS.h"
#include <stdio.h>

void read_bin_file(uint16_t *buffer, const char *filepath) {
    FILE *fptr;
    fptr = fopen(filepath, "rb");

    fread(buffer, sizeof(buffer), 1, fptr);
}

