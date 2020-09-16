/*
 * board_io_tests.h
 *
 *  Created on Aug 13, 2020
 *
 *      Author: joshdellaz
 */

#include "HL_het.h"
#include "HL_gio.h"
#include "HL_i2c.h"
#include "HL_sci.h"
#include "HL_can.h"
#include "HL_spi.h"

void InitIO(void);
uint8_t GPIOSetBit(uint8_t bit, uint8_t value);
int GPIOGetBit(uint8_t bit);
void GPIOTxTest(void);
void UARTTxTest(sciBASE_t * regset);
uint8_t UARTRxTest(sciBASE_t * regset);
void SPIMasterTxTest(spiBASE_t * regset);
uint8_t SPIMasterRxTest(spiBASE_t * regset);
void I2CMasterTxTest(i2cBASE_t * regset, uint8_t addr);
uint8_t I2CMasterRxTest(i2cBASE_t * regset, uint8_t addr);
uint8_t I2CSlaveRxTest(i2cBASE_t * regset, uint8_t addr);
void I2CSlaveTxTest(i2cBASE_t * regset, uint8_t addr);
void CANTxTest(canBASE_t * regset, uint8_t msgbox);
uint8_t CANRxTest(canBASE_t * regset, uint8_t msgbox);
void blinky();
void SDRAM_Test();

