/*
 * hal_athena.c
 *
 *  Created on: Jul 18, 2022
 *      Author: joshd
 */
#include "ina209.h"

uint16_t SOLAR_INA209_CONFIG  = 0xDA73;

int initAthena(void){
#if IS_ATHENA_V2 == 1
    init_ina209(SOLAR_INA209_ADDR);
    ina209_set_configuration((uint8_t)SOLAR_INA209_ADDR, &SOLAR_INA209_CONFIG);
    return 0;
#endif
}
