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
 * @file ft_2u_payload_handler.c
 * @author Daniel Sacro
 * @date 2022-04-12
 */

#include "ft_2u_payload_handler.h"

#include <string.h>

#include "stdio.h"
#include <redconf.h>
#include <redfs.h>
#include <redfse.h>
#include <redposix.h>
#include <redtests.h>
#include <redvolume.h>

static int32_t fileToSend;
static int32_t fileToReceive;

FT_2U_payload_return FT_2U_payload_getFile(char * filename) {
    // Open file
    fileToSend = red_open(filename, RED_O_RDONLY);
    if (fileToSend == -1) {
        return FT_FAIL;
    } else {
        return FT_SUCCESS;
    }
}

FT_2U_payload_return FT_2U_payload_putFile(char * filename) {
    // Open file
    fileToReceive = red_open(filename, RED_O_RDONLY);
    if (fileToSend == -1) {
        return FT_FAIL;
    } else {
        return FT_SUCCESS;
    }
}

FT_2U_payload_return FT_2U_payload_stopFileTransfer() {
    // forcibly close all files
    red_close(fileToSend);
    red_close(fileToReceive);

    // TODO - Error checking ?
    return FT_SUCCESS;
}

FT_2U_payload_return FT_2U_payload_sendDataBytes(filePacket * outgoingPkt) {
    FT_2U_payload_return status;
    // read from file and place data bytes into a packet
    memset(outgoingPkt, 0, sizeof(outgoingPkt)); // Ensure that pkt is empty, should be done in HAL
    int bytesRead = red_read(fileToSend, &(outgoingPkt->byte[0]), MAX_BYTES_TO_READ);

    // TODO - Error checks

    if (bytesRead == -1) {
        // if red_read fails, abort file transfer
        red_close(fileToSend);
        status = FT_FAIL;
    } else if (bytesRead < MAX_BYTES_TO_READ) {
        // if red_read sends less than the MAX_BYTES_TO_READ, there are no more bytes to send
        outgoingPkt->bytesToRead = bytesRead;
        red_close(fileToSend);
        status = FT_SUCCESS;
    } else {
        // else red_read sends exactly MAX_BYTES_TO_READ, meaning there are more bytes to send
        outgoingPkt->bytesToRead = bytesRead;
        status = FT_SUCCESS;
    }
    return status;
}

FT_2U_payload_return FT_2U_payload_receiveDataBytes(filePacket * incomingPkt) {
    // get a packet and write data bytes to a file
    FT_2U_payload_return status;
    int totBytesInPkt = incomingPkt->bytesToRead;
    int err = red_write(fileToReceive, &(incomingPkt->byte[0]), totBytesInPkt);

    // TODO - Error checks
    if (err == -1) {
        // ABORT FILE TRANSFER
    }

    if (totBytesInPkt < MAX_BYTES_TO_READ) {
        // if bytes read from packet is less than the MAX_BYTES_TO_READ, end file transfer
        red_close(fileToReceive);
        status = FT_SUCCESS;
    } else {
        // else there are still more packets to be sent from the GS
        status = FT_SUCCESS;
    }

    return status;

    // automatically close the file if < N bytes are read from the file packet
}
