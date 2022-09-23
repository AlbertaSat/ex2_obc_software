/*
 * sband_sender.h
 *
 *  Created on: Sep. 15, 2022
 *      Author: Robert Taylor
 */

#ifndef EX2_SYSTEM_INCLUDE_SBAND_SENDER_SBAND_SENDER_H_
#define EX2_SYSTEM_INCLUDE_SBAND_SENDER_SBAND_SENDER_H_

#include "FreeRTOS.h"
#include "system.h"
#include <stdbool.h>

bool sband_send_data(void *data, size_t len);

SAT_returnState start_sband_daemon();

#endif /* EX2_SYSTEM_INCLUDE_SBAND_SENDER_SBAND_SENDER_H_ */
