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
 * @author Andrew Rooney, Haoran Qi, Robert Taylor, Dustin Wagner, Arash Yazdani
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
#include <HL_system.h>

#include "main/system.h"
#include "board_io_tests.h"
#include "services.h"
#include "subsystems_ids.h"
#include "mocks/mock_eps.h"
#include "csp/drivers/can.h"
#include "HL_sci.h"
#include "HL_sys_common.h"
#include "system_tasks.h"
#include "mocks/rtc.h"
#include "logger/logger.h"
#include "file_delivery_app.h"
#include "ads7128.h"
#include "pcal9538a.h"
#include "skytraq_gps.h"

#include <FreeRTOS.h>
#include <os_task.h>
#include <os_semphr.h>
#include "uhf.h"
#include "eps.h"
#include "sband.h"
#include "system.h"

#include "sband_binary_tests.h"
#include "deployablescontrol.h"
#include "HL_system.h"

/**
 * The main function must:
 *  - Define the Service_Queues_t service_queues;
 *  - Start the services handlers
 *  - Initialize CSP and the desired interface
 *  - Start the service server, and response server
 *  - Start all platform specific tasks, structures
 *  - Start the FreeRTOS scheduler
 */

#define INIT_PRIO configMAX_PRIORITIES - 1
#define INIT_STACK_SIZE 3000

static void init_filesystem();
static void init_csp();
static void init_software();
static void flatsat_test();
static inline SAT_returnState init_csp_interface();
void vAssertCalled(unsigned long ulLine, const char *const pcFileName);
static FTP ftp_app;

void ex2_init(void *pvParameters) {

    /* Initialization routine */

    /* Hardware Initialization */

    init_filesystem();

#ifndef ADCS_IS_STUBBED
    // PLACEHOLDER: adcs hardware init
#endif

#ifndef ATHENA_IS_STUBBED
    // PLACEHOLDER: athena hardware init
#endif

#ifndef EPS_IS_STUBBED
    // PLACEHOLDER: eps hardware init
#endif

#ifndef UHF_IS_STUBBED
    uhf_i2c_init();
#endif

#ifndef SBAND_IS_STUBBED
    STX_Enable();
    // PLACEHOLDER: sband hardware init
#endif

#ifndef CHARON_IS_STUBBED
    gps_skytraq_driver_init();
    ads7128Init();
    setuppcal9538a();
#endif


    /* Software Initialization */

    /* Start service server, and response server */
    init_csp();
    init_software();
    // create_ftp_task(OBC_APP_ID, &ftp_app);

#ifdef FLATSAT_TEST
    /* Test Task */
    xTaskCreate(flatsat_test, "flatsat_test", 5000, NULL, 4, NULL);
#endif

    vTaskDelete(0); // delete self to free up heap
}

void flatsat_test(void *pvParameters) {
    sband_binary_test();
    uhf_binary_test();

    vTaskDelete(NULL);
}

int ex2_main(void) {
    _enable_IRQ_interrupt_(); // enable inturrupts
    InitIO();
    for (int i = 0; i < 1000000; i++);
    xTaskCreate(ex2_init, "init", INIT_STACK_SIZE, NULL, INIT_PRIO, NULL);

    /* Start FreeRTOS! */
    vTaskStartScheduler();

    for (;;); // Scheduler didn't start
}

/**
 * Initialize service and system tasks
 */
void init_software() {
    /* start system tasks and service listeners */
    if (start_system_tasks() != SATR_OK || start_service_server() != SATR_OK) {
        ex2_log("Initialization error\n");
    }
}

/**
 * Initialize reliance edge file system
 */
