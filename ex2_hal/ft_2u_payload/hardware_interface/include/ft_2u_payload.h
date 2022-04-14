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
 * @file ft_2u_payload.h
 * @author Daniel Sacro
 * @date 2022-04-12
 */

#ifndef FT_2U_PAYLOAD_H
#define FT_2U_PAYLOAD_H

#include "ft_2u_payload_handler.h"

typedef struct __attribute__((__packed__)) {
    uint16_t bytesToRead;
    uint8_t byte[MAX_BYTES_TO_READ];
} FT_2U_PAYLOAD_filePacket;

FT_2U_payload_return HAL_FT_2U_PAYLOAD_getFile(char * filename); // argument should be a file name
FT_2U_payload_return HAL_FT_2U_PAYLOAD_putFile(char * filename); // argument should be filename
FT_2U_payload_return HAL_FT_2U_PAYLOAD_stopFileTransfer();
FT_2U_payload_return HAL_FT_2U_PAYLOAD_sendDataBytes(FT_2U_PAYLOAD_filePacket * outgoingPacket); // argument should be a struct containing data bytes
FT_2U_payload_return HAL_FT_2U_PAYLOAD_receiveDataBytes(FT_2U_PAYLOAD_filePacket * incomingPacket); // argument should be a struct containing data bytes


#endif /* FT_2U_PAYLOAD_H */
