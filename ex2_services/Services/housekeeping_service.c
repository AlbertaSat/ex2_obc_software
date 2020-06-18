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
  uint8_t ser_subtype = (uint8_t)pkt->data[0];
  switch (ser_subtype) {
    case HK_PARAMETERS_REPORT: 
	    if((tc_hk_para_rep(pkt, NORMAL_TICKS_TO_WAIT)) == SATR_OK){
	    		csp_log_info("HK_REPORT_PARAMETERS TASK FINISHED");
		}
	      		csp_buffer_free(pkt);
	      		ground_response_task();
	      		break;
	    
    default:
    	csp_log_error("HK SERVICE NOT FOUND SUBTASK");
    	csp_buffer_free(pkt);
    	break;
	}
	return SATR_OK;
}

csp_packet_t* hk_para_rep(){
		csp_packet_t *packet = csp_buffer_get(100);
		if (packet == NULL) {
			/* Could not get buffer element */
			csp_log_error("Failed to get CSP buffer");
			csp_buffer_free(packet);
			return NULL;
		}
		snprintf((char *) packet->data[1], csp_buffer_data_size(), "HK Data Sample -- EPS CRRENT: 23mA", ++count);
		/*tranfer the task from TC to TM*/
		packet->data[0] = TM_HK_PARAMETERS_REPORT;
		packet->length = (strlen((char *) packet->data) + 1);

		return packet;
}

SAT_returnState tc_hk_para_rep(csp_packet_t *packet, uint32_t timeout){
		/*execute #25 subtask: parameter report, collecting data from platform*/
		packet = hk_para_rep();
		if(packet->data == NULL){
			csp_log_info("HOUSEKEEPING SERVICE REPORT: DATA COLLECTING FAILED")
				return SATR_ERROR;
			}
			if(xQueueSendToBack(response_queue, &packet, timeout) == pdPASS){
				csp_log_info("HOUSEKEEPING SERVICE REPORT: SENT PACKET BACK TO QUEUE, ID: %d\n", packet->id);
			}else{
				csp_log_error("HOUSEKEEPING SERVICE REPORT: ERROR. THE QUEUE WAS FULL, ID: %d\n", packet->id);
				return SATR_ERROR;
			}
		return SATR_OK;
}

// SAT_returnState tm_hk_para_rep(uint32_t timeout){  //csp_conn_t *conn
	 	
// 	 	/*read packet from hk_app_queue*/
// 		if (xQueueReceive(service_queues.hk_app_queue, &packet, NORMAL_TICKS_TO_WAIT) == pdPASS) {
// 		      csp_log_info("HOUSEKEEPING SERVICE RX: %.*s, ID: %d\n", packet.length,
// 		             (char *)packet.data, packet.id);
// 		//       if (!csp_send(conn, packet, 1000)) {
// 		// 		/* Send failed */
// 		// 	  csp_log_error("Send to ground failed");
// 		// 	  csp_buffer_free(packet);
// 		// 	  return HK_PR_ERR;
// 		}
// 		// csp_buffer_free(packet);
// 		return SATR_OK;
// }