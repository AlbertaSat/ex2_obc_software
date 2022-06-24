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
 * @file ft_2u_payload_handler.h
 * @author Daniel Sacro
 * @date 2022-04-12
 */

#ifndef FT_2U_PAYLOAD_HANDLER_H
#define FT_2U_PAYLOAD_HANDLER_H

#include "system.h"
#include <stdint.h>

typedef enum {
    FT_SUCCESS = 0,
    FT_FAIL,
    FT_DOWNLINK,
    FT_UPLINK,

    IS_STUBBED_2U_PAYLOAD = 0
} FT_2U_payload_return;

// This value should exactly match the value for maxBytesToRead in FT_handler.py on the GS
#define MAX_BYTES_TO_READ 100

typedef struct __attribute__((__packed__)) {
    uint16_t bytesToRead;
    uint8_t byte[MAX_BYTES_TO_READ];
} filePacket;

FT_2U_payload_return FT_2U_payload_getFile(char * filename);
FT_2U_payload_return FT_2U_payload_putFile(char * filename);
FT_2U_payload_return FT_2U_payload_stopFileTransfer();
FT_2U_payload_return FT_2U_payload_sendDataBytes(filePacket * outgoingPkt);
FT_2U_payload_return FT_2U_payload_receiveDataBytes(filePacket * incomingPkt);

#endif /* FT_2U_PAYLOAD_HANDLER_H */
