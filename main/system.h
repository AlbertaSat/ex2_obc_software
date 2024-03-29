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
 * This header file is unique to the system being implemented. It just includes
 * system specific headers, and defines system parameters.
 */

#ifndef SYSTEM_H
#define SYSTEM_H

#include "FreeRTOS.h"
#include "os_task.h"
#include <stdint.h>
#include "FreeRTOS.h"

#if IS_FLATSAT == 1
#if IS_ATHENA == 0
#error If IS_FLATSAT is set then IS_ATHENA must be set
#endif
#if HAS_SD_CARD == 0
#warning FlatSat testing requires the SD card on Athena to be present
#endif
#endif

#define DIAGNOSICS_TASK_PRIORITY (tskIDLE_PRIORITY)
#define NORMAL_SERVICE_PRIO (tskIDLE_PRIORITY + 1)
#define STATE_TASK_PRIO (tskIDLE_PRIORITY + 1)
#define HOUSEKEEPING_TASK_PRIO (tskIDLE_PRIORITY + 1)
#define COORDINATE_MANAGEMENT_TASK_PRIO (tskIDLE_PRIORITY + 1)
#define BEACON_TASK_PRIO (tskIDLE_PRIORITY + 1)
#define DIAGNOSTIC_TASK_PRIO (tskIDLE_PRIORITY + 2)
#define SCHEDULER_TASK_PRIO (tskIDLE_PRIORITY + 2)
#define SYSTEM_STATS_TASK_PRIO (tskIDLE_PRIORITY + 1)
#define LOGGER_TASK_PRIO (tskIDLE_PRIORITY + 2)
#define MOCK_RTC_TASK_PRIO (configMAX_PRIORITIES - 1)
#define TASK_MANAGER_PRIO (tskIDLE_PRIORITY + 3)

#define CLI_SVC_SIZE 1000
#define ADCS_SVC_SIZE 1536
#define COMM_SVC_SIZE 1024
#define DFGM_SVC_SIZE 1024
#define FTP_SVC_SIZE 500
#define HK_SVC_SIZE 600
#define LOGGER_SVC_SIZE 1200
#define NS_SVC_SIZE 1024
#define IRIS_SVC_SIZE 1000
#define TIMEMANAGEMENT_SVC_SIZE 300
#define SCHEDULER_SVC_SIZE 1024
#define GENERAL_SVC_SIZE 300
#define CSPSERVER_SVC_SIZE 256

#define COORD_DM_SIZE 128
#define DIAGNOSTIC_DM_SIZE 500
#define HK_DM_SIZE 1200
#define LOGGER_DM_SIZE 500
#define SBANDSEND_DM_SIZE 200
#define RTC_DM_SIZE 256
#define SWWDT_DM_SIZE 128
#define INIT_STACK_SIZE 400
#define NV_DAEMON_STACK_SIZE 400

#if IS_ATHENA == 1
#define CSP_SCI sciREG2  // UART2
#define ADCS_SCI sciREG3 // UART4
#define DFGM_SCI sciREG4 // UART1
#define UHF_SCI CSP_SCI  // UART2
#define GPS_SCI NULL
#define PAYLOAD_SCI NULL
#if IRIS_IS_STUBBED == 0
#undef GPS_SCI
#define GPS_SCI sciREG1 // UART3
#elif NS_IS_STUBBED == 0
#undef PAYLOAD_SCI
#define PAYLOAD_SCI sciREG1 // UART3
#endif
#else
// These values are expected to be adjusted based on what the developer is working on
// NOTE: Multiple definitions of the same sciREG will cause issues in the interrupt switch() statement
#define CSP_SCI sciREG3
#define GPS_SCI sciREG1
#define ADCS_SCI sciREG2
#define PAYLOAD_SCI sciREG1
#define DFGM_SCI sciREG4
#define UHF_SCI CSP_SCI
#endif

#if IS_ATHENA == 1
#define PRINTF_SCI NULL
#else
#define PRINTF_SCI sciREG1
#endif

#if IS_ATHENA == 1
#define IRIS_SPI spiREG4  // SPI1
#define SBAND_SPI spiREG3 // SPI2
#define SD_SPI spiREG1    //?
#else                     // These values are expected to be adjusted based on what the developer is working on
#define IRIS_CONFIG_SPI spiREG4 // SPI1
#define IRIS_SPI spiREG3        // SPI3
#define SBAND_SPI spiREG3       // SPI2
#define SD_SPI spiREG1          //?
#endif

#if IS_ATHENA == 1
#define IMU_I2C i2cREG2
#define TEMPSENSE_I2C i2cREG2
#define RTC_I2C i2cREG2

