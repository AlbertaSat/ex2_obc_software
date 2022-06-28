/*
 * Copyright (C) 2021  University of Alberta
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
 * @file northern_voices_games.h
 * @author Thomas Ganley
 * @date 2022-06-12
 */

#ifndef EX2_HAL_NORTHERN_SPIRIT_EQUIPMENT_HANDLER_INCLUDE_NORTHERN_VOICES_GAMES_H_
#define EX2_HAL_NORTHERN_SPIRIT_EQUIPMENT_HANDLER_INCLUDE_NORTHERN_VOICES_GAMES_H_

#include "northern_spirit_handler.h"
#include <redposix.h>
#define NORTHERN_VOICES_STACK_SIZE 500
#define NORTHERN_VOICES_PRIORITY 3

NS_return NS_start_northern_voices(char *filename);
void NS_stop_northern_voices(void);
bool NS_northern_voices_status(void);

#endif /* EX2_HAL_NORTHERN_SPIRIT_EQUIPMENT_HANDLER_INCLUDE_NORTHERN_VOICES_GAMES_H_ */
