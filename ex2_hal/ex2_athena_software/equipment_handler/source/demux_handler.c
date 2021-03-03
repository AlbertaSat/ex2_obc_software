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
 * @file    demux.c
 * @author  Noah Rozario
 * @date    2020-06-17
 */


#include "HL_gio.h"
#include "HL_het.h"
#include "demux_handler.h"

void demux_enable()
{
    gioSetBit(gioPORTB, 2, HIGH);
}
void demux_disable()
{
    gioSetBit(gioPORTB, 2, LOW);
}

/**
 * @brief
 *      Selects the given output pin.
 * @param port
 *      GIO port register
 * @param pin
 *      Select which output pin to use. (OUT_Y0...OUT_Y7)
 */
void demux_select_pin(enum demux_out_pins pin)
{
    //gioInit(); //initialising gio ports
    switch(pin)
    {
        case OUT_Y0:
            gioSetBit(hetPORT1, 11, LOW);
            gioSetBit(gioPORTB, 7, LOW);
            gioSetBit(gioPORTA, 3, LOW);
            break;

        case OUT_Y1:
            gioSetBit(hetPORT1, 11, HIGH);
            gioSetBit(gioPORTB, 7, LOW);
            gioSetBit(gioPORTA, 3, LOW);
            break;

        case OUT_Y2:
            gioSetBit(hetPORT1, 11, LOW);
            gioSetBit(gioPORTB, 7, HIGH);
            gioSetBit(gioPORTA, 3, LOW);
            break;

        case OUT_Y3:
            gioSetBit(hetPORT1, 11, HIGH);
            gioSetBit(gioPORTB, 7, HIGH);
            gioSetBit(gioPORTA, 3, LOW);
            break;

        case OUT_Y4:
            gioSetBit(hetPORT1, 11, LOW);
            gioSetBit(gioPORTB, 7, LOW);
            gioSetBit(gioPORTA, 3, HIGH);
            break;

        case OUT_Y5:
            gioSetBit(hetPORT1, 11, HIGH);
            gioSetBit(gioPORTB, 7, LOW);
            gioSetBit(gioPORTA, 3, HIGH);
            break;

        case OUT_Y6:
            gioSetBit(hetPORT1, 11, LOW);
            gioSetBit(gioPORTB, 7, HIGH);
            gioSetBit(gioPORTA, 3, HIGH);
            break;

        case OUT_Y7:
            gioSetBit(hetPORT1, 11, HIGH);
            gioSetBit(gioPORTB, 7, HIGH);
            gioSetBit(gioPORTA, 3, HIGH);
            break;

        default:
            //OUT_Y0 default
            gioSetBit(hetPORT1, 11, LOW);
            gioSetBit(gioPORTB, 7, LOW);
            gioSetBit(gioPORTA, 3, LOW);
            break;
    }
}
