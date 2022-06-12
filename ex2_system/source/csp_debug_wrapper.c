/*
 * Copyright (C) 2022  University of Alberta
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
/*
 * csp_debug_wrapper.c
 *
 *  Created on: Jun. 2, 2022
 *      Author: Robert Taylor
 */

#include "csp_debug_wrapper.h"
#include "csp_debug.h"
#include "logger.h"

void csp_wrap_debug(csp_debug_level_t level, const char *format, va_list args) {
    SysLog_Level sys_level;
    switch (level) {
    case CSP_ERROR:
        sys_level = ERROR;
        break;
    case CSP_WARN:
        sys_level = WARN;
        break;
    case CSP_INFO:
        sys_level = INFO;
        break;
    case CSP_BUFFER:
    case CSP_PACKET:
    case CSP_PROTOCOL:
    case CSP_LOCK:
        sys_level = ERROR;
        break;
    default:
        sys_level = ERROR;
        break;
    }
    sys_log(sys_level, format, args);
}
