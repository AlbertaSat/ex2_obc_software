#include "demo_hal.h"

uint32_t current_time;

void HAL_RTC_SetTime(uint32_t unix_timestamp) { current_time = unix_timestamp; }

void HAL_RTC_GetTime(uint32_t *unix_timestamp) {
  *unix_timestamp = current_time;
}

void HAL_sys_setTime(uint32_t unix_timestamp) {
  HAL_RTC_SetTime(unix_timestamp);
}

void HAL_sys_getTime(uint32_t *unix_timestamp) {
  HAL_RTC_GetTime(unix_timestamp);
}
