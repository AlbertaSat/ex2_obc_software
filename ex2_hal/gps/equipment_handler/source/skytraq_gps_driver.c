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

void extract_time(uint32_t _time, TickType_t correction, uint8_t *utc_hour, uint8_t *utc_minute, uint8_t *utc_second, uint16_t *utc_ms) {
    // _time is stored such that: hhmmssss

    int ms_since_logged = (xTaskGetTickCount()* portTICK_PERIOD_MS) - correction*portTICK_PERIOD_MS;
    *utc_hour = _time/1000000;
    *utc_minute = _time / 10000 % 100;
    *utc_second = _time / 100 % 100;
    *utc_ms = (_time % 100) * 10;

    // Apply time correction:
    *utc_ms += ms_since_logged;
    if (*utc_ms / 1000) {
        *utc_second += *utc_ms/1000;
        *utc_ms %= 1000;
    }
    if (*utc_second / 60) {
        *utc_minute += *utc_second/60;
        *utc_second %= 60;
    }
    if (*utc_minute / 60) {
        *utc_hour += *utc_minute/60;
        *utc_minute %= 60;
    }
    if (*utc_hour / 24) {
        *utc_hour %= 24;
    }
    return;
}

bool gps_get_utc_time(uint8_t *utc_hour, uint8_t *utc_minute, uint8_t *utc_second, uint16_t *utc_ms) {

    // this will take GPRMC time if it is available, otherwise GPGGA
    bool GGA = GGA_ENABLED;
    bool RMC = RMC_ENABLED;
    GPGGA_s GGA_s;
    GPRMC_s RMC_s;

    if (RMC) {
        bool RMC_valid = NMEAParser_get_GPRMC(&RMC_s);
        if (RMC_valid) {
            extract_time(RMC_s._time, RMC_s._logtime, utc_hour, utc_minute, utc_second, utc_ms);
            return true;
        }
    }

    if (GGA) {
        bool GGA_valid = NMEAParser_get_GPGGA(&GGA_s);
        if (GGA_valid) {
            extract_time(GGA_s._time, GGA_s._logtime, utc_hour, utc_minute, utc_second, utc_ms);
            return true;
        }
    }
    return false;
}




