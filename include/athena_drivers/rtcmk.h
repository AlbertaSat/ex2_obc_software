/*
 * rtcmk.h
 *
 *  Created on: May 28, 2018
 *      Author: sdamk
 */

#ifndef DRIVERS_RTCMK_H_
#define DRIVERS_RTCMK_H_		

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#define RTCMK_ADDR				(0x64)

#define RTCMK_SEC_SEC				              (0x7FUL << 0)
#define _RTCMK_SEC_SEC_SHIFT		          0
#define _RTCMK_SEC_SEC_MASK 		          0x7FUL
#define RTCMK_MIN_MIN				              (0x7FUL << 0)
#define _RTCMK_MIN_MIN_SHIFT		          0
#define _RTCMK_MIN_MIN_MASK 		          0x7FUL
#define RTCMK_HOUR_HOUR				            (0x3FUL << 0)
#define _RTCMK_HOUR_HOUR_SHIFT		        0
#define _RTCMK_HOUR_HOUR_MASK 		        0x3FUL
#define RTCMK_WEEK_WEEK				            (0x07UL << 0)
#define _RTCMK_WEEK_WEEK_SHIFT		        0
#define _RTCMK_WEEK_WEEK_MASK 		        0x07UL
#define _RTCMK_WEEK_WEEK_SUN		          0x00UL
#define _RTCMK_WEEK_WEEK_MON		          0x01UL
#define _RTCMK_WEEK_WEEK_TUE		          0x02UL
#define _RTCMK_WEEK_WEEK_WED		          0x03UL
#define _RTCMK_WEEK_WEEK_THU		          0x04UL
#define _RTCMK_WEEK_WEEK_FRI		          0x05UL
#define _RTCMK_WEEK_WEEK_SAT		          0x06UL
#define RTCMK_WEEK_WEEK_SUN			          (_RTCMK_WEEK_WEEK_SUN << 0)
#define RTCMK_WEEK_WEEK_MON			          (_RTCMK_WEEK_WEEK_MON << 0)
#define RTCMK_WEEK_WEEK_TUE			          (_RTCMK_WEEK_WEEK_TUE << 0)
#define RTCMK_WEEK_WEEK_WED			          (_RTCMK_WEEK_WEEK_WED << 0)
#define RTCMK_WEEK_WEEK_THU			          (_RTCMK_WEEK_WEEK_THU << 0)
#define RTCMK_WEEK_WEEK_FRI			          (_RTCMK_WEEK_WEEK_FRI << 0)
#define RTCMK_WEEK_WEEK_SAT			          (_RTCMK_WEEK_WEEK_SAT << 0)
#define RTCMK_DAY_DAY				              (0x3FUL << 0)
#define _RTCMK_DAY_DAY_SHIFT		          0
#define _RTCMK_DAY_DAY_MASK 		          0x3FUL
#define RTCMK_MONTH_MONTH			            (0x1FUL << 0)
#define _RTCMK_MONTH_MONTH_SHIFT	        0
#define _RTCMK_MONTH_MONTH_MASK 	        0x1FUL
#define _RTCMK_MONTH_MONTH_JAN		        0x01UL
#define _RTCMK_MONTH_MONTH_FEB		        0x02UL
#define _RTCMK_MONTH_MONTH_MAR		        0x03UL
#define _RTCMK_MONTH_MONTH_MAY		        0x04UL
#define _RTCMK_MONTH_MONTH_APR		        0x05UL
#define _RTCMK_MONTH_MONTH_JUN		        0x06UL
#define _RTCMK_MONTH_MONTH_JUL		        0x07UL
#define _RTCMK_MONTH_MONTH_AUG		        0x08UL
#define _RTCMK_MONTH_MONTH_SEP		        0x09UL
#define _RTCMK_MONTH_MONTH_OCT		        0x0AUL
#define _RTCMK_MONTH_MONTH_NOV		        0x0BUL
#define _RTCMK_MONTH_MONTH_DEC		        0x0CUL
#define RTCMK_MONTH_MONTH_JAN		          (_RTCMK_MONTH_MONTH_JAN << 0)
#define RTCMK_MONTH_MONTH_FEB		          (_RTCMK_MONTH_MONTH_FEB << 0)
#define RTCMK_MONTH_MONTH_MAR		          (_RTCMK_MONTH_MONTH_MAR << 0)
#define RTCMK_MONTH_MONTH_MAY		          (_RTCMK_MONTH_MONTH_MAY << 0)
#define RTCMK_MONTH_MONTH_APR		          (_RTCMK_MONTH_MONTH_APR << 0)
#define RTCMK_MONTH_MONTH_JUN		          (_RTCMK_MONTH_MONTH_JUN << 0)
#define RTCMK_MONTH_MONTH_JUL		          (_RTCMK_MONTH_MONTH_JUL << 0)
#define RTCMK_MONTH_MONTH_AUG		          (_RTCMK_MONTH_MONTH_AUG << 0)
#define RTCMK_MONTH_MONTH_SEP		          (_RTCMK_MONTH_MONTH_SEP << 0)
#define RTCMK_MONTH_MONTH_OCT		          (_RTCMK_MONTH_MONTH_OCT << 0)
#define RTCMK_MONTH_MONTH_NOV		          (_RTCMK_MONTH_MONTH_NOV << 0)
#define RTCMK_MONTH_MONTH_DEC		          (_RTCMK_MONTH_MONTH_DEC << 0)
#define RTCMK_YEAR_YEAR				            (0xFFUL << 0)
#define _RTCMK_YEAR_YEAR_SHIFT		        0
#define _RTCMK_YEAR_YEAR_MASK 		        0xFFUL
#define RTCMK_MINALARM_MIN			          (0x7FUL << 0)
#define _RTCMK_MINALARM_MIN_SHIFT	        0
#define _RTCMK_MINALARM_MIN_MASK 	        0x7FUL
#define RTCMK_MINALARM_AE			            (0x01UL << 7)
#define _RTCMK_MINALARM_AE_SHIFT	        7
#define _RTCMK_MINALARM_AE_MASK 	        0x80UL
#define _RTCMK_MINALARM_AE_ENABLE 	      0x00UL
#define _RTCMK_MINALARM_AE_DISABLE 	      0x01UL
#define RTCMK_MINALARM_AE_ENABLE 	        (_RTCMK_MINALARM_AE_ENABLE  << 7)
#define RTCMK_MINALARM_AE_DISABLE	        (_RTCMK_MINALARM_AE_DISABLE << 7)
#define RTCMK_HOURALARM_HOUR              (0x7FUL << 0)
#define _RTCMK_HOURALARM_HOUR_SHIFT       0
#define _RTCMK_HOURALARM_HOUR_MASK        0x7FUL
#define RTCMK_HOURALARM_AE                (0x01UL << 7)
#define _RTCMK_HOURALARM_AE_SHIFT         7
#define _RTCMK_HOURALARM_AE_MASK          0x80UL
#define _RTCMK_HOURALARM_AE_ENABLE        0x00UL
#define _RTCMK_HOURALARM_AE_DISABLE       0x01UL
#define RTCMK_HOURALARM_AE_ENABLE         (_RTCMK_HOURALARM_AE_ENABLE  << 7)
#define RTCMK_HOURALARM_AE_DISABLE        (_RTCMK_HOURALARM_AE_DISABLE << 7)
#define RTCMK_WEEKDAYALARM_WEEKDAY        (0x7FUL << 0)
#define _RTCMK_WEEKDAYALARM_WEEKDAY_SHIFT 0
#define _RTCMK_WEEKDAYALARM_WEEKDAY_MASK  0x7FUL
#define _RTCMK_WEEKDAYALARM_WEEKDAY_SUN   0x00UL
#define _RTCMK_WEEKDAYALARM_WEEKDAY_MON   0x01UL
#define _RTCMK_WEEKDAYALARM_WEEKDAY_TUE   0x02UL
#define _RTCMK_WEEKDAYALARM_WEEKDAY_WED   0x03UL
#define _RTCMK_WEEKDAYALARM_WEEKDAY_THU   0x04UL
#define _RTCMK_WEEKDAYALARM_WEEKDAY_FRI   0x05UL
#define _RTCMK_WEEKDAYALARM_WEEKDAY_SAT   0x06UL
#define RTCMK_WEEKDAYALARM_WEEKDAY_SUN    (_RTCMK_WEEKDAYALARM_WEEKDAY_SUN << 0)
#define RTCMK_WEEKDAYALARM_WEEKDAY_MON    (_RTCMK_WEEKDAYALARM_WEEKDAY_MON << 0)
#define RTCMK_WEEKDAYALARM_WEEKDAY_TUE    (_RTCMK_WEEKDAYALARM_WEEKDAY_TUE << 0)
#define RTCMK_WEEKDAYALARM_WEEKDAY_WED    (_RTCMK_WEEKDAYALARM_WEEKDAY_WED << 0)
#define RTCMK_WEEKDAYALARM_WEEKDAY_THU    (_RTCMK_WEEKDAYALARM_WEEKDAY_THU << 0)
#define RTCMK_WEEKDAYALARM_WEEKDAY_FRI    (_RTCMK_WEEKDAYALARM_WEEKDAY_FRI << 0)
#define RTCMK_WEEKDAYALARM_WEEKDAY_SAT    (_RTCMK_WEEKDAYALARM_WEEKDAY_SAT << 0)
#define _RTCMK_WEEKDAYALARM_AE_SHIFT      7
#define _RTCMK_WEEKDAYALARM_AE_MASK       0x80UL
#define _RTCMK_WEEKDAYALARM_AE_ENABLE     0x00UL
#define _RTCMK_WEEKDAYALARM_AE_DISABLE    0x01UL
#define RTCMK_WEEKDAYALARM_AE_ENABLE      (_RTCMK_WEEKDAYALARM_AE_ENABLE  << 7)
#define RTCMK_WEEKDAYALARM_AE_DISABLE     (_RTCMK_WEEKDAYALARM_AE_DISABLE << 7)
#define RTCMK_WEEKDAYALARM_RAM            (0x01UL << 6)
#define _RTCMK_WEEKDAYALARM_RAM_SHIFT     6
#define _RTCMK_WEEKDAYALARM_RAM_MASK      0x40UL
#define RTCMK_COUNTER_COUNTER             (0xFFUL << 0)
#define _RTCMK_COUNTER_COUNTER_SHIFT      0
#define _RTCMK_COUNTER_COUNTER_MASK       0xFFUL
#define RTCMK_SELECT_UTS                  (0x01UL << 0)
#define _RTCMK_SELECT_UTS_SHIFT           0
#define _RTCMK_SELECT_UTS_MASK            0x01UL
#define _RTCMK_SELECT_UTS_SEC             0x00UL
#define _RTCMK_SELECT_UTS_MIN             0x01UL
#define RTCMK_SELECT_UTS_SEC              (_RTCMK_SELECT_UTS_SEC << 0)
#define RTCMK_SELECT_UTS_MIN              (_RTCMK_SELECT_UTS_MIN << 0)
#define RTCMK_SELECT_AS                   (0x01UL << 1)
#define _RTCMK_SELECT_AS_SHIFT            1
#define _RTCMK_SELECT_AS_MASK             0x02UL
#define _RTCMK_SELECT_AS_DAYOFWEEK        0x00UL
#define _RTCMK_SELECT_AS_DAY              0x01UL
#define RTCMK_SELECT_AS_WEEK              (_RTCMK_SELECT_AS_WEEK << 1)
#define RTCMK_SELECT_AS_DAYOFWEEK         (_RTCMK_SELECT_AS_DAYOFWEEK << 1)
#define RTCMK_SELECT_AS_DAY               (_RTCMK_SELECT_AS_DAY << 1)
#define RTCMK_SELECT_TSS                  (0x03UL << 2)
#define _RTCMK_SELECT_TSS_SHIFT           2
#define _RTCMK_SELECT_TSS_MASK            0x0CUL
#define _RTCMK_SELECT_TSS_4096HZ          0x00UL
#define _RTCMK_SELECT_TSS_64HZ            0x01UL
#define _RTCMK_SELECT_TSS_1HZ             0x02UL
#define _RTCMK_SELECT_TSS_1PERMIN         0x03UL
#define RTCMK_SELECT_TSS_4096HZ           (_RTCMK_SELECT_TSS_4096HZ  << 2)
#define RTCMK_SELECT_TSS_64HZ             (_RTCMK_SELECT_TSS_64HZ    << 2)
#define RTCMK_SELECT_TSS_1HZ              (_RTCMK_SELECT_TSS_1HZ     << 2)
#define RTCMK_SELECT_TSS_1PERMIN          (_RTCMK_SELECT_TSS_1PERMIN << 2)
#define RTCMK_SELECT_CFS                  (0x03UL << 4)
#define _RTCMK_SELECT_CFS_SHIFT           4
#define _RTCMK_SELECT_CFS_MASK            0x30UL
#define _RTCMK_SELECT_CFS_DEFAULT         0x00UL
#define _RTCMK_SELECT_CFS_32768HZ         0x00UL
#define _RTCMK_SELECT_CFS_1024HZ          0x01UL
#define _RTCMK_SELECT_CFS_32HZ            0x02UL
#define _RTCMK_SELECT_CFS_1HZ             0x03UL
#define RTCMK_SELECT_CFS_DEFAULT          (_RTCMK_SELECT_CFS_DEFAULT << 4)
#define RTCMK_SELECT_CFS_32768HZ          (_RTCMK_SELECT_CFS_32768HZ << 4)
#define RTCMK_SELECT_CFS_1024HZ           (_RTCMK_SELECT_CFS_1024HZ  << 4)
#define RTCMK_SELECT_CFS_32HZ             (_RTCMK_SELECT_CFS_32HZ    << 4)
#define RTCMK_SELECT_CFS_1HZ              (_RTCMK_SELECT_CFS_1HZ     << 4)
#define RTCMK_SELECT_TCS                  (0x03UL << 6)
#define _RTCMK_SELECT_TCS_SHIFT           6
#define _RTCMK_SELECT_TCS_MASK            0xC0UL
#define _RTCMK_SELECT_TCS_DEFAULT         0x00UL
#define _RTCMK_SELECT_TCS_500MS           0x00UL
#define _RTCMK_SELECT_TCS_2S              0x01UL
#define _RTCMK_SELECT_TCS_10S             0x02UL
#define _RTCMK_SELECT_TCS_30S             0x03UL
#define RTCMK_SELECT_TCS_DEFAULT          (_RTCMK_SELECT_TCS_DEFAULT << 6)
#define RTCMK_SELECT_TCS_500MS            (_RTCMK_SELECT_TCS_500MS   << 6)
#define RTCMK_SELECT_TCS_2S               (_RTCMK_SELECT_TCS_2S      << 6)
#define RTCMK_SELECT_TCS_10S              (_RTCMK_SELECT_TCS_10S     << 6)
#define RTCMK_SELECT_TCS_30S              (_RTCMK_SELECT_TCS_30S     << 6)
#define RTCMK_FLAG_UTF                    (0x01UL << 0)
#define _RTCMK_FLAG_UTF_SHIFT             0
#define _RTCMK_FLAG_UTF_MASK              0x01UL
#define RTCMK_FLAG_AF                     (0x01UL << 1)
#define _RTCMK_FLAG_AF_SHIFT              1
#define _RTCMK_FLAG_AF_MASK               0x02UL
#define RTCMK_FLAG_TF                     (0x01UL << 2)
#define _RTCMK_FLAG_TF_SHIFT              2
#define _RTCMK_FLAG_TF_MASK               0x04UL
#define RTCMK_FLAG_VDLF                   (0x01UL << 4)
#define _RTCMK_FLAG_VDLF_SHIFT            4
#define _RTCMK_FLAG_VDLF_MASK             0x10UL
#define _RTCMK_FLAG_VDLF_DEFAULT          0x01UL
#define RTCMK_FLAG_VDLF_DEFAULT           (_RTCMK_FLAG_VDLF_DEFAULT << 5)
#define RTCMK_FLAG_VDHF                   (0x01UL << 5)
#define _RTCMK_FLAG_VDHF_SHIFT            5
#define _RTCMK_FLAG_VDHF_MASK             0x20UL
#define RTCMK_CONTROL_UTIE                (0x01UL << 0)
#define _RTCMK_CONTROL_UTIE_SHIFT         0
#define _RTCMK_CONTROL_UTIE_MASK          0x01UL
#define _RTCMK_CONTROL_UTIE_DEFAULT       0x00UL
#define _RTCMK_CONTROL_UTIE_DISABLE       0x00UL
#define _RTCMK_CONTROL_UTIE_ENABLE        0x01UL
#define RTCMK_CONTROL_UTIE_DEFAULT        (_RTCMK_CONTROL_UTIE_DEFAULT << 0)
#define RTCMK_CONTROL_UTIE_DISABLE        (_RTCMK_CONTROL_UTIE_DISABLE << 0)
#define RTCMK_CONTROL_UTIE_ENABLE         (_RTCMK_CONTROL_UTIE_ENABLE << 0)
#define RTCMK_CONTROL_AIE                 (0x01UL << 1)
#define _RTCMK_CONTROL_AIE_SHIFT          1
#define _RTCMK_CONTROL_AIE_MASK           0x02UL
#define _RTCMK_CONTROL_AIE_DEFAULT        0x00UL
#define _RTCMK_CONTROL_AIE_DISABLE        0x00UL
#define _RTCMK_CONTROL_AIE_ENABLE         0x01UL
#define RTCMK_CONTROL_AIE_DEFAULT         (_RTCMK_CONTROL_AIE_DEFAULT << 1)
#define RTCMK_CONTROL_AIE_DISABLE         (_RTCMK_CONTROL_AIE_DISABLE << 1)
#define RTCMK_CONTROL_AIE_ENABLE          (_RTCMK_CONTROL_AIE_ENABLE << 1)
#define RTCMK_CONTROL_TIE                 (0x01UL << 2)
#define _RTCMK_CONTROL_TIE_SHIFT          2
#define _RTCMK_CONTROL_TIE_MASK           0x04UL
#define _RTCMK_CONTROL_TIE_DEFAULT        0x00UL
#define _RTCMK_CONTROL_TIE_DISABLE        0x00UL
#define _RTCMK_CONTROL_TIE_ENABLE         0x01UL
#define RTCMK_CONTROL_TIE_DEFAULT         (_RTCMK_CONTROL_TIE_DEFAULT << 2)
#define RTCMK_CONTROL_TIE_DISABLE         (_RTCMK_CONTROL_TIE_DISABLE << 2)
#define RTCMK_CONTROL_TIE_ENABLE          (_RTCMK_CONTROL_TIE_ENABLE << 2)
#define RTCMK_CONTROL_TE                  (0x01UL << 3)
#define _RTCMK_CONTROL_TE_SHIFT           3
#define _RTCMK_CONTROL_TE_MASK            0x08UL
#define _RTCMK_CONTROL_TE_DEFAULT         0x00UL
#define _RTCMK_CONTROL_TE_STOP            0x00UL
#define _RTCMK_CONTROL_TE_START           0x01UL
#define RTCMK_CONTROL_TE_DEFAULT          (_RTCMK_CONTROL_TE_DEFAULT << 3)
#define RTCMK_CONTROL_TE_STOP             (_RTCMK_CONTROL_TE_STOP << 3)
#define RTCMK_CONTROL_TE_START            (_RTCMK_CONTROL_TE_START << 3)
#define RTCMK_CONTROL_FIE                 (0x01UL << 4)
#define _RTCMK_CONTROL_FIE_SHIFT          4
#define _RTCMK_CONTROL_FIE_MASK           0x10UL
#define _RTCMK_CONTROL_FIE_DEFAULT        0x00UL
#define _RTCMK_CONTROL_FIE_DISABLE        0x00UL
#define _RTCMK_CONTROL_FIE_ENABLE         0x01UL
#define RTCMK_CONTROL_FIE_DEFAULT         (_RTCMK_CONTROL_FIE_DEFAULT << 4)
#define RTCMK_CONTROL_FIE_DISABLE         (_RTCMK_CONTROL_FIE_DISABLE << 4)
#define RTCMK_CONTROL_FIE_ENABLE          (_RTCMK_CONTROL_FIE_ENABLE << 4)
#define RTCMK_CONTROL_RAM                 (0x01UL << 5)
#define _RTCMK_CONTROL_RAM_SHIFT          5
#define _RTCMK_CONTROL_RAM_MASK           0x20UL
#define RTCMK_CONTROL_TEST                (0x01UL << 6)
#define _RTCMK_CONTROL_TEST_SHIFT         6
#define _RTCMK_CONTROL_TEST_MASK          0x40UL
#define _RTCMK_CONTROL_TEST_DEFAULT       0x00UL
#define _RTCMK_CONTROL_TEST_DISABLE       0x00UL
#define _RTCMK_CONTROL_TEST_ENABLE        0x01UL
#define RTCMK_CONTROL_TEST_DEFAULT        (_RTCMK_CONTROL_TEST_DEFAULT << 6)
#define RTCMK_CONTROL_TEST_DISABLE        (_RTCMK_CONTROL_TEST_DISABLE << 6)
#define RTCMK_CONTROL_TEST_ENABLE         (_RTCMK_CONTROL_TEST_ENABLE << 6)
#define RTCMK_CONTROL_RESET               (0x01UL << 7)
#define _RTCMK_CONTROL_RESET_SHIFT        7
#define _RTCMK_CONTROL_RESET_MASK         0x80UL

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

