/*
 * Copyright (C) 2015  University of Alberta
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
/**
 * @file housekeeping_service.c
 * @author Haoran Qi, Andrew Rooney
 * @date 2020-07-07
 */
#include "housekeeping_service.h"

#include <FreeRTOS.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "service_response.h"
#include "service_utilities.h"
#include "services.h"
#include "system.h"

extern Service_Queues_t service_queues;
unsigned int count = 0;

SAT_returnState hk_service_app(csp_packet_t *pkt) {
  uint8_t ser_subtype = (uint8_t)pkt->data[0];
  ex2_log("Test HK Service 2\n");
  switch (ser_subtype) {
    case HK_PARAMETERS_REPORT:
      if (tc_hk_param_rep() != SATR_OK) {
        csp_log_info("HK_REPORT_PARAMETERS TASK FINISHED");
      }
      // ex2_log("Ground Station Task Checkout\n");
      // if (pkt->data[0] ==
      //     TM_HK_PARAMETERS_REPORT) {  // determine if needed
      //                                 // to send back to ground
      //   ex2_log("Enter Loop\n");
      //   ground_response_task(pkt);
      //   ex2_log("Loop Finished\n");
      // }
      break;

    default:
      csp_log_error("HK SERVICE NOT FOUND SUBTASK");
      return SATR_PKT_ILLEGAL_SUBSERVICE;
  }

  return SATR_OK;
}

/* NB: Basically hk_para_rep will be wrriten in the hardware/platform file.*/

csp_packet_t *hk_param_rep() {
  csp_packet_t *packet = csp_buffer_get(100);
  if (packet == NULL) {
    /* Could not get buffer element */
    ex2_log("Failed to get CSP buffer");
    csp_buffer_free(packet);
    return NULL;
  }
  packet->data[1] = 16;
  ++count;
  // tranfer the task from TC to TM for enabling ground response task
  packet->data[0] = (char)TM_HK_PARAMETERS_REPORT;
  packet->length = (strlen((char *)packet->data) + 1);

  return packet;
}

SAT_returnState tc_hk_param_rep() {
  // execute #25 subtask: parameter report, collecting data from platform
  csp_packet_t *packet = hk_param_rep();

  if (packet == NULL) {
    csp_log_info(
        "HOUSEKEEPING SERVICE REPORT: DATA COLLECTING "
        "FAILED") return SATR_ERROR;
  }

  if (xQueueSendToBack(service_queues.response_queue, packet,
                       NORMAL_TICKS_TO_WAIT) != pdPASS) {
    return SATR_ERROR;
  }

  return SATR_OK;
}

/*
 *Below are the elder version of hk service, may not be processed very
 *soothfully
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
// 		snex2_log((char *) packet->data[1], csp_buffer_data_size(), "HK
// Data Sample -- EPS CRRENT: 23mA", ++count);
// 		/*tranfer the task from TC to TM*/
// 		packet->data[0] = TM_HK_PARAMETERS_REPORT;
// 		packet->length = (strlen((char *) packet->data) + 1);

// 		return packet;
// }

// SAT_returnState tc_hk_para_rep(csp_packet_t *packet, uint32_t timeout){
// 		/*execute #25 subtask: parameter report, collecting data from
// platform*/ 		packet = hk_para_rep(); 		if(packet->data == NULL){
// 			csp_log_info("HOUSEKEEPING SERVICE REPORT: DATA COLLECTING
// FAILED") 				return SATR_ERROR;
// 			}
// 			if(xQueueSendToBack(response_queue, &packet, timeout) ==
// pdPASS){ 				csp_log_info("HOUSEKEEPING SERVICE REPORT: SENT PACKET BACK TO
// QUEUE, ID: %d\n", packet->id); 			}else{ 				csp_log_error("HOUSEKEEPING SERVICE
// REPORT: ERROR. THE QUEUE WAS FULL, ID: %d\n", packet->id); 				return SATR_ERROR;
// 			}
// 		return SATR_OK;
// }
