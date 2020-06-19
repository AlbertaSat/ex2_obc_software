#include "demo.h"

#include <FreeRTOS.h>
#include <stdio.h>

#include "system.h"
#include "task.h"
#include "time_management_service.h"
#include "services.h"

extern service_queues_t service_queues;
/*create a variable to record # of packets sent to ground*/
unsigned int sent_count =0;

static void test_app(void *parameters) {
  csp_packet_t packet;
  for (;;) {
    if (xQueueReceive(service_queues.test_app_queue, &packet,
                      NORMAL_TICKS_TO_WAIT) == pdPASS) {
      printf("TEST SERVICE RX: %.*s, ID: %d\n", packet.length,
             (char *)packet.data, packet.id);
    }
  }
}

static void hk_app(void *parameters) {
  csp_packet_t packet;
  for (;;) {
    if (xQueueReceive(service_queues.hk_app_queue, &packet,
                      NORMAL_TICKS_TO_WAIT) == pdPASS) {
      printf("HOUSEKEEPING SERVICE RX: %.*s, ID: %d\n", packet.length,
             (char *)packet.data, packet.id);
      hk_service_app(&packet);
    }
  }
}

// static void time_management_app_route(void *parameters) {
//   csp_packet_t packet;
//   for (;;) {
//     if (xQueueReceive(service_queues.time_management_app_queue, &packet,
//                       NORMAL_TICKS_TO_WAIT) == pdPASS) {
//       printf("Time time_management_service SERVICE RX: %d, ID: %d\n",
//              packet.data[0], packet.id);
//       // verify a valid sub-service
//       // configASSERT(packet.data[0] <= (unsigned char) 0 && packet.data[0] <
//       // (unsigned char) 0xff); // TODO: figure out the actual range of valid
//       // values
//       time_management_app(&packet);
//     }
//   }
// }

SAT_returnState start_service_handlers() {
  /**
   * Create the queues & tasks for each service implemented by this module
   */
  if (!(service_queues.verification_app_queue =
            xQueueCreate((unsigned portBASE_TYPE)NORMAL_QUEUE_LEN,
                         (unsigned portBASE_TYPE)NORMAL_QUEUE_SIZE))) {
    printf("FAILED TO CREATE VERIFICATION APP QUEUE");
    return SATR_ERROR;
  };

  if (!(service_queues.hk_app_queue =
            xQueueCreate((unsigned portBASE_TYPE)NORMAL_QUEUE_LEN,
                         (unsigned portBASE_TYPE)NORMAL_QUEUE_SIZE))) {
    printf("FAILED TO CREATE HK APP QUEUE");
    return SATR_ERROR;
  };

  if (!(service_queues.test_app_queue =
            xQueueCreate((unsigned portBASE_TYPE)NORMAL_QUEUE_LEN,
                         (unsigned portBASE_TYPE)NORMAL_QUEUE_SIZE))) {
    printf("FAILED TO CREATE TEST APP QUEUE");
    return SATR_ERROR;
  };

  // if (!(service_queues.time_management_app_queue =
  //           xQueueCreate((unsigned portBASE_TYPE)NORMAL_QUEUE_LEN,
  //                        (unsigned portBASE_TYPE)NORMAL_QUEUE_SIZE))) {
  //   printf("FAILED TO CREATE time_management_app_queue");
  //   return SATR_ERROR;
  // };

  xTaskCreate((TaskFunction_t)test_app, "test app", 2048, NULL,
              NORMAL_SERVICE_PRIO, NULL);

  xTaskCreate((TaskFunction_t)hk_app, "hk app", 2048, NULL, NORMAL_SERVICE_PRIO,
              NULL);

  // xTaskCreate((TaskFunction_t)time_management_app_route, "time_management_app",
  //             2048, NULL, NORMAL_SERVICE_PRIO, NULL);

  return SATR_OK;
}

 SAT_returnState ground_response_task(){

    csp_log_info("Sending back to ground station...");

    csp_packet_t *packet;
    portBASE_TYPE err;
    TC_TM_app_id server_address = GND_APP_ID;

    if((err = xQueueReceive(response_queue, packet, 
                                    NORMAL_TICKS_TO_WAIT)) != pdPASS){
        printf("FAILED TO QUEUE MESSAGE TO GROUND");
        csp_buffer_free(packet);
    }

    if (packet == NULL) {
      /* Could not get buffer element */
      csp_log_error("Failed to get CSP buffer");
      csp_buffer_free(packet);
      return SATR_ERROR;
    }

    /* Send ping to server, timeout 1000 mS, ping size 100 bytes */
    int result = csp_ping(server_address, 1000, 100, CSP_O_NONE);
    csp_log_info("Ping address: %u, result %d [mS]", server_address, result);

    /* Send reboot request to server, the server has no actual implementation of csp_sys_reboot() and fails to reboot */
    csp_reboot(server_address);
    csp_log_info("reboot system request sent to address: %u", server_address);

    /* Send data packet (string) to server */

    /* 1. Connect to host on 'ground_address', port RESPONSE_PORT with regular UDP-like protocol and 1000 ms timeout */
    //can different apps map to same port? 
    csp_conn_t * conn = csp_connect(CSP_PRIO_NORM, server_address, TC_HOUSEKEEPING_SERVICE, 1000, CSP_O_NONE);
    if (conn == NULL) {
      /* Connect failed */
      csp_log_error("Connection failed");
      return SATR_ERROR;
    }

    /* 2. Send packet */
    if (!csp_send(conn, packet, 1000)) {
      /* Send failed */
      csp_log_error("Send failed");
      csp_buffer_free(packet);
    }

    /* 3. Close connection */
    sent_count++;
    csp_log_info("#%d PACKET HAS BEEN SENT TO GROUND", sent_count);
    csp_buffer_free(packet);
    csp_close(conn);
 

    return SATR_OK;

/* End of client task */
          
}