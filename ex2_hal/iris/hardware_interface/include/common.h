/*
 * common.h
 *
 *  Created on: May 11, 2022
 *      Author: jenish
 */

#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_
#endif /* INCLUDE_COMMON_H_ */

#include "FreeRTOS.h"
#include <stdio.h>
#include <stdlib.h>

#define PAGE_SIZE 128

uint16_t get_file_size(FILE *fptr);
uint16_t get_num_pages(uint16 fsize);
uint8_t * read_bin_file(FILE *fptr, uint8_t *buffer);

