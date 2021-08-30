/*
 * nmea_service.c
 *
 *  Created on: Mar. 25, 2021
 *      Author: Robert Taylor
 */
#include "nmea_service.h"
#include "FreeRTOS.h"
#include "os_task.h"
#include <string.h>
#include "util/service_utilities.h"
#include "system.h"

static char NMEA_buf[NMEASENTENCE_MAXLENGTH];
static uint32_t wdt_counter = 0;

uint32_t nmea_get_wdt_counter() {
    return wdt_counter;
}

/**
 * @brief Starts NMEA decoding service
 */
void NMEA_service() {
    ex2_log("NMEA task started");

    init_NMEA();

    for (;;) {
        memset(NMEA_buf, 0, NMEASENTENCE_MAXLENGTH);
        wdt_counter++;

        while(xQueueReceive(NMEA_queue, NMEA_buf, DELAY_WAIT_INTERVAL) != pdPASS) {
            wdt_counter++;
        }
        int i;
        for (i = 0; i < strlen(NMEA_buf); i++) {
            NMEAParser_encode(NMEA_buf[i]);
        }
        ex2_log("NMEA message received: %s", NMEA_buf);
    }
}
