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
 * @file system_header.h
 * @author Andrew Rooney, Brandon Danyluk
 * @date 2020-10-03
 */
/*
    Including this header will automatically find the wanted
    sub-system header to include.
*/
#ifndef SYSTEM_HEADER_H
#define SYSTEM_HEADER_H

#ifdef SYSTEM_APP_ID
    #if SYSTEM_APP_ID == _DEMO_APP_ID_
        #include "ex2_demo_software/system.h"
    #elif SYSTEM_APP_ID == _OBC_APP_ID_
        #include "ex2_obc_software/system.h"
    #else
        #error Cannot include proper header due to SYSTEM_APP_ID being defined as an unimplemented value
    #endif
#else
    #error SYSTEM_APP_ID is undefined
#endif

#endif
