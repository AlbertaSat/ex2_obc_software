#include <FreeRTOS.h>
#include <os_task.h>
#include <os_portable.h>
#include <string.h>
#include <csp/csp.h>
#include <csp/arch/csp_malloc.h>
#include <logger/logger.h>
#include "test_sdr.h"
#include "sdr_driver.h"

#define TEST_STACK_SIZE 512

#define BASE_LEN 20

void test_csp_send(void *arg) {
    ex2_log("starting CSP->SDR send");
    vTaskDelay(1000);

    csp_conn_t *conn = 0;
    while (!conn) {
        conn = csp_connect(CSP_PRIO_NORM, 17, 17, 1000, CSP_SO_HMACREQ);
        if (!conn) {
            ex2_log("CSP connection failed %d", xTaskGetTickCount());
            vTaskDelay(1000);
        }
    }

    int i, count = 0;
    while (1) {
        size_t len = ((count & 0x7) + 1) * BASE_LEN;
        csp_packet_t *packet = csp_buffer_get(len);
        if (!packet) {
            ex2_log("no more packets");
            break;
        }

        packet->length = len;
        for (i = 0; i < len; i++) {
            packet->data[i] = i;
        }

        ex2_log("csp->sdr send %d, len %d\n", count++, len);

        if (csp_send(conn, packet, 1000) != 0) {
            ex2_log("send failed");
        }

        vTaskDelay(5000);
    }

    csp_close(conn);
}

void test_uhf_send(void *arg) {
    sdr_interface_data_t *ifdata = arg;

    if (!ifdata) {
        ex2_log("no UHF interface: exiting");
        vTaskDelete(0); // delete self to free up heap
    }

    ex2_log("starting SDR->UART send");
    vTaskDelay(1000);

    int i, count = 0;
    while (1) {
        size_t len = ((count & 0x7) + 1) * BASE_LEN;
        uint8_t *packet = (uint8_t *)pvPortMalloc(len);
        if (!packet) {
            ex2_log("no more packets");
            break;
        }

        for (i = 0; i < len; i++) {
            packet[i] = i;
        }
        ex2_log("sdr->uhf send %d, len %d\n", count++, len);

        int rc = sdr_uhf_tx(ifdata, packet, len);
        if (rc) {
            ex2_log("sdr_uhf_tx returns %d", rc);
        }
        vPortFree(packet);
        vTaskDelay(5000);
    }
}

void test_sband_send(void *arg) {
    sdr_interface_data_t *ifdata = arg;
    int len = 1024;
    uint8_t *packet = (uint8_t *) pvPortMalloc(len);

    if (!ifdata) {
        ex2_log("no S-BAND interface: exiting");
        vTaskDelete(0); // delete self to free up heap
    }

    ex2_log("starting S-BAND send");
    vTaskDelay(1000);

    while (ifdata) {
        /* Simulate the sending of 10MB files */
        sdr_sband_tx_start(ifdata);

        int i, count;
        for (count=0; count < 10*1024; count++) {
            for (i=0; i<len; i++) {
                packet[i] = count & 0x0ff;
            }
            ex2_log("sband send %d, len %d", count, len);

            int rc = sdr_sband_tx(ifdata, packet, len);
            if (rc) {
                ex2_log("sdr_sband_tx returns %d", rc);
            }
        }

        ex2_log("sband rest %d", count/len);
        sdr_sband_tx_stop(ifdata);
        vTaskDelay(10000);
        ex2_log("sband back at it!");
    }
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
    }

    csp_close(conn);
}

static int count;

static void sdr_uhf_receive(void *conf, uint8_t *data, size_t len, void *unused) {
    ex2_log("uhf->sdr receive %d, len %d", ++count, len);
    int i, mismatches = 0;
    for (i = 0; i < len; i++) {
        if (data[i] != i) {
            ex2_log("packet[%d] got %d", i, data[i]);
            mismatches++;
        }
    }
    ex2_log("uhf->csp %d mismatches", mismatches);
}

void start_test_sdr(sdr_interface_data_t *uhf_ifdata, sdr_interface_data_t *sband_ifdata) {
    if (SDR_NO_CSP) {
        uhf_ifdata->sdr_conf->rx_callback = (sdr_rx_callback_t) sdr_uhf_receive;
        uhf_ifdata->sdr_conf->rx_callback_data = NULL;

        xTaskCreate(test_uhf_send, "sdr->uhf", TEST_STACK_SIZE, uhf_ifdata, 0, NULL);
    }
    else {
        xTaskCreate(test_csp_receive, "sdr->csp", TEST_STACK_SIZE, NULL, 0, NULL);
        xTaskCreate(test_csp_send, "csp->sdr", TEST_STACK_SIZE, NULL, 0, NULL);
    }

    xTaskCreate(test_sband_send, "sdr->sband", TEST_STACK_SIZE, sband_ifdata, 0, NULL);
}
