/*
 * Copyright (C) 2020  University of Alberta
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

#ifndef SKYTRAQ_BINARY_TYPES_H
#define SKYTRAQ_BINARY_TYPES_H

//TODO: create enum for messages with sub ID

typedef enum SYSTEM_INPUT_IDs {
  SYSTEM_RESTART                           = 0x01,    // perform a reset
  QUERY_SOFTWARE_VERSION                   = 0x02,    // reset pointer to log buffer
  QUERY_SOFTWARE_CRC                       = 0x03,
  SET_FACTORY_DEFAULTS                     = 0x04,
  CONFIGURE_SERIAL_PORT                    = 0x05,
  CONFIGURE_NMEA                           = 0x08,
  CONFIGURE_MESSAGE_TYPE                   = 0x09,
  UPLOAD_SOFTWARE_IMAGE                    = 0x0B,
  CONFIGURE_POWER_MODE                     = 0x0C,
  CONFIGURE_UPDATE_RATE                    = 0x0E,
  QUERY_UPDATE_RATE                        = 0x10,
  CONFIGURE_NAV_MESSAGE_INTERVAL           = 0x11,
  QUERY_POWER_MODE                         = 0x15

} SYSTEM_INPUT_IDs;

typedef enum GNSS_INPUT_IDs {
  CONFIGURE_DATUM                          = 0x29,
  CONFIGURE_DOP_MASK                       = 0x2A,
  CONFIGURE_ELEVATION_CNR_MASK             = 0x2B,
  QUERY_DATUM                              = 0x2D,
  QUERY_DOP_MASK                           = 0x2E,
  QUERY_ELEVATION_CNR_MASK                 = 0x2F,
  GET_GPS_EPHEMERIS                        = 0x30,
  CONFIGURE_POSITION_PINNING               = 0x39,
  QUERY_POSITION_PINNING                   = 0x3A,
  CONFIGURE_POSITION_PINNING_PARAMETERS    = 0x3B,
  SET_GPS_EPHEMERIS                        = 0x41,
  QUERY_1PPS_TIMING                        = 0x44,
  CONFIGURE_1PPS_CABLE_DELAY               = 0X45,
  QUERY_1PPS_CABLE_DELAY                   = 0x46,
  CONFIGURE_NMEA_TALKER_ID                 = 0x4B,
  QUERY_NMEA_TALKER_ID                     = 0x4F,
  GET_GPS_ALMANAC                          = 0x50,
  SET_GPS_ALMANAC                          = 0x51,
  CONFIGURE_1PPS_TIMING                    = 0x54,
  GET_GLONASS_EPHEMERIS                    = 0x5B,
  SET_GLONASS_EPHEMERIS                    = 0x5C,
  GET_GLONASS_ALMANAC                      = 0x5D,
  SET_GLONASS_ALMANAC                      = 0x5E,
  GET_GLONASS_TIME_CORRECT_                = 0x5F,
  SET_GLONASS_TIME_CORRECT                 = 0x60,

} GNSS_INPUT_IDs;

typedef enum SYSTEM_OUTPUT_IDS {
  SOFTWARE_VERSION                         = 0x80,
  SOFTWARE_CRC                             = 0x81,
  ACK                                      = 0x83,
  NACK                                     = 0x84,
  POSITION_UPDATE_RATE                     = 0x86,
  GLONASS_EPHEMERIS                        = 0x90,
  GLONASS_ALMANAC                          = 0x91,
  GLONASS_TIME_CORRECT                     = 0x92,
  GNSS_NMEA_TALKER_ID                      = 0x93,

} SYSTEM_OUTPUT_IDS;

typedef enum GNSS_OUTPUT_IDS {
  NAVIGATION_DATA_MESSAGE                  = 0xA8,
  GNSS_DATUM                               = 0xAE,
  GNSS_DOP_MASK                            = 0xAF,
  ELEVATION_CNR_MASK                       = 0XB0,
  GPS_EPHEMERIS_DATA                       = 0xB1,
  GNSS_POSITION_PINNING_STATUS             = 0XB4,
  GNSS_POWER_MODE_STATUS                   = 0XB9,
  GNSS_1PPS_CABLE_DELAY                    = 0XBB,
  GPS_ALMANAC_DATA                         = 0XBE,
  GNSS_1PPS_TIMING                         = 0XC2

} GNSS_OUTPUT_IDS;

typedef enum GNSS_INPUT_MESSAGE_SUBIDs {
  // the byte order of these may have to be swapped.
  // Not really sure yet. 
  CONFIGURE_SBAS                           = 0x6201,
  QUERY_SBAS_STATUS                        = 0x6202,
  CONFIGURE_QZSS                           = 0x6203,
  QUERY_QZSS_STATUS                        = 0x6204,
  CONFIGURE_SAEE                           = 0x6301,
  QUERY_SAEE_STATUS                        = 0x6302,
  QUERY_BOOT_STATUS                        = 0x6401,
  CONFIGURE_EXTENDED_NMEA_MESSAGE_INTERVAL = 0x6406,
  QUERY_EXTENDED_NMEA_MESSAGE_INTERVAL     = 0x6407,
  CONFIGURE_INTERFERENCE_DETECTION         = 0x6406,
  QUERY_INTERFERENCE_DETECTION             = 0x6407,
  CONFIGURE_GPS_SEARCH_NUMBER              = 0x640A,
  QUERY_GPS_SEARCH_NUMBER                  = 0x640B,
  CONFIGURE_POSITION_FIX_MASK              = 0x6411,
  QUERY_POSITION_FIX_MASK                  = 0x6412,
  CONFIGURE_UTC_REFERENCE                  = 0x6415,
  QUERY_UTC_REFERENCE                      = 0x6416,
  CONFIGURE_GNSS_NAV_MODE                  = 0x6417,
  QUERY_GNSS_NAV_MODE                      = 0x6418,
  CONFIGURE_GNSS_CONSTELLATION_TYPE        = 0x6419,
  QUERY_GNSS_CONSTELLATION_TYPE            = 0x641A,
  CONFIGURE_LEAP_SECONDS                   = 0x641F,
  QUERY_GPS_TIME                           = 0x6420,
  CONFIGURE_GNSS_DATUM_INDEX               = 0x6427,
  QUERY_GNSS_DATUM_INDEX                   = 0x6428,
  CONFIGURE_GNSS_GEOFENCE_DATA             = 0x642F,
  QUERY_GNSS_GEOFENCE_DATA                 = 0x6430,
  QUERY_GNSS_GEOGENCE_RESULT               = 0x6431,
  CONFIGURE_1PPS_WIDTH                     = 0x6501,
  QUERY_1PPS_WIDTH                         = 0x6502,
  CONFIGURE_1PPS_FREQ                      = 0x6503,
  QUERY_1PPS_FREQ                          = 0x6504,
  SET_BEIDOU_EPHEMERIS                     = 0x6701,
  GET_BEIDOU_EPHEMERIS                     = 0x6702,
  SET_BEIDOU_ALMANAC                       = 0x6703,
  GET_BEIDOU_ALMANAC                       = 0x6704,
  CONFIGURE_RTK_MODE                       = 0x6A01,
  QUERY_RTK_MODE                           = 0x6A02,

} GNSS_INPUT_MESSAGE_SUBIDs;

typedef enum GNSS_OUTPUT_MESSAGE_SUBIDS {
  // the byte order of these may have to be swapped.
  // Not really sure yet. 
  SBAS_STATUS                              = 0x6280,
  QZSS_STATUS                              = 0x6281,
  SAEE_STATUS                              = 0x6380,
  GNSS_BOOT_STATUS                         = 0x6480,
  EXTENDED_NMEA_MESSAGE_INTERVAL           = 0x6481,
  INTERFERENCE_DETECTION_STATUS            = 0x6483,
  GPS_SEARCH_ENGINE_NUMBER                 = 0x6485,
  POSITION_FIX_MASK                        = 0x6488,
  GNSS_NAVIATION_MODE                      = 0x648B,
  GNSS_CONSTELLATION_TYPE                  = 0x648C,
  GPS_TIME                                 = 0x648E,
  GNSS_DATUM_INDEX                         = 0x6492,
  GNSS_GEOFENCE_DATA                       = 0x6496,
  GNSS_GEOFENCE_RESULT                     = 0x6497,
  GNSS_1PPS_PULSE_WIDTH                    = 0x6502,
  GNSS_1PPS_FREQUENCY                      = 0x6581,
  BEIDOU_EPHEMERIS_DATA                    = 0x6780,
  BEIDOU_ALMANAC_DATA                      = 0x6781,
  RTK_MODE                                 = 0x6A80,

} GNSS_OUTPUT_MESSAGE_SUBIDS;

//! (0x01) System Restart
typedef enum StartMode {
    NO_MODE_CHANGE = 0x00,
    HOT_START = 0x01,
    WARM_START = 0x02,
    COLD_START = 0x03,
    TEST_MODE = 0x04,
} StartMode;

typedef enum skytraq_baud_rate {
  r_4800 = 0,
  r_9600 = 1,
  r_19200 = 2,
  r_38400 = 3,
  r_57600 = 4,
  r_115200 = 5,
  r_230400 = 6,
  r_460800 = 7,
  r_921600 = 8,
} skytraq_baud_rate;

typedef enum skytraq_update_attributes {
  UPDATE_TO_SRAM = 0,
  UPDATE_TO_FLASH = 1,
  // TODO: figure out what this means
  UPDATE_TEMPORARILY = 2,
} skytraq_update_attributes;

typedef enum skytraq_message_type {
  NO_OUTPUT = 0,
  NMEA_MESSAGE = 1,
  BINARY_MESSAGE = 2,
} skytraq_message_type;

typedef enum skytraq_power_mode {
  NORMAL,
  POWERSAVE,
} skytraq_power_mode;

typedef enum nmea_talker_IDs {
  GP = 0,
  GN = 1,
} nmea_talker_IDs;

typedef enum enable_disable {
  DISABLE,
  ENABLE
} enable_disable;

typedef enum GPS_RETURNSTATE {
    SUCCESS,
    RESOURCE_BUSY,
    MESSAGE_INVALID,
    UNKNOWN_ERROR,
    INVALID_CHECKSUM,
    INVALID_CHECKSUM_RECEIVE
} GPS_RETURNSTATE;

#endif /* SKYTRAQ_BINARY_TYPES_H */
