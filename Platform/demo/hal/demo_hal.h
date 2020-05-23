#ifndef __DEMO_HAL_
#define __DEMO_HAL_

#include "inttypes.h"

void HAL_sys_getTime(uint32_t *unix_timestamp);
void HAL_sys_setTime(uint32_t unix_timestamp);

#endif
