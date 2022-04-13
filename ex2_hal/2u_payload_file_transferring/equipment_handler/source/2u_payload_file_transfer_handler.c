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

payload_FT_return 2U_PAYLOAD_getFile() {
    payload_FT_return status;
    // Open file
}

payload_FT_return 2U_PAYLOAD_putFile() {
    payload_FT_return status;
    // open file
}

payload_FT_return 2U_PAYLOAD_stopFileTransfer() {
    payload_FT_return status;
    // forcibly close all files
}

payload_FT_return 2U_PAYLOAD_sendDataBytes() {
    payload_FT_return status;
    // read from file and place data bytes into a packet

    // only close the file if < N bytes are read from the file
}

payload_FT_return 2U_PAYLOAD_receiveDataBytes() {
    payload_FT_return status;
    // get a packet and write data bytes to a file

    // only close the file if < N bytes are read from the file
}
