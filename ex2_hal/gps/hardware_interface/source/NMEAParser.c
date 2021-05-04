/*
    NMEAParser.cpp - Library for parsing NMEA strings from a GPS
    Decodes GGA, GSA, GSV, and RMC strings
    Reed A. Foster, July 2017.
    Robert N. Taylor, December 2020
*/

#include "FreeRTOS.h"
#include "NMEAParser.h"
#include "os_task.h"
#include <string.h>

static int NMEAParser_termcmp(const char *str1, const char *str2);
static int NMEAParser_hexToInt(char hex);
static int32_t NMEAParser_parse_decimal(char *p);
static void NMEAParser_parse_degrees(char *p, int32_t *upper, int32_t *lower);
static bool NMEAParser_decode_sentence();

static char _sentence[NMEASENTENCE_MAXLENGTH];
static int  _char_offset;

const static GPGGA_s GPGGA_invalid = {._time = GPS_INVALID_TIME,
                                      ._latitude_lower = GPS_INVALID_ANGLE,
                                      ._latitude_upper = GPS_INVALID_ANGLE,
                                      ._longitude_lower = GPS_INVALID_ANGLE,
                                      ._longitude_upper = GPS_INVALID_ANGLE,
                                      ._fixquality = -1,
                                      ._numsats = GPS_INVALID_SATELLITES,
                                      ._hdop = GPS_INVALID_DOP,
                                      ._altitude = GPS_INVALID_ALTITUDE,
                                      ._logtime = (TickType_t)GPS_INVALID_FIX_TIME};

const static GPGSA_s GPGSA_invalid = {._fixtype = 0,
                                      ._hdop = GPS_INVALID_DOP,
                                      ._vdop = GPS_INVALID_DOP,
                                      ._pdop = GPS_INVALID_DOP,
                                      ._logtime = (TickType_t)GPS_INVALID_FIX_TIME};

const static GPGSV_s GPGSV_invalid = {._numsats_visible = GPS_INVALID_SATELLITES,
                                      ._gsv_sentence = 0,
                                      ._gsv_sentences = 0,
                                      ._snr_count = 0,
                                      ._snr_total = GPS_INVALID_SNR,
                                      ._new_snr_total = 0,
                                      ._snr_avg = GPS_INVALID_SNR,
                                      ._logtime = (TickType_t)GPS_INVALID_FIX_TIME};

const static GPRMC_s GPRMC_invalid = {._time = GPS_INVALID_TIME,
                                      ._latitude_lower = GPS_INVALID_ANGLE,
                                      ._latitude_upper = GPS_INVALID_ANGLE,
                                      ._longitude_lower = GPS_INVALID_ANGLE,
                                      ._longitude_upper = GPS_INVALID_ANGLE,
                                      ._speed = GPS_INVALID_SPEED,
                                      ._course = GPS_INVALID_COURSE,
                                      ._date = GPS_INVALID_DATE,
                                      ._logtime = (TickType_t)GPS_INVALID_FIX_TIME};

/**
 * @brief sets initial memory values for NMEA
 * 
 * @return true success
 * @return false failure
 */
bool init_NMEA() {
    NMEAParser_reset_all_values();
    NMEA_queue = xQueueCreate(NMEA_QUEUE_MAX_LEN, NMEA_QUEUE_ITEM_SIZE);
    if (NMEA_queue == NULL) {
        return false;
    }
    return true;
}

/**
 * @brief gets latest incoming GPGGA packet
 * 
 * @param output struct * to store incoming packet
 * @return true success
 * @return false failure
 */
bool NMEAParser_get_GPGGA(GPGGA_s *output) {
    TickType_t tickCount = xTaskGetTickCount();

    taskENTER_CRITICAL();
    if ((GPGGA._logtime != GPS_INVALID_FIX_TIME) && (tickCount - GPGGA._logtime < GPS_AGE_INVALID_THRESHOLD)) {
        memcpy(output,&GPGGA, sizeof(GPGGA_s));
        taskEXIT_CRITICAL();
        return true;
    }
    taskEXIT_CRITICAL();
    return false;

}
 /**
  * @brief get latest GPGSA packet
  * 
  * @param output struct * to store incoming packet
  * @return true success
  * @return false failure 
  */
bool NMEAParser_get_GPGSA(GPGSA_s *output) {
    TickType_t tickCount = xTaskGetTickCount();

    taskENTER_CRITICAL();
    if ((GPGSA._logtime != GPS_INVALID_FIX_TIME) && (tickCount - GPGSA._logtime < GPS_AGE_INVALID_THRESHOLD)) {
        memcpy(output,&GPGSA, sizeof(GPGSA_s));
        taskEXIT_CRITICAL();
        return true;

    }
    taskEXIT_CRITICAL();
    return false;
}

 /**
  * @brief get GPGSV packet
  * 
  * @param output struct * to store incoming packet
  * @return true success
  * @return false failure 
  */
