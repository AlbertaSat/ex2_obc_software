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
#include <redposix.h>
#include "printf.h"
#include <string.h>

#define str(s) #s

void createErrorOutput(char *pcWriteBuffer, size_t xWriteBufferLen) {
    const char *errorMsg;
    switch (red_errno) {
    case 1:
        errorMsg = "RED_EPERM";
        break;
    case 2:
        errorMsg = "RED_ENOENT";
        break;
    case 5:
        errorMsg = "RED_EIO";
        break;
    case 9:
        errorMsg = "RED_EBADF";
        break;
    case 16:
        errorMsg = "RED_EBUSY";
        break;
    case 17:
        errorMsg = "RED_EEXIST";
        break;
    case 18:
        errorMsg = "RED_EXDEV";
        break;
    case 20:
        errorMsg = "RED_ENOTDIR";
        break;
    case 21:
        errorMsg = "RED_EISDIR";
        break;
    case 22:
        errorMsg = "RED_EINVAL";
        break;
    case 23:
        errorMsg = "RED_ENFILE";
        break;
    case 24:
        errorMsg = "RED_EMFILE";
        break;
    case 27:
        errorMsg = "RED_EFBIG";
        break;
    case 28:
        errorMsg = "RED_ENOSPC";
        break;
    case 30:
        errorMsg = "RED_EROFS";
        break;
    case 31:
        errorMsg = "RED_EMLINK";
        break;
    case 34:
        errorMsg = "RED_ERANGE";
        break;
    case 36:
        errorMsg = "RED_ENAMETOOLONG";
        break;
    case 38:
        errorMsg = "RED_ENOSYS";
        break;
    case 39:
        errorMsg = "RED_ENOTEMPTY";
        break;
    case 61:
        errorMsg = "RED_ENODATA";
        break;
    case 87:
        errorMsg = "RED_EUSERS";
        break;
    default:
        errorMsg = "Unknown Error";
    }
    snprintf(pcWriteBuffer, xWriteBufferLen, "%s\n", errorMsg);
    red_errno = 0;
}

static BaseType_t prvPWDCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    char pszBuffer[REDCONF_NAME_MAX];
    char *cwd = red_getcwd(pszBuffer, REDCONF_NAME_MAX);

    if (cwd == NULL) {
        createErrorOutput(pcWriteBuffer, xWriteBufferLen);
    } else {
        snprintf(pcWriteBuffer, xWriteBufferLen, "%s\n", cwd);
    }
    return pdFALSE;
}

static BaseType_t prvCDCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    // We can guarantee there will be one parameter because FreeRTOS+CLI won't call this function unless it has
    // exactly one parameter
    BaseType_t parameterLen;
    const char *parameter = FreeRTOS_CLIGetParameter(
        /* The command string itself. */
        pcCommandString,
        /* Return the first parameter. */
        1,
        /* Store the parameter string length. */
        &parameterLen);
    int32_t error = red_chdir(parameter);
    if (error < 0) {
        createErrorOutput(pcWriteBuffer, xWriteBufferLen);
    }
    return pdFALSE;
}

static BaseType_t prvLSCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    static bool firstRun = true;
    static REDDIR *dir;
    if (firstRun) {
        red_errno = 0;
        char pszBuffer[REDCONF_NAME_MAX];
        char *cwd = red_getcwd(pszBuffer, REDCONF_NAME_MAX);
        if (cwd == NULL) {
            createErrorOutput(pcWriteBuffer, xWriteBufferLen);
            return pdFALSE;
        }
        dir = red_opendir(cwd);
        if (dir == NULL) {
            createErrorOutput(pcWriteBuffer, xWriteBufferLen);
            return pdFALSE;
        }
        firstRun = false;
    }
    red_errno = 0;
    REDDIRENT *current = red_readdir(dir);
    if (current == NULL) {
        if (red_errno == 0) {
            snprintf(pcWriteBuffer, xWriteBufferLen, "\n");
        } else {
            createErrorOutput(pcWriteBuffer, xWriteBufferLen);
        }
        red_closedir(dir);
        firstRun = true;
        return pdFALSE;
    } else {
        snprintf(pcWriteBuffer, xWriteBufferLen, "%s\n", current->d_name);
        return pdTRUE;
    }
}

static BaseType_t prvMKDIRCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    // We can guarantee there will be one parameter because FreeRTOS+CLI won't call this function unless it has
    // exactly one parameter
    BaseType_t parameterLen;
    const char *parameter = FreeRTOS_CLIGetParameter(
        /* The command string itself. */
        pcCommandString,
        /* Return the first parameter. */
        1,
        /* Store the parameter string length. */
        &parameterLen);
    int32_t error = red_mkdir(parameter);
    if (error < 0) {
        createErrorOutput(pcWriteBuffer, xWriteBufferLen);
    }
    return pdFALSE;
}

