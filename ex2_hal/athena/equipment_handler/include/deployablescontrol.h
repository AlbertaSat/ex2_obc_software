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
 * @file    deployablecontrol.h
 * @author  Josh Lazaruk
 * @date    2020-08-13
 */

#ifndef DEPLOYABLES_H_
#define DEPLOYABLES_H_

#include "HL_gio.h"

typedef enum {
    // DFGM
    DFGM = 0,
    // UHF antennas
    UHF_P = 1,
    UHF_Z = 2,
    UHF_S = 3,
    UHF_N = 4,
    // solar panels
    Port = 5,
    Payload = 6,
    Starboard = 7,
} Deployable_t;

void activate(Deployable_t knife);

bool switchstatus(Deployable_t sw);

bool deploy(Deployable_t deployable);

#endif /* DEPLOYABLES_H_ */
