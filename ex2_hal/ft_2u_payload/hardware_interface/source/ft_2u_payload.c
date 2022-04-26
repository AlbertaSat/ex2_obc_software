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
 * @file ft_2u_payload.c
 * @author Daniel Sacro
 * @date 2022-04-12
 */

#include "ft_2u_payload.h"
#include "services.h"
#include <string.h>


FT_2U_payload_return HAL_FT_2U_PAYLOAD_getFile(char * filename) {
    FT_2U_payload_return status;
#ifndef PAYLOAD_2U_IS_STUBBED
    status = FT_2U_payload_getFile(filename);
#else
    status = IS_STUBBED_2U_PAYLOAD;
#endif
    return status;
}

FT_2U_payload_return HAL_FT_2U_PAYLOAD_putFile(char * filename) {
    FT_2U_payload_return status;
#ifndef PAYLOAD_2U_IS_STUBBED
    status = FT_2U_payload_putFile(filename);
#else
    status = IS_STUBBED_2U_PAYLOAD;
#endif
    return status;
}

FT_2U_payload_return HAL_FT_2U_PAYLOAD_stopFileTransfer() {
    FT_2U_payload_return status;
#ifndef PAYLOAD_2U_IS_STUBBED
    status = FT_2U_payload_stopFileTransfer();
#else
    status = IS_STUBBED_2U_PAYLOAD;
#endif
    return status;
}

FT_2U_payload_return HAL_FT_2U_PAYLOAD_sendDataBytes(FT_2U_PAYLOAD_filePacket * outgoingPacket) {
    FT_2U_payload_return status;
#ifndef PAYLOAD_2U_IS_STUBBED
    filePacket outgoingPkt = {0};

    // Get file data SD card and copy to outgoingPacket
    status = FT_2U_payload_sendDataBytes(&outgoingPkt);
    memcpy(outgoingPacket, &outgoingPkt, sizeof(outgoingPkt));
#else
    status = IS_STUBBED_2U_PAYLOAD;
#endif
    return status;
}

FT_2U_payload_return HAL_FT_2U_PAYLOAD_receiveDataBytes(FT_2U_PAYLOAD_filePacket * incomingPacket) {
    FT_2U_payload_return status;
#ifndef PAYLOAD_2U_IS_STUBBED
    filePacket incomingPkt = {0};

    // Get file data from incomingPacket and save to SD card
    memcpy(&incomingPkt, incomingPacket, sizeof(incomingPkt));
    status = FT_2U_payload_receiveDataBytes(&incomingPkt);
#else
    status = IS_STUBBED_2U_PAYLOAD;
#endif
    return status;
}
