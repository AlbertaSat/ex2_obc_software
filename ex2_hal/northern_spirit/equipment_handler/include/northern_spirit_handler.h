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
 * @file northern_spirit_handler.h
 * @author Thomas Ganley
 * @date 2021-10-22
 */

#ifndef NORTHERN_SPIRIT_HANDLER
#define NORTHERN_SPIRIT_HANDLER

#include <stdint.h>

// Lengths of commands being sent
#define NS_STANDARD_CMD_LEN 3

// Lengths of expected answers
#define NS_STANDARD_ANS_LEN 2
#define NS_HEARTBEAT_ANS_LEN 2
#define NS_SWVERSION_ANS_LEN 11
#define NS_TELEMETRY_DATA_LEN 74
#define NS_CAPTURE_IMAGE_ANS_LEN 1

#define NS_TELEMETRY_COLLECTION_DELAY pdMS_TO_TICKS(30000)
#define NS_COMMAND_MUTEX_TIMEOUT pdMS_TO_TICKS(2000)

typedef enum{
    NS_OK,
    NS_FAIL,
    NS_BAD_CMD,
    NS_BAD_ANS,
    NS_HANDLER_BUSY,
    NS_UART_FAIL,
    NS_UART_BUSY,
    NS_MALLOC_FAIL,
}NS_return;

NS_return NS_capture_image(void);
NS_return NS_get_heartbeat(uint8_t* heartbeat);
NS_return NS_get_software_version(uint8_t* version);
NS_return NS_get_telemetry(uint8_t* telemetry);

#endif // NORTHERN_SPIRIT_HANDLER
