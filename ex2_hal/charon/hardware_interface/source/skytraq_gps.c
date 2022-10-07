#include "skytraq_gps.h"
#include "skytraq_binary.h"
#include "FreeRTOS.h"
#include "HL_sci.h"
#include "NMEAParser.h"
#include "os_queue.h"
#include "os_task.h"
#include <stdbool.h>
#include <string.h>
#include "ex2_time.h"

bool GGA_ENABLED = false;
bool GSA_ENABLED = false;
bool GSV_ENABLED = false;
bool RMC_ENABLED = false;

struct gps_date {
    int day;
    int month;
    int year;
};

struct gps_time {
    int ms;
    int second;
    int minute;
    int hour;
};

time_t make_unix(struct gps_time *g_t, struct gps_date *g_d) {
    tmElements_t tm;
    tm.Second = g_t->second;
    tm.Minute = g_t->minute;
    tm.Hour = g_t->hour;
    tm.Day = g_d->day;
    tm.Month = g_d->month - 1;
    tm.Year = g_d->year;
    return makeTime(tm);
}

/**
 * @brief Initialises all of the skytraq and driver
 *
 * @return true success
 * @return false failure
 */
bool gps_skytraq_driver_init() {
    skytraq_binary_init();

    GPS_RETURNSTATE gps_enable_all = gps_configure_message_types(0, 0, 0, 3);
    if (gps_enable_all != GPS_SUCCESS) {
        return false;
    }
    vTaskDelay(200 * portTICK_PERIOD_MS);

    // the manufacturer software restarts the gps with all 0's. Copied here
    GPS_RETURNSTATE restart = skytraq_restart_receiver(HOT_START, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    if (restart != GPS_SUCCESS) {
        return false;
    }
    vTaskDelay(200 * portTICK_PERIOD_MS);

    GPS_RETURNSTATE powerMode = skytraq_configure_power_mode(POWERSAVE, UPDATE_TO_FLASH);
    if (powerMode != GPS_SUCCESS) {
        return false;
    }
    vTaskDelay(200 * portTICK_PERIOD_MS);

    // TODO: maybe implement a way to request CRC and compare it with a stored CRC?
    return true;
}

#if FLIGHT_CONFIGURATION == 1
GPS_RETURNSTATE b16_configure_nmea_output_rate(uint8_t GGA_interval, uint8_t GNS_interval,
                                                   uint8_t GSA_interval, uint8_t GSV_interval,
                                                   uint8_t GLL_interval, uint8_t RMC_interval,
                                                   uint8_t VTG_interval, uint8_t ZDA_interval,
                                                   uint8_t DTM_interval, uint8_t GBS_interval,
                                                   uint8_t GRS_interval, uint8_t GST_interval,
                                                   uint8_t THS_interval, uint8_t HDT_interval,
                                                   skytraq_update_attributes attribute){
    GPS_RETURNSTATE retval;
    retval = skytraq_configure_nmea_string_interval("GGA", GGA_interval, attribute);
    if(retval){return retval;}
    retval = skytraq_configure_nmea_string_interval("GNS", GNS_interval, attribute);
    if(retval){return retval;}
    retval = skytraq_configure_nmea_string_interval("GSA", GSA_interval, attribute);
    if(retval){return retval;}
    retval = skytraq_configure_nmea_string_interval("GSV", GSV_interval, attribute);
    if(retval){return retval;}
    retval = skytraq_configure_nmea_string_interval("GLL", GLL_interval, attribute);
    if(retval){return retval;}
    retval = skytraq_configure_nmea_string_interval("RMC", RMC_interval, attribute);
    if(retval){return retval;}
    retval = skytraq_configure_nmea_string_interval("VTG", VTG_interval, attribute);
    if(retval){return retval;}
    retval = skytraq_configure_nmea_string_interval("ZDA", ZDA_interval, attribute);
    if(retval){return retval;}
    retval = skytraq_configure_nmea_string_interval("DTM", DTM_interval, attribute);
    if(retval){return retval;}
    retval = skytraq_configure_nmea_string_interval("GBS", GBS_interval, attribute);
    if(retval){return retval;}
    retval = skytraq_configure_nmea_string_interval("GRS", GRS_interval, attribute);
    if(retval){return retval;}
    retval = skytraq_configure_nmea_string_interval("GST", GST_interval, attribute);
    if(retval){return retval;}
    retval = skytraq_configure_nmea_string_interval("THS", THS_interval, attribute);
    if(retval){return retval;}
    retval = skytraq_configure_nmea_string_interval("HDT", HDT_interval, attribute);

    return retval;
}
#endif
/**
 * @brief Configure skytraq output messages
 *
 * @param GGA GGA interval
 * @param GSA GSA interval
 * @param GSV GSV interval
 * @param RMC RMC interval
 * @return GPS_RETURNSTATE
 */
GPS_RETURNSTATE gps_configure_message_types(uint8_t GGA, uint8_t GSA, uint8_t GSV, uint8_t RMC) {
    GGA_ENABLED = GGA ? true : false;
    GSA_ENABLED = GSA ? true : false;
    GSV_ENABLED = GSV ? true : false;
    RMC_ENABLED = RMC ? true : false;
#if FLIGHT_CONFIGURATION == 1
    return b16_configure_nmea_output_rate(GGA, 0, GSA, GSV, 0, RMC, 0, 0, 0, 0, 0, 0, 0, 0, UPDATE_TO_FLASH);
#else
    return skytraq_configure_nmea_output_rate(GGA, GSA, GSV, 0, RMC, 0, 0, UPDATE_TO_FLASH);
#endif
}

/**
 * @brief Disable all gps messages
 *
 * @return GPS_RETURNSTATE
 */
GPS_RETURNSTATE gps_disable_NMEA_output() { return gps_configure_message_types(0, 0, 0, 0); }

/**
 * @brief Get CRC of software on the skytraq
 * @param crc pointer to uint16_t to store crc
 *
 * @return GPS_RETURNSTATE
 */
GPS_RETURNSTATE gps_skytraq_get_software_crc(uint16_t *crc) {
    uint8_t reply[11];
    GPS_RETURNSTATE result = skytraq_query_software_CRC(reply, 11);
    if (result != GPS_SUCCESS) {
        return result;
    }
    *crc = (reply[6] << 8) | reply[7]; // extract 16 bit CRC
    return GPS_SUCCESS;
}

/**
 * @brief Get software version on the skytraq
 * @param pointer to uint8_t to store version
 *        Version "XX.YY.ZZ" -> *version = 0x000000XXYYZZ
 *
 * @return GPS_RETURNSTATE
 */
GPS_RETURNSTATE gps_skytraq_get_software_version(uint32_t *version) {
    uint8_t reply[21];
    GPS_RETURNSTATE result = skytraq_query_software_version(reply, 21);
    if (result != GPS_SUCCESS) {
        return result;
    }
    *version = (reply[11] << 16) | (reply[12] << 8) | (reply[13]);
    return GPS_SUCCESS;
}

/**
 * @brief takes time as NMEA integer and extracts it to a struct
 *
 * @param _time Integer representing time
 * @param correction Tick the data was acquired
 * @param utc_time struct to put the time in
 * @return true date overflowed to next date
 * @return false date did not overflow
 */
bool extract_time(uint32_t _time, TickType_t correction, struct gps_time *utc_time) {
    // _time is stored such that: hhmmssss

    int ms_since_logged = (xTaskGetTickCount() * portTICK_PERIOD_MS) - correction * portTICK_PERIOD_MS;
    utc_time->hour = _time / 1000000;
    utc_time->minute = _time / 10000 % 100;
    utc_time->second = _time / 100 % 100;
    utc_time->ms = (_time % 100) * 10;

    // Apply time correction:
    utc_time->ms += ms_since_logged;
    if (utc_time->ms / 1000) {
        utc_time->second += utc_time->ms / 1000;
        utc_time->ms %= 1000;
    }
    if (utc_time->second / 60) {
        utc_time->minute += utc_time->second / 60;
        utc_time->second %= 60;
    }
    if (utc_time->minute / 60) {
        utc_time->hour += utc_time->minute / 60;
        utc_time->minute %= 60;
    }
    if (utc_time->hour / 24) {
        utc_time->hour %= 24;
        return true;
    }
    return false;
}
/**
 * @brief Extract date from NMEA integer in to date struct
 *
 * @param date Integer to extract date from
 * @param date_overflow True if date overflowed since data was collected
 * @param utc_date Struct to store date in
 */
void extract_date(uint32_t date, bool date_overflow, struct gps_date *utc_date) {
    utc_date->day = date / 10000;
    utc_date->month = date / 100 % 100;
    utc_date->year = date % 100;

    if (date_overflow) {
        utc_date->day++;
        switch (utc_date->month) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            if (utc_date->day / 31) {
                utc_date->day %= 31;
                utc_date->month++;
            }
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            if (utc_date->day / 30) {
                utc_date->day %= 30;
                utc_date->month++;
            }
            break;
        case 2:
            // check if the year is a leap year
            if (((utc_date->year % 4 == 0) && (utc_date->year % 100 != 0)) || (utc_date->year % 400 == 0)) {
                if (utc_date->day / 29) {
                    utc_date->day %= 29;
                    utc_date->month++;
                }
            } else {
                if (utc_date->day / 28) {
                    utc_date->day %= 28;
                    utc_date->month++;
                }
            }
            break;
        }
        if (utc_date->month / 12) {
            utc_date->month %= 12;
            utc_date->year++;
        }
    }
    return;
}

