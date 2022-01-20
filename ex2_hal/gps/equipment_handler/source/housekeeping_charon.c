/*
 * Copyright (C) 2021  University of Alberta
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
 * @file    housekeeping_charon.c
 * @author  Thomas Ganley
 * @date    2021-12-29
 */

#include "housekeeping_charon.h"

int Charon_getHK(charon_housekeeping * hk){
    // Read temperature sensors
    if(readAllTemps(&hk->temparray[0])){
        return 1;
    }

    // Read GPS firmware CRC
    if(skytraq_query_software_CRC(&hk->crc)){
        return 1;
    }
}
