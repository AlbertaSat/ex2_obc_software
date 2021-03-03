/*
 * ina226.h
 *
 *  Created on: May 20, 2018
 *  Modified on Aug 13, 2020
 *  ---ported to TIs Cortex R5F drivers
 *      Authors: sdamk, joshdellaz
 */

#ifndef DRIVERS_INA226_H_
#define DRIVERS_INA226_H_

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

                                                  /*    A1  |   A0  */
                                                  /* ---------- */
#define INA226_ADDR80     (0x80)  /*    GND |   GND */
#define INA226_ADDR82     (0x82)  /*    GND |   VS  */
#define INA226_ADDR84     (0x84)  /*    GND |   SDA */
#define INA226_ADDR86     (0x86)  /*    GND |   SCL */
#define INA226_ADDR88     (0x88)  /*    VS  |   GND */
#define INA226_ADDR8A     (0x8A)  /*    VS  |   VS  */
#define INA226_ADDR8C     (0x8C)  /*    VS  |   SCA */
#define INA226_ADDR8E     (0x8E)  /*    VS  |   SCL */
#define INA226_ADDR90     (0x90)  /*    SDA |   GND */
#define INA226_ADDR92     (0x92)  /*    SDA |   VS  */
#define INA226_ADDR94     (0x94)  /*    SDA |   SDA */
#define INA226_ADDR96     (0x96)  /*    SDA |   SCL */
#define INA226_ADDR98     (0x98)  /*    SCL |   GND */
#define INA226_ADDR9A     (0x9A)  /*    SCL |   VS  */
#define INA226_ADDR9C     (0x9C)  /*    SCL |   SDA */
#define INA226_ADDR9E     (0x9E)  /*    SCL |   SCL */