typedef enum
{
  RTCMK_RegSec 			    =   0x00,  /**< Seconds register */
  RTCMK_RegMin 			    =   0x01,  /**< Minutes register */
  RTCMK_RegHour 		    = 	0x02,  /**< Hours register */
  RTCMK_RegWeek 		    =	  0x03,  /**< Day of week register */
  RTCMK_RegDay 			    = 	0x04,  /**< Day of month register */
  RTCMK_RegMonth 		    = 	0x05,  /**< Month register */
  RTCMK_RegYear 		    = 	0x06,  /**< Year register */
  RTCMK_RegMinAlarm 	  =   0x07,  /**< Alarm minutes register */
  RTCMK_RegHourAlarm    =   0x08,  /**< Alarm hours register */
  RTCMK_RegWeekDayAlarm =   0x09,  /**< Alarm weeks or days register */
  RTCMK_RegCounter		  =   0x0A,  /**< Timer counter register  */
  RTCMK_RegSelect		    =   0x0B,  /**< Select register */
  RTCMK_RegFlag			    =   0x0C,  /**< Flag register */
  RTCMK_RegControl		  =   0x0D   /**< Control register */
} RTCMK_Register_TypeDef;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

int RTCMK_RegisterSet(/*I2C_TypeDef *i2c,*/
                        uint8_t addr,
                        RTCMK_Register_TypeDef reg,
                        uint8_t val);

int RTCMK_RegisterGet(/*I2C_TypeDef *i2c,*/
                        uint8_t addr,
                        RTCMK_Register_TypeDef reg,
                        uint8_t *val);

int RTCMK_ResetTime(/*I2C_TypeDef *i2c,*/
                        uint8_t addr);

int RTCMK_ReadSeconds(/*I2C_TypeDef *i2c,*/
                        uint8_t addr,
                        uint8_t *val);

int RTCMK_ReadMinutes(/*I2C_TypeDef *i2c,*/
                        uint8_t addr,
                        uint8_t *val);

int RTCMK_ReadHours(/*I2C_TypeDef *i2c,*/
                        uint8_t addr,
                        uint8_t *val);

#endif /* DRIVERS_RTCMK_H_ */
