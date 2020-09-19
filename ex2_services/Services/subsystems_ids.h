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

#ifndef SERVICES_SUBSYSTEMS_IDS_H
#define SERVICES_SUBSYSTEMS_IDS_H

#define SYS_LITTLE_ENDIAN 0
#define SYS_BIG_ENDIAN 1

/**
 * Each subsystem has a unique APP ID that is used to route ECSS packets
 * within the different satellite subsystems.
 * The general structure of the nodes is desribed by CSP thus,
 +-------+-------+-------+-------+       bus: I2C, CAN, KISS
|       |       |       |       |
+---+   +---+   +---+   +---+   +---+
|OBC|   |COM|   |EPS|   |PL1|   |PL2|     Nodes 0 - 15 (Space segment)
+---+   +---+   +---+   +---+   +---+
        ^
        |  Radio
        v
      +---+          +----+
      |TNC|          | PC |             Nodes 16 - 31 (Ground segment)
      +---+          +----+
        |               |
        +---------------+               bus: KISS, ZMQ
 *
 */

/* Space Segment */
#define _OBC_APP_ID_ 0
#define _EPS_APP_ID_ 4
#define _ADCS_APP_ID_ 2
#define _COMMS_APP_ID_ 3

/* Ground Segment */
#define _GND_APP_ID_ 16
#define _DEMO_APP_ID_ 30
#define _LAST_APP_ID_ 31

#endif /* SERVICES_SUBSYSTEMS_IDS_H */
