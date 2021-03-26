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

// IMPORTANT: This function is expected to run on a cold boot of the OBC
bool gps_skytraq_driver_init() {
    skytraq_binary_init();

    ErrorCode gps_enable_all = gps_configure_message_types(false,false,false,true,1);
    if (gps_enable_all != SUCCESS) {
        return false;
    }
    vTaskDelay(500*portTICK_PERIOD_MS);

    ErrorCode restart = skytraq_restart_receiver(HOT_START, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    if (restart != SUCCESS) {
        return false;
    }
    vTaskDelay(500*portTICK_PERIOD_MS);

    ErrorCode powerMode = skytraq_configure_power_mode(POWERSAVE, UPDATE_TO_FLASH);
    if (powerMode != SUCCESS) {
        return false;
    }
    vTaskDelay(500*portTICK_PERIOD_MS);

    //TODO: maybe implement a way to request CRC and compare it with a stored CRC?
    return true;
}

ErrorCode gps_configure_message_types(bool GGA, bool GSA, bool GSV, bool RMC, uint8_t interval) {
    uint8_t GGA_int = GGA ? interval : 0;
    uint8_t GSA_int = GSA ? interval : 0;
    uint8_t GSV_int = GSV ? interval : 0;
    uint8_t RMC_int = RMC ? interval : 0;
    GGA_ENABLED = GGA;
    GSA_ENABLED = GSA;
    GSV_ENABLED = GSV;
    RMC_ENABLED = RMC;
    return skytraq_configure_nmea_output_rate(GGA_int, GSA_int, GSV_int, 0, RMC_int, 0,0, UPDATE_TO_FLASH);
}

ErrorCode gps_disable_NMEA_output() {
    return gps_configure_message_types(false,false,false,false, 0);
}


// returns true if date overflows
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



