/*
 * scheduler.c
 *
 *  Created on: Nov. 22, 2021
 *      Author: Grace Yi
 */

#include <FreeRTOS.h>
#include "os_timer.h"
#include "csp.h"
#include "services.h"
#include "util/service_utilities.h"
#include "scheduler/scheduler.h"
#include "system.h"
#include "logger/logger.h"
#include <redposix.h>

static const char *schedule_filename = "VOL0:/schedule.txt";

// schedule context: timer, run semaphore, recursive change mutex, next_command_to_run (base64)

void scheduler_runner(void *pvParameters) {
    // parameter is schedule context
    // init
    // highest priority task

    for (;;) {
        // wait on semaphore for timer
        // grab change mutex
        // decode next command to run
        // send CSP packet
        // receive reply??
        // if no reply, just log that
        // encode reply to base64
        // log reply
        // dispatch schedule
        // release change mutex
        // continue
    }
    vTaskDelete(0); // Better to delete than return
}

void scheduler_timer_cb(TimerHandle_t timer) {
    // timer ID stores pointer to schedule context
    // send semaphore telling scheduler implementer to run
    // done
}

void schedule_dispatch(void *data) {
    // parameter is schedule context
    // grab change mutex
    // load next command from schedule file
    // set timer for the right amount of time
    // start timer
    // release change mutex
}

/**
 * @brief
 *      Private. Collect scheduled commands from the groundstation
 * @param gs_cmds
 *      pointer to the struct of all the gs commands
 * @return SAT_returnState
 *      SATR_OK or SATR_ERROR
 */
// SAT_returnState scheduler_service_app(char *gs_cmds) {
SAT_returnState scheduler_service_app(csp_packet_t *gs_cmds) {
    uint8_t ser_subtype = (uint8_t)gs_cmds->data[SUBSERVICE_BYTE];
    int8_t status;

    switch (ser_subtype) {
    default:
        sys_log(ERROR, "No such subservice");
        status = SATR_PKT_ILLEGAL_SUBSERVICE;
        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&gs_cmds->data[OUT_DATA_BYTE], &status, sizeof(int8_t));
        set_packet_length(gs_cmds, 2 * sizeof(int8_t) + 1); // plus one for sub-service
        return SATR_PKT_ILLEGAL_SUBSERVICE;
    }

    /*consider if struct should hold error codes returned from these functions*/
    return SATR_OK;
}

/**
 * @brief
 *      FreeRTOS gs scheduler server task
 * @details
 *      Accepts incoming gs scheduler packets and executes the application
 * @param void* param
 * @return SAT_returnState
 */
SAT_returnState scheduler_service() {
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_HMACREQ);
    csp_bind(sock, TC_SCHEDULER_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN); // TODO: SERVICE_BACKLOG_LEN constant TBD
    //

    for (;;) {
        csp_conn_t *conn;
        csp_packet_t *packet;

        if ((conn = csp_accept(sock, CSP_MAX_TIMEOUT)) == NULL) {
            continue;
        }

        while ((packet = csp_read(conn, 50)) != NULL) {
            if (scheduler_service_app(packet) != SATR_OK) {
                int number_of_cmds = -1;
                memcpy(&packet->data[OUT_DATA_BYTE], &number_of_cmds, sizeof(int8_t));
                set_packet_length(packet, 2 * sizeof(int8_t) + 1); // plus one for sub-service
            }
            if (!csp_send(conn, packet, 50)) {
                csp_buffer_free(packet);
            }
        }
        csp_close(conn); // frees buffers used
    }
}

/**
 * @brief
 *      Start scheduler service
 * @details
 *      Starts the FreeRTOS task responsible for accepting incoming
 *      schedule packets
 * @param None
 * @return SAT_returnState
 *      success report
 */
SAT_returnState start_scheduler_service(void) {

    if (xTaskCreate((TaskFunction_t)scheduler_service, "scheduler_service", SCHEDULER_SVC_SIZE, NULL,
                    NORMAL_SERVICE_PRIO, NULL) != pdPASS) {
        sys_log(ERROR, "FAILED TO CREATE TASK scheduler_service\n");
        return SATR_ERROR;
    }
    sys_log(NOTICE, "Scheduler service started\n");
    return SATR_OK;
}
