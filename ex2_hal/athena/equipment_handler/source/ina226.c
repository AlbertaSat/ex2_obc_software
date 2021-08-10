/*
 * ina226.c
 *
 *  Created on: May 20, 2018
 *  Modified on Aug 13, 2020
 *  ---ported to TIs Cortex R5F drivers
 *      Authors: sdamk, joshdellaz
 */



#include "HL_i2c.h"
#include "ina226.h"

//I2C_TypeDef replaced by i2cBASE_t
//The various INA226s could end up on different ports

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

  return i2c_Send(i2c, addr, 2, &data);
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

  if (i2c_Send(i2c, addr, 1, &reg) == -1) {return -1;}

  if (i2c_Receive(i2c, addr, 2, &data) == -1) {return -1;}

  *val = (((uint16_t)(data[0])) << 8) | data[1];
  return 0;
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
