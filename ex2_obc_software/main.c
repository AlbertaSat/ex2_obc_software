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
 * @file main.c
 * @author Andrew Rooney, Haoran Qi
 * @date 2020-06-06
 */

#include <FreeRTOS.h>
#include <TempSensor/TempSensor.h>
#include <csp/csp.h>
#include <csp/drivers/usart.h>
#include <csp/interfaces/csp_if_can.h>
#include <csp/drivers/can.h>
#include <ex2_system/system_stats.h>
#include <redconf.h>
#include <redfs.h>
#include <redfse.h>
#include <redposix.h>
#include <redvolume.h>
#include <service_utilities.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <task.h>

#include "board_io_tests.h"
#include "service_response.h"
#include "services.h"
#include "system.h"  // platform definitions
#include "subsystems_ids.h"
#include "eps.h"
#include "mocks/mock_eps.h"
#include "ex2_system/housekeeping_task.h"
#include "csp/drivers/can.h"
#include "HL_sci.h"
#include "HL_sys_common.h"

/**
 * The main function must:
 *  - Define the Service_Queues_t service_queues;
 *  - Start the services handlers
 *  - Initialize CSP and the desired interface
 *  - Start the service server, and response server
 *  - Start all platform specific tasks, structures
 *  - Start the FreeRTOS scheduler
 */

/* Create handler mutexes */
Equipment_Mutex_t equipment_mutex;

void vAssertCalled(unsigned long ulLine, const char *const pcFileName);
static inline SAT_returnState init_interface();

int ex2_main(int argc, char **argv) {
  int32_t iErr;

  InitIO();
//  gioToggleBit(gioPORTA, 0U);
//  const char *pszVolume0 = gaRedVolConf[0].pszPathPrefix;
//  iErr = red_init();
//
//  if (iErr == -1) {
//    exit(red_errno);
//  }
//
//  iErr = red_format(pszVolume0);
//  if (iErr == -1) {
//    exit(red_errno);
//  }
//
//  iErr = red_mount(pszVolume0);
//
//  if (iErr == -1) {
//    exit(red_errno);
//  }

  ex2_log("-- starting command demo --\n");
  TC_TM_app_id my_address = EPS_APP_ID;

  /* Init CSP with address and default settings */
  csp_conf_t csp_conf;
  csp_conf_get_defaults(&csp_conf);
  csp_conf.address = my_address;
  int error = csp_init(&csp_conf);
  if (error != CSP_ERR_NONE) {
    ex2_log("csp_init() failed, error: %d\n", error);
    return -1;
  }
  ex2_log("Running at %d\n", my_address);
  /* Set default route and start router & server */
  csp_route_start_task(1000, 2);
  init_interface();

  /* Start service server, and response server */
  if (start_service_server() != SATR_OK ||
      start_detection_server() != SATR_OK ||
      start_task_stats() != SATR_OK) {
    ex2_log("Initialization error\n");
    return -1;
  }
  start_eps_mock();
//  start_housekeeping_daemon();

  /* Start FreeRTOS! */
  vTaskStartScheduler();

  for (;;)
    ;

  return 0;
}

/**
 * @brief
 * 		initialize zmq interface, and configure the routing table
 * @details
 * 		start the localhost zmq server and add it to the default route
 * with no VIA address
 */
static inline SAT_returnState init_interface() {
  csp_iface_t *uart_iface = NULL;
  csp_iface_t *can_iface = NULL;
  csp_usart_conf_t conf = {.device = "UART",
                           .baudrate = 9600, /* supported on all platforms */
                           .databits = 8,
                           .stopbits = 2,
                           .paritysetting = 0,
                           .checkparity = 0};

  int error = csp_can_open_and_add_interface("CAN", &can_iface);
  if (error != CSP_ERR_NONE) {
    return SATR_ERROR;
  }

  error = csp_usart_open_and_add_kiss_interface(
      &conf, CSP_IF_KISS_DEFAULT_NAME, &uart_iface);
  if (error != CSP_ERR_NONE) {
    return SATR_ERROR;
  }


  csp_rtable_load("1 CAN, 16 CAN 1");

  return SATR_OK;
}


void vAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
    /* Called if an assertion passed to configASSERT() fails.  See
    http://www.freertos.org/a00110.html#configASSERT for more information. */

    /* Parameters are not used. */
    ( void ) ulLine;
    ( void ) pcFileName;

    ex2_log( "ASSERT! Line %d, file %s\r\n", ulLine, pcFileName);
    for(;;);
}

static void prvSaveTraceFile( void )
{
    // TODO: implement this with relianceEdge
}

void initializeProfiler()
{
    /* Enable PMU Cycle Counter for Profiling */
    RAISE_PRIVILEGE;
    _pmuInit_();
    _pmuEnableCountersGlobal_();
    _pmuResetCycleCounter_();
    _pmuStartCounters_(pmuCYCLE_COUNTER);
    RESET_PRIVILEGE;
}

uint32 getProfilerTimerCount()
{
    RAISE_PRIVILEGE;
    return _pmuGetCycleCount_() / GCLK_FREQ;
    portRESET_PRIVILEGE( xRunningPrivileged );
    RESET_PRIVILEGE;
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName ) {
    for(;;);
}

void vApplicationMallocFailedHook( void ) {
    for(;;);
}

void SciSendBuf( char *buf, uint32_t bufSize )
{
    while ( bufSize > 0 && *buf != '\0' ) {
        sciSend(sciREG4, 1, *buf);
        buf++;
        bufSize--;
    }
}
