/*
To build: gcc client_server.c -o client_server.o -c -I
../ex2_on_board_computer/Source/include/ -I ../ex2_on_board_computer/Project/ -I
../ex2_on_board_computer/libcsp/include/ -I
../ex2_on_board_computer/Source/portable/GCC/POSIX/ -I
../ex2_on_board_computer/libcsp/build/include/ -m32 -lpthread -std=c99 -lrt &&
ar -rsc client_server.a *.o (sorry for the long ass command)
*/

/* This file should really be in CSP, since it's technically the link layer */

#include "FreeRTOS.h"
#include "if_fifo.h"
#include <csp/arch/csp_thread.h>
#include <csp/csp.h>
#include <csp/csp_interface.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <task.h>
#include <unistd.h>
#include "system.h"

extern csp_iface_t csp_if_fifo;
uint16_t telecommandId = 0;

int csp_fifo_tx(csp_iface_t *ifc, csp_packet_t *packet, uint32_t timeout) {
  /* Write packet to fifo */
  if (write(tx_channel, &packet->length,
            packet->length + sizeof(uint32_t) + sizeof(uint16_t)) < 0)
    printf("Failed to write frame\r\n");
  csp_buffer_free(packet);
  return CSP_ERR_NONE;
}

void fifo_rx(void *parameters) {
  /* Wait for packet on fifo */
  csp_packet_t *buf = csp_buffer_get(TM_TC_BUFF_SIZE);
  for (;;) {
    while (read(rx_channel, &buf->length, TM_TC_BUFF_SIZE) > 0) {
      csp_new_packet(buf, &csp_if_fifo, NULL);
      buf = csp_buffer_get(TM_TC_BUFF_SIZE);
    }
  }
}
