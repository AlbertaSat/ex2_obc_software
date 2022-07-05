#include <FreeRTOS.h>
#include <os_task.h>
#include <string.h>
#include <csp/arch/csp_malloc.h>
#include "util/service_utilities.h"
#include "test_sdr.h"

#define TEST_STACK_SIZE 512

#define BASE_LEN 20

static void test_csp_send(void *arg) {
    ex2_log("starting CSP->UART send");
    vTaskDelay(1000);

    csp_conn_t *conn = 0;
    while (!conn) {
        conn = csp_connect(CSP_PRIO_NORM, 23, 23, 1000, CSP_O_NONE);
        if (!conn) {
            ex2_log("CSP connection failed %d", xTaskGetTickCount());
            vTaskDelay(1000);
            // return;
        }
    }

    int i, count = 0;
    while (1) {
        size_t len = ((count & 0x7) + 1) * BASE_LEN;
        csp_packet_t *packet = csp_buffer_get(len);
        if (!packet) {
            ex2_log("no more CSP packets");
            break;
        }
        packet->length = len;
        for (i = 0; i < len; i++) {
            packet->data[i] = i;
        }
        ex2_log("csp->uhf send %d, len %d\n", count++, packet->length);

        if (!csp_send(conn, packet, 1000)) {
            ex2_log("send failed");
            break;
        }

        csp_buffer_free(packet);
        vTaskDelay(5000);
    }

    csp_close(conn);
}

static void test_csp_receive(void *arg) {
    csp_socket_t *sock = csp_socket(CSP_SO_NONE);

    /* Bind socket to UHF ports, e.g. all incoming connections will be handled here */
    csp_bind(sock, 23);

    /* Create a backlog of 10 connections, i.e. up to 10 new connections can be queued */
    csp_listen(sock, CSP_ANY);

    /* Wait for connections and then process packets on the connection */
    csp_conn_t *conn;
    int tries = 10;
    while ((conn = csp_accept(sock, 10000)) == NULL) {
        ++tries;
        if (tries > 10)
            break;
    }

    if (!conn) {
        conn = csp_connect(CSP_PRIO_NORM, 23, 23, 1000, CSP_O_NONE);
        ex2_log("no connection! forcing conn %p", conn);
    } else
        ex2_log("legitimate connection! conn %p", conn);

    int count = 0;
    while (1) {
        csp_packet_t *packet;
        if (!(packet = csp_read(conn, CSP_MAX_TIMEOUT))) {
            ex2_log("read failed");
            break;
        }

        ex2_log("uhf->csp receive %d, len %d", ++count, packet->length);
        int i, mismatches = 0;
        for (i = 0; i < packet->length; i++) {
            if (packet->data[i] != i) {
                ex2_log("packet[%d] got %d", i, packet->data[i]);
                mismatches++;
            }
        }
        ex2_log("uhf->csp %d mismatches", mismatches);

        csp_buffer_free(packet);
    }

    csp_close(conn);
}

void start_test_sdr(void) {
    xTaskCreate(test_csp_receive, "uhf->csp", TEST_STACK_SIZE, NULL, 0, NULL);
    xTaskCreate(test_csp_send, "csp->uhf", TEST_STACK_SIZE, NULL, 0, NULL);
}
