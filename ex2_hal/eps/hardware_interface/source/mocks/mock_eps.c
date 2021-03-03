/*
 * mock_eps.c
 *
 *  Created on: Jan. 28, 2021
 *      Author: Andrew
 */
#include <FreeRTOS.h>
#include <os_task.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>

#include "util/service_utilities.h"
#include "main/system.h"
#include "eps.h"

static SAT_returnState prv_fill_dummy_data(csp_packet_t *packet);
void nop();

void mock_eps(void * param) {
  csp_socket_t *sock;
  sock = csp_socket(CSP_SO_CRC32REQ);
  csp_bind(sock, EPS_INSTANTANEOUS_TELEMETRY);
  csp_listen(sock, SERVICE_BACKLOG_LEN);

  for(;;) {
    csp_conn_t *conn;
    csp_packet_t *packet;
    if ((conn = csp_accept(sock, CSP_MAX_TIMEOUT)) == NULL) {
      continue;
    }
    while ((packet = csp_read(conn, 50)) != NULL) {
      if (prv_fill_dummy_data(packet) != SATR_OK) {
        // something went wrong in the service
        csp_buffer_free(packet);
      } else {
        if (!csp_send(conn, packet, 50)) {
          csp_buffer_free(packet);
        }
      }
    }
    csp_close(conn);
  }
}

SAT_returnState start_eps_mock(void) {
  if (xTaskCreate((TaskFunction_t)mock_eps,
                  "mock_eps", 1024, NULL, NORMAL_SERVICE_PRIO,
                  NULL) != pdPASS) {
    ex2_log("FAILED TO CREATE TASK mock_eps\n");
    return SATR_ERROR;
  }
  ex2_log("Service handlers started\n");
  return SATR_OK;
}

static SAT_returnState prv_fill_dummy_data(csp_packet_t *packet) {
    char subsrv = (uint8_t) packet->data[0];
    switch (subsrv) {
    case 0:
        // get instantaneous telemetry
        nop();
        eps_instantaneous_telemetry_t telem = {
                                               .cmd = 0,
                                               .status = 0,
                                               .mpptConverterVoltage = {1, 2, 3, 4},
                                               .curSolarPanels = {1, 2, 3, 4},
                                               .vBatt = 10,
                                               .curSolar = 10,
                                               .curBattIn = 10,
                                               .curBattOut = 10,
                                               .reserved1 = 0,
                                               .curOutput = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
                                               .AOcurOutput = {1, 2},
                                               .OutputConverterVoltage = {1, 2, 3, 4},
                                               .outputConverterState = {1, 2, 3, 4},
                                               .outputStatus = 10,
                                               .outputFaultStatus = 10,
                                               .outputOnDelta = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
                                               .outputOffDelta = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
                                               .outputFaultCnt = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10},
                                               .reserved2 = {1, 2, 3, 4, 5, 6},
                                               .wdt_gs_time_left = 10,
                                               .reserved3 = {1, 2, 3, 4, 5, 6, 7},
                                               .wdt_gs_counter = 10,
                                               .reserved4 = {1, 2, 3, 4},
                                               .temp = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
                                               .rstReason = 0x14000000,
                                               .bootCnt = 10,
                                               .battMode = 2,
                                               .mpptMode = 2,
                                               .batHeaterMode = 1,
                                               .batHeaterState = 1,
                                               .reserved5 = 0
        };
//        prv_instantaneous_telemetry_letoh(&telem);
        memcpy(&packet->data, &telem, sizeof(eps_instantaneous_telemetry_t));
        set_packet_length(packet, sizeof(eps_instantaneous_telemetry_t));
    }
    return SATR_OK;
}

void nop() {
    return;
}