#define INA226_CONFIG_MODE                    (0x7UL << 0)
#define _INA226_CONFIG_MODE_SHIFT         0
#define _INA226_CONFIG_MODE_MASK            0x7UL
#define _INA226_CONFIG_MODE_DEFAULT       0x7UL
#define _INA226_CONFIG_MODE_POWERDOWN1  0x0UL
#define _INA226_CONFIG_MODE_SVTRIG      0x1UL
#define _INA226_CONFIG_MODE_BVTRIG        0x2UL
#define _INA226_CONFIG_MODE_SBTRIG        0x3UL
#define _INA226_CONFIG_MODE_POWERDOWN2  0x4UL
#define _INA226_CONFIG_MODE_SVCONT        0x5UL
#define _INA226_CONFIG_MODE_BVCONT        0x6UL
#define _INA226_CONFIG_MODE_SBCONT        0x7UL
#define INA226_CONFIG_MODE_DEFAULT        (_INA226_CONFIG_MODE_DEFAULT << 0)
#define INA226_CONFIG_MODE_POWERDOWN1   (_INA226_CONFIG_MODE_POWERDOWN1 << 0)
#define INA226_CONFIG_MODE_SVTRIG         (_INA226_CONFIG_MODE_SVTRIG << 0)
#define INA226_CONFIG_MODE_BVTRIG         (_INA226_CONFIG_MODE_BVTRIG << 0)
#define INA226_CONFIG_MODE_SBTRIG         (_INA226_CONFIG_MODE_SBTRIG << 0)
#define INA226_CONFIG_MODE_POWERDOWN2   (_INA226_CONFIG_MODE_POWERDOWN2 << 0)
#define INA226_CONFIG_MODE_SVCONT         (_INA226_CONFIG_MODE_SVCONT << 0)
#define INA226_CONFIG_MODE_BVCONT         (_INA226_CONFIG_MODE_BVCONT << 0)
#define INA226_CONFIG_MODE_SBCONT         (_INA226_CONFIG_MODE_SBCONT << 0)
#define INA226_CONFIG_VSHCT                   (0x7UL << 3)
#define _INA226_CONFIG_VSHCT_SHIFT        3
#define _INA226_CONFIG_VSHCT_MASK           0x38UL
#define _INA226_CONFIG_VSHCT_DEFAULT      0x5UL
#define _INA226_CONFIG_VSHCT_140US        0x0UL
#define _INA226_CONFIG_VSHCT_204US        0x1UL
#define _INA226_CONFIG_VSHCT_332US        0x2UL
#define _INA226_CONFIG_VSHCT_588US        0x3UL
#define _INA226_CONFIG_VSHCT_1100US       0x4UL
#define _INA226_CONFIG_VSHCT_2116US       0x5UL
#define _INA226_CONFIG_VSHCT_4156US       0x6UL
#define _INA226_CONFIG_VSHCT_8244US       0x7UL
#define INA226_CONFIG_VSHCT_DEFAULT       (_INA226_CONFIG_VSHCT_DEFAULT << 3)
#define INA226_CONFIG_VSHCT_140US           (_INA226_CONFIG_VSHCT_140US << 3)
#define INA226_CONFIG_VSHCT_204US           (_INA226_CONFIG_VSHCT_204US << 3)
#define INA226_CONFIG_VSHCT_332US           (_INA226_CONFIG_VSHCT_332US << 3)
#define INA226_CONFIG_VSHCT_588US           (_INA226_CONFIG_VSHCT_588US << 3)
#define INA226_CONFIG_VSHCT_1100US        (_INA226_CONFIG_VSHCT_1100US << 3)
#define INA226_CONFIG_VSHCT_2116US        (_INA226_CONFIG_VSHCT_2116US << 3)
#define INA226_CONFIG_VSHCT_4156US        (_INA226_CONFIG_VSHCT_4156US << 3)
#define INA226_CONFIG_VSHCT_8244US        (_INA226_CONFIG_VSHCT_8244US << 3)
#define INA226_CONFIG_VBUSCT                  (0x7UL << 6)
#define _INA226_CONFIG_VBUSCT_SHIFT       6
#define _INA226_CONFIG_VBUSCT_MASK        0x700UL
#define _INA226_CONFIG_VBUSCT_DEFAULT     0x5UL
#define _INA226_CONFIG_VBUSCT_140US       0x0UL
#define _INA226_CONFIG_VBUSCT_204US       0x1UL
#define _INA226_CONFIG_VBUSCT_332US       0x2UL
#define _INA226_CONFIG_VBUSCT_588US       0x3UL
#define _INA226_CONFIG_VBUSCT_1100US      0x4UL
#define _INA226_CONFIG_VBUSCT_2116US      0x5UL
#define _INA226_CONFIG_VBUSCT_4156US      0x6UL
#define _INA226_CONFIG_VBUSCT_8244US      0x7UL
#define INA226_CONFIG_VBUSCT_DEFAULT      (_INA226_CONFIG_VBUSCT_DEFAULT << 6)
#define INA226_CONFIG_VBUSCT_140US        (_INA226_CONFIG_VBUSCT_140US << 6)
#define INA226_CONFIG_VBUSCT_204US        (_INA226_CONFIG_VBUSCT_204US << 6)
#define INA226_CONFIG_VBUSCT_332US        (_INA226_CONFIG_VBUSCT_332US << 6)
#define INA226_CONFIG_VBUSCT_588US        (_INA226_CONFIG_VBUSCT_588US << 6)
#define INA226_CONFIG_VBUSCT_1100US       (_INA226_CONFIG_VBUSCT_1100US << 6)
#define INA226_CONFIG_VBUSCT_2116US       (_INA226_CONFIG_VBUSCT_2116US << 6)
#define INA226_CONFIG_VBUSCT_4156US       (_INA226_CONFIG_VBUSCT_4156US << 6)
#define INA226_CONFIG_VBUSCT_8244US       (_INA226_CONFIG_VBUSCT_8244US << 6)
#define INA226_CONFIG_AVG                     (0x7UL << 9)
#define _INA226_CONFIG_AVG_SHIFT            9
#define _INA226_CONFIG_AVG_MASK             0xE00UL
#define _INA226_CONFIG_AVG_DEFAULT        0x0UL
#define _INA226_CONFIG_AVG_1SAMPLE      0x0UL
#define _INA226_CONFIG_AVG_4SAMPLE      0x1UL
#define _INA226_CONFIG_AVG_16SAMPLE     0x2UL
#define _INA226_CONFIG_AVG_64SAMPLES    0x3UL
#define _INA226_CONFIG_AVG_128SAMPLES   0x4UL
#define _INA226_CONFIG_AVG_256SAMPLES   0x5UL
#define _INA226_CONFIG_AVG_512SAMPLES   0x6UL
#define _INA226_CONFIG_AVG_1024SAMPLES  0x7UL
#define INA226_CONFIG_AVG_DEFAULT           (_INA226_CONFIG_AVG_DEFAULT << 9)
#define INA226_CONFIG_AVG_1SAMPLE       (_INA226_CONFIG_AVG_1SAMPLE << 9)
#define INA226_CONFIG_AVG_4SAMPLE       (_INA226_CONFIG_AVG_4SAMPLE << 9)
#define INA226_CONFIG_AVG_16SAMPLE      (_INA226_CONFIG_AVG_16SAMPLE << 9)
#define INA226_CONFIG_AVG_64SAMPLES     (_INA226_CONFIG_AVG_64SAMPLES << 9)
#define INA226_CONFIG_AVG_128SAMPLES    (_INA226_CONFIG_AVG_128SAMPLES << 9)
#define INA226_CONFIG_AVG_256SAMPLES    (_INA226_CONFIG_AVG_256SAMPLES << 9)
#define INA226_CONFIG_AVG_512SAMPLES    (_INA226_CONFIG_AVG_512SAMPLES << 9)
#define INA226_CONFIG_AVG_1024SAMPLES   (_INA226_CONFIG_AVG_1024SAMPLES << 9)
#define INA226_CONFIG_RST                     (0x1UL << 15)
#define _INA226_CONFIG_RST_SHIFT            15
#define _INA226_CONFIG_RST_MASK             0x8000

