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
 * @author Andrew Rooney, Haoran Qi, Robert Taylor, Dustin Wagner, Arash Yazdani, Thomas Ganley
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
#include "main/version.h"
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
#include "dfgm.h"
#include "leop.h"
#include "adcs.h"
#include "iris.h"
#include "iris_spi.h"

#include "deployablescontrol.h"

#ifdef FLATSAT_TEST
//#include "sband_binary_tests.h"
static void flatsat_test();
#endif

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

static inline SAT_returnState init_csp_interface();
void vAssertCalled(unsigned long ulLine, const char *const pcFileName);

void ex2_init(void *pvParameters) {

    init_filesystem();

    /* LEOP */

#ifdef EXECUTE_LEOP
    if (leop_init() != true) {
        // TODO: Do what if leop fails?
    }
#endif

    /* Initialization routine */

    /* Subsystem Hardware Initialization */

#ifndef ADCS_IS_STUBBED
    // PLACEHOLDER: adcs hardware init
    init_adcs_io();
#endif

#ifndef ATHENA_IS_STUBBED
    // PLACEHOLDER: athena hardware init
#endif

#ifndef EPS_IS_STUBBED
    // PLACEHOLDER: eps hardware init
#endif

#ifndef UHF_IS_STUBBED
    uhf_uart_init();
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

#ifndef DFGM_IS_STUBBED
    DFGM_init();
#endif

    /* Software Initialization */

    /* Start service server, and response server */
    init_csp();
    init_software();

#ifdef FLATSAT_TEST
    /* Test Task */
    xTaskCreate(flatsat_test, "flatsat_test", 1000, NULL, 4, NULL);
#endif

    vTaskDelete(0); // delete self to free up heap
}

#ifdef FLATSAT_TEST
void flatsat_test(void *pvParameters) {

    vTaskDelete(NULL);
}
#endif

TaskHandle_t iris_spi_handle;

void iris_spi_test(void * pvParameters) {
    iris_spi_init();
    //iris_take_pic();

    iris_housekeeping_data hk_data;
    uint16_t image_count;

    for(;;) {
//        spi_write_read(1, &tx_data, rx_data);
//        vTaskDelay(pdMS_TO_TICKS( 1000UL ));
        //iris_take_pic();
        //iris_get_image_length();
        //iris_get_housekeeping(hk_data);
        //iris_get_image_count(&image_count);
        iris_transfer_image(2000000);
        //iris_update_sensor_i2c_reg();
    }
    //vTaskDelay(pdMS_TO_TICKS( 1000UL ));
}

int ex2_main(void) {
    _enable_IRQ_interrupt_(); // enable inturrupts
    InitIO();
    for (int i = 0; i < 1000000; i++)
        ;
    //xTaskCreate(ex2_init, "init", INIT_STACK_SIZE, NULL, INIT_PRIO, NULL);
    xTaskCreate(iris_spi_test, "IRIS SPI", 256, NULL, (tskIDLE_PRIORITY + 1),
                &iris_spi_handle);
    /* Start FreeRTOS! */
    vTaskStartScheduler();

    for (;;)
        ; // Scheduler didn't start
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
    /* Init CSP with address and default settings */
    csp_conf_t csp_conf;
    csp_conf.address = 1;
    csp_conf.hostname = "Athena";
    csp_conf.model = "Ex-Alta2";
    csp_conf.revision = "2";
    csp_conf.conn_max = 20;
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
    int error;
    csp_iface_t *uart_iface = NULL;
    csp_usart_conf_t conf = {.device = "UART",
                             .baudrate = 115200, /* supported on all platforms */
                             .databits = 8,
                             .stopbits = 2,
                             .paritysetting = 0,
                             .checkparity = 0};

#ifndef EPS_IS_STUBBED
    csp_iface_t *can_iface = NULL;
    error = csp_can_open_and_add_interface("CAN", &can_iface);
    if (error != CSP_ERR_NONE) {
        return SATR_ERROR;
    }
#endif

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
