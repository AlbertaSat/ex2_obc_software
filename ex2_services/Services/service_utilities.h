/*
 * Copyright (C) 2015  University of Alberta
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SERVICE_UTILITIES_H
#define SERVICE_UTILITIES_H

#include <stdint.h>

#include "services.h"

void ex2_log(const char *format, ...);

void copy_packet_header(csp_packet_t *packet);

void cnv32_8(const uint32_t from, uint8_t *to);

void cnv16_8(const uint16_t from, uint8_t *to);

void cnv8_32(uint8_t *from, uint32_t *to);

void cnv8_16(uint8_t *from, uint16_t *to);

void cnv8_16LE(uint8_t *from, uint16_t *to);

void cnvF_8(const float from, uint8_t *to);

void cnv8_F(uint8_t *from, float *to);

void cnvD_8(const double from, uint8_t *to);

void cnv8_D(uint8_t *from, double *to);

uint16_t htons(uint16_t x);
uint16_t ntohs(uint16_t x);

#endif /* SERVICE_UTILITIES_H */
