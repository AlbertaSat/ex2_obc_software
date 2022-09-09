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
 * @file    housekeeping_athena.c
 * @author  Dustin Wagner
 * @date    2021-05-13
 */
#include "FreeRTOS.h"
#include "task.h"
#include <stdint.h>
#include "system.h"
#include "version.h"
#include "housekeeping_athena.h"
#include <stdlib.h>
#include <string.h>
#include <csp/csp_endian.h>
#include "os_portmacro.h"
#include "version.h"
#include "bl_eeprom.h"
#include "redstat.h"
#include "redposix.h"
#include "redconf.h"
#include "ina209.h"

/**
 * @brief
 *      Getter to supply data from temperature sensors on athena
 *
 * @param temparray
 * 		  the array to store temperature readings
 * @return
 * 		0 for success. other for failure
 */
int Athena_hk_get_temps(int16_t *MCU_core_temp, int16_t *converter_temp) {
    *MCU_core_temp = 25565; // TODO: Work on temperature sensors
    *converter_temp = 25565;
    return 0;
}

/**
 * @brief
 *      Getter to Athena uptime
 * @return
 * 		integer OBC_uptime to store Athena uptime
 *      Seconds = OBC_uptime*10
 */
uint32_t Athena_get_OBC_uptime() {

    TickType_t OBC_ticks = xTaskGetTickCount(); // 1 tick/ms
    uint32_t OBC_uptime = OBC_ticks / 1000;

    return OBC_uptime;
}

/**
 * @brief
 *      Getter to Athena solar panel supply current
 * @return
 * 		  integer solar_panel_supply_curr to store solar panel supply current
 */
uint16_t Athena_get_solar_supply_curr() {
    // insert getter function for solar panel supply current;
    uint8_t addr = 0b1000101;
    uint16_t retval;
    int get_status = ina209_get_current(addr, &retval);
    return retval;
}

/**
 * @brief
 * 		Athena composition housekeeping getter
 * @details
 * 		Contains calls to other functions that each return a portion of Athena
 *    telemetry data. Generally reports back to housekeeping_service.
 * @attention
 * 		New components should follow the structure of the existing components.
 *    All data needs to be deep copied
 * @param athena_hk
 * 		Pointer to struct of athena housekeeping data
 * @return
 * 		Last found error will be returned. else no error returned
 */
int Athena_getHK(athena_housekeeping *athena_hk) {
    int temp_status;
    int return_code = 0;

    /*Add athena HAL housekeeping getters here and put fields in h file
    create HAL functions here following format of existing
    also add endianness conversion in Athena_hk_convert_endianness*/
    temp_status = Athena_hk_get_temps(&athena_hk->MCU_core_temp, &athena_hk->converter_temp);

    // Get last 8 digits of the software version
    athena_hk->version_major = VERSION_MAJOR;
    athena_hk->version_minor = VERSION_MINOR;
    athena_hk->version_patch = VERSION_PATCH;

    // Get OBC uptime: Seconds = value*10. Max = 655360 seconds (7.6 days)
    athena_hk->OBC_uptime = Athena_get_OBC_uptime();

    // Get boot info
    boot_info info;
    if (eeprom_get_boot_info(&info)) {
        return_code = -1;
    }
    athena_hk->boot_cnt = info.count;
    athena_hk->last_reset_reason = info.reason.swr_reason;
    athena_hk->boot_src = info.reason.rstsrc;

    // Get solar panel supply current
    athena_hk->solar_panel_supply_curr = Athena_get_solar_supply_curr();

    if (temp_status != 0)
        return_code = temp_status;

    // Get solar panel supply current
    athena_hk->solar_panel_supply_curr = Athena_get_solar_supply_curr();

    // Get SD card usage percentages
    int32_t iErr;
    REDSTATFS volstat;
    iErr = red_statvfs("VOL0:", &volstat);
    if (iErr == -1) {
        athena_hk->vol0_usage_percent = 255;
    } else {
        athena_hk->vol0_usage_percent =
            (uint8_t)((float)(volstat.f_bfree) * 100.0 /
                      ((float)(volstat.f_blocks))); // assuming block size == sector size = 512B
    }

    iErr = red_statvfs("VOL1:", &volstat);
    if (iErr == -1) {
        athena_hk->vol1_usage_percent = 255;
    } else {
        athena_hk->vol1_usage_percent =
            (uint8_t)((float)(volstat.f_bfree) * 100.0 /
                      ((float)(volstat.f_blocks))); // assuming block size == sector size = 512B
    }

    if (temp_status != 0)
        return_code = temp_status;

    athena_hk->heap_free = xPortGetFreeHeapSize();
    athena_hk->lowest_heap_free = xPortGetMinimumEverFreeHeapSize();

    return return_code;
}
