// #ifndef __SERVER__
// #define __SERVER__
//
// #include <csp/arch/csp_thread.h>
// #include <csp/csp.h>
// #include <csp/csp_interface.h>
// #include <task.h>
//
// #include "FreeRTOS.h"
// #include "telecommand/telecommand_handler.h"
//
// #define TYPE_SERVER 1
// #define TYPE_CLIENT 2
// #define PORT 10
// #define BUF_SIZE 250
// #define SERVER 1
// #define CLIENT 2
// #define TELECOMMAND_PORT 1
// #define RESPONSE_TICKS_TO_WAIT 1
// #define RESPONSE_SERVER_PRIO 5
// #define RESPONSE_QUEUE_LENGTH 16
// #define RESPONSE_QUEUE_SIZE (sizeof(telecommand_t))
//
// TaskHandle_t rx_thread, server_loop_thread, client_loop_thread,
//     response_server_task;
// xQueueHandle response_queue;
// int rx_channel, tx_channel;
//
// int csp_fifo_tx(csp_iface_t *ifc, csp_packet_t *packet, uint32_t timeout);
//
// void client_loop(void *parameters);
// void server_loop(void *parameters);
// void fifo_rx(void *parameters);
//
// typedef struct {
//   int type, me, other;
//   char *rx_channel_name, *tx_channel_name;
// } fifo_data;
//
// csp_iface_t csp_if_fifo;
//
// #endif
