/*
 * rtcmk.h
 *
 *  Created on: May 28, 2018
 *      Author: sdamk
 */

#ifndef DRIVERS_RTCMK_H_
#define DRIVERS_RTCMK_H_

#include <stdint.h>
#include "ex2_time.h"

// A cached copy of the RTC's unix time
extern time_t unix_timestamp;

// The I2C address of our RTC
#define RTCMK_ADDR (0x32)

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/* unix_timestamp is set when RTCMK_SetUnix is called, and then updated every
 * second by an RTC interrupt. RTCMK_Unix_Now() is the preferred way of getting
 * the current unix time.
 */
static inline time_t RTCMK_Unix_Now() { return unix_timestamp; }

// Note that the RTC does not actually keep milliseconds itself.
int RTCMK_GetMs();

int RTCMK_SetUnix(time_t new_time);

/* This call is here for the RTC daemon to actually read the RTC to check for
 * drift in the unix_timestamp.
 */
int RTCMK_GetUnix(time_t *unix_time);

int RTCMK_EnableInt(uint8_t addr);

int RTCMK_ResetTime(uint8_t addr);

#endif /* DRIVERS_RTCMK_H_ */
