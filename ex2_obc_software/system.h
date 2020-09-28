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
 * This header file is unique to the system being implemented. It just includes
 * system specific headers, and defines system parameters.
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#include <semphr.h>

#include "hal/obc_hal.h"
#include "obc.h"

#define SYSTEM_APP_ID _OBC_APP_ID_

int ex2_main(int argc, char **argv);

typedef struct {
  SemaphoreHandle_t temp_sensor;
} Equipment_Mutex_t;

#endif /* SYSTEM_H */