/**
 * @brief Get UTC time from latest NMEA packet containing time
 *
 * @param utc_time time_t * to store time
 * @return true Time updated
 * @return false Time unavailable
 */
bool gps_get_utc_time(time_t *utc_time) {

    struct gps_time g_t;
    struct gps_date g_d;

    // this will take RMC time
    bool RMC = RMC_ENABLED;
    RMC_s RMC_s;

    if (RMC) {
        bool RMC_valid = NMEAParser_get_RMC(&RMC_s);
        if (RMC_valid) {
            // time will be used to correct for if we overflow to a new utc date
            bool date_overflow = extract_time(RMC_s._time, RMC_s._logtime, &g_t);
            extract_date(RMC_s._date, date_overflow, &g_d);
            *utc_time = make_unix(&g_t, &g_d);
            return true;
        }
    }
    return false;
}

/**
 * @brief Gets latest altitude from GPS in centimeters
 *
 * @param alt variable to store altitude
 * @return true Success
 * @return false Altitude unavailable
 */
bool gps_get_altitude(uint32_t *alt) {
    bool GGA = GGA_ENABLED;
    GPGGA_s GGA_s;
    if (GGA) {
        bool GGA_valid = NMEAParser_get_GPGGA(&GGA_s);
        if (GGA_valid) {
            *alt = GGA_s._altitude;
            return true;
        }
    }
    return false;
}

