/*
To build: gcc src/*.c src/telecommand/*.c -c -I include/telecommand/ -I
../ex2_on_board_computer/Source/include/ -I ../ex2_on_board_computer/Project/ -I
../ex2_on_board_computer/libcsp/include/ -I
../ex2_on_board_computer/Source/portable/GCC/POSIX/ -I
../ex2_on_board_computer/libcsp/build/include/ -m32 -lpthread -std=c99 -lrt &&
ar -rsc client_server.a *.o

(sorry for the long ass command)
*/
#include <FreeRTOS.h>
#include <csp/csp.h>
#include <csp/interfaces/csp_if_zmqhub.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <task.h>
#include <unistd.h>

#include "demo.h"
#include "scheduling_service.h"
#include "services.h"
#include "system.h"

service_queues_t service_queues;

void server_loop(void *parameters);
void vAssertCalled(unsigned long ulLine, const char *const pcFileName);
SAT_returnState init_local_gs();

#ifdef USE_LOCAL_GS
#endif

int main(int argc, char **argv) {
  fprintf(stdout, "-- starting command demo --");
  TC_TM_app_id my_address = DEMO_APP_ID;

  if (start_service_handlers() != SATR_OK) {
    printf("COULD NOT START TELECOMMAND HANDLER");
    return -1;
  }

  /* Init CSP with address and default settings */
  csp_conf_t csp_conf;
  csp_conf_get_defaults(&csp_conf);
  csp_conf.address = my_address;
  int error = csp_init(&csp_conf);
  if (error != CSP_ERR_NONE) {
    printf("csp_init() failed, error: %d", error);
    return -1;
  }
  printf("Running at %d\n", my_address);
  /* Set default route and start router & server */
  // csp_route_set(CSP_DEFAULT_ROUTE, &this_interface, CSP_NODE_MAC);
  csp_route_start_task(500, 0);

#ifdef USE_LOCALHOST
  init_local_gs();
// csp_iface_t this_interface = csp_if_fifo;
#else
// implement other interfaces
#endif

  xTaskCreate((TaskFunction_t)server_loop, "SERVER THREAD", 2048, NULL, 1,
              NULL);

  vTaskStartScheduler();

  for (;;) {
  }

  return 0;
}

SAT_returnState init_local_gs() {
  csp_iface_t *default_iface = NULL;
  int error =
      csp_zmqhub_init(csp_get_address(), "localhost", 0, &default_iface);
  if (error != CSP_ERR_NONE) {
    printf("failed to add ZMQ interface [%s], error: %d", "localhost", error);
    return SATR_ERROR;
  }
  csp_rtable_set(CSP_DEFAULT_ROUTE, 0, default_iface, CSP_NO_VIA_ADDRESS);
  return SATR_OK;
}

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
          printf("%s - %d", packet->data, packet->id);
          err = xQueueSendToBack(service_queues.hk_app_queue, packet,
                                 NORMAL_TICKS_TO_WAIT);
          if (err != pdPASS) {
            printf("FAILED TO QUEUE MESSAGE");
          }
          csp_buffer_free(packet);
          break;

        case TC_TEST_SERVICE:
          err = xQueueSendToBack(service_queues.test_app_queue, packet,
                                 NORMAL_TICKS_TO_WAIT);
          if (err != pdPASS) {
            printf("FAILED TO QUEUE MESSAGE");
          }
          csp_buffer_free(packet);
          break;

        case TC_TIME_MANAGEMENT_SERVICE:
          err = xQueueSendToBack(service_queues.time_management_app_queue,
                                 packet, NORMAL_TICKS_TO_WAIT);
          if (err != pdPASS) {
            printf("FAILED TO QUEUE MESSAGE");
          }
          csp_buffer_free(packet);
          break;

        default:
          csp_service_handler(conn, packet);
          break;
      }
    }
    csp_close(conn);
  }
}
