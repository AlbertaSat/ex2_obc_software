#include "hal.h"

typedef struct {
  // THIS IS AN EXAMPLE, not how to actually
  // implement the RTC handler
  uint32_t unix_time;
} HAL_rtc_t;
static HAL_rtc_t HAL_rtc;

void HAL_RTC_SetTime(uint32_t unix_timestamp) {
  HAL_rtc.unix_time = unix_timestamp;
}

void HAL_RTC_GetTime(uint32_t *unix_timestamp) {
  *unix_timestamp = HAL_rtc.unix_time;
}
