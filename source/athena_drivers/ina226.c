/*
 * ina226.c
 *
 *  Created on: May 20, 2018
 *  Modified on Aug 13, 2020
 *  ---ported to TIs Cortex R5F drivers
 *      Authors: sdamk, joshdellaz
 */



#include "HL_i2c.h"
//#include "em_i2c.h"
//#include "em_emu.h"
#include "ina226.h"

//I2C_TypeDef replaced by i2cBASE_t

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Set content of a register.
 *
 * @param[in] i2c
 *   Pointer to I2C peripheral register block.
 *
 * @param[in] addr
 *   I2C address, in 8 bit format, where LSB is reserved for R/W bit.
 *
 * @param[in] reg
 *   Register to write (input register cannot be written).
 *
 * @param[in] val
 *   Value used when writing to register.
 *
 * @return
 *   Returns 0 if register written, <0 if unable to write to register.
 ******************************************************************************/
int INA226_RegisterSet(i2cBASE_t *i2c,
                         uint8_t addr,
                         INA226_Register_TypeDef reg,
                         uint16_t val)
{
  //I2C_TransferSeq_TypeDef seq;
  uint8_t data[2] = {0};
  data[0] = val >> 8;
  data[1] = val & 0xFF;


//  //Check if the busy is currently busy
//  if((i2c->STR & (uint32)I2C_BUSBUSY ) == 0)
//  {

    i2cSetSlaveAdd(i2cREG2, addr);
    i2cSetDirection(i2cREG2, I2C_TRANSMITTER);
    i2cSetCount(i2cREG2, 3);
    i2cSetMode(i2cREG2, I2C_MASTER);
    i2cSetStop(i2cREG2);
    i2cSetStart(i2cREG2);
    i2cSendByte(i2cREG2, reg);
    i2cSend(i2cREG2, 2, data);
    while(i2cIsBusBusy(i2cREG2) == true);
    while(i2cIsStopDetected(i2cREG2) == 0);
    i2cClearSCD(i2cREG2);

      //while (I2C_getStatus(i2c) == i2cTransferInProgress)
      //while (i2c_IsBusBusy(i2c) == true)
      //{
        /* Enter EM1 while waiting for I2C interrupt */
        //if(MAX_RETRIES < retries++)break;

        //EMU_EnterEM1();
        /* Could do a timeout function here. */
      //}
//  }
//  else
//  {
      //Busy is busy return a error
//      error = -1;
//  }
  //return((int)I2C_getStatus(i2c));
  return i2cIsBusBusy(i2cREG2);
}

/***************************************************************************//**
 * @brief
 *   Get current content of a register.
 *
 * @param[in] i2c
 *   Pointer to I2C peripheral register block.
 *
 * @param[in] addr
 *   I2C address, in 8 bit format, where LSB is reserved for R/W bit.
 *
 * @param[in] reg
 *   Register to read.
 *
 * @param[out] val
 *   Reference to place register read.
 *
 * @return
 *   Returns 0 if register read, <0 if unable to read register.
 ******************************************************************************/
int INA226_RegisterGet(i2cBASE_t *i2c,
                         uint8_t addr,
                         INA226_Register_TypeDef reg,
                         uint16_t *val)
{
  uint8_t data[2];


  i2cSetSlaveAdd(i2c, addr);
  i2cSetDirection(i2c, I2C_TRANSMITTER);
  i2cSetCount(i2c, 1);
  i2cSetMode(i2c, I2C_MASTER);
  i2cSetStop(i2c);
  i2cSetStart(i2c);
  i2cSend(i2c, 1, &reg);

  while(i2cIsBusBusy(i2c) == true);
  while(i2cIsStopDetected(i2c) == 0);
  i2cClearSCD(i2c);

  int temp;
  for (temp = 0; temp < 0x10000; temp++);//temporary fix... don't want delay down the road

  i2cSetSlaveAdd(i2c, addr);
  /* Set direction to receiver */
  i2cSetDirection(i2c, I2C_RECEIVER);
  i2cSetCount(i2c, 2);
  /* Set mode as Master */
  i2cSetMode(i2c, I2C_MASTER);
  i2cSetStop(i2c);
  /* Transmit Start Condition */
  i2cSetStart(i2c);
  i2cReceive(i2c, 2, data);


//  unsigned int retries = 0;
//  while (I2C_getStatus(i2c) == i2cTransferInProgress)
//  {
//    /* Enter EM1 while waiting for I2C interrupt */
//    if(MAX_RETRIES < retries++)break;
//    /* Enter EM1 while waiting for I2C interrupt */
//    EMU_EnterEM1();
//    /* Could do a timeout function here. */
//  }

  /* Wait until Bus Busy is cleared */
  while(i2cIsBusBusy(i2c) == true);

  /* Wait until Stop is detected */
  while(i2cIsStopDetected(i2c) == 0);

  /* Clear the Stop condition */
  i2cClearSCD(i2c);

  *val = (((uint16_t)(data[0])) << 8) | data[1];

  return((int)i2cIsBusBusy(i2c));
}

int INA226_ReadShuntVoltage(i2cBASE_t *i2c,
                         uint8_t addr,
                         uint16_t *val)
{

  int ret = -1;

  uint16_t tmp = 0;

  ret = INA226_RegisterGet(i2c,addr,INA226_RegShuntV,&tmp);

  if (ret < 0)
  {
    return(ret);
  }

  if(tmp >> 15)
  {
    *val = -(tmp & 0x7FFF);
  }
  else *val = tmp;

  return(ret);

}

int INA226_ReadBusVoltage(i2cBASE_t *i2c,
                         uint8_t addr,
                         int *val)
{

  int ret = -1;

  uint16_t tmp = 0;

  ret = INA226_RegisterGet(i2c,addr,INA226_RegBusV,&tmp);
  if (ret < 0)
  {
    return(ret);
  }

  *val = tmp & 0x7FFF;

  return(ret);

}

int INA226_ReadPower(i2cBASE_t *i2c,
                         uint8_t addr,
                         int *val)
{

  int ret = -1;

  uint16_t tmp = 0;

  ret = INA226_RegisterGet(i2c,addr,INA226_RegPower,&tmp);
  if (ret < 0)
  {
    return(ret);
  }

  *val = tmp;

  return(ret);

}

int INA226_ReadCurr(i2cBASE_t *i2c,
                         uint8_t addr,
                         int *val)
{

  int ret = -1;

  uint16_t tmp = 0;

  ret = INA226_RegisterGet(i2c,addr,INA226_RegCurr,&tmp);
  if (ret < 0)
  {
    return(ret);
  }

  if(tmp >> 15)
  {
    *val = -(tmp & 0x7FFF);
  }
  else *val = tmp;

  return(ret);

}
