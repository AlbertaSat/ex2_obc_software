///*
//* Copyright (C) 2009-2015 Texas Instruments Incorporated - www.ti.com
//*
//*
//*  Redistribution and use in source and binary forms, with or without
//*  modification, are permitted provided that the following conditions
//*  are met:
//*
//*    Redistributions of source code must retain the above copyright
//*    notice, this list of conditions and the following disclaimer.
//*
//*    Redistributions in binary form must reproduce the above copyright
//*    notice, this list of conditions and the following disclaimer in the
//*    documentation and/or other materials provided with the
//*    distribution.
//*
//*    Neither the name of Texas Instruments Incorporated nor the names of
//*    its contributors may be used to endorse or promote products derived
//*    from this software without specific prior written permission.
//*
//*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES INCLUDING, BUT NOT
//*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION HOWEVER CAUSED AND ON ANY
//*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//*  INCLUDING NEGLIGENCE OR OTHERWISE ARISING IN ANY WAY OUT OF THE USE
//*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//*
//*/
//
///* USER CODE BEGIN (0) */
///* USER CODE END */
//
///* Include Files */
//
//#include "HL_sys_common.h"
//#include "HL_system.h"
//
///* USER CODE BEGIN (1) */
//
//#include "HL_can.h"
//#include "HL_esm.h"
//#include "HL_sys_core.h"
//
//#define D_COUNT  8
//
//uint32 cnt=0, error =0, tx_done =0;
//uint8 tx_data1[D_COUNT] = {1,2,3,4,5,6,7,8};
//uint8 tx_data2[D_COUNT] = {11,12,13,14,15,16,17,18};
//uint8 tx_data3[D_COUNT] = {21,22,23,24,25,26,27,28};
//uint8 tx_data4[D_COUNT] = {31,32,33,34,35,36,37,38};
//
//uint8 rx_data1[D_COUNT] = {0};
//uint8 rx_data2[D_COUNT] = {0};
//uint8 rx_data3[D_COUNT] = {0};
//uint8 rx_data4[D_COUNT] = {0};
//
//uint8 *dptr=0;
//
//
///* USER CODE END */
//
///** @fn void main(void)
//*   @brief Application main function
//*   @note This function is empty by default.
//*
//*   This function is called after startup.
//*   The user can use this function to implement the application.
//*/
//
///* USER CODE BEGIN (2) */
///* USER CODE END */
//
//void main(void)
//{
///* USER CODE BEGIN (3) */
//
//    /* enable irq interrupt in */
//    _enable_IRQ_interrupt_();
//
//    /** - configuring CAN1 MB1,Msg ID-1 to transmit and CAN2 MB1 to receive */
//    canInit();
//    canEnableloopback(canREG1, Internal_Lbk);
//    canEnableloopback(canREG2, Internal_Lbk);
//
//    /** - enabling error interrupts */
//    canEnableErrorNotification(canREG1);
//    canEnableErrorNotification(canREG2);
//
//    canTransmit(canREG1, canMESSAGE_BOX1, (const uint8 *) &tx_data1[0]);
//    canTransmit(canREG2, canMESSAGE_BOX1, (const uint8 *) &tx_data2[0]);
//
//    while(1){};
///* USER CODE END */
//}
//
///* USER CODE BEGIN (4) */
//
//
///* can interrupt notification */
//void canMessageNotification(canBASE_t *node, uint32 messageBox)
//{
//     if(node==canREG1)
//     {
//         canGetData(canREG1, canMESSAGE_BOX2, (uint8 * )&rx_data1[0]); /* copy to RAM */
//     }
//     if(node==canREG2)
//     {
//         canGetData(canREG2, canMESSAGE_BOX2, (uint8 * )&rx_data2[0]); /* copy to RAM */
//     }
//}
///* USER CODE END */

/*
Cubesat Space Protocol - A small network-layer protocol designed for Cubesats
Copyright (C) 2012 GomSpace ApS (http://www.gomspace.com)
Copyright (C) 2012 AAUSAT3 Project (http://aausat3.space.aau.dk)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <FreeRTOS.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "csp/drivers/can.h"
#include <csp/csp.h>
#include <csp/arch/csp_thread.h>
#include <csp/drivers/usart.h>
#include <csp/drivers/can_socketcan.h>
#include <csp/interfaces/csp_if_zmqhub.h>

/* Server port, the port the server listens on for incoming connections from the client. */
#define MY_SERVER_PORT      10

/* Commandline options */
static uint8_t server_address = 255;

/* test mode, used for verifying that host & client can exchange packets over the loopback interface */
static bool test_mode = false;
static unsigned int server_received = 0;