bool NMEAParser_get_GPGSV(GPGSV_s *output) {
    TickType_t tickCount = xTaskGetTickCount();

    taskENTER_CRITICAL();
    if ((GPGSV._logtime != GPS_INVALID_FIX_TIME) && (tickCount - GPGSV._logtime < GPS_AGE_INVALID_THRESHOLD)) {
        memcpy(output,&GPGSV, sizeof(GPGSV_s));
        taskEXIT_CRITICAL();
        return true;
    }

    taskEXIT_CRITICAL();
    return false;
}

 /**
  * @brief get GPRMC packet
  * 
  * @param output struct * to store incoming packet
  * @return true success
  * @return false failure 
  */
bool NMEAParser_get_GPRMC(GPRMC_s *output) {
    TickType_t tickCount = xTaskGetTickCount();

    taskENTER_CRITICAL();
    if ((GPRMC._logtime != GPS_INVALID_FIX_TIME) && (tickCount - GPRMC._logtime < GPS_AGE_INVALID_THRESHOLD)) {
        memcpy(output,&GPRMC, sizeof(GPRMC_s));
        taskEXIT_CRITICAL();
        return true;
    }
    taskEXIT_CRITICAL();
    return false;
}

/**
 * @brief reset all packet storage to invalid values
 * 
 */
void NMEAParser_reset_all_values(void) {
    NMEAParser_clear_GPGSV();
    NMEAParser_clear_GPRMC();
    NMEAParser_clear_GPGGA();
    NMEAParser_clear_GPGSA();
}

/**
 * @brief reset GPGGA packet to invalid values
 * 
 */
void NMEAParser_clear_GPGGA(void) {
    taskENTER_CRITICAL();
    memcpy(&GPGGA, &GPGGA_invalid, sizeof(GPGGA_s));
    taskEXIT_CRITICAL();
}

/**
 * @brief reset GPGSA packet to invalid values
 * 
 */
void NMEAParser_clear_GPGSA(void) {
    taskENTER_CRITICAL();
    memcpy(&GPGSA, &GPGSA_invalid, sizeof(GPGSA_s));
    taskEXIT_CRITICAL();
}

/**
 * @brief reset GPGSV packet to invalid values
 * 
 */
void NMEAParser_clear_GPGSV(void) {
    taskENTER_CRITICAL();
    memcpy(&GPGSV, &GPGSV_invalid, sizeof(GPGSV_s));
    taskEXIT_CRITICAL();
}

/**
 * @brief reset GPRMC packet to invalid values
 * 
 */
void NMEAParser_clear_GPRMC(void) {
    taskENTER_CRITICAL();
    memcpy(&GPRMC, &GPRMC_invalid, sizeof(GPRMC_s));
    taskEXIT_CRITICAL();
}

/**
 * @brief string names for each packet type
 * 
 */
const char _GPGGA_TERM[7] = "$GPGGA";
const char _GPGLL_TERM[7] = "$GPGLL";
const char _GPGSA_TERM[7] = "$GPGSA";
const char _GPGSV_TERM[7] = "$GPGSV";
const char _GPRMC_TERM[7] = "$GPRMC";
const char _GPVTG_TERM[7] = "$GPVTG";
const char _GPZDA_TERM[7] = "$GPZDA";

/**
 * @brief takes NMEA string one char at a time until \n
 * 
 * @param c character to register with decoder
 * @return true data decoded
 * @return false data failure
 */
bool NMEAParser_encode(char c)
{
    bool new_data = false;
    if (c == '\n')
    {
        if (_sentence[_char_offset - 4] == '*') // checksum exists
        {
            int checksum = NMEAParser_hexToInt(_sentence[_char_offset - 2]) + NMEAParser_hexToInt(_sentence[_char_offset - 3]) * 16;
            _char_offset -= 5;
            while (_sentence[_char_offset] && _sentence[_char_offset] != '$')
                checksum ^= _sentence[_char_offset--];
            if (checksum == 0) // checksum is valid
            {
                taskENTER_CRITICAL();
                new_data = NMEAParser_decode_sentence();
                taskEXIT_CRITICAL();
            }
            _char_offset = 0;
            _sentence[0] = 0;
        }
    }
    else
        _sentence[_char_offset++] = c;
    return new_data;
}

