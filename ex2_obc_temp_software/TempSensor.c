/*
 * TempSensor.c
 *
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

#include <TempSensor.h>
#include "HL_sys_common.h"
#include "HL_adc.h"
#include "HL_pinmux.h"

/*
 * HALCoGen Setup
 * 		File -> New -> Project
 * 		Family -> Hercules
 * 		Device -> <TMS570LC43xx/RM48x>
 *		Ok
 *
 *		Driver Enable
 * 		- Enable ADC, [SCI1 and GIO](optional for debug),  drivers
 *
 *
 *		Select the target <TMS570LC43xx/RM48x>
 *
 *		Uncheck default file location and point to the HALCoGen project path
 *
 *		Use the same project name as the HALCoGen project.
 *
 *		Select empty project without main.c
 *
 *		Finish
 *
 *		Open the project properties
 *
 *		CCS Build -> ARM Compiler -> include option
 *
 *		Add a new #include search path "../include"
 *
 */
/* Thermistor */
#ifndef __little_endian__
#define __little_endian__  0
#endif

#if __little_endian__
	typedef struct OTP_temperature_calibration_data
	{
		uint16_t Temperature;
		uint16_t AdcValue;
	} OTP_temperature_calibration_data_t;
#else
	typedef struct OTP_temperature_calibration_data
	{
		uint16_t AdcValue;
		uint16_t Temperature;
	} OTP_temperature_calibration_data_t;
#endif

#define THERMISTOR_CAL_DATA		0xF0080310 /* OTP Temperature Sensor Data Location */

typedef struct Thermistor_Calibration
{
    float slope;
    float offset;
    float rsquared;
} Thermistor_CAL_t;


static Thermistor_CAL_t Thermistor_Fit = {0.0, 0.0, 0.0};


/*************************************************************/
/**** Start of Temp Sensor functions                       ***/
/*************************************************************/

/** @fn float thermistor_read(void)
*   @brief read on-chip thermistor 3
*   @note This will return the temperature of thermistor 3 in Kelvin
*
*   This requires adcInit() to be called before to setup ADC2.
*
*   This function will modify the Pin Muxing and ADC2 to read the thermistor,
*   care has been taken to restore modified configurations however the user is
*   responsible for verifying both the Pin Muxing and ADC are configured
*   as desired.
*
*   The returned temperature will need to be scaled by the reference voltage difference
*   Calibration values are taken at nominal voltage 3.30V.
*
*   Kelvin = ReturnValue * (VccADrefHi - VccADrefLow)/3.30V
*	Celsius = Kelvin - 273.15;
*	Fahrenheit = (Kelvin - 273.15) * 1.8 + 32;
*
*/
float thermistor_read()
{	unsigned int value, pinmux_restore, GxSEL_restore;
	float JunctionTempK;
	adcBASE_t *adcreg;

	/* Select the ADC */
	adcreg = adcREG2;

	if(adcreg->G1SR != 0x00000008 )
		return (-1.0); // Group 1 is being used

	/* Check that we have valid calibration data */
	if(Thermistor_Fit.rsquared == 0.0)
		return (-2.0); //Calibration data missing, must run thermistor_calibration() first

	/* Enable Temperature Sensors in Pin Muxing */

	/* Enable Pin Muxing */
	pinMuxReg->KICKER0 = 0x83E70B13U;
	pinMuxReg->KICKER1 = 0x95A4F1E0U;

	/* Enable Temp Sensor */
	pinMuxReg->PINMUX[174] &= 0xFEFFFFFF;

	/* Connect Sensor 3 - Temperature sensor 3's output is connected to AD2IN[30] */
	pinmux_restore = pinMuxReg->PINMUX[174];
	pinMuxReg->PINMUX[174] = (pinMuxReg->PINMUX[174] & 0xfffffffe) | 0x00000002;

	/* Start Converting, Choose Channel */
	GxSEL_restore = adcreg->GxSEL[1U]; // Save the original value in the channel select register
	adcreg->GxSEL[1U] = 0x40000000;

	/* Poll for end of Conversion */
	while(!(adcreg->G1SR & 1));

	/* Read adc value */
	value = adcreg->GxBUF[1U].BUF0;

	/* Disable Temperature Sensor */
	pinMuxReg->PINMUX[174] |= 0x01000000;

	/* Restore Sensor 3 Pin Muxing */
	pinMuxReg->PINMUX[174] = pinmux_restore;

	/* Disable Pin Muxing */
	pinMuxReg->KICKER0 = 0x00000000U;
	pinMuxReg->KICKER1 = 0x00000000U;

	/* Restore Channel Select Register */
	adcreg->GxSEL[1U] = GxSEL_restore;

	/* Convert ADC value into floating point temp Kelvin */
	JunctionTempK = (((float)value) - Thermistor_Fit.offset) *
			Thermistor_Fit.slope;

	return JunctionTempK;
} // thermistor_read


/** @fn void thermistor_calibration(void)
*   @brief Load the thermister calibration information
*   @note
*/
bool thermistor_calibration(void)
{
	OTP_temperature_calibration_data_t *OTPdataptr;
	int i, cal_data_count=0;
	float slope,offset,sumtemp=0,sumconv=0,sumtempxconv=0,sumtempxtemp=0;
	float cal_adc_code_array[4],avgconv,cal_temperature_array[4],yx=0.0,ya=0.0,ym,yn;

	/* Create pointer to temp sensor 3 calibration data */
	OTPdataptr = (OTP_temperature_calibration_data_t *)(THERMISTOR_CAL_DATA + (2 * 0x10));

	/* Check for valid calibration data */
	/* Valid codes are 0 to 0xFFF, valid temperatures are 0 to 400 kelvin */
	for(i = 0; i  < 4; i++)
	{

		/* Calculate Slope and Offset for the 4 possible value pairs */
		if((OTPdataptr[i].AdcValue   < 0xFFF) && (OTPdataptr[i].Temperature < 401))
		{
			/* Load valid calibration information */
			cal_temperature_array[i] = (float)(OTPdataptr[i].Temperature);
			cal_adc_code_array[i] = (float)OTPdataptr[i].AdcValue;
			sumtemp += cal_temperature_array[i];
			sumconv += cal_adc_code_array[i];
			sumtempxconv += cal_temperature_array[i] * cal_adc_code_array[i];
			sumtempxtemp += cal_temperature_array[i] * cal_temperature_array[i];
			cal_data_count++;
		}
	}

	/* Calculate slope and Offset for the 4 possible value pairs */
	if(cal_data_count < 2)
		return FALSE;
	else
	{
		slope = (sumtempxtemp * cal_data_count - sumtemp * sumtemp) /
				(sumtempxconv * cal_data_count - sumtemp * sumconv);
		offset = (sumconv - sumtemp / slope) / cal_data_count;
		Thermistor_Fit.slope = slope;
		Thermistor_Fit.offset = offset;
		avgconv = sumconv / cal_data_count;
	}

	/* Calculate R-Squared Value */
	for(i = 0; i < cal_data_count; i++)
	{
		yn = (((cal_temperature_array[i] / slope) + offset) - cal_adc_code_array[i]);
		yx = yx + (yn * yn);
		ym = (avgconv - cal_adc_code_array[i]);
		ya = ya + (ym * ym);
	}
	Thermistor_Fit.rsquared = 1.0 - (yx / ya);

	return TRUE;

} // thermistor_calibrate
