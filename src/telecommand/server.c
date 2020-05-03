/*
To build: gcc client_server.c -o client_server.o -c -I ../ex2_on_board_computer/Source/include/ -I ../ex2_on_board_computer/Project/ -I ../ex2_on_board_computer/libcsp/include/ -I ../ex2_on_board_computer/Source/portable/GCC/POSIX/ -I ../ex2_on_board_computer/libcsp/build/include/ -m32 -lpthread -std=c99 -lrt && ar -rsc client_server.a *.o
(sorry for the long ass command)
*/
#include "FreeRTOS.h"
#include <task.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>

#include <csp/csp.h>
#include <csp/arch/csp_thread.h>
#include <csp/csp_interface.h>

#include "telecommand/server.h"
#include "telecommand/telecommand_handler.h"

extern csp_iface_t csp_if_fifo;
uint16_t telecommandId = 0;

int csp_fifo_tx(csp_iface_t *ifc, csp_packet_t *packet, uint32_t timeout) {
    /* Write packet to fifo */
    if (write(tx_channel, &packet->length, packet->length + sizeof(uint32_t) + sizeof(uint16_t)) < 0)
        printf("Failed to write frame\r\n");
    csp_buffer_free(packet);
    return CSP_ERR_NONE;
}

void fifo_rx(void * parameters) {
    /* Wait for packet on fifo */
		csp_packet_t *buf = csp_buffer_get(BUF_SIZE);
    for (;;) {
				while (read(rx_channel, &buf->length, BUF_SIZE) > 0) {
						csp_new_packet(buf, &csp_if_fifo, NULL);
						buf = csp_buffer_get(BUF_SIZE);
				}
    }
}

void response_server(void * parameters) {
  csp_conn_t *conn;
  csp_packet_t *packet;
  telecommand_t *response;
  for(;;) {
        /* SEND RESPONSES TO COMMANDS */
        if (xQueueReceive(response_queue, (void*) &response,
          RESPONSE_TICKS_TO_WAIT) == pdPASS) {
            packet = csp_buffer_get(BUF_SIZE);
            if (packet) {
              packet->length = BUF_SIZE;
              conn = csp_connect(CSP_PRIO_NORM, CLIENT, PORT, 1000, CSP_O_NONE);
              printf("Sending: %s\r\n", packet->data);
              if (!conn || !csp_send(conn, packet, 1000))
                  printf("Send failed\n"); // log an error
              csp_close(conn);
            }
        }
    }
}

void server_loop(void * parameters) {
    csp_socket_t *sock;
    csp_conn_t *conn;
    csp_packet_t *packet;
    telecommand_t incoming_telecommand;

    response_queue = xQueueCreate (
      (unsigned portBASE_TYPE) RESPONSE_QUEUE_LENGTH,
      (unsigned portBASE_TYPE) RESPONSE_QUEUE_SIZE
    );

    if (!response_queue) {
      printf("COULD NOT CREATE REPONSE QUEUE");
    }

    xTaskCreate(
      (TaskFunction_t) response_server,
      "response server",
      2048,
      NULL,
      RESPONSE_SERVER_PRIO,
      &response_server_task
    );

    /* Create socket and listen for incoming connections */
    sock = csp_socket(CSP_SO_NONE);
    csp_bind(sock, CSP_ANY);
    csp_listen(sock, 5);
    portBASE_TYPE err;

    /* Super loop */
    for(;;) {
        /* Process incoming packet */
        if ((conn = csp_accept(sock, 10000)) == NULL) {
          /* timeout */
          continue;
        }
        while ((packet = csp_read(conn, 50)) != NULL) {
            switch (csp_conn_dport(conn)) {
                case TELECOMMAND_PORT:
                  strcpy(incoming_telecommand.data, packet->data);
                  incoming_telecommand.id = telecommandId++;
                  // Send event to telecommand handler
                  err = xQueueSendToBack(
                    telecommand_queue,
                    (void *) &incoming_telecommand,
                    TELECOMMAND_TICKS_TO_WAIT
                  );

                  if (err != pdPASS) {
                    printf("FAILED TO QUEUE MESSAGE");
                  }
                  csp_buffer_free(packet);
                  break;
                default:
          				/* Call the default CSP service handler, handle pings, buffer use, etc. */
          				csp_service_handler(conn, packet);
                  csp_buffer_free(packet);
          				break;
            }
        }
        csp_close(conn);
    }
}
