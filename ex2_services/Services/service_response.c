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

#include "system.h"

extern Service_Queues_t service_queues;  // Implemented by the host platform

/**
 * @brief
 * 		Wait on a queue of responses to be sent to other CSP nodes (usually
 * the ground)
 * @details
 * 		CSP client server will wake when data is in the queue for downlink
 * (telemetery)
 * @param void * param
 * 		Not used
 * @return
 * 		should not return
 */
void service_response_task(void *param) {
  TC_TM_app_id my_address = SYSTEM_APP_ID;
  csp_packet_t packet;
  for (;;) {
    /* To get conn from the response queue */
    if (xQueueReceive(service_queues.response_queue, &packet,
                      NORMAL_TICKS_TO_WAIT) == pdPASS) {
      ex2_log("%d", packet.data[0]);
    }

    // if (conn == NULL) {
    //   /* Could not get buffer element */
    //   csp_log_error("Failed to get CSP CONNECTION");
    //   return SATR_ERROR;
    // }
    //
    // /*  Send packet to ground */
    // if (!csp_send(conn, packet, 1000)) {
    //   /* Send failed */
    //   csp_log_error("Send failed");
    //   csp_buffer_free(packet);
    // }
    //
    // /*  Close connection */
    // sent_count++;
    // csp_log_info("#%d PACKET HAS BEEN SENT TO GROUND\n", sent_count);
    // csp_close(conn);
  }

  return;
}