/**
 * @brief Gets latest position update from GPS in ten millionths of a degree
 *
 * @param latitude_upper
 * @param latitude_lower
 * @param longitude_upper
 * @param longitude_lower
 * @return true Position available
 * @return false Position unavailable
 */
bool gps_get_position(int32_t *latitude_upper, int32_t *latitude_lower, int32_t *longitude_upper,
                      int32_t *longitude_lower) {

    // this will take RMC position if it is available, otherwise GPGGA
    bool GGA = GGA_ENABLED;
    bool RMC = RMC_ENABLED;
    GPGGA_s GGA_s;
    RMC_s RMC_s;

    if (RMC) {
        bool RMC_valid = NMEAParser_get_RMC(&RMC_s);
        if (RMC_valid) {
            *latitude_upper = RMC_s._latitude_upper;
            *latitude_lower = RMC_s._latitude_lower;
            *longitude_upper = RMC_s._longitude_upper;
            *longitude_lower = RMC_s._longitude_lower;
            return true;
        }
    }

    if (GGA) {
        bool GGA_valid = NMEAParser_get_GPGGA(&GGA_s);
        if (GGA_valid) {
            *latitude_upper = GGA_s._latitude_upper;
            *latitude_lower = GGA_s._latitude_lower;
            *longitude_upper = GGA_s._longitude_upper;
            *longitude_lower = GGA_s._longitude_lower;
            return true;
        }
    }
    return false;
}

/**
 * @brief Gets latest satellite in view count
 *
 * @param numsats Variable to store number of satellites
 * @return true Number available
 * @return false Number unavailable
 */
bool gps_get_visible_satellite_count(uint8_t *numsats) {
    bool GGA = GGA_ENABLED;
    GPGGA_s GGA_s;
    if (GGA) {
        bool GGA_valid = NMEAParser_get_GPGGA(&GGA_s);
        if (GGA_valid) {
            *numsats = GGA_s._numsats;
            return true;
        }
    }
    return false;
}

/**
 * @brief Gets latest speed from GPS in hundreths of KM/h
 *
 * @param speed Variable to store speed
 * @return true Speed available
 * @return false Speed unavailable
 */
bool gps_get_speed(uint32_t *speed) {
    bool RMC = RMC_ENABLED;
    RMC_s RMC_s;
    if (RMC) {
        bool RMC_valid = NMEAParser_get_RMC(&RMC_s);
        if (RMC_valid) {
            *speed = RMC_s._speed;
            return true;
        }
    }
    return false;
}

/**
 * @brief Gets latest course from GPS in hundredths of a degree
 *
 * @param course
 * @return true
 * @return false
 */
bool gps_get_course(uint32_t *course) {
    bool RMC = RMC_ENABLED;
    RMC_s RMC_s;
    if (RMC) {
        bool RMC_valid = NMEAParser_get_RMC(&RMC_s);
        if (RMC_valid) {
            *course = RMC_s._course;
            return true;
        }
    }
    return false;
}
