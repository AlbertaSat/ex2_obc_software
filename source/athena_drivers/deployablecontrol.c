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
 * @file    deployablecontrol.c
 * @author  Josh Lazaruk
 * @date    2020-08-13
 */

#include "HL_gio.h"
#include "HL_het.h"
#include "deployablescontrol.h"

void activate(Deployable_TypeDef knife){
    int temp;
    switch(knife){
        case Port:
            gioSetBit(hetPORT1, 8, 1);
            for (temp = 0; temp < 0x8000000; temp++);//~6.5 seconds at 300 MHz
            gioSetBit(hetPORT1, 8, 0);
            break;
        case UHF_P:
            gioSetBit(hetPORT2, 5, 1);
            for (temp = 0; temp < 0x8000000; temp++);//~6.5 seconds at 300 MHz
            gioSetBit(hetPORT2, 5, 0);
            break;
        case UHF_Z:
            gioSetBit(hetPORT1, 26, 1);
            for (temp = 0; temp < 0x8000000; temp++);//~6.5 seconds at 300 MHz
            gioSetBit(hetPORT1, 26, 0);
            break;
        case Payload:
            gioSetBit(hetPORT1, 14, 1);
            for (temp = 0; temp < 0x8000000; temp++);//~6.5 seconds at 300 MHz
            gioSetBit(hetPORT1, 14, 0);
            break;
        case UHF_S:
            gioSetBit(gioPORTA, 0, 1);
            for (temp = 0; temp < 0x8000000; temp++);//~6.5 seconds at 300 MHz
            gioSetBit(gioPORTA, 0, 0);
            break;
        case UHF_N:
            gioSetBit(hetPORT1, 22, 1);
            for (temp = 0; temp < 0x8000000; temp++);//~6.5 seconds at 300 MHz
            gioSetBit(hetPORT1, 22, 0);
            break;
        case Starboard:
            gioSetBit(hetPORT2, 1, 1);
            for (temp = 0; temp < 0x8000000; temp++);//~6.5 seconds at 300 MHz
            gioSetBit(hetPORT2, 1, 0);
            break;
        case DFGM:
            gioSetBit(hetPORT1, 16, 1);
            for (temp = 0; temp < 0x8000000; temp++);//~6.5 seconds at 300 MHz
            gioSetBit(hetPORT1, 16, 0);
            break;
        default:
            return -1;
    }
    return 0;
}


bool switchstatus(Deployable_TypeDef sw){
    switch(sw){
        case Port:
            return gioGetBit(hetPORT2, 4);
        case UHF_P:
            return gioGetBit(hetPORT1, 10);
        case UHF_Z:
            return gioGetBit(hetPORT1, 30);
        case Payload:
            return gioGetBit(hetPORT1, 4);
        case UHF_S:
            return gioGetBit(hetPORT2, 2);
        case UHF_N:
            return gioGetBit(hetPORT1, 12);
        case Starboard:
            return gioGetBit(gioPORTA, 4);
        case DFGM:
            return gioGetBit(hetPORT1, 29);
        default:
            return 1;//check this
    }
}


//This should return 1, assuming switches are not connected to ground when undepressed
bool deploy(Deployable_TypeDef deployable){
    activate(deployable);
    return switchstatus(deployable);
}
