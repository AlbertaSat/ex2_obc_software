/*
 * rtcmk.c
 *
 *  Created on: May 28, 2018
 *  Most recent edit: Aug 11, 2020
 *      Author: sdamk, jdlazaru
 *
 *
 *
 *      TODO: Check if interrupts are needed/how to implement
 */

#include "rtcmk.h"
#include "HL_i2c.h"
#include "i2c_io.h"

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Convert 8 bit binary to 8 bit
                                                                               *BCD
                                                                               *
                                                                               * @param[in] val
                                                                               *   binary value to convert
                                                                               *
                                                                               * @return
                                                                               *   Input represented as BCD
                                                                               ******************************************************************************/
unsigned int toBCD(unsigned int val) { return val + 6 * (val / 10); }

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Convert 8 bit BCD to 8 bit
                                                                               *binary
                                                                               *
                                                                               * @param[in] val
                                                                               *   BCD value to convert
                                                                               *
                                                                               * @return
                                                                               *   Input represented as binary
                                                                               ******************************************************************************/
unsigned int toBIN(unsigned int val) { return val - 6 * (val >> 4); }

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Set time to unix time
                                                                               *
                                                                               * @param[in] new_time
                                                                               *   unix time to set
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if time updated, <0 if
                                                                               *unable to update time.
                                                                               ******************************************************************************/
