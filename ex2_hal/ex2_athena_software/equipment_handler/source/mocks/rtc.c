#include "mocks/rtc.h"

#include <stdio.h>
#include <time.h>

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

void HAL_get_temperature(float *temp) { *temp = (float)18.01; }

void HAL_get_current_1(float *current) { *current = (float)5.34; }

void HAL_get_current_2(float *current) { *current = (float)5.03; }

void HAL_get_voltage_1(float *voltage) { *voltage = (float)2.10; }

void HAL_get_voltage_2(float *voltage) { *voltage = (float)1.45; }
