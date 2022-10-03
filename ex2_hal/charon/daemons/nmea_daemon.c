/*
 * nmea_service.c
 *
 *  Created on: Mar. 25, 2021
 *      Author: Robert Taylor
 */
#include "nmea_daemon.h"
#include "FreeRTOS.h"
#include "os_task.h"
#include <string.h>
#include "util/service_utilities.h"
#include "system.h"
#include "NMEAParser.h"

#define NMEA_TASK_SIZE 384

/**
 * @brief Starts NMEA decoding service
 */
void NMEA_daemon() {
    ex2_log("NMEA task started");

    init_NMEA();
    static char NMEA_buf[NMEASENTENCE_MAXLENGTH];

    for (;;) {
        memset(NMEA_buf, 0, NMEASENTENCE_MAXLENGTH);

        while (xQueueReceive(NMEA_queue, NMEA_buf, DELAY_WAIT_INTERVAL) != pdPASS) {
            continue;
        }
        int i;
        for (i = 0; i < strlen(NMEA_buf); i++) {
            NMEAParser_encode(NMEA_buf[i]);
        }
        ex2_log("NMEA message received: %s", NMEA_buf);
    }
}

SAT_returnState start_NMEA_daemon() {
    TaskHandle_t nmea_handle;
    if (xTaskCreate((TaskFunction_t)NMEA_daemon, "NMEA_daemon", NMEA_TASK_SIZE, NULL, 1, &nmea_handle) != pdPASS) {
        return SATR_ERROR;
    }
    return SATR_OK;
}
