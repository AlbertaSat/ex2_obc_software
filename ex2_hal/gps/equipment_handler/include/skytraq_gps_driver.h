/*
 * skytraq_gps_driver.h
 *
 *  Created on: Feb. 10, 2021
 *      Author: Robert Taylor
 */

#ifndef SKYTRAQ_GPS_DRIVER_H_
#define SKYTRAQ_GPS_DRIVER_H_

#define GPS_SYSTEM

#include "skytraq_binary_types.h"
#include <stdbool.h>
#include <time.h>

bool gps_get_altitude(uint32_t *alt);

GPS_RETURNSTATE gps_configure_message_types(uint8_t GGA, uint8_t GSA, uint8_t GSV, uint8_t RMC);

GPS_RETURNSTATE gps_disable_NMEA_output();

bool gps_get_position(int32_t *latitude_upper, int32_t *latitude_lower, int32_t *longitude_upper,
                      int32_t *longitude_lower);

bool gps_get_visible_satellite_count(uint8_t *numsats);

bool gps_get_utc_time(time_t *utc_time);

bool gps_get_gps_time(char *reply);

bool gps_get_speed(uint32_t *speed);

bool gps_get_course(uint32_t *course);

bool gps_skytraq_driver_init();

#endif /* SKYTRAQ_GPS_DRIVER_H_ */
