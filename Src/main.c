/*
To build: gcc src/*.c src/telecommand/*.c -c -I include/telecommand/ -I ../ex2_on_board_computer/Source/include/ -I ../ex2_on_board_computer/Project/ -I ../ex2_on_board_computer/libcsp/include/ -I ../ex2_on_board_computer/Source/portable/GCC/POSIX/ -I ../ex2_on_board_computer/libcsp/build/include/ -m32 -lpthread -std=c99 -lrt && ar -rsc client_server.a *.o

(sorry for the long ass command)
*/
#include "FreeRTOS.h"
#include <csp/arch/csp_thread.h>
#include <csp/csp.h>
#include <csp/csp_interface.h>
#include <fcntl.h>
#include <services.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <task.h>
#include <unistd.h>
#include <system.h>
#include "if_fifo.h"

service_queues_t service_queues;

csp_iface_t csp_if_fifo = {
    .name = "fifo",
    .nexthop = csp_fifo_tx,
    .mtu = TM_TC_BUFF_SIZE,
};

void vAssertCalled(unsigned long ulLine, const char *const pcFileName) {
  printf("error line: %lu in file: %s", ulLine, pcFileName);
}

void server_loop(void *parameters) {
  csp_socket_t *sock;
  csp_conn_t *conn;
  csp_packet_t *packet;

  /* Create socket and listen for incoming connections */
  sock = csp_socket(CSP_SO_NONE);
  csp_bind(sock, CSP_ANY);
  csp_listen(sock, 5);
  portBASE_TYPE err;

  /* Super loop */
  for (;;) {
    /* Process incoming packet */
    if ((conn = csp_accept(sock, 10000)) == NULL) {
      /* timeout */
      continue;
    }
    while ((packet = csp_read(conn, 50)) != NULL) {
      switch (csp_conn_dport(conn)) {

        case TC_HOUSEKEEPING_SERVICE:
          err =
              xQueueSendToBack(service_queues.hk_app_queue, (void *)packet,
                               NORMAL_TICKS_TO_WAIT);
          if (err != pdPASS) {
            printf("FAILED TO QUEUE MESSAGE");
          }
          csp_buffer_free(packet);
          break;

        case TC_TEST_SERVICE:
          err =
              xQueueSendToBack(service_queues.test_app_queue, (void *)packet,
                               NORMAL_TICKS_TO_WAIT);
          if (err != pdPASS) {
            printf("FAILED TO QUEUE MESSAGE");
          }
          csp_buffer_free(packet);
          break;

        default:
          csp_service_handler(conn, packet);
          csp_buffer_free(packet);
          break;
      }
    }
    csp_close(conn);
  }
}

int main(int argc, char **argv) {
  char *tx_channel_name = "server_to_client";
  char *rx_channel_name = "client_to_server";

  if (start_service_handlers() != SATR_OK) {
    printf("COULD NOT START TELECOMMAND HANDLER");
    return -1;
  }

  /* Set type */
  xTaskCreate((TaskFunction_t)server_loop, "SERVER THREAD", 2048, NULL, 1,
              NULL);

  /* Init CSP and CSP buffer system */
  if (csp_init(TC_TM_app_id[DEMO_APP_ID]) != CSP_ERR_NONE ||
      csp_buffer_init(64, 512) != CSP_ERR_NONE) {
    printf("Failed to init CSP\r\n");
    return -1;
  }

  tx_channel = open(tx_channel_name, O_RDWR);
  if (tx_channel < 0) {
    printf("Failed to open TX channel\r\n");
    return -1;
  }

  rx_channel = open(rx_channel_name, O_RDWR);
  if (rx_channel < 0) {
    printf("Failed to open RX channel\r\n");
    return -1;
  }

  /* Start fifo RX task */
  xTaskCreate((TaskFunction_t)fifo_rx, "RX_THREAD", 2048, NULL, 1, &rx_thread);

  /* Set default route and start router */
  csp_route_set(CSP_DEFAULT_ROUTE, &csp_if_fifo, CSP_NODE_MAC);
  csp_route_start_task(0, 0);

  vTaskStartScheduler();

  for (;;) {
  }

  close(rx_channel);
  close(tx_channel);

  return 0;
}
