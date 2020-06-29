/*

@ This file is for testing housekeeping service loop only, mainly consisting of main.c, demo.c and housekeeping_service.c 

@ Current testing hardware platform: DEMO  

@ To complie, use command: 

    gcc Platform/demo/demo_housekeeping.c Platform/demo/hal/*.c -c -I . 
    -I Platform/demo -I Platform/hal -I Services/ -I ../upsat-ecss-services/services/ 
    -I ../SatelliteSim/Source/include/ -I ../SatelliteSim/Project/ -I ../SatelliteSim/libcsp/include/ 
    -I ../SatelliteSim/Source/portable/GCC/POSIX/ -I ../SatelliteSim/libcsp/build/include/ 
    -lpthread -std=c99 -lrt && ar -rsc client_server.a *.o

*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <FreeRTOS.h>
#include <csp/csp.h>
#include <csp/arch/csp_thread.h>
#include <csp/drivers/usart.h>
#include <csp/drivers/can_socketcan.h>
#include <csp/interfaces/csp_if_zmqhub.h>

#include "housekeeping_service.h"
#include "services.h"
#include "demo.h"
#include "scheduling_service.h"
#include "system.h"

#ifndef DEMO_APP_ID
#define DEMO_APP_ID 8
#endif

#ifndef GND_APP_ID
#define GND_APP_ID 6
#endif

#ifdef USE_LOCAL_GS
#endif

void server_loop(void *parameters);
void vAssertCalled(unsigned long ulLine, const char *const pcFileName);
SAT_returnState init_local_gs();

unsigned int count = 0;
unsigned int sent_count = 0;
service_queues_t service_queues;
xQueueHandle response_queue;

/*Basically hk_para_rep will be wrriten in the hardware file.*/
csp_packet_t* hk_para_rep(){
		csp_packet_t *packet = csp_buffer_get(100);
		if (packet == NULL) {
			/* Could not get buffer element */
			csp_log_error("Failed to get CSP buffer");
			csp_buffer_free(packet);
			return NULL;
		}
		snprintf((char *) packet->data[1], csp_buffer_data_size(), "HK Data Sample -- EPS CRRENT: 23mA", ++count);
		/*tranfer the task from TC to TM for enabling ground response task*/
		packet->data[0] = TM_HK_PARAMETERS_REPORT;
		packet->length = (strlen((char *) packet->data) + 1);

		return packet;
}

csp_packet_t* tc_hk_para_rep(csp_packet_t *packet){
		/*execute #25 subtask: parameter report, collecting data from platform*/
		packet = hk_para_rep();
		if(packet->data == NULL){
			  csp_log_info("HOUSEKEEPING SERVICE REPORT: DATA COLLECTING FAILED")
				return SATR_ERROR;
			}
		return packet;
}

SAT_returnState hk_service_app(csp_packet_t *pkt) {
  uint8_t ser_subtype = (uint8_t)pkt->data[0];
  switch (ser_subtype) {
    case HK_PARAMETERS_REPORT: 
	    	pkt = tc_hk_para_rep(pkt);
		    if(pkt->data[1]!= NULL){
		    		csp_log_info("HK_REPORT_PARAMETERS TASK FINISHED");
			  }
		  	if(pkt->data[0] == TM_HK_PARAMETERS_REPORT)//determine if needed to send back to ground
	  		 	  ground_response_task(pkt);
	  		break;	    
    default:
	    	csp_log_error("HK SERVICE NOT FOUND SUBTASK");
	    	csp_buffer_free(pkt);
	    	return SATR_ERROR;
	}

	csp_buffer_free(pkt);
	return SATR_OK;
}

SAT_returnState ground_response_task(csp_packet_t *packet){

    csp_log_info("Sending back to ground station...");

    csp_conn_t *conn;

    portBASE_TYPE err;
    TC_TM_app_id server_address = GND_APP_ID;
    /*To get conn from the response queue*/
    if((err = xQueueReceive(response_queue, conn, 
                                    NORMAL_TICKS_TO_WAIT)) != pdPASS){
        printf("FAILED TO QUEUE MESSAGE TO GROUND");
        csp_buffer_free(packet);
    }

    if (conn == NULL) {
      /* Could not get buffer element */
      csp_log_error("Failed to get CSP CONNECTION");
      return SATR_ERROR;
    }

    /*  Send packet to ground */
    if (!csp_send(conn, packet, 1000)) {
      /* Send failed */
      csp_log_error("Send failed");
      csp_buffer_free(packet);
    }

    /*  Close connection */
    sent_count++;
    csp_log_info("#%d PACKET HAS BEEN SENT TO GROUND", sent_count);
    csp_buffer_free(packet);
    csp_close(conn);
 

    return SATR_OK;
          
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

/**
  * Create the queues & tasks for each service implemented by this module
  */
SAT_returnState start_service_handlers() {
  
  if (!(service_queues.hk_app_queue =
            xQueueCreate((unsigned portBASE_TYPE)NORMAL_QUEUE_LEN,
                         (unsigned portBASE_TYPE)NORMAL_QUEUE_SIZE))) {
    printf("FAILED TO CREATE HK APP QUEUE");
    return SATR_ERROR;
  };

  if (!(response_queue =
            xQueueCreate((unsigned portBASE_TYPE)NORMAL_QUEUE_LEN,
                         (unsigned portBASE_TYPE)NORMAL_QUEUE_SIZE))) {
    printf("FAILED TO CREATE RESPONSE QUEUE");
    return SATR_ERROR;
  };

  xTaskCreate((TaskFunction_t)hk_app, "hk app", 2048, NULL, NORMAL_SERVICE_PRIO,
              NULL);

  return SATR_OK;
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
  portBASE_TYPE err,err2;

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
	          err2 = xQueueSendToBack(response_queue, conn,
	                                 NORMAL_TICKS_TO_WAIT);

	          if (err != pdPASS || err2 != pdPASS) {
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

int main(int argc, char **argv){
//int main() {
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