static BaseType_t prvRMDIRCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    // We can guarantee there will be one parameter because FreeRTOS+CLI won't call this function unless it has
    // exactly one parameter
    BaseType_t parameterLen;
    const char *parameter = FreeRTOS_CLIGetParameter(
        /* The command string itself. */
        pcCommandString,
        /* Return the first parameter. */
        1,
        /* Store the parameter string length. */
        &parameterLen);
    int32_t error = red_rmdir(parameter);
    if (error < 0) {
        createErrorOutput(pcWriteBuffer, xWriteBufferLen);
    }
    return pdFALSE;
}

static BaseType_t prvMKCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    // We can guarantee there will be one parameter because FreeRTOS+CLI won't call this function unless it has
    // exactly one parameter
    BaseType_t parameterLen;
    const char *parameter = FreeRTOS_CLIGetParameter(
        /* The command string itself. */
        pcCommandString,
        /* Return the first parameter. */
        1,
        /* Store the parameter string length. */
        &parameterLen);
    int32_t fd = red_open(parameter, RED_O_CREAT | RED_O_EXCL | RED_O_RDWR);
    if (fd < 0) {
        createErrorOutput(pcWriteBuffer, xWriteBufferLen);
        return pdFALSE;
    }
    red_close(fd);
    return pdFALSE;
}

static BaseType_t prvRMCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    // We can guarantee there will be one parameter because FreeRTOS+CLI won't call this function unless it has
    // exactly one parameter
    BaseType_t parameterLen;
    const char *parameter = FreeRTOS_CLIGetParameter(
        /* The command string itself. */
        pcCommandString,
        /* Return the first parameter. */
        1,
        /* Store the parameter string length. */
        &parameterLen);
    int32_t error = red_unlink(parameter);
    if (error < 0) {
        createErrorOutput(pcWriteBuffer, xWriteBufferLen);
    }
    return pdFALSE;
}

static BaseType_t prvSTATCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    // We can guarantee there will be one parameter because FreeRTOS+CLI won't call this function unless it has
    // exactly one parameter
    static bool firstRun = true;
    static REDSTAT pStat;
    BaseType_t parameterLen;
    if (firstRun == true) {
        const char *parameter = FreeRTOS_CLIGetParameter(
            /* The command string itself. */
            pcCommandString,
            /* Return the first parameter. */
            1,
            /* Store the parameter string length. */
            &parameterLen);
        int fd = red_open(parameter, RED_O_RDONLY);
        if (fd < 0) {
            createErrorOutput(pcWriteBuffer, xWriteBufferLen);
            return pdFALSE;
        }
        int error = red_fstat(fd, &pStat);
        red_close(fd);
        if (error < 0) {
            createErrorOutput(pcWriteBuffer, xWriteBufferLen);
            return pdFALSE;
        }
        firstRun = false;
    }
    const char *fmt = "dev %d\nino %d\nmode %hX\nnlink %hd\nsize %lld\natime %d\nmtime %d\nctime %d\n";
    int written = snprintf(pcWriteBuffer, xWriteBufferLen, fmt, pStat.st_dev, pStat.st_ino, pStat.st_mode,
                           pStat.st_nlink, pStat.st_size, pStat.st_atime, pStat.st_mtime, pStat.st_ctime);
    firstRun = true;
    memset(&pStat, 0, sizeof(REDSTAT));
    return pdFALSE;
}

static BaseType_t prvREADCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    // We can guarantee there will be one parameter because FreeRTOS+CLI won't call this function unless it has
    // exactly one parameter
    static bool firstRun = true;
    BaseType_t parameterLen;
    static int fd;
    if (firstRun == true) {
        const char *parameter = FreeRTOS_CLIGetParameter(
            /* The command string itself. */
            pcCommandString,
            /* Return the first parameter. */
            1,
            /* Store the parameter string length. */
            &parameterLen);
        fd = red_open(parameter, RED_O_RDONLY);
        if (fd < 0) {
            createErrorOutput(pcWriteBuffer, xWriteBufferLen);
            return pdFALSE;
        }
        firstRun = false;
    }
    int32_t status = red_read(fd, pcWriteBuffer, xWriteBufferLen);
    if (status == 0) {
        red_close(fd);
        firstRun = true;
        return pdFALSE;
    }
    if (status < 0) {
        createErrorOutput(pcWriteBuffer, xWriteBufferLen);
        red_close(fd);
        firstRun = true;
        return pdFALSE;
    }
    return pdTRUE;
}

static BaseType_t prvTRANSACTCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    // We can guarantee there will be one parameter because FreeRTOS+CLI won't call this function unless it has
    // exactly one parameter
    BaseType_t parameterLen;
    const char *parameter = FreeRTOS_CLIGetParameter(
        /* The command string itself. */
        pcCommandString,
        /* Return the first parameter. */
        1,
        /* Store the parameter string length. */
        &parameterLen);
    int32_t error = red_transact(parameter);
    if (error < 0) {
        createErrorOutput(pcWriteBuffer, xWriteBufferLen);
    }
    return pdFALSE;
}

