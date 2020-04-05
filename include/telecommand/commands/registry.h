/*
 * Copyright (C) 2020 Andrew Rooney
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
 *
 * bbruner@ualberta.ca
 */
/**
 * @file registry.h
 * @author Andrew Rooney
 * @date Apr. 5, 2020
 */
#ifndef INCLUDE_COMMAND_REGISTRY_H_
#define INCLUDE_COMMAND_REGISTRY_H_

#include "telecommand/telecommand.h"

/********************************************************************************/
/* Structure Defines															*/
/********************************************************************************/

typedef struct telecommand_counter_t{
  telecommand_t _super_;
  int *count;
} telecommand_counter_t;

/********************************************************************************/
/* Constructor Declare															*/
/********************************************************************************/

void initialize_command_counter( telecommand_counter_t *, int *, driver_toolkit_t * );

#endif /*INCLUDE_COMMAND_REGISTRY_H_*/
