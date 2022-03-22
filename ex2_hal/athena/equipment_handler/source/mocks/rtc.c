#include "mocks/rtc.h"

#include "system.h"
#include <stdbool.h>
#include <time.h>

#define SOFTWARE_RTC_STACK_LEN 50

static uint32 unix_time;
static ex2_time_t utc_time;
static date_t utc_date;

void mock_RTC_set_unix_time(uint32_t unix_timestamp) {
    struct tm t;
    t = *gmtime(&unix_timestamp);
    utc_date.year = (1970 + t.tm_year) - 2000;
    utc_date.month = t.tm_mon + 1;
    utc_date.day = t.tm_mday;
    utc_time.hour = t.tm_hour;
    utc_time.minute = t.tm_min;
    utc_time.second = t.tm_sec;
    utc_time.ms = 0;
}

void mock_RTC_get_unix_time(uint32_t *unix_timestamp) {
    struct tm t;
    time_t unix;
    t.tm_year = (utc_date.year + 2000 - 1970);
    t.tm_mon = utc_date.month - 1;
    t.tm_mday = utc_date.day;
    t.tm_hour = utc_time.hour;
    t.tm_min = utc_time.minute;
    t.tm_sec = utc_time.second;
    unix = mktime(&t);
    *unix_timestamp = (uint32_t)unix;
}

void mock_RTC_get_time(ex2_time_t *out_time) { memcpy(out_time, &utc_time, sizeof(ex2_time_t)); }

void mock_RTC_set_time(ex2_time_t new_time) { memcpy(&utc_time, &new_time, sizeof(ex2_time_t)); }

void mock_RTC_get_date(date_t *out_date) { memcpy(out_date, &utc_date, sizeof(date_t)); }

void mock_RTC_set_date(date_t new_date) { memcpy(&utc_date, &new_date, sizeof(date_t)); }

void HAL_get_temperature(float *temp) { *temp = (float)18.01; }

void HAL_get_current_1(float *current) { *current = (float)5.34; }

void HAL_get_current_2(float *current) { *current = (float)5.03; }

void HAL_get_voltage_1(float *voltage) { *voltage = (float)2.10; }

void HAL_get_voltage_2(float *voltage) { *voltage = (float)1.45; }

void handle_overflow() {
    bool date_overflow = false;
    if (utc_time.second / 60) {
        utc_time.minute += utc_time.second / 60;
        utc_time.second %= 60;

        if (utc_time.minute / 60) {
            utc_time.hour += utc_time.minute / 60;
            utc_time.minute %= 60;
        }
        if (utc_time.hour / 24) {
            utc_time.hour %= 24;
            date_overflow = true;
        }
        if (date_overflow) {
            utc_date.day++;
            switch (utc_date.month) {
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
                if (utc_date.day / 31) {
                    utc_date.day %= 31;
                    utc_date.month++;
                }
                break;
            case 4:
            case 6:
            case 9:
            case 11:
                if (utc_date.day / 30) {
                    utc_date.day %= 30;
                    utc_date.month++;
                }
                break;
            case 2:
                // check if the year is a leap year
                if (((utc_date.year % 4 == 0) && (utc_date.year % 100 != 0)) || (utc_date.year % 400 == 0)) {
                    if (utc_date.day / 29) {
                        utc_date.day %= 29;
                        utc_date.month++;
                    }
                } else {
                    if (utc_date.day / 28) {
                        utc_date.day %= 28;
                        utc_date.month++;
                    }
                }
                break;
            }
            if (utc_date.month / 12) {
                utc_date.month %= 12;
                utc_date.year++;
            }
        }
    }
}

void mock_rtc_task() {
    for (;;) {
        utc_time.second++;
        handle_overflow();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

BaseType_t start_mock_rtc(TaskHandle_t *software_RTC_handle) {
    BaseType_t created = xTaskCreate((TaskFunction_t)mock_rtc_task, "Software_RTC", SOFTWARE_RTC_STACK_LEN, NULL,
                                     MOCK_RTC_TASK_PRIO, software_RTC_handle);
    return created;
}
