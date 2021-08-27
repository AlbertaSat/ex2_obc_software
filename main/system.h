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

#include <stdint.h>
#include "FreeRTOS.h"

#define config_IS_ATHENA 0
#define config_IS_FLATSAT 0

#if config_IS_ATHENA == 1
#define IS_ATHENA
#endif

#if config_IS_FLATSAT == 1
    #ifndef IS_ATHENA
    #error If config_IS_FLATSAT is set to 1 IS_ATHENA must be defined
    #endif
#define IS_FLATSAT
#endif

#define SYSTEM_APP_ID _OBC_APP_ID_

#define NORMAL_SERVICE_PRIO               1
#define STATE_TASK_PRIO                   1
#define HOUSEKEEPING_TASK_PRIO            1
#define COORDINATE_MANAGEMENT_TASK_PRIO   1
#define BEACON_TASK_PRIO                  1
#define DIAGNOSTIC_TASK_PRIO              1
#define SYSTEM_STATS_TASK_PRIO            1
#define LOGGER_TASK_PRIO                  2
#define MOCK_RTC_TASK_PRIO                configMAX_PRIORITIES-1
#define TASK_MANAGER_PRIO                 3

#if defined(IS_3U) && defined(IS_2U)
#error "Can not be both 2U and 3U sized satellites"
#endif

#if defined(IS_ATHENA)
    #define CSP_SCI sciREG2
    #define ADCS_SCI sciREG4
    #define DFGM_SCI sciREG1
    #define UHF_SCI CSP_SCI
#if defined(IS_3U)
    #define GPS_SCI sciREG3
#else if defined(IS_2U)
    #define PAYLOAD_SCI sciREG3
#endif
#else // These values are expected to be adjusted based on what the developer is working on
    #define CSP_SCI sciREG3
    #define GPS_SCI sciREG2
    #define ADCS_SCI 0
    #define PAYLOAD_SCI 0
    #define DFGM_SCI 0
#endif

#ifndef IS_FLATSAT
    #ifdef IS_ATHENA
    #define PRINTF_SCI sciREG4
    #else
    #define PRINTF_SCI sciREG1
    #endif
#else
#define PRINTF_SCI NULL
#endif

#if defined(IS_ATHENA)
    #define IRIS_CONFIG_SPI spiREG1
    #define IRIS_SPI spiREG3
    #define SBAND_SPI spiREG2
    #define SD_SPI spiREG3
#else // These values are expected to be adjusted based on what the developer is working on
    #define IRIS_CONFIG_SPI spiREG1
    #define IRIS_SPI spiREG4
    #define SBAND_SPI spiREG2
    #define SD_SPI spiREG3
#endif

#if defined(IS_ATHENA)
    #define IMU_I2C i2cREG2
    #define SOLAR_I2C i2cREG2
    #define TEMPSENSE_I2C i2cREG2
    #define RTC_I2C i2cREG2

    #define SBAND_I2C i2cREG1
    #define ADCS_I2C i2cREG1
    #define UHF_I2C i2cREG1
#else // These values are expected to be adjusted based on what the developer is working on
    #define IMU_I2C i2cREG2
    #define SOLAR_I2C i2cREG2
    #define TEMPSENSE_I2C i2cREG2
    #define RTC_I2C i2cREG2

    #define SBAND_I2C i2cREG1
    #define ADCS_I2C i2cREG1
    #define UHF_I2C i2cREG1
#endif

// watchdog timer expires in 447ms
#define WDT_DELAY 200 // 200 miliseconds gives a a good window
#define WDT_CHECK_INTERVAL 10000 // only actually checks the tasks this often
#define DELAY_WAIT_INTERVAL 8000 // a pretty long time, but lets the watchdog know that the task hasn't failed

typedef enum {
  SATR_PKT_ILLEGAL_APPID = 0,
  SATR_PKT_ILLEGAL_SUBSERVICE,
  SATR_OK,
  SATR_ERROR,
  SATR_RETURN_FROM_TASK,
  SATR_BUFFER_ERR,
  /*LAST*/
  SATR_LAST
} SAT_returnState;

/* Subsystems Pins & Ports */
#define UHF_GIO_PORT    hetPORT2
#define UHF_GIO_PIN     22
// TODO: Numbers to be set
#define UHF_PWR_CHNL    1
#define STX_PWR_CHNL    1
#define IRIS_PWR_CHNL   1
#define DFGM_PWR_CHNL   1
#define ADCS_PWR_CHNL   1

int ex2_main(void);
void SciSendBuf(char *buf, uint32_t bufSize);

#endif /* SYSTEM_H */
