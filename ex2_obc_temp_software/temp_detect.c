/** @file HL_sys_main.c
*   @brief Application main file
*   @date 28.Aug.2015
*   @version 04.05.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/*
* Copyright (C) 2009-2015 Texas Instruments Incorporated - www.ti.com
*
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Include Files */
#include <FreeRTOS.h>
#include <csp/csp.h>
#include <TempSensor.h>
#include "HL_sys_common.h"

/* USER CODE BEGIN (1) */
#include "HL_adc.h"
#include "queue.h"
#include "obc.h"
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
#define MAX_JUNCTION_TEMP 150    // Check datasheet for absolute maximum junction temperature
unsigned int ErrorCode;
static float MaxTemp=0, CurrentTemp=0;
/* USER CODE END */

void demo_get_temp(void)
{
/* USER CODE BEGIN (3) */
    float JunctionTemp, AverageTemp;
    unsigned int i;
    csp_packet_t *temp_pkt;

    /*  ADC group 1 was set to 500ns discharge and 1us sample time using HalCoGen */
    /*      these values are not the default values                               */
    adcInit();
    adcMidPointCalibration(adcREG2);

    /* Calculate slope and intercept for sensor 3 (only need to do once) */
    if(thermistor_calibration() == FALSE)
    {
        // Put warning about no temperature calibration data in OTP
        ErrorCode = 1;
        return;
    }
    else
    {
        for(;;)
        {
            if((JunctionTemp=thermistor_read()) < 0.0)
            {
                /* Put warning about failure                                  */
                /*   Return = -1; ADC2 Group 1 in use                         */
                /*   Return = -2; Missing calibration values                  */
                ErrorCode = 2;
                return;
            }
            else
            {
                // Assumes ADC reference voltage is 3.30V so no scaling is required
                JunctionTemp = JunctionTemp - 273.15;  // Convert from Kelvin to Celcius
                CurrentTemp = JunctionTemp;            // Saving in static to make it easy to watch
                if(JunctionTemp > MAX_JUNCTION_TEMP)
                {
                    // Do four readings to be sure this was not just noise on the ADC voltage
                    AverageTemp = 0.0;
                    for(i = 0; i < 4; i++)
                        AverageTemp += thermistor_read();
                    JunctionTemp = (AverageTemp / 4.0) - 273.15;
                    if(JunctionTemp > MAX_JUNCTION_TEMP)
                    {
                        // Put warning "junction temperature too high" routine here
                        ErrorCode = 3;
                        return;
                    }
                }
                if(JunctionTemp > MaxTemp)
                    MaxTemp = JunctionTemp;
                temp_pkt->data[2] = JunctionTemp;//Problem is how to translate the data type here.
                /*Send the value back to temperature queue*/
                if (xQueueOverwrite(equipment_queues.temp_sensor_queue, (void *)&temp_pkt) != pdPASS) {
                    return;
                  }
                csp_buffer_free(temp_pkt);//it seems that this command can be removed
            }
        }
    }
    return;
/* USER CODE END */
}

/* USER CODE BEGIN (4) */

/* USER CODE END */




