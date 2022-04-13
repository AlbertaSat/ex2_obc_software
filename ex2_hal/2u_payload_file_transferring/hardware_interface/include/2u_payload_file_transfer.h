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
 * @file 2u_payload_file_transfer.h
 * @author Daniel Sacro
 * @date 2022-04-12
 */

#ifndef 2U_PAYLOAD_FT_H
#define 2U_PAYLOAD_FT_H

#include "2U_payload_file_transfer_handler.h"

payload_FT_return HAL_2U_PAYLOAD_getFile(); // argument should be a file name
payload_FT_return HAL_2U_PAYLOAD_putFile(); // argument should be filename
payload_FT_return HAL_2U_PAYLOAD_stopFileTransfer();
payload_FT_return HAL_2U_PAYLOAD_sendDataBytes(); // argument should be a struct containing data bytes
payload_FT_return HAL_2U_PAYLOAD_receiveDataBytes(); // argument should be a struct containing data bytes


#endif /* 2U_PAYLOAD_FT_H */