/* Server task - handles requests from clients */
void task_server(void* params) {

//    csp_log_info("Server task started");

    /* Create socket with no specific socket options, e.g. accepts CRC32, HMAC, XTEA, etc. if enabled during compilation */
    csp_socket_t *sock = csp_socket(CSP_SO_NONE);

    /* Bind socket to all ports, e.g. all incoming connections will be handled here */
    csp_bind(sock, CSP_ANY);

    /* Create a backlog of 10 connections, i.e. up to 10 new connections can be queued */
    csp_listen(sock, 10);

    /* Wait for connections and then process packets on the connection */
    while (1) {

        /* Wait for a new connection, 10000 mS timeout */
        csp_conn_t *conn;
        if ((conn = csp_accept(sock, 10000)) == NULL) {
            /* timeout */
            continue;
        }

        /* Read packets on connection, timout is 100 mS */
        csp_packet_t *packet;
        while ((packet = csp_read(conn, 50)) != NULL) {
            switch (csp_conn_dport(conn)) {
            case MY_SERVER_PORT:
                /* Process packet here */
                csp_log_info("Packet received on MY_SERVER_PORT: %s", (char *) packet->data);
                csp_buffer_free(packet);
                ++server_received;
                break;

            default:
                /* Call the default CSP service handler, handle pings, buffer use, etc. */
                csp_service_handler(conn, packet);
                break;
            }
        }

        /* Close current connection */
        csp_close(conn);

    }

    return CSP_TASK_RETURN;

}
/* End of server task */

/* Client task sending requests to server task */
void task_client (void * params) {

//    csp_log_info("Client task started");

    unsigned int count = 0;

    while (1) {

        csp_sleep_ms(test_mode ? 200 : 1000);

        /* Send ping to server, timeout 1000 mS, ping size 100 bytes */
        int result = csp_ping(server_address, 1000, 100, CSP_O_NONE);
        csp_log_info("Ping address: %u, result %d [mS]", server_address, result);

        /* Send reboot request to server, the server has no actual implementation of csp_sys_reboot() and fails to reboot */
        csp_reboot(server_address);
        csp_log_info("reboot system request sent to address: %u", server_address);

        /* Send data packet (string) to server */

        /* 1. Connect to host on 'server_address', port MY_SERVER_PORT with regular UDP-like protocol and 1000 ms timeout */
        csp_conn_t * conn = csp_connect(CSP_PRIO_NORM, server_address, MY_SERVER_PORT, 1000, CSP_O_NONE);
        if (conn == NULL) {
            /* Connect failed */
//            csp_log_error("Connection failed");
            return CSP_TASK_RETURN;
        }

        /* 2. Get packet buffer for message/data */
        csp_packet_t * packet = csp_buffer_get(100);
        if (packet == NULL) {
            /* Could not get buffer element */
//            csp_log_error("Failed to get CSP buffer");
            return CSP_TASK_RETURN;
        }

        /* 3. Copy data to packet */
        snprintf((char *) packet->data, csp_buffer_data_size(), "Hello World (%u)", ++count);

        /* 4. Set packet length */
        packet->length = (strlen((char *) packet->data) + 1); /* include the 0 termination */

        /* 5. Send packet */
        if (!csp_send(conn, packet, 1000)) {
            /* Send failed */
            csp_log_error("Send failed");
            csp_buffer_free(packet);
        }

        /* 6. Close connection */
        csp_close(conn);
    }

    return CSP_TASK_RETURN;
}
/* End of client task */

/* main - initialization of CSP and start of server/client tasks */
int main(int argc, char * argv[]) {

    uint8_t address = 1;
    csp_debug_level_t debug_level = CSP_INFO;

    const char * rtable = NULL;
    int opt;
    csp_debug_level_t i;
    for (i = 0; i <= CSP_LOCK; ++i) {
        csp_debug_set_level(i, (i <= debug_level) ? true : false);
    }

    /* Init CSP with address and default settings */
    csp_conf_t csp_conf;
    csp_conf_get_defaults(&csp_conf);
    csp_conf.address = address;
    int error = csp_init(&csp_conf);
    if (error != CSP_ERR_NONE) {
        csp_log_error("csp_init() failed, error: %d", error);
        exit(1);
    }

    /* Start router task with 10000 bytes of stack (priority is only supported on FreeRTOS) */
    csp_route_start_task(500, 0);

    /* Add interface(s) */
    csp_iface_t * default_iface = NULL;

    // TODO: make a client/server example with 2 boards
    error = can_init(&default_iface);
    if (error != CSP_ERR_NONE) {
        exit(1);
    }

    csp_rtable_set(CSP_DEFAULT_ROUTE, 0, default_iface, CSP_NO_VIA_ADDRESS);


    /* Start server thread */
    if ((server_address == 255) || (default_iface == NULL) ||1) {
        /* no interfaces specified -> run server & client via loopback */
        xTaskCreate(task_server, "SERVER", 500, NULL, 0, NULL);

    }

    /* Start client thread */
    if ((server_address != 255) ||  /* server address specified, I must be client */
        (default_iface == NULL) || 1) {  /* no interfaces specified -> run server & client via loopback */
        xTaskCreate(task_client, "CLIENT", 500, NULL, 0, NULL);
    }
    vTaskStartScheduler();
    /* Wait for execution to end (ctrl+c) */
    while(1) {
        csp_sleep_ms(3000);

        if (test_mode) {
            /* Test mode is intended for checking that host & client can exchange packets over loopback */
            if (server_received < 5) {
//                csp_log_error("Server received %u packets", server_received);
                exit(1);
            }
//            csp_log_info("Server received %u packets", server_received);
            exit(0);
        }
    }

    return 0;
}
