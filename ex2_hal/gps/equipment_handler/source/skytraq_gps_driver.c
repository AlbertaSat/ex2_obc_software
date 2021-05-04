#include "HL_sci.h"
#include "NMEAParser.h"
#include <stdbool.h>
#include "FreeRTOS.h"
#include <string.h>
#include "os_queue.h"
#include "os_task.h"
#include "skytraq_gps_driver.h"

bool GGA_ENABLED = false;
bool GSA_ENABLED = false;
bool GSV_ENABLED = false;
bool RMC_ENABLED = false;

/**
 * @brief Initialises all of the skytraq and driver
 * 
 * @return true success
 * @return false failure
 */
bool gps_skytraq_driver_init() {
    skytraq_binary_init();

    GPS_RETURNSTATE gps_enable_all = gps_configure_message_types(2,0,0,3);
    if (gps_enable_all != SUCCESS) {
        return false;
    }
    vTaskDelay(500*portTICK_PERIOD_MS);

    // the manufacturer software restarts the gps with all 0's. Copied here
    GPS_RETURNSTATE restart = skytraq_restart_receiver(HOT_START, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    if (restart != SUCCESS) {
        return false;
    }
    vTaskDelay(500*portTICK_PERIOD_MS);

    GPS_RETURNSTATE powerMode = skytraq_configure_power_mode(POWERSAVE, UPDATE_TO_FLASH);
    if (powerMode != SUCCESS) {
        return false;
    }
    vTaskDelay(500*portTICK_PERIOD_MS);

    //TODO: maybe implement a way to request CRC and compare it with a stored CRC?
    return true;
}

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
    return skytraq_configure_nmea_output_rate(GGA, GSA, GSV, 0, RMC, 0,0, UPDATE_TO_FLASH);
}

/**
 * @brief Disable all gps messages
 * 
 * @return GPS_RETURNSTATE 
 */
GPS_RETURNSTATE gps_disable_NMEA_output() {
    return gps_configure_message_types(0,0,0,0);
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
bool extract_time(uint32_t _time, TickType_t correction, time_t *utc_time) {
    // _time is stored such that: hhmmssss

    int ms_since_logged = (xTaskGetTickCount()* portTICK_PERIOD_MS) - correction*portTICK_PERIOD_MS;
    utc_time->hour = _time/1000000;
    utc_time->minute = _time / 10000 % 100;
    utc_time->second = _time / 100 % 100;
    utc_time->ms = (_time % 100) * 10;

    // Apply time correction:
    utc_time->ms += ms_since_logged;
    if (utc_time->ms / 1000) {
        utc_time->second += utc_time->ms/1000;
        utc_time->ms %= 1000;
    }
    if (utc_time->second / 60) {
        utc_time->minute += utc_time->second/60;
        utc_time->second %= 60;
    }
    if (utc_time->minute / 60) {
        utc_time->hour += utc_time->minute/60;
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
void extract_date(uint32_t date, bool date_overflow, date_t *utc_date) {
    utc_date->day = date / 10000;
    utc_date->month = date / 100 % 100;
    utc_date->year = date % 100;

    if (date_overflow) {
        utc_date->day++;
        switch(utc_date->month) {
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
            } break;
        case 4:
        case 6:
        case 9:
        case 11:
            if (utc_date->day / 30) {
                utc_date->day %= 30;
                utc_date->month++;
            } break;
        case 2:
            // check if the year is a leap year
            if (((utc_date->year % 4 == 0) && (utc_date->year % 100!= 0)) || (utc_date->year % 400 == 0)) {
                if (utc_date->day / 29) {
                    utc_date->day %= 29;
                    utc_date->month++;
                }
            } else {
                if (utc_date->day / 28) {
                    utc_date->day %= 28;
                    utc_date->month++;
                }
            } break;
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
 * @param utc_time Struct to store time
 * @return true Time updated
 * @return false Time unavailable
 */
bool gps_get_utc_time(time_t *utc_time) {

    // this will take GPRMC time if it is available, otherwise GPGGA
    bool GGA = GGA_ENABLED;
    bool RMC = RMC_ENABLED;
    GPGGA_s GGA_s;
    GPRMC_s RMC_s;

    if (RMC) {
        bool RMC_valid = NMEAParser_get_GPRMC(&RMC_s);
        if (RMC_valid) {
            extract_time(RMC_s._time, RMC_s._logtime, utc_time);
            return true;
        }
    }

    if (GGA) {
        bool GGA_valid = NMEAParser_get_GPGGA(&GGA_s);
        if (GGA_valid) {
            extract_time(GGA_s._time, GGA_s._logtime, utc_time);
            return true;
        }
    }
    return false;
}

/**
 * @brief Get UTC date from latest NMEA packet containing date
 * 
 * @param utc_time Struct to store time
 * @return true Date updated
 * @return false Date unavailable
 */
bool gps_get_date(date_t *utc_date) {
    // date is only present on gprmc
    time_t utc_time;
    bool RMC = RMC_ENABLED;
    GPRMC_s RMC_s;

    if (RMC) {
        bool RMC_valid = NMEAParser_get_GPRMC(&RMC_s);
        if (RMC_valid) {
            // time will be used to correct for if we overflow to a new utc date
            bool date_overflow = extract_time(RMC_s._time, RMC_s._logtime, &utc_time);
            extract_date(RMC_s._date, date_overflow, utc_date);
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
bool gps_get_altitude(uint32_t *alt){
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
bool gps_get_position(int32_t *latitude_upper, int32_t *latitude_lower, int32_t *longitude_upper, int32_t *longitude_lower) {
    
    // this will take GPRMC position if it is available, otherwise GPGGA
    bool GGA = GGA_ENABLED;
    bool RMC = RMC_ENABLED;
    GPGGA_s GGA_s;
    GPRMC_s RMC_s;

    if (RMC) {
        bool RMC_valid = NMEAParser_get_GPRMC(&RMC_s);
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
    GPRMC_s RMC_s;
    if (RMC) {
        bool RMC_valid = NMEAParser_get_GPRMC(&RMC_s);
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
    GPRMC_s RMC_s;
    if (RMC) {
        bool RMC_valid = NMEAParser_get_GPRMC(&RMC_s);
        if (RMC_valid) {
            *course = RMC_s._course;
            return true;
        }
    }
    return false;
}

