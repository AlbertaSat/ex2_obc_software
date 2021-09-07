/*
 * Copyright (C) 2015  University of Alberta
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
/**
 * @file    housekeeping_athena.c
 * @author  Dustin Wagner
 * @date    2021-05-13
 */

#include "housekeeping_athena.h"
#include <stdlib.h>

/**
 * @brief
 *      Getter to supply data from temperature sensors on athena
 *
 * @param temparray
 * 		  the array to store temperature readings
 * @return
 * 		0 for success. other for failure
 */
int HAL_get_temp_all(long *temparray) {
#ifdef ATHENA_IS_STUBBED
    return 0;
#else
    return gettemp_all(temparray);
#endif
}

/**
 * @brief
 * 		Athena composition housekeeping getter
 * @details
 * 		Contains calls to other functions that each return a portion of Athena
 *    telemetry data. Generally reports back to housekeeping_service.
 * @attention
 * 		New components should follow the structure of the existing components.
 *    All data needs to be deep copied
 * @param athena_hk
 * 		Pointer to struct of athena housekeeping data
 * @return
 * 		Last found error will be returned. else no error returned
 */
int Athena_getHK(athena_housekeeping *athena_hk) {
    int temporary;
    int return_code = 0;

    /*Add athena HAL housekeeping getters here and put fields in h file
    create HAL functions here following format of existing
    also add endianness conversion in Athena_hk_convert_endianness*/
    temporary = HAL_get_temp_all(&athena_hk->temparray);

    if (temporary != 0)
        return_code = temporary;

    return return_code;
}

/**
 * @brief
 *      Converts endianness of values in athena_housekeeping struct
 *
 * @param athena_hk
 * 		  struct of athena housekeeping data
 * @return
 * 		0 for success. other for failure
 */
int Athena_hk_convert_endianness(athena_housekeeping *athena_hk) {
    uint8_t i;
    for (i = 0; i < 6; i++) {
        athena_hk->temparray[i] = (long)csp_hton32((uint32_t)athena_hk->temparray[i]);
    }
    return 0;
}
