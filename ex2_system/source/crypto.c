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
#include "bl_eeprom.h"
#include <stdbool.h>

#if KEY_TEST_MODE
const char test_key[] =
    "6e477331cd51d63d6492fa969a3acfc75fc26370446465a339fe380c096193fa1fc7d866f17ec1bce02b9b5f955c9df41bdd26927891c"
    "d4c8c877913138bd6ca27bb05167462c2e028b0afeb372cd23720278f48715f065fd7bab587d6e0e7a86d904580aa6ad1f771f9d651e6"
    "934f361d2816187d934ad87691f977bd5b964fc8e6ed4debbc32f0144e03bb6c94982ea801fa5d2efdd381836fd63a28bebf1f877efdf"
    "0e12f7063d13de186ecf1bf295cd64c65ab7b74893578b3fde314cfcabc4946ffec142faab6019aedfd2cfc723ae51c3771a45b2004ab"
    "77865261e91e763c76b271086f069f4598b25ed8567ef72b4a554046b395d4815bf7974d2962";
#endif

#if KEY_SET_MODE
#include <stdio.h>
#endif

void set_keys_from_keyfile() {
#if KEY_SET_MODE
    FILE *fh = fopen("hmacKey.dat", "rb");
    FILE *fx = fopen("encryptKey.dat", "rb");

    char hmac_key[KEY_LEN] = {0};
    fread(&hmac_key, KEY_LEN, 1, fh);
    set_crypto_key(HMAC_KEY, (char *)&hmac_key, KEY_LEN);

    char encrypt_key[KEY_LEN] = {0};
    fread(&encrypt_key, KEY_LEN, 1, fx);
    set_crypto_key(ENCRYPT_KEY, (char *)&encrypt_key, KEY_LEN);

    fclose(fh);
    fclose(fx);
#endif
    return;
}

static key_store keys = {0};
static bool keys_initialized = false;

void init_keys() {
    eeprom_get_key_store(&keys);
    keys_initialized = true;
}

void set_hmac_key(char *key, int key_len) {
    if (key_len != KEY_LEN) {
        return; // This can silently return since it is intended to run with supervision
    }
    eeprom_get_key_store(&keys);
    memcpy(&keys.hmac_key.key, key, key_len);
    keys.hmac_key.key_len = key_len;
    eeprom_set_key_store(&keys);
}

void get_hmac_key(char **hmac_key, int *key_len) {
    if (keys_initialized == false) {
        init_keys();
    }
    *hmac_key = (char *)&keys.hmac_key.key;
    *key_len = (int)keys.hmac_key.key_len;
}

void set_xtea_key(char *key, int key_len) {
    if (key_len != KEY_LEN) {
        return; // This can silently return since it is intended to run with supervision
    }
    eeprom_get_key_store(&keys);
    memcpy(&keys.encrypt_key.key, key, key_len);
    keys.encrypt_key.key_len = key_len;
    eeprom_set_key_store(&keys);
}

void get_xtea_key(char **xtea_key, int *key_len) {
    if (keys_initialized == false) {
        init_keys();
    }
    *xtea_key = (char *)&keys.encrypt_key.key;
    *key_len = (int)keys.encrypt_key.key_len;
}

void get_crypto_key(CRYPTO_KEY_T type, char **key, int *key_len) {
    (void)type; // Same key for test mode

#if KEY_TEST_MODE
    *key = (char *)&test_key;
    *key_len = (int)strlen(test_key);
#else
    if (type == ENCRYPT_KEY) {
        get_xtea_key(key, key_len);
    } else if (type == HMAC_KEY) {
        get_hmac_key(key, key_len);
    }
#endif
}

void set_crypto_key(CRYPTO_KEY_T type, char *key, int key_len) {
#if KEY_TEST_MODE
    return;
#else
    if (type == ENCRYPT_KEY) {
        set_xtea_key(key, key_len);
    } else if (type == HMAC_KEY) {
        set_hmac_key(key, key_len);
    }
#endif
}
