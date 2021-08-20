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
 * @file logger_service.h
 * @author Dustin Wagner
 * @date 2021-07-14
 */

#ifndef LOGGERSERVICE_H
#define LOGGERSERVICE_H

#include "services.h"

typedef enum {
   GET_FILE = 0,
   GET_OLD_FILE = 1,
   GET_FILE_SIZE = 2,
   SET_FILE_SIZE = 3 
} logger_subservice;

SAT_returnState start_logger_service(void);

#endif
