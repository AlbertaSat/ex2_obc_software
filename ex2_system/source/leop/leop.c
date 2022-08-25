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
 * @file leop.c
 * @author Grace Yi, Thomas Ganley, Robert Taylor
 * @date Oct. 2021
 */

#include "leop/leop.h"
#include "logger/logger.h"
#include "printf.h"
#include <redposix.h>

#define TWO_MIN_DELAY pdMS_TO_TICKS(120 * 1000)
#define FOUR_MIN_DELAY pdMS_TO_TICKS(240 * 1000)
#define TWENTY_SEC_DELAY pdMS_TO_TICKS(20 * 1000)
#define LEOP_LOG_BUF_SIZE 128

int leop_log_fd = -1;

void leop_log(const char *format, ...) {
    if (leop_log_fd != -1) {
        char outbuf[LEOP_LOG_BUF_SIZE] = {0};
        va_list arg;
        va_start(arg, format);
        vsnprintf(outbuf, LEOP_LOG_BUF_SIZE, format, arg);
        red_write(leop_log_fd, outbuf, strlen(outbuf));
    }
}

char *deployable_to_str(Deployable_t sw) {
    switch (sw) {
    case DFGM:
        return "DFGM";
    case UHF_P:
        return "UHF_P";
    case UHF_Z:
        return "UHF_Z";
    case UHF_S:
        return "UHF_S";
    case UHF_N:
        return "UHF_N";
    case Port:
        return "PORT";
    case Payload:
        return "PAYLOAD";
    case Starboard:
        return "STARBOARD";
    default:
        return "Unknown";
    }
}

void log_switch(Deployable_t sw, int switch_status, int expected_deployed_state) {
    if ((switch_status != expected_deployed_state)) {
        leop_log("%s does not report deployed\n", deployable_to_str(sw));
    } else {
        leop_log("%s reports deployed\n", deployable_to_str(sw));
    }
}

/**
 * @brief
 *      Deploy all deployable systems
 * @return void
 *      No need for a return, LEOP success is determined by hope
 */
void deploy_all_deployables() {
    int dfgm_switch_deployed_state = 1;
    Deployable_t dfgm = DFGM;
    int switch_status = deploy(dfgm, MAX_ATTEMPTS);
    log_switch(dfgm, switch_status, dfgm_switch_deployed_state);

    vTaskDelay(TWO_MIN_DELAY);

    int uhf_switch_deployed_state = 1;
    for (Deployable_t sw = UHF_P; sw <= UHF_N; sw++) {
        switch_status = deploy(sw, MAX_ATTEMPTS);
        log_switch(sw, switch_status, uhf_switch_deployed_state);
    }
}

/**
 * @brief
 *      Execute LEOP if it has not been executed before
 * @details
 *      Checks if LEOP sequence has been successfully executed
 *      If not, execute LEOP sequence
 *      Otherwise, skip LEOP sequence
 * @return void
 */
void execute_leop() {
    leop_log_fd = red_open(
        "VOL0:/leop.log",
        RED_O_WRONLY |
            RED_O_CREAT); // We don't care about failures. If this fails to open, so be it, leop can't fail
    if (leop_log_fd == -1) {
        leop_log_fd =
            red_open("VOL1:/leop.log", RED_O_WRONLY | RED_O_CREAT); // May as well try both volumes anyway
    }
    bool eeprom_flag = false;
    eeprom_flag = eeprom_get_leop_status();
    if (eeprom_flag != true) {
        leop_log("LEOP sequence begun\n");
        // If leop sequence was never executed, execute it and hope for the best
        deploy_all_deployables();
        // Set EEPROM flag to true. LEOP has been attempted, it is up to the operators now to verify it.
        eeprom_set_leop_status();
    }
    if (leop_log_fd != -1) {
        red_close(leop_log_fd);
    }
}
