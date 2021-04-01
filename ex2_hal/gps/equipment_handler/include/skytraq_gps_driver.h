/*
 * skytraq_gps_driver.h
 *
 *  Created on: Feb. 10, 2021eb.m,,,jhn
 *      Author: robert
 */

#ifndef SKYTRAQ_GPS_DRIVER_H_
#define SKYTRAQ_GPS_DRIVER_H_

#include <stdbool.h>
#include "skytraq_binary_types.h"
#include "time_struct.h"

// gets most recent
// returns true on successful altitude acquiring, false otherwise
// @PARAM: updates int with altitude in centimeters
bool gps_get_altitude(uint32_t *alt);

ErrorCode gps_configure_message_types(uint8_t GGA, uint8_t GSA, uint8_t GSV, uint8_t RMC);
ErrorCode gps_disable_NMEA_output();

// gets most recent
// returns true on successful altitude acquiring, false otherwise
bool gps_get_position(int32_t *latitude_upper, int32_t *latitude_lower, int32_t *longitude_upper, int32_t *longitude_lower);

bool gps_get_visible_satellite_count(uint8_t *numsats);

//utc time in hundreths of a second
bool gps_get_utc_time(time_t *utc_time);

// utc date
bool gps_get_date(date_t *utc_date);

// time of GPS receiver. No idea what this actually gives
bool gps_get_gps_time(char *reply);

// speed in hundredths of kilometers per hour
bool gps_get_speed(uint32_t *speed);

bool gps_get_course(uint32_t *course);

bool gps_skytraq_driver_init();


#endif /* SKYTRAQ_GPS_DRIVER_H_ */
