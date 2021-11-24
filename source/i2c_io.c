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

/**
 * @Brief
 *    Initialize freertos structures for the driver
 **/
void init_i2c_driver() {
    // these can't fail
    i2csemphr_t[0].i2c_mutex = xSemaphoreCreateMutex();
    i2csemphr_t[1].i2c_mutex = xSemaphoreCreateMutex();
    i2csemphr_t[0].i2c_block = xSemaphoreCreateBinary();
    i2csemphr_t[1].i2c_block = xSemaphoreCreateBinary();
    i2csemphr_t[0].hadFailure = false;
    i2csemphr_t[1].hadFailure = false;
}

/**
 * @brief
 *   Receive an array from an i2c device
 *
 * @param[in] i2c
 *   Pointer to I2C peripheral register block.
 *
 * @param[in] addr
 *   I2C address, in 7 bit format, where LSB is reserved for R/W bit.
 *
 * @param[in] size
 *   Size of the data to expect to receive
 *
 * @param[in] buf
 *   Buffer to store received data
 *
 * @return
 *   Returns 0 data read, <0 if unable to read data.
 **/

int i2c_Receive(i2cBASE_t *i2c, uint8_t addr, uint16_t size, uint8_t *buf) {
    uint8 ret = 0;
    uint32 index = i2c == i2cREG1 ? 0U : 1U;
    uint32 i2c_mutex_timeout = pdMS_TO_TICKS(20);

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
    }

    /* Clear the Stop condition */
    i2cClearSCD(i2c);
    vTaskDelay(i2c_mutex_timeout);
    xSemaphoreGive(i2csemphr_t[index].i2c_mutex);
    return ret;
}
/**
 * @brief
 *   Send an array to an i2c device
 *
 * @param[in] i2c
 *   Pointer to I2C peripheral register block.
 *
 * @param[in] addr
 *   I2C address, in 7 bit format, where LSB is reserved for R/W bit.
 *
 * @param[in] size
 *   Size of the data to send
 *
 * @param[in] buf
 *   Buffer with data to send
 *
 * @return
 *   Returns 0 data written, <0 if unable to write data.
 **/

int i2c_Send(i2cBASE_t *i2c, uint8_t addr, uint16_t size, uint8_t *buf) {
    uint8 ret = 0;
    uint32 index = i2c == i2cREG1 ? 0U : 1U;
    uint32 i2c_mutex_timeout = pdMS_TO_TICKS(6);

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
        }
    }

    /* Clear the Stop condition */
    i2cClearSCD(i2c);
    vTaskDelay(i2c_mutex_timeout);
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

