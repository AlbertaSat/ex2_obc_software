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
 * csp_debug_wrapper.h
 *
 *  Created on: Jun. 2, 2022
 *      Author: Robert Taylor
 */

#ifndef EX2_SYSTEM_INCLUDE_CSP_DEBUG_WRAPPER_H_
#define EX2_SYSTEM_INCLUDE_CSP_DEBUG_WRAPPER_H_

#include "csp_debug.h"

void csp_wrap_debug(csp_debug_level_t level, const char *format, va_list args);

#endif /* EX2_SYSTEM_INCLUDE_CSP_DEBUG_WRAPPER_H_ */
