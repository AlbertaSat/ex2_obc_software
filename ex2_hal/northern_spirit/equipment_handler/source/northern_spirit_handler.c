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
 * @file northern_spirit_handler.c
 * @author Thomas Ganley
 * @date 2021-10-22
 */

#include "northern_spirit_handler.h"
#include "northern_spirit_io.h"

// Functions fulfilling functionality common to AuroraSat and YukonSat

NS_return NS_get_heartbeat(uint8_t* heartbeat){
    uint8_t command[3] = {'h', 'h', 'h'};
    uint8_t answer[ARI_HEARTBEAT_ANS_LEN] = {0};

    send_NS_command(command, 3, answer, ARI_HEARTBEAT_ANS_LEN);

    *heartbeat = answer[0];

    for(int i = 0; i < ARI_HEARTBEAT_ANS_LEN; i++){
        printf("Heartbeat answer[%x]: %x\n", i, answer[i]);
    }
    return NS_OK;
}

NS_return NS_get_software_version(uint8_t* version){
    uint8_t command[3] = {'v', 'v', 'v'};
    uint8_t answer[ARI_SWVERSION_ANS_LEN] = {0};

    send_NS_command(command, sizeof(command), answer, sizeof(answer));

    memcpy(version, answer, ARI_SWVERSION_ANS_LEN);

    for(int i = 0; i < ARI_SWVERSION_ANS_LEN; i++){
        printf("Version answer[%x]: %x\n", i, answer[i]);
    }
    return NS_OK;
}

NS_return NS_get_telemetry(uint8_t* telemetry){
    uint8_t command[3] = {'t', 't', 't'};
    uint8_t answer[ARI_TELEMETRY_ANS_LEN] = {0};

    send_NS_command(command, sizeof(command), answer, sizeof(answer));

    memcpy(telemetry, answer, ARI_TELEMETRY_ANS_LEN);

    for(int i = 0; i < ARI_TELEMETRY_ANS_LEN; i++){
        printf("Telemetry answer[%x]: %x\n", i, answer[i]);
    }
    return NS_OK;
}

#ifdef IS_YUKONSAT
// Functions fulfilling functionality specific to YukonSat

#endif

#ifdef IS_AURORASAT
// Functions fulfilling functionality specific to AuroraSat


#endif
