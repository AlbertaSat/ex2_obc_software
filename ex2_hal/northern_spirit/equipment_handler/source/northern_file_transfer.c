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

#include <northern_file_transfer.h>
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
    // Open file for reading
    fileToSend = red_open(filename, RED_O_RDONLY);
    if (fileToSend == -1) {
        // File doesn't exist (or some other error has occurred)
        return FT_FAIL;
    } else {
        // File exists and was opened successfully
        return FT_DOWNLINK;
    }
}

FT_2U_payload_return FT_2U_payload_putFile(char * filename) {
    // Open file for writing. Existing data should be overwritten
    fileToReceive = red_open(filename, RED_O_CREAT | RED_O_WRONLY | RED_O_TRUNC);
    if (fileToSend == -1) {
        // File could not be opened
        return FT_FAIL;
    } else {
        // File opened successfully
        return FT_UPLINK;
    }
}

FT_2U_payload_return FT_2U_payload_stopFileTransfer() {
    FT_2U_payload_return status = FT_SUCCESS;
    int err;

    // Close all files
    err = red_close(fileToSend);
    if (err == -1) {
        // File couldn't close somehow
        if (red_errno == RED_EIO || red_errno == RED_EUSERS) {
            // RED_EBADF means the file wasn't open
            status = FT_FAIL;
        }
    }

    err = red_close(fileToReceive);
    if (err == -1) {
        // File couldn't close somehow
        if (red_errno == RED_EIO || red_errno == RED_EUSERS) {
            // RED_EBADF means the file wasn't open
            status = FT_FAIL;
        }
    }

    return status;
}

FT_2U_payload_return FT_2U_payload_sendDataBytes(filePacket * outgoingPkt) {
    FT_2U_payload_return status;

    // Read from file and place data bytes into a packet
    int bytesRead = red_read(fileToSend, &(outgoingPkt->byte[0]), MAX_BYTES_TO_READ);
    if (bytesRead == -1) {
        // If reading data fails, abort the file transfer
        red_close(fileToSend);
        status = FT_FAIL;
    } else if (bytesRead < MAX_BYTES_TO_READ) {
        // If less than the MAX_BYTES_TO_READ are sent, there are no more bytes to send after
        outgoingPkt->bytesToRead = bytesRead;
        status = FT_2U_payload_stopFileTransfer();
    } else {
        // Else, exactly MAX_BYTES_TO_READ are sent, there are still more bytes to be sent after
        outgoingPkt->bytesToRead = bytesRead;
        status = FT_SUCCESS;
    }

    return status;
}

FT_2U_payload_return FT_2U_payload_receiveDataBytes(filePacket * incomingPkt) {
    FT_2U_payload_return status;

    // Get a packet and write data bytes to a file
    int totBytesInPkt = incomingPkt->bytesToRead;
    int err = red_write(fileToReceive, &(incomingPkt->byte[0]), totBytesInPkt);
    if (err == -1) {
        // If writing data fails, abort the file transfer
        red_close(fileToReceive);
        status = FT_FAIL;
    } else if (totBytesInPkt < MAX_BYTES_TO_READ) {
        // If less than the MAX_BYTES_TO_READ are in the packet, there are no more bytes to receive
        status = FT_2U_payload_stopFileTransfer();
    } else {
        // Else, exactly MAX_BYTES_TO_READ are received, there are still more bytes to receive
        status = FT_SUCCESS;
    }

    return status;
}
