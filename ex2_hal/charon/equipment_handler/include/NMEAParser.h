/*
    NMEAParser.h - Library for parsing NMEA strings from a GPS
    Decodes GGA, GSA, GSV, and RMC strings
    Reed A. Foster, July 2017.
    Robert N. Taylor, December 2020
*/

#ifndef NMEAParser_h
#define NMEAParser_h

#include <stdbool.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "NMEA_types.h"
#include "os_queue.h"
#include "os_semphr.h"

#define NMEA_GGA 0
#define NMEA_GSA 1
#define NMEA_GSV 2
#define NMEA_GLL 3
#define NMEA_RMC 4
#define NMEA_VTG 5
#define NMEA_ZDA 6
#define NMEA_UNKNOWN 7

#define NMEASENTENCE_MAXLENGTH 120
#define NMEASENTENCE_MAXTERMS 25

// Set sentences invalid after 10 seconds
#define GPS_AGE_INVALID_THRESHOLD 10000 * portTICK_RATE_MS

enum {
    GPS_INVALID_DOP = 0xFFFF,
    GPS_INVALID_ANGLE = 999999999,
    GPS_INVALID_ALTITUDE = 999999999,
    GPS_INVALID_DATE = 0,
    GPS_INVALID_TIME = 0xFFFFFFFF,
    GPS_INVALID_SPEED = 999999999,
    GPS_INVALID_FIX_TIME = 0xFFFFFFFF,
    GPS_INVALID_SATELLITES = 0xFF,
    GPS_INVALID_AGE = 0xFFFFFFFF,
    GPS_INVALID_SNR = 0xFFFFFFFF,
    GPS_INVALID_COURSE = 0xFFFF
};

#define NMEA_QUEUE_ITEM_SIZE NMEASENTENCE_MAXLENGTH
#define NMEA_QUEUE_MAX_LEN 2

extern QueueHandle_t NMEA_queue;

extern SemaphoreHandle_t NMEA_mutex;

bool init_NMEA();

static GPGGA_s GPGGA;
static GPGSA_s GPGSA;
static GPGSV_s GPGSV;
static RMC_s RMC;

bool NMEAParser_get_GPGGA(GPGGA_s *output);
bool NMEAParser_get_GPGSA(GPGSA_s *output);
bool NMEAParser_get_GPGSV(GPGSV_s *output);
bool NMEAParser_get_RMC(RMC_s *output);

bool NMEAParser_encode(char c);
void NMEAParser_clear_GPGGA(void);      // set GPGGA storage to invalid
void NMEAParser_clear_GPGSA(void);      // set GPGSA storage to invalid
void NMEAParser_clear_GPGSV(void);      // set GPGSV storage to invalid
void NMEAParser_clear_RMC(void);      // set RMC storage to invalid
void NMEAParser_reset_all_values(void); // set all storage to invalid

#endif
