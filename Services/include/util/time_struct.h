/*
 * time_struct.h
 *
 *  Created on: Mar. 22, 2021
 *      Author: robert
 */

#ifndef TIME_STRUCT_H_
#define TIME_STRUCT_H_

#include <stdint.h>

typedef struct ex2_time_t {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t ms;
} ex2_time_t;

typedef struct date_t {
    uint8_t day;
    uint8_t month;
    uint8_t year;
} date_t;

#endif /* TIME_STRUCT_H_ */

