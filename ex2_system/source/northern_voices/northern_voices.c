/*
 * northern_voices.c
 *
 *  Created on: Sep. 21, 2022
 *      Author: Robert Taylor
 */
#include "FreeRTOS.h"
#include "os_task.h"
#include "os_semphr.h"
#include "services.h"
#include "northern_voices/northern_voices.h"
#include "redposix.h"
#include "logger/logger.h"
#include "csp/csp.h"

#define NV_DELAY_WAIT vTaskDelay(pdMS_TO_TICKS(5000))
#define NV_TIME_BETWEEN_SENDS (pdMS_TO_TICKS(10000))
#define NV_BLOCKSIZE 512
#define NV_REPEATS 5

typedef struct {
    int repeats;
    int repeat_goal;
    bool enabled;
    int fd;
    int packetno;
    int dest_addr;
    int dest_port;
    csp_conn_t *conn;
    SemaphoreHandle_t ctx_mtx;
} nv_ctx_t;

static nv_ctx_t nv_ctx;

BaseType_t get_lock(nv_ctx_t *ctx, int timeout) {
    if (ctx->ctx_mtx) {
        return xSemaphoreTake(ctx->ctx_mtx, timeout);
    }
    return pdFALSE;
}

void give_lock(nv_ctx_t *ctx) {
    if (ctx->ctx_mtx) {
        xSemaphoreGive(ctx->ctx_mtx);
    }
}

void nv_daemon(void *pvParameters) {
    nv_ctx_t *ctx = (nv_ctx_t *)pvParameters;

    while (1) {
        if (get_lock(ctx, 0) != pdTRUE) {
            vTaskDelay(pdMS_TO_TICKS(10)); // just prevent it from spinning constantly
            continue;
        }
        if (ctx->enabled) {
            if (ctx->repeats >= ctx->repeat_goal) { // Quick hack to make it not go forever
                give_lock(ctx);
                NV_DELAY_WAIT;
                continue;
            }
            // Do stuff
            char data[NV_BLOCKSIZE] = {0};
            int read = red_read(ctx->fd, data, NV_BLOCKSIZE);
            if (read < 0) {
                sys_log(WARN, "Read failed, errno: %d", red_errno);
                give_lock(ctx);
                continue;
            } else if (read == 0) {
                ctx->repeats++;
                sys_log(INFO, "Reached end of NV transmission, restarting soon");
                red_lseek(ctx->fd, 0, RED_SEEK_SET);
                vTaskDelay(NV_TIME_BETWEEN_SENDS);
                give_lock(ctx);
                continue;
            }

            // Get packet
            csp_packet_t *packet = csp_buffer_get(sizeof(nv_data_packet_header_t) + read);
            packet->length = sizeof(nv_data_packet_header_t) + read;

            // Construct header
            nv_data_packet_header_t header = {0};
            header.data_size = read;
            header.packetno = ctx->packetno++;

            memcpy(packet->data, &header, sizeof(header));
            memcpy(packet->data + sizeof(header), data, read);

            if (!csp_send(ctx->conn, packet, 1000)) {
                csp_buffer_free(packet);
                sys_log(WARN, "Failed to send CSP packet!");
                give_lock(ctx);
                continue;
            }
            give_lock(ctx);
            continue;
        } else {
            give_lock(ctx);
            NV_DELAY_WAIT;
            continue;
        }
    }
}

bool start_nv_transmit(uint16_t repeats, char *filename) {
    if (!stop_nv_transmit()) {
        return false;
    }
    if (get_lock(&nv_ctx, 1000) != pdTRUE) {
        return false;
    }
    int fd = red_open(filename, RED_O_RDONLY);
    nv_ctx.fd = fd;

    csp_conn_t *conn = csp_connect(1, nv_ctx.dest_addr, nv_ctx.dest_port, 100000, CSP_O_CRC32);
    nv_ctx.conn = conn;
    nv_ctx.enabled = 1;
    nv_ctx.repeat_goal = repeats;
    give_lock(&nv_ctx);
    sys_log(INFO, "Started NV transmission");
    return true;
}

bool stop_nv_transmit() {
    nv_ctx.enabled = false; // Yes, this can cause race conditions. But we need it to stop
    if (get_lock(&nv_ctx, 5000) != pdTRUE) {
        return false;
    } else {
        if (nv_ctx.conn) {
            csp_close(nv_ctx.conn);
            nv_ctx.conn = 0;
        }
        if (nv_ctx.fd) {
            red_close(nv_ctx.fd);
            nv_ctx.fd = 0;
        }
        nv_ctx.enabled = false;
        nv_ctx.packetno = 0;
        nv_ctx.repeats = 0;
        nv_ctx.repeat_goal = 0;
        give_lock(&nv_ctx);
        sys_log(INFO, "Stopped NV transmission");
        return true;
    }
}
SAT_returnState start_nv_daemon() {
    memset(&nv_ctx, 0, sizeof(nv_ctx));
    nv_ctx.ctx_mtx = xSemaphoreCreateMutex();
    nv_ctx.dest_addr = GS_CSP_ADDR;
    nv_ctx.dest_port = TC_NV_SERVICE;
    if (xTaskCreate(nv_daemon, "nv_daemon", NV_DAEMON_STACK_SIZE, (void *)&nv_ctx, NORMAL_SERVICE_PRIO, NULL) !=
        pdTRUE) {
        return SATR_ERROR;
    }
    return SATR_OK;
}