static BaseType_t prvCPCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    BaseType_t fromParameterLen;
    char *copyFrom = (char *)FreeRTOS_CLIGetParameter( // I know casting away from const is bad, a null terminator needs to be placed in the array :(
        /* The command string itself. */
        pcCommandString,
        /* Return the first parameter. */
        1,
        /* Store the parameter string length. */
        &fromParameterLen);
    BaseType_t toParameterLen;
    const char *copyTo = FreeRTOS_CLIGetParameter(
        /* The command string itself. */
        pcCommandString,
        /* Return the second parameter. */
        2,
        /* Store the parameter string length. */
        &toParameterLen);
    copyFrom[fromParameterLen] = 0;
    int from_fd = red_open(copyFrom, RED_O_RDONLY);
    if (from_fd < 0) {
        createErrorOutput(pcWriteBuffer, xWriteBufferLen);
        return pdFALSE;
    }
    int to_fd = red_open(copyTo, RED_O_WRONLY | RED_O_CREAT);
    if (to_fd < 0) {
        createErrorOutput(pcWriteBuffer, xWriteBufferLen);
        red_close(from_fd);
        return pdFALSE;
    }
    int8_t *buf = pvPortMalloc(512);
    if (buf == NULL) {
        snprintf(pcWriteBuffer, xWriteBufferLen, "%s\n", "failed to allocate buffer");
        red_close(to_fd);
        red_close(from_fd);
        return pdFALSE;
    }
    bool done = false;
    while (!done) {
        int32_t bytes_read = red_read(from_fd, buf, 512);
        if (bytes_read == 0) {
            done = true;
            continue;
        } 
        if (bytes_read < 0) {
            done = true;
            createErrorOutput(pcWriteBuffer, xWriteBufferLen);
            continue;
        }
        int32_t bytes_written = red_write(to_fd, buf, 512);
        if (bytes_written < 0) {
            done = true;
            createErrorOutput(pcWriteBuffer, xWriteBufferLen);
            continue; 
        }
    }
    red_close(to_fd);
    red_close(from_fd);
    vPortFree(buf);
    return pdFALSE;
}

static const CLI_Command_Definition_t xPWDCommand = {"pwd", "pwd:\n\tGet current working directory\n",
                                                     prvPWDCommand, 0};
static const CLI_Command_Definition_t xLSCommand = {"ls", "ls:\n\tGet list of tiles in cwd\n", prvLSCommand, 0};
static const CLI_Command_Definition_t xCDCommand = {"cd", "cd:\n\tChange current working directory\n",
                                                    prvCDCommand, 1};
static const CLI_Command_Definition_t xMKDIRCommand = {"mkdir", "mkdir:\n\tMake a new directory\n",
                                                       prvMKDIRCommand, 1};
static const CLI_Command_Definition_t xRMDIRCommand = {
    "rmdir", "rmdir:\n\tRemove a directory only if it is empty\n", prvRMDIRCommand, 1};
static const CLI_Command_Definition_t xMKCommand = {"mk", "mk:\n\tCreate a new empty file\n", prvMKCommand, 1};
static const CLI_Command_Definition_t xRMCommand = {
    "rm", "rm:\n\tDelete files. Can take any number of file parameters\n", prvRMCommand, -1};
static const CLI_Command_Definition_t xSTATCommand = {"stat", "stat:\n\tStat a file\n", prvSTATCommand, 1};
static const CLI_Command_Definition_t xREADCommand = {"read", "read:\n\tRead contents of a file\n", prvREADCommand,
                                                      1};
static const CLI_Command_Definition_t xTRANSACTCommand = {
    "transact",
    "transact:\n\tTell Reliance-edge to transact the filesystem.\n\tMust include volume prefix to transact\n",
    prvTRANSACTCommand, 1};
static const CLI_Command_Definition_t xCPCommand = {"cp", "cp:\n\tCopy first parameter to second parameter\n", prvCPCommand, 2};

void register_fs_utils() {
    FreeRTOS_CLIRegisterCommand(&xPWDCommand);
    FreeRTOS_CLIRegisterCommand(&xLSCommand);
    FreeRTOS_CLIRegisterCommand(&xCDCommand);
    FreeRTOS_CLIRegisterCommand(&xMKDIRCommand);
    FreeRTOS_CLIRegisterCommand(&xRMDIRCommand);
    FreeRTOS_CLIRegisterCommand(&xMKCommand);
    FreeRTOS_CLIRegisterCommand(&xRMCommand);
    FreeRTOS_CLIRegisterCommand(&xSTATCommand);
    FreeRTOS_CLIRegisterCommand(&xREADCommand);
    FreeRTOS_CLIRegisterCommand(&xTRANSACTCommand);
    FreeRTOS_CLIRegisterCommand(&xCPCommand);

}
