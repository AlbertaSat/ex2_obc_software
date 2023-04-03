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

/*
 * fs_utils.c
 *
 *  Created on: Dec. 24, 2021
 *      Author: Robert Taylor
 */

#include <FreeRTOS.h>
#include <FreeRTOS-Plus-CLI/FreeRTOS_CLI.h>
#include "printf.h"
#include <string.h>
#include "cli/gps_utils.h"

static const char gpsHelpText[] = "gps: Set/Get GPS status. Possible options:";

static BaseType_t prvGPSgetPosition(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {}

static BaseType_t prvGPSgetSpeed(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {}

static BaseType_t prvGPSgetCourse(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {}

static BaseType_t prvGPSGetVisibleSatellites(char *pcWriteBuffer, size_t xWriteBufferLen,
                                             const char *pcCommandString) {}

static BaseType_t prvGPSgetAltitude(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {}

static BaseType_t prvGPSconfigureMessageTypesCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
                                                     const char *pcCommandString) {}

/**
 * This command set is a bit different in that it uses subcommands, instead of a command for each operation
 */
static BaseType_t prvGPSgpsCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    BaseType_t subCommandLen;
    char *subCommand = FreeRTOS_CLIGetParameter(pcCommandString, 1, &subCommandLen);
    if (strncmp("position", subCommand, xWriteBufferLen) == 0) {
        prvGPSgetPosition(pcWriteBuffer, xWriteBufferLen, pcCommandString);

    } else if (strncmp("speed", subCommand, xWriteBufferLen) == 0) {
        prvGPSgetSpeed(pcWriteBuffer, xWriteBufferLen, pcCommandString);

    } else if (strncmp("course", subCommand, xWriteBufferLen) == 0) {
        prvGPSgetCourse(pcWriteBuffer, xWriteBufferLen, pcCommandString);

    } else if (strncmp("satellites", subCommand, xWriteBufferLen) == 0) {
        prvGPSGetVisibleSatellites(pcWriteBuffer, xWriteBufferLen, pcCommandString);

    } else if (strncmp("satellites", subCommand, xWriteBufferLen) == 0) {
        prvGPSGetVisibleSatellites(pcWriteBuffer, xWriteBufferLen, pcCommandString);

    } else if (strncmp("messagetypes", subCommand, xWriteBufferLen) == 0) {
        prvGPSconfigureMessageTypesCommand(pcWriteBuffer, xWriteBufferLen, pcCommandString);

    } else {
        snprintf(pcWriteBuffer, xWriteBufferLen, "Invalid subcommand");
    }
    return pdFALSE;
}

static const CLI_Command_Definition_t xGPSCommand = {"gps", gpsHelpText, prvGPSgpsCommand, -1};

void register_gps_utils() { FreeRTOS_CLIRegisterCommand(&xGPSCommand); }
