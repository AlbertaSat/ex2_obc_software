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
 * @file service_reponse.c
 * @author Haoran Qi, Andrew Rooney
 * @date 2020-06-06
 */
#include "service_response.h"

#include <FreeRTOS.h>
#include <csp/csp.h>
#include <stdint.h>
#include <task.h>

#include "service_utilities.h"
#include "services.h"
#include "system.h"

extern Service_Queues_t service_queues;  // Implemented by the host platform

/**
 * @brief
 * 		Wait on a queue of responses to be sent to other CSP nodes
 *              (usually the ground)
 * @details
 * 		CSP client server will wake when data is in the queue for
 *              downlink (telemetery)
 * @param void * param
 * 		Not used
 * @return
 * 		A SAT_returnState depending on whether we were successful
 *              in connecting. Not typically used for anything.
 */
SAT_returnState service_response_task(void *param) {
  TC_TM_app_id my_address = SYSTEM_APP_ID;
  csp_packet_t *packet;
  uint32_t in;
  for (;;) {

    if (xQueueReceive(service_queues.response_queue, &packet,
                      NORMAL_TICKS_TO_WAIT) == pdPASS) {
      cnv8_32(&packet->data[DATA_BYTE], &in);
      csp_log_info("SERVICE_RESPONSE_TASK Set to %u\n", (uint32_t)in);
    
      // csp_connect(priority, destination address, dest. port, timeout(ms), options);
      // TODO: use the variable for the dport instead (2 is for GET_TIME specifically)
      csp_conn_t *conn = csp_connect(CSP_PRIO_NORM, GND_APP_ID, 2, 1000, CSP_O_NONE);

      if (conn == NULL) {
	csp_log_error("Failed to get CSP CONNECTION");
	return SATR_ERROR;
      }
   
      /* Send packet to ground */
      if (!csp_send(conn, packet, 1000)) { 
	csp_log_error("Send failed");
	csp_buffer_free(packet);
      }
    
      /* Close connection */
      csp_log_info("Packet sent to ground.\n");
      csp_close(conn);
    }
  }

  csp_buffer_free(packet);
  csp_log_info("Packet buffer freed.\n");
  
  return SATR_OK;
}

SAT_returnState queue_response(csp_packet_t *packet) {
  if (xQueueSendToBack(service_queues.response_queue, (void *)&packet,
                       NORMAL_TICKS_TO_WAIT) != pdPASS) {
    return SATR_ERROR;
  }
  return SATR_OK;
}

/**
 * @brief
 * 		Start the response_queue, and response task
 * @details
 * 		intitializes the FreeRTOS queue and task
 * @param void
 * @return SAT_returnState
 * 		success or failure
 */
SAT_returnState start_service_response() {
  if (!(service_queues.response_queue =
            xQueueCreate((unsigned portBASE_TYPE)RESPONSE_QUEUE_LEN,
                         (unsigned portBASE_TYPE)CSP_PKT_QUEUE_SIZE))) {
    return SATR_ERROR;
  }

  if (xTaskCreate((TaskFunction_t)service_response_task, "RESPONSE SERVER",
                  1024, NULL, 1, NULL) != pdPASS) {
    return SATR_ERROR;
  }
  return SATR_OK;
}
