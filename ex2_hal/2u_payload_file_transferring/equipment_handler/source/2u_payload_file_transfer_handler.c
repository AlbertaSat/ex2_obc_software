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
 * @file 2u_payload_file_transfer_handler.c
 * @author Daniel Sacro
 * @date 2022-04-12
 */

#include "2u_payload_file_transfer_handler.h"

static int32_t fileToSend;
static int32_t fileToReceive;

payload_FT_return 2U_PAYLOAD_getFile(char * filename) {
    // Open file
    fileToSend = red_open(filename, RED_O_RDONLY);
    if (fileToSend == -1) {
        return FT_FAIL;
    } else {
        return FT_SUCCESS;
    }
}

payload_FT_return 2U_PAYLOAD_putFile(char * filename) {
    // Open file
    fileToReceive = red_open(filename, RED_O_RDONLY);
    if (fileToSend == -1) {
        return FT_FAIL;
    } else {
        return FT_SUCCESS;
    }
}

payload_FT_return 2U_PAYLOAD_stopFileTransfer() {
    // forcibly close all files
    red_close(fileToSend);
    red_close(fileToReceive);

    // TODO - Error checking ?
    return FT_SUCCESS;
}

payload_FT_return 2U_PAYLOAD_sendDataBytes(struct filePacket * outgoingPkt) {
    // read from file and place data bytes into a packet
    memset(outgoingPkt, 0, sizeof(outgoingPkt)); // Ensure that pkt is empty, should be done in HAL
    int bytesRead = red_read(fileToSend, outgoingPkt.byte[0], MAX_BYTES_TO_READ);

    // TODO - Error checks

    if (bytesRead == -1) {
        // if red_read fails, abort file transfer
        red_close(fileToSend);
        return FT_FAIL;
    } else if (bytesRead < MAX_BYTES_TO_READ) {
        // if red_read sends less than the MAX_BYTES_TO_READ, there are no more bytes to send
        outgoingPkt->bytesToRead = bytesRead;
        red_close(fileToSend);
        return FT_SUCCESS;
    } else {
        // else red_read sends exactly MAX_BYTES_TO_READ, meaning there are more bytes to send
        outgoingPkt->bytesToRead = bytesRead;
        return FT_SUCCESS;
    }
}

payload_FT_return 2U_PAYLOAD_receiveDataBytes(struct filePacket * incomingPkt) {
    // get a packet and write data bytes to a file
    int totBytesInPkt = incomingPkt->bytesToRead;
    int err = red_write(fileToReceive, outgoingPkt.byte[0], totBytesInPkt);

    // TODO - Error checks
    if (err == -1) {
        // ABORT FILE TRANSFER
    }

    if (totBytesInPkt < MAX_BYTES_TO_READ) {
        // if bytes read from packet is less than the MAX_BYTES_TO_READ, end file transfer
        red_close(fileToReceive);
        return FT_SUCCESS
    } else {
        // else there are still more packets to be sent from the GS
        return FT_SUCCESS
    }


    // automatically close the file if < N bytes are read from the file packet
}
