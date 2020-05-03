/*
To build: gcc src/*.c src/telecommand/*.c -c -I include/telecommand/ -I ../ex2_on_board_computer/Source/include/ -I ../ex2_on_board_computer/Project/ -I ../ex2_on_board_computer/libcsp/include/ -I ../ex2_on_board_computer/Source/portable/GCC/POSIX/ -I ../ex2_on_board_computer/libcsp/build/include/ -m32 -lpthread -std=c99 -lrt && ar -rsc client_server.a *.o

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

#include "telecommand/telecommand_handler.h"

csp_iface_t csp_if_fifo = {
    .name = "fifo",
    .nexthop = csp_fifo_tx,
    .mtu = BUF_SIZE,
};

void vAssertCalled(
unsigned long ulLine, const char * const pcFileName
)
{
		printf("error line: %lu in file: %s", ulLine, pcFileName);
}

int main(int argc, char **argv) {
    fifo_data *data = pvPortMalloc(sizeof(fifo_data));

    if (!start_telecommand_handler()) {
      printf("COULD NOT START TELECOMMAND HANDLER");
      return -1;
    }

    /* Run as either server or client */
    if (argc != 2) {
        printf("usage: %s <server/client>\r\n", argv[0]);
        return -1;
    }

    /* Set type */
    if (strcmp(argv[1], "server") == 0) {
        data->me = 1;
        data->other = 2;
        data->tx_channel_name = "server_to_client";
        data->rx_channel_name = "client_to_server";
        data->type = TYPE_SERVER;
				xTaskCreate( (TaskFunction_t) server_loop, "SERVER THREAD", 2048, NULL, 1, &server_loop_thread);
    } else if (strcmp(argv[1], "client") == 0) {
        data->me = 2;
        data->other = 1;
        data->tx_channel_name = "client_to_server";
        data->rx_channel_name = "server_to_client";
        data->type = TYPE_CLIENT;
				xTaskCreate( (TaskFunction_t) client_loop, "CLIENT THREAD", 2048, (void *) data, 1, &client_loop_thread);
    } else {
        printf("Invalid type. Must be either 'server' or 'client'\r\n");
        return -1;

    }

    /* Init CSP and CSP buffer system */
    if (csp_init(data->me) != CSP_ERR_NONE || csp_buffer_init(64, 512) != CSP_ERR_NONE) {
        printf("Failed to init CSP\r\n");
        return -1;
    }

		tx_channel = open(data->tx_channel_name, O_RDWR);
		if (tx_channel < 0) {
				printf("Failed to open TX channel\r\n");
				return -1;
		}

		rx_channel = open(data->rx_channel_name, O_RDWR);
		if (rx_channel < 0) {
				printf("Failed to open RX channel\r\n");
				return -1;
		}

    /* Start fifo RX task */
	 	xTaskCreate( (TaskFunction_t) fifo_rx, "RX_THREAD", 2048, NULL, 1, &rx_thread);

    /* Set default route and start router */
    csp_route_set(CSP_DEFAULT_ROUTE, &csp_if_fifo, CSP_NODE_MAC);
    csp_route_start_task(0, 0);

    vTaskStartScheduler();

		for(;;) {}

    close(rx_channel);
    close(tx_channel);

    return 0;
}
