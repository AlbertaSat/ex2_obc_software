/*
 * i2c_io.c
 *
 *  Created on: Aug. 4, 2021
 *      Author: Robert Taylor
 */
#include <i2c_io.h>
#include "FreeRTOS.h"
#include "HL_i2c.h"
#include "os_semphr.h"
#include <stdint.h>
#include "os_task.h"

/** @struct i2Csemphr
*   @brief Interrupt mode globals
*
*/
static struct i2csemphr
{
    SemaphoreHandle_t i2c_mutex;
    SemaphoreHandle_t i2c_block;
    bool hadFailure;
} i2csemphr_t[2U];

void init_i2c_driver() {
    // these can't fail
    i2csemphr_t[0].i2c_mutex = xSemaphoreCreateMutex();
    i2csemphr_t[1].i2c_mutex = xSemaphoreCreateMutex();
    i2csemphr_t[0].i2c_block = xSemaphoreCreateBinary();
    i2csemphr_t[1].i2c_block = xSemaphoreCreateBinary();
    i2csemphr_t[0].hadFailure = false;
    i2csemphr_t[1].hadFailure = false;
}

int i2c_Receive(i2cBASE_t *i2c, uint8_t addr, uint16_t size, void *buf) {
    uint8 ret;
    uint32 index = i2c == i2cREG1 ? 0U : 1U;
    if (xSemaphoreTake(i2csemphr_t[index].i2c_mutex, I2C_TIMEOUT_MS) != pdTRUE) {
        return -1;
    }

    /* Configure address of Slave to talk to */
    i2cSetSlaveAdd(i2c, addr);
    i2cSetDirection(i2c, I2C_RECEIVER);
    i2cSetCount(i2c, size);
    i2cSetMode(i2c, I2C_MASTER);
    i2cSetStop(i2c);
    i2cSetStart(i2c);
    i2cReceive(i2c, size, buf);


    if (xSemaphoreTake(i2csemphr_t[index].i2c_block, I2C_TIMEOUT_MS) != pdTRUE) {
        i2cSetStop(i2c);
        ret = -1;
    } else {
        ret = 0;
    }

    /* Clear the Stop condition */
    i2cClearSCD(i2c);
    xSemaphoreGive(i2csemphr_t[index].i2c_mutex);
    return ret;
}

// ret =  0 on success, -1 on failure
int i2c_Send(i2cBASE_t *i2c, uint8_t addr, uint16_t size, void *buf) {
    uint8 ret;
    uint32 index = i2c == i2cREG1 ? 0U : 1U;
    if (xSemaphoreTake(i2csemphr_t[index].i2c_mutex, I2C_TIMEOUT_MS) != pdTRUE) {
        return -1;
    }

    /* Configure address of Slave to talk to */
    i2cSetSlaveAdd(i2c, addr);
    i2cSetDirection(i2c, I2C_TRANSMITTER);
    i2cSetCount(i2c, size);
    i2cSetMode(i2c, I2C_MASTER);
    i2cSetStop(i2c);
    i2cSetStart(i2c);
    i2cSend(i2c, size, buf);

    if (xSemaphoreTake(i2csemphr_t[index].i2c_block, I2C_TIMEOUT_MS) != pdTRUE) {
        i2cSetStop(i2c);
        ret = -1;
    } else {
        if (i2csemphr_t[index].hadFailure == true) {
            ret = -1;
            i2csemphr_t[index].hadFailure = false; // reset failure flag
        } else {
            ret = 0;
        }
    }

    /* Clear the Stop condition */
    i2cClearSCD(i2c);
    xSemaphoreGive(i2csemphr_t[index].i2c_mutex);
    return ret;
}

void i2cNotification(i2cBASE_t *i2c, uint32 flags) {
    uint32 reg = i2c == i2cREG1 ? 0U : 1U;

    switch (flags) {

    case I2C_NACK_INT: // nack received after start byte. attempt to recover. A nack on the start byte does not trigger an interrupt
        i2c->STR = (uint32)I2C_NACK_INT;
        i2cSetStop(i2c);
        break;

    case I2C_AL_INT: // arbitration lost, attempt to recover
        i2c->STR = (uint32)I2C_AL_INT;
        i2cSetStop(i2c);
        break;

    case I2C_SCD_INT:
        xSemaphoreGiveFromISR(i2csemphr_t[reg].i2c_block, NULL);
        i2cClearSCD(i2c);
        break;

    case I2C_ARDY_INT:
        i2csemphr_t[reg].hadFailure = true;
        i2cSetStop(i2c);
        break;

    case I2C_AAS_INT: // this shouldn't happen since Athena is not configured as a slave device
        i2cSetStop(i2c);
        break;

    default:
        break;
    }
}