/**
 * @brief decodes sentence contained in glocal _sentence
 * 
 * @return true sentence was valid
 * @return false sentence was invalid
 */
static bool NMEAParser_decode_sentence()
{
    TickType_t logtime = xTaskGetTickCount();

    // determine sentence type
    int sentence_type;
    char title[7];
    int i;
    for (i = 0; i < 6; i++)
        title[i] = _sentence[i];
    title[6] = 0;
    if (!NMEAParser_termcmp(title, _GPGGA_TERM))
        sentence_type = NMEA_GGA;
    else if (!NMEAParser_termcmp(title, _GPGSA_TERM))
        sentence_type = NMEA_GSA;
    else if (!NMEAParser_termcmp(title, _GPGSV_TERM))
        sentence_type = NMEA_GSV;
    else if (!NMEAParser_termcmp(title, _GPRMC_TERM))
        sentence_type = NMEA_RMC;

    // Add additional NMEA sentence type here

    else
        sentence_type = NMEA_UNKNOWN;

    if (sentence_type == NMEA_UNKNOWN)
        return false;

    char *p;
    p = _sentence;

    while (*p++ != ',') ; // skip to next term

    // verify validity of sentences (if possible)
    bool data_valid = false;
    switch (sentence_type)
    {
        int i;
        case NMEA_GGA:
            for (i = 0; i < 5; i++) // skip over sentence until start of 5th term
                while (*p++ != ',') ;
            data_valid = *p > '0';
            break;
        case NMEA_GSA:
            for (i = 0; i < 1; i++)
                while (*p++ != ',') ;
            data_valid = *p > '1';
            break;
        case NMEA_GSV:
            data_valid = true;
            break;
        case NMEA_RMC:
            for (i = 0; i < 1; i++)
                while (*p++ != ',') ;
            data_valid = *p == 'A';
            break;

        // Add validity checking for additional NMEA sentences here

    }
    p = _sentence;

    if (!data_valid)
        return false;

    // reset whichever sentence struct we're decoding
    switch(sentence_type) {
        case NMEA_GGA: NMEAParser_clear_GPGGA(); GPGGA._logtime = logtime; break;
        case NMEA_GSA: NMEAParser_clear_GPGSA(); GPGSA._logtime = logtime; break;
        case NMEA_GSV: NMEAParser_clear_GPGSV(); GPGSV._logtime = logtime; break;
        case NMEA_RMC: NMEAParser_clear_GPRMC(); GPRMC._logtime = logtime; break;
    }


    while (*p++ != ',') ; //advance to first term after sentence id/title

    // decode data
    int term_number = 0;
    bool decoded = false;
    while (!decoded)
    {
        switch (sentence_type)
        {
            case NMEA_GGA:
                switch (term_number)
                {
                    case 0: // UTC Time
                        //_last_time_fix = logtime;
                        GPGGA._time = NMEAParser_parse_decimal(p);
                        break;
                    case 1: // Latitude
                        //_last_position_fix = logtime;
                        NMEAParser_parse_degrees(p, &(GPGGA._latitude_upper), &(GPGGA._latitude_lower));
                        break;
                    case 2: // Latitude Indicator
                        GPGGA._latitude_upper = *p == 'S' ? -GPGGA._latitude_upper : GPGGA._latitude_upper;
                        break;
                    case 3: // Longitude
                        NMEAParser_parse_degrees(p, &(GPGGA._longitude_upper), &(GPGGA._longitude_lower));
                        break;
                    case 4: // Longitude Indicator
                        GPGGA._longitude_upper = *p == 'W' ? -GPGGA._longitude_upper : GPGGA._longitude_upper;
                        break;
                    case 5: // Fix Quality
                        GPGGA._fixquality = *p - '0';
                        break;
                    case 6: // Number of Satellites (tracked/used for fix)
                        GPGGA._numsats = (*p - '0') * 10 + *(p + 1) - '0';
                        break;
                    case 7: // HDOP
                        GPGGA._hdop = (short) NMEAParser_parse_decimal(p);
                        break;
                    case 8: // Altitude
                        GPGGA._altitude = NMEAParser_parse_decimal(p);
                        break;
                }
                break;
            case NMEA_GSA:
                switch (term_number)
                {
                    case 1: // Fix Type
                        GPGSA._fixtype = *p - '0';
                        break;
                    case 14: // PDOP
                        GPGSA._pdop = (short) NMEAParser_parse_decimal(p);
                        break;
                    case 15: // HDOP
                        GPGSA._hdop = (short) NMEAParser_parse_decimal(p);
                        break;
                    case 16: // VDOP
                        GPGSA._vdop = (short) NMEAParser_parse_decimal(p);
                        break;
                }
                break;
            case NMEA_GSV:
                switch (term_number)
                {
                    case 0: // GSV Sentence Count
                        GPGSV._gsv_sentences = *p - '0';
                        break;
                    case 1: // GSV Current Sentence Number
                        GPGSV._gsv_sentence = *p - '0';
                        break;
                    case 2: // Number of Satellites (in view)
                        GPGSV._numsats_visible = (*p - '0') * 10 + *(p + 1) - '0';
                        break;
                    case 6: // SNR 1
                    case 10: // SNR 2
                    case 14: // SNR 3
                    case 18: // SNR 4
                        if (*p >= '0' && *p <= '9' && *(p + 1) >= '0' && *(p + 1) <= '9')
                            GPGSV._snr_count++;
                        GPGSV._new_snr_total += NMEAParser_parse_decimal(p);
                        char *ptemp;
                        ptemp = p;
                        while (*ptemp != ',' && *ptemp != '*') ptemp++;
                        if ((GPGSV._gsv_sentence == GPGSV._gsv_sentences) && (*ptemp ==  '*')) // check to see if multiline gsv message is complete
                        {
                            GPGSV._snr_total = GPGSV._new_snr_total;
                            GPGSV._snr_avg = GPGSV._snr_total / GPGSV._snr_count;
                            GPGSV._new_snr_total = 0;
                            GPGSV._snr_count = 0;
                        }
                        break;
                }
                break;
            case NMEA_RMC:
                switch (term_number)
                {
                    case 0: // UTC Time
                        //_last_time_fix = logtime;
                        GPRMC._time = NMEAParser_parse_decimal(p);
                        break;
                    case 2: // Latitude
                        //_last_position_fix = logtime;
                        NMEAParser_parse_degrees(p, &(GPRMC._latitude_upper), &(GPRMC._latitude_lower));
                        break;
                    case 3: // Latitude Indicator
                        GPRMC._latitude_upper = *p == 'S' ? -GPRMC._latitude_upper : GPRMC._latitude_upper;
                        break;
                    case 4: // Longitude
                        NMEAParser_parse_degrees(p, &(GPRMC._longitude_upper), &(GPRMC._longitude_lower));
                        break;
                    case 5: // Longitude Indicator
                        GPRMC._longitude_upper = *p == 'W' ? -GPRMC._longitude_upper : GPRMC._longitude_upper;
                        break;
                    case 6: // Speed
                        GPRMC._speed = NMEAParser_parse_decimal(p);
                        break;
                    case 7: // Course
                        GPRMC._course = NMEAParser_parse_decimal(p);
                        break;
                    case 8: // UTC Date
                        GPRMC._date = NMEAParser_parse_decimal(p) / 100;
                        break;
                }
                break;

            // Add functionality for additional NMEA sentences here

        }
        while (*p != ',' && *p != '*')
            p++;
        p++;
        term_number++;
        if (*(p - 1) == '*')
        {
            decoded = true;
        }
        if (term_number > NMEASENTENCE_MAXTERMS)
            return false;
    }
    return true;
}

