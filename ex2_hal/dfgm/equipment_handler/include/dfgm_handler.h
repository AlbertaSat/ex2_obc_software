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
 * @file
 * @author
 * @date
 */


#ifndef DFGM_HANDLER_H
#define DFGM_HANDLER_H

// Headers
#include "system.h"
#include <stdint.h>
#include <time.h>

typedef enum {
    DFGM_SUCCESS = 0,

    DFGM_BAD_PARAM = 1,
    DFGM_BUSY = 2,

    IS_STUBBED_DFGM = 0
} DFGM_return;

// Macros for conversions from converter and filter header and source files (QUEUE_DEPTH, etc.)
// Conversion constants for mag data (Have to be calibrated for each sensor/acquisition system)
#define XDACScale 1.757421875
#define XADCScale -0.0353
#define XOffset 0
#define YDACScale 2.031835938
#define YADCScale -0.0267
#define YOffset 0
#define ZDACScale 1.934375
#define ZADCScale -0.0302
#define ZOffset 0

// Conversion constants for HK data (Might have to rearrange for other boards)
#define HK0Scale (2.5/4096.0 * 1000.0) // for core voltage (in mV)
#define HK0Offset 0
#define HK1Scale (2.5*1000000.0/(4096.0*5000.0)) // for sensor temperature
#define HK1Offset -273.15
#define HK2Scale 0.0313 // for reference temperature
#define HK2Offset -20.51
#define HK3Scale 0.0313 // for board temperature
#define HK3Offset -20.51
#define HK4Scale (5.0*2.5/4096.0 * 1000.0) // for positive rail voltage (in mV)
#define HK4Offset 0
#define HK5Scale (5.0*2.5/4096.0 * 1000.0) // for input voltage (in mV)
#define HK5Offset 0
#define HK6Scale (2.5/4096.0 * 1000.0) // for reference voltage (in mV)
#define HK6Offset 0
#define HK7Scale 0.107 // for input current
#define HK7Offset 0
#define HK8Scale 1 // for RESERVED
#define HK8Offset 0
#define HK9Scale 1 // for RESERVED
#define HK9Offset 0
#define HK10Scale 1 // for RESERVED
#define HK10Offset 0
#define HK11Scale 1 // for RESERVED
#define HK11Offset 0

// For data collection task & HK
#define minRuntime 1 // in seconds; 2 seconds so that it can be filtered
#define timeThreshold 180 // in seconds
#define QUEUE_DEPTH 2500

// Structs from converter and filter - dfgm_data_tuple_t, dfgm_packet_t, dfgm_data_t, HK data struct etc.
// SECOND + dfgm_1Hz_file_t combo? should include timestamps, 100Hz data, and filtered data
typedef struct __attribute__((packed)) {
    uint32_t X; // [xdac, xadc]
    uint32_t Y; // [ydac, yadc]
    uint32_t Z; // [zdac, zadc]
} dfgm_data_tuple_t;

typedef struct __attribute__((__packed__)) {
    uint8  dle;
    uint8  stx;
    uint8  pid;
    uint8  packet_type;
    uint16 packet_length;
    uint16 fs;
    uint32 pps_offset;
    uint16 hk[12];
    dfgm_data_tuple_t tup[100];
    uint16 board_id;
    uint16 sensor_id;
    uint8  reservedA;
    uint8  reservedB;
    uint8  reservedC;
    uint8  reservedD;
    uint8  reservedE;
    uint8 etx;
    uint16 crc;
} dfgm_packet_t;

typedef struct __attribute__((packed)) {
    time_t time;
    dfgm_packet_t pkt;
} dfgm_data_t;

typedef struct {
    time_t timestamp;
    float coreVoltage;
    float sensorTemp;
    float refTemp;
    float boardTemp;
    float posRailVoltage;
    float inputVoltage;
    float refVoltage;
    float inputCurrent;
    float reserved1;
    float reserved2;
    float reserved3;
    float reserved4;
} dfgm_housekeeping;

// Rename this to a more intuitive name
struct SECOND {
    time_t time;
    double X[100];
    double Y[100];
    double Z[100];
    char flag;
    double Xfilt;
    double Yfilt;
    double Zfilt;
};

// Used for reading and writing data files
typedef struct __attribute__((packed)) {
    time_t time;
    uint32_t X;
    uint32_t Y;
    uint32_t Z;
} dfgm_data_sample_t;

// Functions used in converter
void dfgm_convert_mag(dfgm_packet_t *const data);
void dfgm_convert_HK(dfgm_packet_t *const data);
void save_packet(dfgm_data_t *data, char *filename);

//void clear_file(char* filename); // Will probably need modifications
void dfgm_init();

// Functions used in filters
void shift_sptr(void);
void apply_filter(void);
void save_second(struct SECOND *second, char * filename); // rename "second"
void convert_100Hz_to_1Hz(char *filename100Hz, char *filename1Hz);
//void convert_100Hz_to_10Hz(char *filename100Hz, char *filename10Hz, dfgm_filter_settings *filterSettings);
void update_1HzFile(void);
//void update_10HzFile(void);

// Functions called in hardware interface
DFGM_return DFGM_startDataCollection(int givenRuntime);
DFGM_return DFGM_stopDataCollection();
DFGM_return DFGM_getHK(dfgm_housekeeping *hk);

#endif /* DFGM_HANDLER_H */
