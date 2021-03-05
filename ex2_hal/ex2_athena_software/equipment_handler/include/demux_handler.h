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
 * demux.h
 * Noah Rozario
 * 2020/6/17
 */
#ifndef DEMUX_HANDLER_H
#define DEMUX_HANDLER_H

#define HIGH            1
#define LOW             0

//TODO: Set these
#define DEMUX_A0_PIN    0
#define DEMUX_A1_PIN    0
#define DEMUX_A2_PIN    0

//Updated this to DEMUX output channels instead**
enum demux_out_pins{ //defining data type panel_CS which represents hyperion panels
    OUT_Y0 = 0,
    OUT_Y1 = 1,
    OUT_Y2 = 2,
    OUT_Y3 = 3,
    OUT_Y4 = 4,
    OUT_Y5 = 5,
    OUT_Y6 = 6,
    OUT_Y7 = 7
};

void demux_enable();
void demux_disable();

//Bring output pin (Y0...Y7) to low
void demux_select_pin(enum demux_out_pins pin);

// //Set all output pins back to high
// void demux_reset();

#endif