static int NMEAParser_termcmp(const char *str1, const char *str2)
{
    while (*str1 && (*str1 == *str2))
        str1++, str2++;
    return (*str1 > *str2) - (*str2 > *str1);
}

static int NMEAParser_hexToInt(char hex)
{
    if (hex >= 'A' && hex <= 'F')
        return hex - 'A' + 10;
    else if (hex >= 'a' && hex <= 'f')
        return hex - 'a' + 10;
    else
        return hex - '0';
}

static int32_t NMEAParser_parse_decimal(char *p)
{
    bool neg = *p == '-';
    if (neg) p++;
    long ret = 0L;
    while (*p >= '0' && *p <= '9')
        ret = ret * 10L + *p++ - '0';
    ret *= 100L;
    if (*p++ == '.')
    {
        if (*p >= '0' && *p <= '9')
        {
            ret += 10L * (*p++ - '0');
            if (*p >= '0' && *p <= '9')
                ret += *p - '0';
        }
    }
    return neg ? -ret : ret;
}

static void NMEAParser_parse_degrees(char *p, int32_t *upper, int32_t *lower)
{
    long deg = 0L;
    while ((*(p + 2) != '.') && (*p >= '0' && *p <= '9'))
        deg = deg * 10L + *p++ - '0';
    *upper = deg;

    long min = (*p++ - '0') * 10;
    min += *p++ - '0';
    p++;
    while ((*p >= '0' && *p <= '9'))
        min = min * 10L + *p++ - '0';

    min *= 100L;

    *lower = min / 6;
}
