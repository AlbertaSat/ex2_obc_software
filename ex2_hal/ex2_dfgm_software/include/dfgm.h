/*
 * Copyright (C) 2015  University of Alberta
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
 * @file dfgm.h
 * @author Andrew Williams
 * @date 2020-06-18
 */

#ifndef DFGM_H
#define DFGM_H

#include <stdint.h>

#include <FreeRTOS.h>
//#include "semphr.h"

typedef xSemaphoreHandle mutex_t;

#define DFGM_PACKET_SIZE 1248
#define DFGM_FS 100

#define DFGM_QUEUE_SIZE 2*4096

typedef enum
{
    RAW_ENABLE = 0,         /* Raw data selected. */
    FILTER_ENABLE = 1,      /* Filetered data selected. */
    CONFIG_TIMEOUT          /* Timeout determining selected stream. */
} dfgm_config_status_e;

struct dfgm_config_t
{
    dfgm_config_status_e stream;
    // mutex_t lock;
};

/**
 * @struct dfgm_t
 * @brief
 *      Abstract class defining interface to the DFGM.
 * @details
 *      Abstract class defining interface to the DFGM.
 */
typedef struct dfgm_t dfgm_t;

struct dfgm_t
{
    // bool_t (*power)( dfgm_t*, eps_t*, bool_t );
    // bool_t (*deploy)( dfgm_t*, hub_t* );
    struct dfgm_config_t dfgm_config;
};

struct __attribute__((packed)) dfgm_packet_t{
    uint8_t pid;
    uint8_t packet_type;
    uint8_t packet_length;
    uint16_t fs;
    uint32_t pps_offset;
    int16_t hk[12];
    int32_t x[DFGM_FS];
    int32_t y[DFGM_FS];
    int32_t z[DFGM_FS];
    uint16_t board_id;
    uint16_t sensor_id;
    uint16_t crc;

};

struct __attribute__((packed)) dfgm_1Hz_file_t{
    uint32_t timestamp;
    uint8_t pid;
    uint32_t pps_offset;
    int32_t x;
    int32_t y;
    int32_t z;
    int16_t hk[12];
};

int testFunction();

#endif /* DFGM_H_ */
