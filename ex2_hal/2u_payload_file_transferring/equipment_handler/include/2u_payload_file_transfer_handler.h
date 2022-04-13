/*
 * Copyright (C) 2021  University of Alberta
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
 * @file 2u_payload_file_transfer_handler.h
 * @author Daniel Sacro
 * @date 2022-04-12
 */

#ifndef 2U_PAYLOAD_FT_HANDLER_H
#define 2U_PAYLOAD_FT_HANDLER_H

#include "system.h"

typedef enum {
    FT_SUCCESS = 0,
    FT_FAIL,

    IS_STUBBED_2U_PAYLOAD = 0
} payload_FT_return;

#define MAX_BYTES_TO_READ 900;

typedef struct __attribute__((__packed__)) {
    uint16 bytesToRead;
    uint8 byte[MAX_BYTES_TO_READ];
} filePacket;

payload_FT_return 2U_PAYLOAD_getFile(char * filename); // argument should be a file name
payload_FT_return 2U_PAYLOAD_putFile(char * filename); // argument should be filename
payload_FT_return 2U_PAYLOAD_stopFileTransfer();
payload_FT_return 2U_PAYLOAD_sendDataBytes(struct filePacket * outgoingPkt); // argument should be a struct containing data bytes
payload_FT_return 2U_PAYLOAD_receiveDataBytes(struct filePacket * incomingPkt); // argument should be a struct containing data bytes

#endif /* 2U_PAYLOAD_FT_HANDLER_H */