#define INA226_MASKEN_LEN                       (0x1UL << 0)
#define _INA226_MASKEN_LEN_SHIFT            0
#define _INA226_MASKEN_LEN_MASK             0x1UL
#define _INA226_MASKEN_LEN_DEFAULT        0x0UL
#define INA226_MASKEN_LEN_DEFAULT         (_INA226_MASKEN_LEN_DEFAULT << 0)
#define INA226_MASKEN_APOL                    (0x1UL << 1)
#define _INA226_MASKEN_APOL_SHIFT           1
#define _INA226_MASKEN_APOL_MASK            0x2UL
#define _INA226_MASKEN_APOL_DEFAULT       0x0UL
#define INA226_MASKEN_APOL_DEFAULT        (_INA226_MASKEN_APOL_DEFAULT << 0)
#define INA226_MASKEN_OVF                       (0x1UL << 2)
#define _INA226_MASKEN_OVF_SHIFT            2
#define _INA226_MASKEN_OVF_MASK             0x4UL
#define _INA226_MASKEN_OVF_DEFAULT        0x0UL
#define INA226_MASKEN_OVF_DEFAULT         (_INA226_MASKEN_OVF_DEFAULT << 0)
#define INA226_MASKEN_CVRF                    (0x1UL << 3)
#define _INA226_MASKEN_CVRF_SHIFT           3
#define _INA226_MASKEN_CVRF_MASK            0x8UL
#define _INA226_MASKEN_CVRF_DEFAULT       0x0UL
#define INA226_MASKEN_CVRF_DEFAULT        (_INA226_MASKEN_CVRF_DEFAULT << 0)
#define INA226_MASKEN_AFF                       (0x1UL << 4)
#define _INA226_MASKEN_AFF_SHIFT            4
#define _INA226_MASKEN_AFF_MASK             0x20UL
#define _INA226_MASKEN_AFF_DEFAULT        0x0UL
#define INA226_MASKEN_AFF_DEFAULT         (_INA226_MASKEN_AFF_DEFAULT << 0)
#define INA226_MASKEN_CNVR                    (0x1UL << 10)
#define _INA226_MASKEN_CNVR_SHIFT           10
#define _INA226_MASKEN_CNVR_MASK            0x400UL
#define _INA226_MASKEN_CNVR_DEFAULT       0x0UL
#define INA226_MASKEN_CNVR_DEFAULT        (_INA226_MASKEN_CNVR_DEFAULT << 0)
#define INA226_MASKEN_POL                       (0x1UL << 11)
#define _INA226_MASKEN_POL_SHIFT            11
#define _INA226_MASKEN_POL_MASK             0x800UL
#define _INA226_MASKEN_POL_DEFAULT        0x0UL
#define INA226_MASKEN_POL_DEFAULT         (_INA226_MASKEN_POL_DEFAULT << 0)
#define INA226_MASKEN_BUL                       (0x1UL << 12)
#define _INA226_MASKEN_BUL_SHIFT            12
#define _INA226_MASKEN_BUL_MASK             0x1000UL
#define _INA226_MASKEN_BUL_DEFAULT        0x0UL
#define INA226_MASKEN_BUL_DEFAULT         (_INA226_MASKEN_BUL_DEFAULT << 0)
#define INA226_MASKEN_BOL                       (0x1UL << 13)
#define _INA226_MASKEN_BOL_SHIFT            13
#define _INA226_MASKEN_BOL_MASK             0x2000UL
#define _INA226_MASKEN_BOL_DEFAULT        0x0UL
#define INA226_MASKEN_BOL_DEFAULT         (_INA226_MASKEN_BOL_DEFAULT << 0)
#define INA226_MASKEN_SUL                       (0x1UL << 14)
#define _INA226_MASKEN_SUL_SHIFT            14
#define _INA226_MASKEN_SUL_MASK             0x4000UL
#define _INA226_MASKEN_SUL_DEFAULT        0x0UL
#define INA226_MASKEN_SUL_DEFAULT         (_INA226_MASKEN_SUL_DEFAULT << 0)
#define INA226_MASKEN_SOL                       (0x1UL << 15)
#define _INA226_MASKEN_SOL_SHIFT            15
#define _INA226_MASKEN_SOL_MASK             0x8000UL
#define _INA226_MASKEN_SOL_DEFAULT        0x0UL
#define INA226_MASKEN_SOL_DEFAULT         (_INA226_MASKEN_SOL_DEFAULT << 0)

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