int RTCMK_SetUnix(time_t new_time) {
    // TODO: Make these use a single array
    struct tm t;
    t = *gmtime(&new_time);
    if (RTCMK_SetSecond(RTCMK_ADDR, t.tm_sec) == -1)
        return -1;
    if (RTCMK_SetMinute(RTCMK_ADDR, t.tm_min) == -1)
        return -1;
    if (RTCMK_SetHour(RTCMK_ADDR, t.tm_hour) == -1)
        return -1;
    if (RTCMK_SetDay(RTCMK_ADDR, t.tm_mday) == -1)
        return -1;
    if (RTCMK_SetMonth(RTCMK_ADDR, t.tm_mon) == -1)
        return -1;
    if (RTCMK_SetYear(RTCMK_ADDR, (t.tm_year % 100)) == -1)
        return -1;
    return 0;
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Get time represented as unix
                                                                               *time
                                                                               *
                                                                               * @param[in] *unix_time
                                                                               *   Variable to store time
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if time updated, <0 if
                                                                               *unable to update time.
                                                                               ******************************************************************************/
int RTCMK_GetUnix(time_t *unix_time) {
    // TODO: make these use a single array
    struct tm t = {0};

    uint8_t sec;
    if (RTCMK_ReadSeconds(RTCMK_ADDR, &sec) == -1)
        return -1;
    t.tm_sec = (uint32_t)sec;

    uint8_t min;
    if (RTCMK_ReadMinutes(RTCMK_ADDR, &min) == -1)
        return -1;
    t.tm_min = (uint32_t)min;

    uint8_t hour;
    if (RTCMK_ReadHours(RTCMK_ADDR, &hour) == -1)
        return -1;
    t.tm_hour = (uint32_t)hour;

    uint8_t day;
    if (RTCMK_ReadDay(RTCMK_ADDR, &day) == -1)
        return -1;
    t.tm_mday = (uint32_t)day;

    uint8_t mon;
    if (RTCMK_ReadMonth(RTCMK_ADDR, &mon) == -1)
        return -1;
    t.tm_mon = (uint32_t)mon;

    uint8_t year;
    if (RTCMK_ReadYear(RTCMK_ADDR, &year) == -1)
        return -1;
    t.tm_year = (uint32_t)year;
    t.tm_year += 100;
    t.tm_isdst = -1;
    *unix_time = mktime(&t);
    return 0;
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Set content of a register.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[in] reg
                                                                               *   Register to write (input
                                                                               *register cannot be written).
                                                                               *
                                                                               * @param[in] val
                                                                               *   Value used when writing to
                                                                               *register.
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if register written,
                                                                               *<0 if unable to write to register.
                                                                               ******************************************************************************/
int RTCMK_RegisterSet(uint8_t addr, RTCMK_Register_TypeDef reg, uint8_t val) {
    uint8_t data[2];

    data[0] = reg;
    data[1] = val;

    return i2c_Send(RTCMK_PORT, addr, 2, &data);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Get current content of a
                                                                               *register.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[in] reg
                                                                               *   Register to read.
                                                                               *
                                                                               * @param[out] val
                                                                               *   Reference to place register
                                                                               *read.
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if register read, <0
                                                                               *if unable to read register.
                                                                               ******************************************************************************/
int RTCMK_RegisterGet(uint8_t addr, RTCMK_Register_TypeDef reg, uint8_t *val) {
    uint8_t data;

    data = reg;

    if (i2c_Send(RTCMK_PORT, addr, 1, &data) == -1) {
        return -1;
    }

    return (int)i2c_Receive(RTCMK_PORT, addr, 1, val);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Write 0's to time and calender
                                                                               *registors (0x00 to 0x06).
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_ResetTime(uint8_t addr) {

    uint8_t data[8] = {0};

    data[0] = ((uint8_t)RTCMK_RegSec) << 1;

    return i2c_Send(RTCMK_PORT, addr, 8, &data);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Returns current content of
                                                                               *seconds register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[out] val
                                                                               *   Reference to place result.
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_ReadSeconds(uint8_t addr, uint8_t *val) {
    int ret = -1;

    uint8_t tmp = 0;

    ret = RTCMK_RegisterGet(addr, RTCMK_RegSec, &tmp);
    if (ret < 0) {
        return (ret);
    }

    tmp &= _RTCMK_SEC_SEC_MASK;

    *val = toBIN(tmp);

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Returns current content of
                                                                               *minutes register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[out] val
                                                                               *   Reference to place result.
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_ReadMinutes(uint8_t addr, uint8_t *val) {
    int ret = -1;

    uint8_t tmp = 0;

    ret = RTCMK_RegisterGet(addr, RTCMK_RegMin, &tmp);
    if (ret < 0) {
        return (ret);
    }

    tmp &= _RTCMK_MIN_MIN_MASK;

    *val = toBIN(tmp);

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Returns current content of hours
                                                                               *register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[out] val
                                                                               *   Reference to place result.
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_ReadHours(uint8_t addr, uint8_t *val) {
    int ret = -1;

    uint8_t tmp = 0;

    ret = RTCMK_RegisterGet(addr, RTCMK_RegHour, &tmp);
    if (ret < 0) {
        return (ret);
    }

    tmp &= _RTCMK_HOUR_HOUR_MASK;

    *val = toBIN(tmp);

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Returns current content of hours
                                                                               *register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[out] val
                                                                               *   Reference to place result.
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_ReadWeek(uint8_t addr, uint8_t *val) {
    int ret = -1;

    uint8_t tmp = 0;

    ret = RTCMK_RegisterGet(addr, RTCMK_RegWeek, &tmp);
    if (ret < 0) {
        return (ret);
    }

    tmp &= _RTCMK_WEEK_WEEK_MASK;

    *val = toBIN(tmp);

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Returns current content of hours
                                                                               *register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[out] val
                                                                               *   Reference to place result.
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_ReadMonth(uint8_t addr, uint8_t *val) {
    int ret = -1;

    uint8_t tmp = 0;

    ret = RTCMK_RegisterGet(addr, RTCMK_RegMonth, &tmp);
    if (ret < 0) {
        return (ret);
    }

    tmp &= _RTCMK_MONTH_MONTH_MASK;

    *val = toBIN(tmp);

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Returns current content of hours
                                                                               *register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[out] val
                                                                               *   Reference to place result.
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_ReadYear(uint8_t addr, uint8_t *val) {
    int ret = -1;

    uint8_t tmp = 0;

    ret = RTCMK_RegisterGet(addr, RTCMK_RegYear, &tmp);
    if (ret < 0) {
        return (ret);
    }

    tmp &= _RTCMK_YEAR_YEAR_MASK;

    *val = toBIN(tmp);

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Returns current content of hours
                                                                               *register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[out] val
                                                                               *   Reference to place result.
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_ReadDay(uint8_t addr, uint8_t *val) {
    int ret = -1;

    uint8_t tmp = 0;

    ret = RTCMK_RegisterGet(addr, RTCMK_RegDay, &tmp);
    if (ret < 0) {
        return (ret);
    }

    tmp &= _RTCMK_DAY_DAY_MASK;

    *val = toBIN(tmp);

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Sets the content of day register
                                                                               *register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[in] val
                                                                               *   Value to set to
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_SetDay(uint8_t addr, uint8_t val) {
    int ret = -1;
    int day = toBCD(val);
    day &= _RTCMK_DAY_DAY_MASK;
    int bcdVal = toBCD(val);
    bcdVal &= _RTCMK_DAY_DAY_MASK;
    ret = RTCMK_RegisterSet(addr, RTCMK_RegDay, day);
    if (ret < 0) {
        return (ret);
    }

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Sets the content of hour
                                                                               *register register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[in] val
                                                                               *   Value to set to
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_SetHour(uint8_t addr, uint8_t val) {
    int ret = -1;
    int bcdVal = toBCD(val);
    bcdVal &= _RTCMK_HOUR_HOUR_MASK;
    ret = RTCMK_RegisterSet(addr, RTCMK_RegHour, bcdVal);
    if (ret < 0) {
        return (ret);
    }

    return (ret);
}
/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Sets the content of minute
                                                                               *register register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[in] val
                                                                               *   Value to set to
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_SetMinute(uint8_t addr, uint8_t val) {
    int ret = -1;
    int bcdVal = toBCD(val);
    bcdVal &= _RTCMK_MIN_MIN_MASK;
    ret = RTCMK_RegisterSet(addr, RTCMK_RegMin, bcdVal);
    if (ret < 0) {
        return (ret);
    }

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Sets the content of minute
                                                                               *register register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[in] val
                                                                               *   Value to set to
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_SetMonth(uint8_t addr, uint8_t val) {
    int ret = -1;

    int bcdVal = toBCD(val);
    bcdVal &= _RTCMK_MONTH_MONTH_MASK;
    ret = RTCMK_RegisterSet(addr, RTCMK_RegMonth, bcdVal);
    if (ret < 0) {
        return (ret);
    }

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Sets the content of second
                                                                               *register register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[in] val
                                                                               *   Value to set to
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_SetSecond(uint8_t addr, uint8_t val) {
    int ret = -1;
    int bcdVal = toBCD(val);
    bcdVal &= _RTCMK_SEC_SEC_MASK;
    ret = RTCMK_RegisterSet(addr, RTCMK_RegSec, bcdVal);
    if (ret < 0) {
        return (ret);
    }

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Sets the content of year
                                                                               *register register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[in] val
                                                                               *   Value to set to
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_SetYear(uint8_t addr, uint8_t val) {
    int ret = -1;

    int bcdVal = toBCD(val);
    bcdVal &= _RTCMK_YEAR_YEAR_MASK;
    ret = RTCMK_RegisterSet(addr, RTCMK_RegYear, bcdVal);
    if (ret < 0) {
        return (ret);
    }

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Sets the content of week
                                                                               *register register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[in] val
                                                                               *   Value to set to
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_SetWeek(uint8_t addr, uint8_t val) {
    int ret = -1;

    int bcdVal = toBCD(val);
    bcdVal &= _RTCMK_WEEK_WEEK_MASK;
    ret = RTCMK_RegisterSet(addr, RTCMK_RegWeek, bcdVal);
    if (ret < 0) {
        return (ret);
    }

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Returns current content of
                                                                               *minutes alarm register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[out] val
                                                                               *   Reference to place result.
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_ReadMinutesAlarm(uint8_t addr, uint8_t *val) {
    int ret = -1;

    uint8_t tmp = 0;

    ret = RTCMK_RegisterGet(addr, RTCMK_RegMinAlarm, &tmp);
    if (ret < 0) {
        return (ret);
    }

    tmp &= _RTCMK_MINALARM_MIN_MASK;

    *val = ((tmp & 0xF0) >> 4) * 10 + (tmp & 0x0F);

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Returns current content of hour
                                                                               *alarm register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[out] val
                                                                               *   Reference to place result.
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_ReadHourAlarm(uint8_t addr, uint8_t *val) {
    int ret = -1;

    uint8_t tmp = 0;

    ret = RTCMK_RegisterGet(addr, RTCMK_RegHourAlarm, &tmp);
    if (ret < 0) {
        return (ret);
    }

    tmp &= _RTCMK_HOURALARM_HOUR_MASK;

    *val = ((tmp & 0xF0) >> 4) * 10 + (tmp & 0x0F);

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Returns current content of week
                                                                               *day alarm register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[out] val
                                                                               *   Reference to place result.
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_ReadWeekAlarm(uint8_t addr, uint8_t *val) {
    int ret = -1;

    uint8_t tmp = 0;

    ret = RTCMK_RegisterGet(addr, RTCMK_RegWeekDayAlarm, &tmp);
    if (ret < 0) {
        return (ret);
    }

    tmp &= _RTCMK_WEEKDAYALARM_WEEKDAY_MASK;

    *val = ((tmp & 0xF0) >> 4) * 10 + (tmp & 0x0F);

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Returns current content of
                                                                               *select register.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[out] val
                                                                               *   Reference to place result.
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_ReadSelect(uint8_t addr, uint8_t *val) {
    int ret = -1;

    uint8_t tmp = 0;

    ret = RTCMK_RegisterGet(addr, RTCMK_RegSelect, &tmp);
    if (ret < 0) {
        return (ret);
    }

    tmp &= _RTCMK_COUNTER_COUNTER_MASK;

    *val = ((tmp & 0xF0) >> 4) * 10 + (tmp & 0x0F);

    return (ret);
}
/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Returns current content of flag
                                                                               *register.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[out] val
                                                                               *   Reference to place result.
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_ReadFlag(uint8_t addr, uint8_t *val) {
    int ret = -1;

    uint8_t tmp = 0;

    ret = RTCMK_RegisterGet(addr, RTCMK_RegFlag, &tmp);
    if (ret < 0) {
        return (ret);
    }

    *val = ((tmp & 0xF0) >> 4) * 10 + (tmp & 0x0F);

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Returns current content of
                                                                               *control register.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[out] val
                                                                               *   Reference to place result.
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_ReadControl(uint8_t addr, uint8_t *val) {
    int ret = -1;

    uint8_t tmp = 0;

    ret = RTCMK_RegisterGet(addr, RTCMK_RegControl, &tmp);
    if (ret < 0) {
        return (ret);
    }

    *val = ((tmp & 0xF0) >> 4) * 10 + (tmp & 0x0F);

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Sets the content of week alarm
                                                                               *register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[in] val
                                                                               *   Value to set to
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_SetWeekAlarm(uint8_t addr, uint8_t val) {
    int ret = -1;

    ret = RTCMK_RegisterSet(addr, RTCMK_RegWeekDayAlarm, val);
    if (ret < 0) {
        return (ret);
    }

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Sets the content of minute alarm
                                                                               *register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[in] val
                                                                               *   Value to set to
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_SetMinAlarm(uint8_t addr, uint8_t val) {
    int ret = -1;

    ret = RTCMK_RegisterSet(addr, RTCMK_RegMinAlarm, val);
    if (ret < 0) {
        return (ret);
    }

    return (ret);
}

/***************************************************************************/ /**
                                                                               * @brief
                                                                               *   Sets the content of hour alarm
                                                                               *register in decimal.
                                                                               *
                                                                               * @param[in] addr
                                                                               *   I2C address, in 8 bit format,
                                                                               *where LSB is reserved for R/W bit.
                                                                               *
                                                                               * @param[in] val
                                                                               *   Value to set to
                                                                               *
                                                                               * @return
                                                                               *   Returns 0 if registers written,
                                                                               *<0 if unable to write to registers.
                                                                               ******************************************************************************/
int RTCMK_SetHourAlarm(uint8_t addr, uint8_t val) {
    int ret = -1;

    ret = RTCMK_RegisterSet(addr, RTCMK_RegHourAlarm, val);
    if (ret < 0) {
        return (ret);
    }

    return (ret);
}
