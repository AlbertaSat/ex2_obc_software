/*
 * hal_athena.h
 *
 *  Created on: Jul 18, 2022
 *      Author: joshd, Liam Droog
 */
#include "HL_reg_het.h"
#include <ina209.h>

#ifndef EX2_HAL_ATHENA_HARDWARE_INTERFACE_INCLUDE_HAL_ATHENA_H_
#define EX2_HAL_ATHENA_HARDWARE_INTERFACE_INCLUDE_HAL_ATHENA_H_

// ------ Solar Panel Current Sense Pin Definitions ------ //
#define SOLAR_CURRENTSENSE_ALERT_PIN 0
#define SOLAR_CURRENTSENSE_ALERT_PORT hetPORT2
#define SOLAR_CURRENTSENSE_SHDN_PIN 12
#define SOLAR_CURRENTSENSE_SHDN_PORT hetPORT2

int initAthena(void);
void is_SolarPanel_overcurrent(void *pvParameters);

#endif /* EX2_HAL_ATHENA_HARDWARE_INTERFACE_INCLUDE_HAL_ATHENA_H_ */
