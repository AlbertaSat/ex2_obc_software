#ifndef NMEA_TYPES_H
#define NMEA_TYPES_H

#include <stdint.h>
#include "FreeRTOS.h"

typedef struct GPGGA_s {
    uint32_t   _time; // UTC time in hundredths of a second. as hhmmssss
    int32_t    _latitude_lower, _latitude_upper; // latitude in ten millionths of a degree
    int32_t    _longitude_lower, _longitude_upper; // longitude in ten millionths of a degree
    int8_t    _fixquality;
    uint8_t    _numsats; // number of satellites used for fix
    uint16_t   _hdop; // horizontal dilution of position (scaled by 100, i.e. 120 corresponds to a dop of 1.2)
    int32_t    _altitude; // altitude in centimeters
    TickType_t _logtime; // relative time packet was received
} GPGGA_s;

typedef struct GPGSA_s {
    uint8_t    _fixtype;
    uint16_t   _hdop; // horizontal dilution of position (scaled by 100, i.e. 120 corresponds to a dop of 1.2)
    uint16_t   _pdop; // positional dop (same scale as hdop)
    uint16_t   _vdop; // vertical dop (same scale as hdop)
    TickType_t _logtime; // relative time packet was received
} GPGSA_s;

typedef struct GPGSV_s {
    uint8_t    _numsats_visible; // number of satellites visible to gps
    uint8_t    _gsv_sentence, _gsv_sentences; // counter and total for gsv messages
    uint8_t    _snr_count; // count of satellites with Signal to Noise Ratio
    uint32_t   _snr_total, _new_snr_total; // sum of Signal to Noise Ratios (C/No, in dB) from all satellites
    uint32_t   _snr_avg; // average snr of gsv message (over all sentences) scaled by 100 (i.e. 4500 corresponds to an average SNR of 45)
    TickType_t _logtime; // relative time packet was received
} GPGSV_s;

typedef struct GPRMC_s {
    uint32_t   _time; // UTC time in hundredths of a second
    int32_t    _latitude_lower, _latitude_upper; // latitude in ten millionths of a degree
    int32_t    _longitude_lower, _longitude_upper; // longitude in ten millionths of a degree
    uint32_t   _speed; // speed in hundredths of kph
    uint32_t   _course; // course in hundredths of a degree
    uint32_t   _date; // UTC date
    TickType_t _logtime; // relative time packet was received
} GPRMC_s;

#endif //NMEA_TYPES_H
