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

#define SYSTEM_APP_ID _OBC_APP_ID_

#define NORMAL_SERVICE_PRIO               1
#define STATE_TASK_PRIO                   1
#define HOUSEKEEPING_TASK_PRIO            1
#define COORDINATE_MANAGEMENT_TASK_PRIO   1
#define BEACON_TASK_PRIO                  1
#define DIAGNOSTIC_TASK_PRIO              1
#define SYSTEM_STATS_TASK_PRIO            1
#define MOCK_RTC_TASK_PRIO                configMAX_PRIORITIES-1

#define GPS_SCI sciREG2
#define CSP_SCI sciREG3

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

int ex2_main(int argc, char **argv);
void SciSendBuf(char *buf, uint32_t bufSize);

#endif /* SYSTEM_H */
