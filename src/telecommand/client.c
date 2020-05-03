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

void client_loop(void * parameters) {
		fifo_data* data = ((void*) parameters);
		csp_conn_t *conn;
		csp_packet_t *packet;
		char buf[BUF_SIZE];
		for(;;) {

        /* Send a new packet */
				if (fgets((char *) buf, BUF_SIZE, stdin)) {
						packet = csp_buffer_get(BUF_SIZE);
						if (packet) {
							strcpy((char *) packet->data, buf);
							packet->length = BUF_SIZE;
							conn = csp_connect(CSP_PRIO_NORM, SERVER, TELECOMMAND_PORT, 1000, CSP_O_NONE);
							printf("Sending: %s\r\n", packet->data);
							if (!conn || !csp_send(conn, packet, 1000))
									printf("Send failed\n"); // log an error
							csp_close(conn);
						}
				}
		}
}