#define SBAND_I2C i2cREG1
#define ADCS_I2C i2cREG1
#define UHF_I2C i2cREG1
#define IRIS_I2C i2cREG1
#define PCAL9538A_PORT i2cREG1

#if IS_ATHENA_V2 == 1
#define SOLAR_I2C i2cREG2
#else
#define SOLAR_I2C i2cREG1
#endif

#else // These values are expected to be adjusted based on what the developer is working on
#define IMU_I2C i2cREG2
#define SOLAR_I2C i2cREG2
#define TEMPSENSE_I2C i2cREG2
#define RTC_I2C i2cREG2
#define SOLAR_I2C i2cREG2

#define IRIS_I2C i2cREG1
#define SBAND_I2C i2cREG1
#define ADCS_I2C i2cREG1
#define UHF_I2C i2cREG1
#define PCAL9538A_PORT i2cREG1

#endif

// watchdog timer expires in 447ms
#define WDT_DELAY 100            // 100 miliseconds gives a a good window
#define DELAY_WAIT_INTERVAL 8000 // a pretty long time, but lets the watchdog know that the task hasn't failed
#define ONE_SECOND pdMS_TO_TICKS(1000)
#define ONE_MINUTE pdMS_TO_TICKS(60000)
#define CSP_TIMEOUT 100

typedef enum {
    SATR_OK,
    SATR_ERROR,
    SATR_PKT_ILLEGAL_APPID,
    SATR_PKT_ILLEGAL_SUBSERVICE,
    SATR_RETURN_FROM_TASK,
    SATR_BUFFER_ERR,
    /*LAST*/
    SATR_LAST
} SAT_returnState;

/* Subsystems Pins & Ports */
#define UHF_GIO_PORT hetPORT2
#define UHF_GIO_PIN 22

#define RTC_INT_PORT gioPORTA
#define RTC_INT_PIN 2

#define NS_RESET_GIO_PORT hetPORT1
#define NS_RESET_GIO_PIN 3

#define ADCS_5V0_PWR_CHNL 1
#if IS_SN0072_EPS == 1
#warning                                                                                                          \
    "IS_SN0072_EPS swaps assignment for channels 2 and 5 because of incorrect output config of the engineering model \
    Nanoavionics EPS, SN 0072"
#define ADCS_3V3_PWR_CHNL 2
#else
#define DFGM_5V0_PWR_CHNL 2
#endif

#if IS_SN0072_EPS == 1 && IS_EXALTA2 == 1
#warning                                                                                                          \
    "IS_SN0072_EPS swaps assignment for channels 3 and 1 because of incorrect output config of the engineering \
     model Nanoavionics EPS, SN 0072"
#define PYLD_5V0_PWR_CHNL 1
#else
#define PYLD_5V0_PWR_CHNL 3
#endif

// Channel 4 was assigned to be 3V3 for 1W UHF in case 2W UHFs didn't work out.
#if IS_SN0072_EPS == 1
#define DFGM_5V0_PWR_CHNL 5
#else
#define ADCS_3V3_PWR_CHNL 5
#endif

// Channel 6 is the OBC power channel, and should not be accessed by the OBC
#define CHARON_3V3_PWR_CHNL 7
#define UHF_5V0_PWR_CHNL 8
#define DEPLOYABLES_5V0_PWR_CHNL 9
#define PYLD_3V3_PWR_CHNL 10
// SBAND_PWR_CHNL does not exist as it is on the 5V_AO (always on) channel

/**
 * SANITY CHECKS
 */
#if IS_SATELLITE == 1
#if (IS_EXALTA2 == 1 && IS_AURORASAT == 1) || (IS_EXALTA2 == 1 && IS_YUKONSAT == 1) ||                            \
    (IS_AURORASAT == 1 && IS_YUKONSAT == 1)
#error "Too many satellites defined!"
#elif IS_EXALTA2 == 0 && IS_YUKONSAT == 0 && IS_AURORASAT == 0
#error "Need to define a satellite!"
#endif
#endif

#if NS_IS_STUBBED == 0 && IRIS_IS_STUBBED == 0
#error "Can't have NS and IRIS payloads"
#endif

#if GOLDEN_IMAGE == 1 && WORKING_IMAGE == 1
#error "Must be either GOLDEN_IMAGE or WORKING_IMAGE"
#endif

#if CSP_FREERTOS == 0
#error "CSP_FREERTOS must be 1"
#endif

#if CSP_USE_KISS == 0 && CSP_USE_SDR == 0 || CSP_USE_KISS == 1 && CSP_USE_SDR == 1
#error "CSP must use one of KISS or SDR"
#endif /* !defined(CSP_USE_KISS) && !defined(CSP_USE_SDR) || defined(CSP_USE_KISS) && defined(CSP_USE_SDR) */

#endif /* SYSTEM_H */
