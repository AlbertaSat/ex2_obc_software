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

/*
 * b64_encode_decode.h
 *
 *  Created on: Nov. 29, 2021
 *      Author: Grace Yi
 */

#ifndef B64_ENCODE_DECODE_H_
#define B64_ENCODE_DECODE_H_

#include <stdint.h>
#include <stdlib.h>

char *base64_encode(const unsigned char *data, size_t input_length, size_t *output_length);
unsigned char *base64_decode(const char *data, size_t input_length, size_t *output_length);

#endif /* B64_ENCODE_DECODE_H_ */
