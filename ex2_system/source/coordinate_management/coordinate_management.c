/*
 * Copyright (C) 2020  University of Alberta
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
 * @file coordinate_management.c
 * @author Andrew R. Rooney
 * @date Mar. 6, 2021
 */
#include "coordinate_management/coordinate_management.h"

#include <FreeRTOS.h>
#include <os_task.h>

static void * coordinate_management_daemon(void *pvParameters);
SAT_returnState start_coordinate_management_daemon(void);

int is_china(double lat,double lon){
    const double lata =30.747500833466347;
    const double longa=111.181640625;
    const double rada=1500;
    const double latb=38.54129088479034;
    const double longb =88.41796875;
    const double radb=1350;
    const double latc=46.313549654089094;
    const double longc=123.92578125;
    const double radc=1000;

    const double earth_radius=6371;

    double siga,sigb,sigc;

    siga=earth_radius*acos(sin(lat*M_PI/180)*sin(lata*M_PI/180)+cos(lat*M_PI/180)*cos(lata*M_PI/180)*cos(fabs((lon-longa)*M_PI/180)));
    sigb=earth_radius*acos(sin(lat*M_PI/180)*sin(latb*M_PI/180)+cos(lat*M_PI/180)*cos(latb*M_PI/180)*cos(fabs((lon-longb)*M_PI/180)));
    sigc=earth_radius*acos(sin(lat*M_PI/180)*sin(latc*M_PI/180)+cos(lat*M_PI/180)*cos(latc*M_PI/180)*cos(fabs((lon-longc)*M_PI/180)));

    if(siga<=rada||sigb<=radb||sigc<=radc) return 1;
    else return 0;
}

/**
 * Coordinate management. Handle updates of current latitude, longitude, and time as
 * reported by the Global Positioning System.
 *
 * @param pvParameters
 *    task parameters (not used)
 */
static void * coordinate_management_daemon(void *pvParameters) {
    TickType_t delay = pdMS_TO_TICKS(1000);
    for ( ;; ) {
        // TODO Application level management of the GPS

        vTaskDelay(delay);
    }
}

/**
 * Start the coordinate management daemon
 *
 * @returns status
 *   error report of task creation
 */
SAT_returnState start_coordinate_management_daemon(void) {
    if (xTaskCreate((TaskFunction_t)coordinate_management_daemon,
                  "coordinate_management_daemon", 2048, NULL, COORDINATE_MANAGEMENT_TASK_PRIO,
                  NULL) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK coordinate_management_daemon\n");
        return SATR_ERROR;
    }
    ex2_log("Coordinate management started\n");
    return SATR_OK;
}

