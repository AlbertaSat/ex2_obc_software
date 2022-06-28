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

/**
 * @file northern_voices_games.c
 * @author Thomas Ganley
 * @date 2022-06-12
 */

#include "northern_voices_games.h"

static void northern_voices_task(void *pvParameters);

bool northern_voices_active = false;

NS_return NS_start_northern_voices(char *filename) {
    if (northern_voices_active == true) {
        sys_log(NOTICE, "Tried to start northern voices when it was already active");
        return NS_FAIL;
    }

    if (xTaskCreate(northern_voices_task, "NVM", NORTHERN_VOICES_STACK_SIZE, (void *)filename,
                    NORTHERN_VOICES_PRIORITY, NULL) != pdPASS) {
        return NS_FAIL;
    } else {
        northern_voices_active = true;
    }
    return NS_OK;
}

void NS_stop_northern_voices(void) { northern_voices_active = false; }

bool NS_northern_voices_status(void) { return northern_voices_active; }

static void northern_voices_task(void *pvParameters) {
    char *filename = (char *)pvParameters;
    int32_t iErr;
    iErr = red_open(filename, RED_O_RDONLY);
    if (iErr == -1) {
        sys_log(ERROR, "Failed to open file for northern voices");
        northern_voices_active = false;
        vTaskDelete(NULL);
    }
    while (northern_voices_active) {
    }
    vTaskDelete(NULL);
}
