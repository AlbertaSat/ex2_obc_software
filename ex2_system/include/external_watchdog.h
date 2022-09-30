/*
 * external_watchdog.h
 *
 *  Created on: Sep 26, 2022
 *      Author: joshd
 */

#ifndef EX2_SYSTEM_INCLUDE_EXTERNAL_WATCHDOG_H_
#define EX2_SYSTEM_INCLUDE_EXTERNAL_WATCHDOG_H_

#include "system.h"


SAT_returnState start_ext_watchdog_handler();
void irisWdoNotification();

#endif /* EX2_SYSTEM_INCLUDE_EXTERNAL_WATCHDOG_H_ */
