/*
 * time.h
 *
 *  Created on: Dec. 23, 2021
 *      Author: Robert Taylor
 */

#ifndef EX2_SYSTEM_INCLUDE_EX2_TIME_H_
#define EX2_SYSTEM_INCLUDE_EX2_TIME_H_

#include <stdint.h>
#include <time.h>

// convenience macros to convert to and from tm years
#define tmYearToCalendar(Y) ((Y) + 1970) // full four digit year
#define CalendarYrToTm(Y) ((Y)-1970)
#define tmYearToY2k(Y) ((Y)-30) // offset is from 2000
#define y2kYearToTm(Y) ((Y) + 30)

/* Useful Constants */
#define SECS_PER_MIN ((time_t)(60UL))
#define SECS_PER_HOUR ((time_t)(3600UL))
#define SECS_PER_DAY ((time_t)(SECS_PER_HOUR * 24UL))
#define DAYS_PER_WEEK ((time_t)(7UL))
#define SECS_PER_WEEK ((time_t)(SECS_PER_DAY * DAYS_PER_WEEK))
#define SECS_PER_YEAR ((time_t)(SECS_PER_DAY * 365UL)) // TODO: ought to handle leap years
#define SECS_YR_2000 ((time_t)(946684800UL))           // the time at the start of y2k

typedef struct {
    uint8_t Second;
    uint8_t Minute;
    uint8_t Hour;
    uint8_t Wday; // day of week, sunday is day 1
    uint8_t Day;
    uint8_t Month;
    uint8_t Year; // offset from 1970;
} tmElements_t;

void breakTime(time_t timeInput, tmElements_t *tm);

time_t makeTime(const tmElements_t tm);

#endif /* EX2_SYSTEM_INCLUDE_EX2_TIME_H_ */
