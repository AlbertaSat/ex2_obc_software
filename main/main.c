/*
 * Copyright (C) 2021  University of Alberta
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
#include <csp/csp.h>
#include <csp/drivers/usart.h>
#include <csp/interfaces/csp_if_can.h>
#include <performance_monitor/system_stats.h>
#include <redconf.h>
#include <redfs.h>
#include <redfse.h>
#include <redposix.h>
#include <redvolume.h>
#include <util/service_utilities.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <os_task.h>

#include "main/system.h"
#include "board_io_tests.h"
#include "services.h"
#include "subsystems_ids.h"
#include "eps.h"
#include "mocks/mock_eps.h"
#include "csp/drivers/can.h"
#include "HL_sci.h"
#include "HL_sys_common.h"
#include "system_tasks.h"
#include "mocks/rtc.h"
#include "logger/logger.h"
#include "file_delivery_app.h"

/**
 * The main function must:
 *  - Define the Service_Queues_t service_queues;
 *  - Start the services handlers
 *  - Initialize CSP and the desired interface
 *  - Start the service server, and response server
 *  - Start all platform specific tasks, structures
 *  - Start the FreeRTOS scheduler
 */

#include "test_main.h"
#define DEV_RUN_TEST_SUITE 0

#define LEOP_SEQUENCE_TIMER_MS 10000

static void init_filesystem();
static void init_csp();
static void init_software();
static inline SAT_returnState init_csp_interface();
static void init_system_tasks();
void vAssertCalled(unsigned long ulLine, const char *const pcFileName);

void run_test(void* args) {
  test_main();
  for(;;);
}

int ex2_main(int argc, char **argv) {

#if DEV_RUN_TEST_SUITE == 1
  if (xTaskCreate((TaskFunction_t)run_test, "test", 4800, NULL,
                  1, NULL) != pdPASS) {
    return 1;
  }
  vTaskStartScheduler();
  for (;;);
  return 0;
#endif

  const TickType_t leop_time_ms = pdMS_TO_TICKS(LEOP_SEQUENCE_TIMER_MS);

  _enable_IRQ_interrupt_(); // enable inturrupts
  InitIO();

  /* Initialization routine */
  //init_filesystem();
  init_csp();
  /* Start service server, and response server */
  init_software();

//  start_eps_mock();
/*
  FTP app;
  void *task_handler = create_ftp_task(OBC_APP_ID, &app);
  if (task_handler == NULL) {
      return -1;
  }
*/
  /* Start FreeRTOS! */
  vTaskStartScheduler();

  for (;;); // Scheduler didn't start
}

/**
 * Initialize service and system tasks
 */
void init_software() {
    /* start system tasks and service listeners */
    if (start_service_server() != SATR_OK ||
        start_system_tasks() != SATR_OK) {
      ex2_log("Initialization error\n");
    }
}

/**
 * Initialize reliance edge file system
 */
static void init_filesystem() {
  int32_t iErr;
  gioToggleBit(gioPORTA, 0U);
  const char *pszVolume0 = gaRedVolConf[0].pszPathPrefix;
  iErr = red_init();

  if (iErr == -1) {
    exit(red_errno);
  }

  iErr = red_format(pszVolume0);
  if (iErr == -1) {
    exit(red_errno);
  }

  iErr = red_mount(pszVolume0);

  if (iErr == -1) {
    exit(red_errno);
  }
}

/**
 * Initialize CSP network
 */
static void init_csp() {
  TC_TM_app_id my_address = OBC_APP_ID;

  /* Init CSP with address and default settings */
  csp_conf_t csp_conf;
  csp_conf_get_defaults(&csp_conf);
  csp_conf.address = my_address;
  int error = csp_init(&csp_conf);
  if (error != CSP_ERR_NONE) {
    //ex2_log("csp_init() failed, error: %d\n", error);
    exit(SATR_ERROR);
  }
  //ex2_log("Running at %d\n", my_address);
  /* Set default route and start router & server */
  csp_route_start_task(1000, 2);
  if (init_csp_interface() != SATR_OK) {
    exit(SATR_ERROR);
  }
  return;
}

/**
 * Initialize CSP interfaces
 * @details
 * 		start the localhost zmq server and add it to the default route
 * with no VIA address
 */
static inline SAT_returnState init_csp_interface() {
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

#ifndef EPS_IS_STUBBED
  csp_rtable_load("16 KISS, 4 CAN");
#else
  csp_rtable_load("16 KISS");
#endif

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

// TODO: This might need to be put in application_defined_privileged_functions.h
uint32 getProfilerTimerCount()
{
    RAISE_PRIVILEGE;
    uint32_t ret = _pmuGetCycleCount_() / GCLK_FREQ;
    RESET_PRIVILEGE;
    return ret;
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName ) {
    for(;;);
}

void vApplicationMallocFailedHook( void ) {
    for(;;);
}

void vApplicationDaemonTaskStartupHook( void ) {
    init_logger_queue();
}

void SciSendBuf( char *buf, uint32_t bufSize )
{
    while ( bufSize > 0 && *buf != '\0' ) {
        sciSend(sciREG4, 1, *buf);
        buf++;
        bufSize--;
    }
}
