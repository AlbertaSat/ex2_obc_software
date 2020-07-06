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

unsigned int count = 0;
extern service_queues_t service_queues;


SAT_returnState hk_service_app(csp_packet_t *pkt) {
 printf("Test HK Service 1");
  uint8_t ser_subtype = (uint8_t)pkt->data[0];
 printf("Test HK Service 2");
  switch (ser_subtype) {
    case HK_PARAMETERS_REPORT: 
			printf("Test HK Service 3");
	    	pkt = tc_hk_para_rep(pkt);
			printf("Test HK Service 4");
		    if(pkt->data[1]!= NULL){
		    		csp_log_info("HK_REPORT_PARAMETERS TASK FINISHED");
			  }
			sleep(1);
			printf("Ground Station Task Checkout");
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

/* NB: Basically hk_para_rep will be wrriten in the hardware/platform file.*/

csp_packet_t* hk_para_rep(){
		csp_packet_t *packet = csp_buffer_get(100);
		if (packet == NULL) {
			/* Could not get buffer element */
			csp_log_error("Failed to get CSP buffer");
			csp_buffer_free(packet);
			return NULL;
		}
		snprintf((char *) packet->data[1], csp_buffer_data_size(), "HK Data Sample -- EPS CRRENT: 23mA", ++count);
		//tranfer the task from TC to TM for enabling ground response task
		packet->data[0] = TM_HK_PARAMETERS_REPORT;
		packet->length = (strlen((char *) packet->data) + 1);

		return packet;
}

csp_packet_t* tc_hk_para_rep(csp_packet_t* packet){
		//execute #25 subtask: parameter report, collecting data from platform
		packet = hk_para_rep();
		if(packet->data[1] == NULL){
			  csp_log_info("HOUSEKEEPING SERVICE REPORT: DATA COLLECTING FAILED")
				return SATR_ERROR;
			}
		return packet;
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

/*
 *Below are the elder version of hk service, may not be processed very soothfully
 */

// SAT_returnState hk_service_app(csp_packet_t *pkt) {
//   uint8_t ser_subtype = (uint8_t)pkt->data[0];
//   switch (ser_subtype) {
//     case HK_PARAMETERS_REPORT: 
// 	    if((tc_hk_para_rep(pkt, NORMAL_TICKS_TO_WAIT)) == SATR_OK){
// 	    		csp_log_info("HK_REPORT_PARAMETERS TASK FINISHED");
// 		}
// 	      		csp_buffer_free(pkt);
// 	      		ground_response_task();
// 	      		break;
	    
//     default:
//     	csp_log_error("HK SERVICE NOT FOUND SUBTASK");
//     	csp_buffer_free(pkt);
//     	break;
// 	}
// 	return SATR_OK;
// }

// csp_packet_t* hk_para_rep(){
// 		csp_packet_t *packet = csp_buffer_get(100);
// 		if (packet == NULL) {
// 			/* Could not get buffer element */
// 			csp_log_error("Failed to get CSP buffer");
// 			csp_buffer_free(packet);
// 			return NULL;
// 		}
// 		snprintf((char *) packet->data[1], csp_buffer_data_size(), "HK Data Sample -- EPS CRRENT: 23mA", ++count);
// 		/*tranfer the task from TC to TM*/
// 		packet->data[0] = TM_HK_PARAMETERS_REPORT;
// 		packet->length = (strlen((char *) packet->data) + 1);

// 		return packet;
// }

// SAT_returnState tc_hk_para_rep(csp_packet_t *packet, uint32_t timeout){
// 		/*execute #25 subtask: parameter report, collecting data from platform*/
// 		packet = hk_para_rep();
// 		if(packet->data == NULL){
// 			csp_log_info("HOUSEKEEPING SERVICE REPORT: DATA COLLECTING FAILED")
// 				return SATR_ERROR;
// 			}
// 			if(xQueueSendToBack(response_queue, &packet, timeout) == pdPASS){
// 				csp_log_info("HOUSEKEEPING SERVICE REPORT: SENT PACKET BACK TO QUEUE, ID: %d\n", packet->id);
// 			}else{
// 				csp_log_error("HOUSEKEEPING SERVICE REPORT: ERROR. THE QUEUE WAS FULL, ID: %d\n", packet->id);
// 				return SATR_ERROR;
// 			}
// 		return SATR_OK;
// }