typedef enum
{
  INA226_RegConfig      =   0x00,  /**< Configuration register */
  INA226_RegShuntV      =   0x01,  /**< Shunt voltage register (read-only) */
  INA226_RegBusV            =   0x02,  /**< Bus voltage register (read-only) */
  INA226_RegPower           =   0x03,  /**< Power register (read-only) */
  INA226_RegCurr            =   0x04,  /**< Current register (read-only) */
  INA226_RegCalib           =   0x05,  /**< Calibration register */
  INA226_RegMaskEn      =   0x06,  /**< Mask/Enable register */
  INA226_RegAlertLim        =   0x07,  /**< Alert limit register */
  INA226_RegManufID     =   0xFE,  /**< Manufacturer ID register */
  INA226_RegDieID       =   0xFF   /**< Die ID register */
} INA226_Register_TypeDef;


typedef enum
{
  INA226_ModePowerDown      =   0,  /**< Power-down mode */
  INA226_ModeShuntTrig      =   1,  /**< Shunt voltage, triggered mode */
  INA226_ModeBusTrig        =   2,  /**< Bus voltage, triggered mode */
  INA226_ModeShuntBusTrig   =   3,  /**< Shunt and bus, triggered mode */
  INA226_ModePowerDown2     =   4,  /**< Power-down mode */
  INA226_ModeShuntCont      =   5,  /**< Shunt voltage, continuous mode */
  INA226_ModeBusCont        =   6,  /**< Bus voltage, continuous mode */
  INA226_RegShuntBusTrig    =   7   /**< Shunt and bus, continuous mode (default) */
} INA226_Mode_TypeDef;

typedef enum
{
  INA226_BusCT140us         =   0,
  INA226_BusCT204us         =   1,
  INA226_BusCT332us         =   2,
  INA226_BusCT588us         =   3,
  INA226_BusCT1100us        =   4,  /**< (default) */
  INA226_BusCT2116us        =   5,
  INA226_BusCT4156us        =   6,
  INA226_BusCT8244us        =   7
} INA226_BusCT_TypeDef;

typedef enum
{
  INA226_ShuntCT140us       =   0,
  INA226_ShuntCT204us       =   1,
  INA226_ShuntCT332us       =   2,
  INA226_ShuntCT588us       =   3,
  INA226_ShuntCT1100us      =   4, /**< (default) */
  INA226_ShuntCT2116us      =   5,
  INA226_ShuntCT4156us      =   6,
  INA226_ShuntCT8244us      =   7
} INA226_ShuntCT_TypeDef;

typedef enum
{
  INA226_AVG1               =   0, /**< (default) */
  INA226_AVG4               =   1,
  INA226_AVG16              =   2,
  INA226_AVG64              =   3,
  INA226_AVG128             =   4,
  INA226_AVG256             =   5,
  INA226_AVG512             =   6,
  INA226_AVG1024            =   7
} INA226_AVG_TypeDef;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

int INA226_RegisterSet(i2cBASE_t *i2c,
                         uint8_t addr,
                         INA226_Register_TypeDef reg,
                         uint16_t val);

int INA226_RegisterGet(i2cBASE_t *i2c,
                         uint8_t addr,
                         INA226_Register_TypeDef reg,
                         uint16_t *val);

int INA226_ReadShuntVoltage(i2cBASE_t *i2c,
                         uint8_t addr,
                         uint16_t *val);

int INA226_ReadBusVoltage(i2cBASE_t *i2c,
                         uint8_t addr,
                         int *val);

int INA226_ReadPower(i2cBASE_t *i2c,
                         uint8_t addr,
                         int *val);

int INA226_ReadCurr(i2cBASE_t *i2c,
                         uint8_t addr,
                         int *val);

#endif /* DRIVERS_INA226_H_ */
