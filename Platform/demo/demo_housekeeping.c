// #include "demo.h"

// #include <stdio.h>

// #include <csp/csp.h>
// #include <csp/arch/csp_thread.h>
// #include <csp/drivers/usart.h>
// #include <csp/drivers/can_socketcan.h>
// #include <csp/interfaces/csp_if_zmqhub.h>

// #include "system.h"
// #include "task.h"
// #include "housekeeping_service.h"
// #include "service.h"


// /* test mode, used for verifying that host & client can exchange packets over the loopback interface */
// static bool test_mode = false;

// void hk_request_app(void *parameters) {
//   csp_packet_t packet;

//   csp_log_info("Demo housekeeping task started");

// 	unsigned int count = 0;

// 	while (1) {

// 		//csp_sleep_ms(test_mode ? 200 : 1000);

// 		/* Send ping to server, timeout 1000 mS, ping size 100 bytes */
// 		int result = csp_ping(DEMO_APP_ID, 1000, 100, CSP_O_NONE);
// 		csp_log_info("Ping address: %u, result %d [mS]", DEMO_APP_ID, result);

// 		/* Send reboot request to server, the server has no actual implementation of csp_sys_reboot() and fails to reboot */
// 		csp_reboot(DEMO_APP_ID);
// 		csp_log_info("reboot system request sent to DEMO APP,ID: %u", DEMO_APP_ID);

// 		/* Send data packet (string) to server */

// 		 1. Connect to host on 'DEMO_APP_ID', port HK_PORT with regular UDP-like protocol and 1000 ms timeout 
// 		csp_conn_t * conn = csp_connect(CSP_PRIO_NORM, DEMO_APP_ID, TC_HOUSEKEEPING_SERVICE, 1000, CSP_O_NONE);
// 		if (conn == NULL) {
// 			/* Connect failed */
// 			csp_log_error("Connection failed");
// 			return CSP_TASK_RETURN;
// 		}

// 		/* 2. Send request packet to hk service queue*/
// 		if (!csp_send(conn, packet, 1000)) {
// 			/* Send failed */
// 			csp_log_error("Send failed");
// 			csp_buffer_free(packet);
// 		}else{
// 			csp_log_info("HK APP REQUEST PACKET HAS SENT");
// 			csp_buffer_free(packet);
// 		}

// 		csp_packet_t *temp_packet;

// 		/* 3. Wait for filled packet sent back*/
// 		for(;;){
// 			csp_sleep_ms(10);
// 			temp_packet = csp_read(conn, 50);
// 			if(packet->data == NULL && packet->length == NULL){
// 				csp_log_info("Waiting for fullfilled packet sent back...");
// 				csp_buffer_free(temp_packet);
// 				continue;
// 			}else{
// 				csp_log_info("Packet Received");
// 				break;
// 			}

// 		}
// 		/* 4. Report and clear packet*/
// 		csp_log_info("HOUSEKEEPING SERVICE RX: %.*s, ID: %d\n", packet.length,
// 		             (char *)packet.data, packet.id);
// 		csp_buffer_free(temp_packet);

// 		/* 5. Close connection */
// 		csp_close(conn);
// 		}
// 	}
//   