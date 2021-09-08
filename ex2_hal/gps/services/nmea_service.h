/*
 * nmea_service.h
 *
 *  Created on: Mar. 25, 2021
 *      Author: Robert Taylor
 */

#ifndef EX2_HAL_EX2_GPS_SOFTWARE_SERVICES_INCLUDE_NMEA_SERVICE_H_
#define EX2_HAL_EX2_GPS_SOFTWARE_SERVICES_INCLUDE_NMEA_SERVICE_H_

#include "FreeRTOS.h"
#include "NMEAParser.h"
#include "os_task.h"

uint32_t nmea_get_wdt_counter();
void NMEA_service();

#endif /* EX2_HAL_EX2_GPS_SOFTWARE_SERVICES_INCLUDE_NMEA_SERVICE_H_ */
