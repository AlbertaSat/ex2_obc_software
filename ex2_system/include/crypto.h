/*
 * Copyright (C) 2022  University of Alberta
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
 * hmac_key.h
 *
 *  Created on: Jun. 2, 2022
 *      Author: Robert Taylor
 */

#ifndef EX2_SYSTEM_INCLUDE_CRYPTO_H_
#define EX2_SYSTEM_INCLUDE_CRYPTO_H_

typedef enum {
    HMAC_KEY,
    ENCRYPT_KEY,
} CRYPTO_KEY_T;

void get_crypto_key(CRYPTO_KEY_T type, char **key, int *key_len);
void set_crypto_key(CRYPTO_KEY_T type, char *key, int key_len);

#endif /* EX2_SYSTEM_INCLUDE_CRYPTO_H_ */
