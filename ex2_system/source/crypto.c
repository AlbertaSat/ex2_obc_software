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
 * crypto.c
 *
 *  Created on: Jun. 2, 2022
 *      Author: Robert Taylor
 */

#include "crypto.h"

#define KEY_TEST_MODE

#ifdef KEY_TEST_MODE
const char test_key[] =
    "6e477331cd51d63d6492fa969a3acfc75fc26370446465a339fe380c096193fa1fc7d866f17ec1bce02b9b5f955c9df41bdd26927891c"
    "d4c8c877913138bd6ca27bb05167462c2e028b0afeb372cd23720278f48715f065fd7bab587d6e0e7a86d904580aa6ad1f771f9d651e6"
    "934f361d2816187d934ad87691f977bd5b964fc8e6ed4debbc32f0144e03bb6c94982ea801fa5d2efdd381836fd63a28bebf1f877efdf"
    "0e12f7063d13de186ecf1bf295cd64c65ab7b74893578b3fde314cfcabc4946ffec142faab6019aedfd2cfc723ae51c3771a45b2004ab"
    "77865261e91e763c76b271086f069f4598b25ed8567ef72b4a554046b395d4815bf7974d2962";
#endif

void get_crypto_key(CRYPTO_KEY_T type, char **key, int *key_len) {
    (void)type; // Same key for test mode

#ifdef KEY_TEST_MODE
    *key = &test_key;
    *key_len = strlen(test_key);
#else
    *key = 0;
    *key_len = 0;
#endif
}
void set_crypto_key(CRYPTO_KEY_T type, char *key, int *key_len);
