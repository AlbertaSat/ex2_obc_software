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
 * @file dfgm_handler.h
 * @author Daniel Sacro, Robert Taylor, Andrew Rooney
 * @date 2022-02-08
 */

#ifndef DFGM_HANDLER_H
#define DFGM_HANDLER_H

#include "system.h"
#include "logger.h"
#include <stdint.h>
#include <time.h>

typedef enum {
    DFGM_SUCCESS = 0,
    DFGM_BAD_PARAM = 1,
    DFGM_BUSY = 2,
    DFGM_HK_FAIL = 3,

    IS_STUBBED_DFGM = 0
} DFGM_return;

#if (IS_EXALTA2 == 1) /* DFGM B2 */ | (IS_FLATSAT == 1) | (IS_SATELLITE == 0)
// Conversion macros for mag data (have to be calibrated for each sensor/acquisition system)
#define X_DAC_SCALE 4.19948 // DAC stands for Digital to Analog Converter
#define Y_DAC_SCALE 4.22865
#define Z_DAC_SCALE 4.05755

#define X_ADC_SCALE 0.15112 // ADC stands for Analog to Digital Converter
#define Y_ADC_SCALE 0.1823565
#define Z_ADC_SCALE 0.2841758

#define X_OFFSET 512.5
#define Y_OFFSET 46.5
#define Z_OFFSET -554

#elif IS_AURORASAT == 1      // DFGM D1
// Conversion macros for mag data (have to be calibrated for each sensor/acquisition system)
#define X_DAC_SCALE 4.215625 // DAC stands for Digital to Analog Converter
#define Y_DAC_SCALE 4.236979
#define Z_DAC_SCALE 4.0640625

#define X_ADC_SCALE 0.1606543 // ADC stands for Analog to Digital Converter
#define Y_ADC_SCALE 0.1913664
#define Z_ADC_SCALE 0.2860879

#define X_OFFSET -142
#define Y_OFFSET 26
#define Z_OFFSET -543.5

#elif IS_YUKONSAT == 1      // DFGM E3
// Conversion macros for mag data (have to be calibrated for each sensor/acquisition system)
#define X_DAC_SCALE 4.14375 // DAC stands for Digital to Analog Converter
#define Y_DAC_SCALE 3.82083
#define Z_DAC_SCALE 4.02214

#define X_ADC_SCALE 0.1612974 // ADC stands for Analog to Digital Converter
#define Y_ADC_SCALE 0.2719562
#define Z_ADC_SCALE 0.25733607

#define X_OFFSET -67.5
#define Y_OFFSET 58.5
#define Z_OFFSET -566.5

#endif

// Conversion macros for housekeeping (HK) data
#define HK_SCALE_0 (2.5 / 4096.0 * 1000.0) // for core voltage (in mV)
#define HK_OFFSET_0 0
#define HK_SCALE_1 (2.5 * 1000000.0 / (4096.0 * 5000.0)) // for sensor temperature (in deg C)
#define HK_OFFSET_1 -273.15
#define HK_SCALE_2 0.0313 // for reference temperature (in deg C)
#define HK_OFFSET_2 -20.51
#define HK_SCALE_3 0.0313 // for board temperature (in deg C)
#define HK_OFFSET_3 -20.51
#define HK_SCALE_4 (5.0 * 2.5 / 4096.0 * 1000.0) // for positive rail voltage (in mV)
#define HK_OFFSET_4 0
#define HK_SCALE_5 (5.0 * 2.5 / 4096.0 * 1000.0) // for input voltage (in mV)
#define HK_OFFSET_5 0
#define HK_SCALE_6 (2.5 / 4096.0 * 1000.0) // for reference voltage (in mV)
#define HK_OFFSET_6 0
#define HK_SCALE_7 0.107 // for input current (in mA)
#define HK_OFFSET_7 0
#define HK_SCALE_8 1 // for RESERVED 1
#define HK_OFFSET_8 0
#define HK_SCALE_9 1 // for RESERVED 2
#define HK_OFFSET_9 0
#define HK_SCALE_10 1 // for RESERVED 3
#define HK_OFFSET_10 0
#define HK_SCALE_11 1 // for RESERVED 4
#define HK_OFFSET_11 0

// Misc. macros
#define DFGM_MIN_RUNTIME 1     // in seconds
#define DFGM_TIME_THRESHOLD 20 // in seconds
#define DFGM_QUEUE_DEPTH 1248
#define DFGM_FILE_NAME_MAX_SIZE 25
#define DFGM_RX_TASK_SIZE 500
#define DFGM_HK_COLLECTION_MAX_RUNTIME 3 * ONE_SECOND // in ticks

typedef struct __attribute__((packed)) {
    uint32_t x; // [xdac, xadc]
    uint32_t y; // [ydac, yadc]
    uint32_t z; // [zdac, zadc]
} dfgm_data_tuple_t;

typedef struct __attribute__((__packed__)) {
    uint8 DLE;
    uint8 STX;
    uint8 PID;
    uint8 packetType;
    uint16 packetLength;
    uint16 FS;
    uint32 PPS_offset;
    uint16 HK[12];
    dfgm_data_tuple_t tuple[100];
    uint16 board_ID;
    uint16 sensor_ID;
    uint8 reservedA;
    uint8 reservedB;
    uint8 reservedC;
    uint8 reservedD;
    uint8 reservedE;
    uint8 ETX;
    uint16 CRC;
} dfgm_packet_t;

typedef struct __attribute__((packed)) {
    time_t time;
    dfgm_packet_t packet;
} dfgm_data_t;

typedef struct {
    time_t time;
    uint16 coreVoltage;
    uint16 sensorTemp;
    uint16 refTemp;
    uint16 boardTemp;
    uint16 posRailVoltage;
    uint16 inputVoltage;
    uint16 refVoltage;
    uint16 inputCurrent;
    uint16 reserved1;
    uint16 reserved2;
    uint16 reserved3;
    uint16 reserved4;
} dfgm_housekeeping;

struct dfgm_second {
    time_t time;
    double x[100];
    double y[100];
    double z[100];
    double xFiltered;
    double yFiltered;
    double zFiltered;
};

// Used for reading and writing files containing DFGM data
typedef struct __attribute__((packed)) {
    time_t time;
    float x;
    float y;
    float z;
} dfgm_data_sample_t;

void DFGM_init();

// Functions called in hardware interface
DFGM_return DFGM_startDataCollection(int givenRuntime);
DFGM_return DFGM_stopDataCollection();
DFGM_return DFGM_get_HK(dfgm_housekeeping *hk);

#endif /* DFGM_HANDLER_H */