static void init_filesystem() {
#if defined(HAS_SD_CARD) // TODO: tolerate non-existent SD Card
    int32_t iErr = 0;
    const char *pszVolume0 = gaRedVolConf[0].pszPathPrefix;
    iErr = red_init();

    if (iErr == -1) {
        exit(red_errno);
    }

#ifdef SD_CARD_REFORMAT

    iErr = red_format(pszVolume0);
    if (iErr == -1) {
        exit(red_errno);
    }

#endif

    iErr = red_mount(pszVolume0);

    if (iErr == -1) {
        exit(red_errno);
    }

#ifdef IS_ATHENA_V2 // TODO: make this IS_ATHENA once V2 is actively used
    iErr = 0;
    const char *pszVolume1 = gaRedVolConf[1].pszPathPrefix;

    iErr = red_format(pszVolume1);
    if (iErr == -1) {
        exit(red_errno);
    }

    iErr = red_mount(pszVolume1);

    if (iErr == -1) {
        exit(red_errno);
    }
#endif // IS_ATHENA_V2
#endif // defined(HAS_SD_CARD)
}

/**
 * Initialize CSP network
 */
static void init_csp() {
    TC_TM_app_id my_address = OBC_APP_ID;

    /* Init CSP with address and default settings */
    csp_conf_t csp_conf;
    csp_conf.address = 1;
    csp_conf.hostname = "Athena";
    csp_conf.model = "Ex-Alta2";
    csp_conf.revision = "2";
    csp_conf.conn_max =10;
    csp_conf.conn_queue_length = 10;
    csp_conf.fifo_length = 25;
    csp_conf.port_max_bind = 24;
    csp_conf.rdp_max_window = 20;
    csp_conf.buffers = 10;
    csp_conf.buffer_data_size = 1024;
    csp_conf.conn_dfl_so = CSP_O_NONE;

    int error = csp_init(&csp_conf);
    if (error != CSP_ERR_NONE) {
        // ex2_log("csp_init() failed, error: %d\n", error);
        exit(SATR_ERROR);
    }
    // ex2_log("Running at %d\n", my_address);
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
 *      start the localhost zmq server and add it to the default route
 * with no VIA address
 */
static inline SAT_returnState init_csp_interface() {
    csp_iface_t *uart_iface = NULL;
    csp_iface_t *can_iface = NULL;
    csp_usart_conf_t conf = {.device = "UART",
                             .baudrate = 115200, /* supported on all platforms */
                             .databits = 8,
                             .stopbits = 2,
                             .paritysetting = 0,
                             .checkparity = 0};

    int error = csp_can_open_and_add_interface("CAN", &can_iface);
    if (error != CSP_ERR_NONE) {
        return SATR_ERROR;
    }

    error = csp_usart_open_and_add_kiss_interface(&conf, CSP_IF_KISS_DEFAULT_NAME, &uart_iface);
    if (error != CSP_ERR_NONE) {
        return SATR_ERROR;
    }

#ifndef EPS_IS_STUBBED
    csp_rtable_load("16 KISS, 4 CAN, 10 KISS");
#else
    csp_rtable_load("16 KISS, 10 KISS");
#endif

    return SATR_OK;
}

void vAssertCalled(unsigned long ulLine, const char *const pcFileName) {
    /* Called if an assertion passed to configASSERT() fails.  See
    http://www.freertos.org/a00110.html#configASSERT for more information. */

    /* Parameters are not used. */
    (void)ulLine;
    (void)pcFileName;

    ex2_log("ASSERT! Line %d, file %s\r\n", ulLine, pcFileName);
    for (;;)
        ;
}

static void prvSaveTraceFile(void) {
    // TODO: implement this with relianceEdge
}

void initializeProfiler() {
    /* Enable PMU Cycle Counter for Profiling */
    RAISE_PRIVILEGE;
    _pmuInit_();
    _pmuEnableCountersGlobal_();
    _pmuResetCycleCounter_();
    _pmuStartCounters_(pmuCYCLE_COUNTER);
    RESET_PRIVILEGE;
}

// TODO: This might need to be put in application_defined_privileged_functions.h
uint32 getProfilerTimerCount() {
    RAISE_PRIVILEGE;
    uint32_t ret = _pmuGetCycleCount_() / GCLK_FREQ;
    RESET_PRIVILEGE;
    return ret;
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    for (;;)
        ;
}

void vApplicationMallocFailedHook(void) {
    for (;;)
        ;
}

void vApplicationDaemonTaskStartupHook(void) { init_logger_queue(); }

