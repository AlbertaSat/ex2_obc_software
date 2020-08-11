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
/**
 * @file service_utilities.c
 * @author upSat, Andrew Rooney
 * @date 2020-06-06
 */
#include "service_utilities.h"

#include <stdarg.h>
#include <stdio.h>

#include "time_management_service.h"

void ex2_log(const char *format, ...) {
//  va_list arg;
//  va_start(arg, format);
//  printf(format, arg, 0);
//  va_end(arg);
//  fflush(stdout);
  return;
}

/**
 * @brief
 * 		Copy elements from the packet header to return it to the sender
 * @details
 * 		copies elements from the CSP header but swap source/destination
 * @param csp_packet_t *packet
 *    Packet to swap source/destination
 * @return void
 */
void return_packet_header(csp_packet_t *packet) {
  // copy header data to return to sender
  packet->id.dst = packet->id.src;
  packet->id.dport = packet->id.sport;
  packet->id.src = packet->id.dst;
  packet->id.sport = packet->id.dport;
}

/* The following functions convert integer types from one representation to
 * another. Implementation from
 * https://gitlab.com/librespacefoundation/upsat/upsat-ecss-services
 */
void cnv32_8(const uint32_t from, uint8_t *to) {
  union _cnv cnv;

  cnv.cnv32 = from;
  to[0] = cnv.cnv8[0];
  to[1] = cnv.cnv8[1];
  to[2] = cnv.cnv8[2];
  to[3] = cnv.cnv8[3];
}

void cnv16_8(const uint16_t from, uint8_t *to) {
  union _cnv cnv;

  cnv.cnv16[0] = from;
  to[0] = cnv.cnv8[0];
  to[1] = cnv.cnv8[1];
}

void cnv8_32(uint8_t *from, uint32_t *to) {
  union _cnv cnv;

  cnv.cnv8[3] = from[3];
  cnv.cnv8[2] = from[2];
  cnv.cnv8[1] = from[1];
  cnv.cnv8[0] = from[0];
  *to = cnv.cnv32;
}

void cnv8_16LE(uint8_t *from, uint16_t *to) {
  union _cnv cnv;

  cnv.cnv8[1] = from[1];
  cnv.cnv8[0] = from[0];
  *to = cnv.cnv16[0];
}

void cnv8_16(uint8_t *from, uint16_t *to) {
  union _cnv cnv;

  cnv.cnv8[1] = from[0];
  cnv.cnv8[0] = from[1];
  *to = cnv.cnv16[0];
}

void cnvF_8(const float from, uint8_t *to) {
  union _cnv cnv;

  cnv.cnvF = from;
  to[0] = cnv.cnv8[0];
  to[1] = cnv.cnv8[1];
  to[2] = cnv.cnv8[2];
  to[3] = cnv.cnv8[3];
}

void cnv8_F(uint8_t *from, float *to) {
  union _cnv cnv;

  cnv.cnv8[3] = from[3];
  cnv.cnv8[2] = from[2];
  cnv.cnv8[1] = from[1];
  cnv.cnv8[0] = from[0];
  *to = cnv.cnvF;
}

void cnvD_8(const double from, uint8_t *to) {
  union _cnv cnv;

  cnv.cnvD = from;
  to[0] = cnv.cnv8[0];
  to[1] = cnv.cnv8[1];
  to[2] = cnv.cnv8[2];
  to[3] = cnv.cnv8[3];
  to[4] = cnv.cnv8[4];
  to[5] = cnv.cnv8[5];
  to[6] = cnv.cnv8[6];
  to[7] = cnv.cnv8[7];
}

void cnv8_D(uint8_t *from, double *to) {
  union _cnv cnv;

  cnv.cnv8[7] = from[7];
  cnv.cnv8[6] = from[6];
  cnv.cnv8[5] = from[5];
  cnv.cnv8[4] = from[4];
  cnv.cnv8[3] = from[3];
  cnv.cnv8[2] = from[2];
  cnv.cnv8[1] = from[1];
  cnv.cnv8[0] = from[0];
  *to = cnv.cnvD;
}

uint16_t htons(uint16_t x) {
#if (SYSTEM_ENDIANESS == SYS_LITTLE_ENDIAN)
  uint16_t ret = 0x0;
  ret = (x & 0xFF00) >> 8;
  ret |= (x & 0x00FF) << 8;
  return ret;
#else
  return x;
#endif
}

uint16_t ntohs(uint16_t x) { return htons(x); }
