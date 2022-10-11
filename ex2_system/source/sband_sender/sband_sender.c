/*
 * sband_sender.c
 *
 *  Created on: Sep. 15, 2022
 *      Author: Robert Taylor
 */
#include "FreeRTOS.h"
#include "sband_sender/sband_sender.h"
#include "os_queue.h"
#include <stdbool.h>
#include "sdr_driver.h"
#include "logger/logger.h"
#include "error_correctionWrapper.h"
#include "rfModeWrapper.h"
#include "fec.h"

typedef struct {
    void *data;
    uint32_t len;
} sband_data_ctx_t;

typedef enum {
    WAITING,
    SENDING,
    ENDING,
} sband_sender_state;

QueueHandle_t send_queue = 0;

bool sband_send_data(void *data, size_t len) {
    sband_data_ctx_t ctx = {0};
    ctx.data = data;
    ctx.len = len;
    BaseType_t ret = xQueueSend(send_queue, &ctx, 1000);
    return (bool)ret;
}

bool sband_send_queue_full() { return uxQueueSpacesAvailable(send_queue) > 0; }

void sband_sender(void *pvParameters) {
    sband_sender_state state = WAITING;
    sband_data_ctx_t ctx = {0};
    sdr_interface_data_t ifdata = {0};
    sdr_conf_t conf = {0};
    ifdata.sdr_conf = &conf;
    ifdata.mac_data = fec_create(RF_MODE_3, NO_FEC);
    sdr_sband_driver_init(&ifdata);
    while (1) {
        switch (state) {
        case WAITING: {
            if (xQueuePeek(send_queue, &ctx, portMAX_DELAY) !=
                pdPASS) { // Found something in the queue, start transmitting
                continue;
            }
            sys_log(INFO, "Starting sband transfer");
            sdr_sband_tx_start(&ifdata);
            state = SENDING;
            break;
        };
        case SENDING: {
            if (xQueueReceive(send_queue, &ctx, 0) !=
                pdPASS) { // Don't wait for more data. A new transaction is better than an overrun
                state = ENDING;
                break;
            }
            sdr_sband_tx(&ifdata, ctx.data, ctx.len);
            vPortFree(ctx.data);
        };
        case ENDING: {
            sys_log(INFO, "Ending sband transfer");
            sdr_sband_tx_stop(&ifdata);
            state = WAITING;
            break;
        };
        default: {
            sys_log(WARN, "sband_sender in bad state. Undefined behavior may follow");
            state = WAITING;
            break;
        };
        };
    }
}

SAT_returnState start_sband_daemon() {
    send_queue = xQueueCreate(10, sizeof(sband_data_ctx_t));
    if (xTaskCreate(sband_sender, "sband_daemon", SBANDSEND_DM_SIZE, NULL, configMAX_PRIORITIES - 1, NULL) !=
        pdPASS) {
        ex2_log("Could not start sband_daemon task");
        return SATR_ERROR;
    }
    return SATR_OK;
}
