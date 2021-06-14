//*****************************************************************************
//
// bl_flash.c     : The file holds the main control loop of the boot loader.
// Author         : QJ Wang. qjwang@ti.com
// Date           : 9-19-2012
//
// Copyright (c) 2006-2011 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
//
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
//
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
//
//*****************************************************************************

#define _L2FMC
#include "bl_eeprom.h"
#include "bl_flash.h"
#include "F021.h"
#include "flash_defines.h"
#include "F021_API/Helpers.h"

//#define Freq_In_MHz = SYS_CLK_FRE#include "bl_config.h"Q;

//*****************************************************************************
//
// Returns the size of the ist sector size of the flash in bytes.
//
//*****************************************************************************
uint32_t
BLInternalFlashFirstSectorSizeGet(void)
{
	uint32_t firstSectorSize;
	firstSectorSize = (uint32_t)(flash_sector[0].start) + flash_sector[0].length;
    return (firstSectorSize);
}
//*****************************************************************************
//
// Returns the size of the internal flash in bytes.
//
// This function returns the total number of bytes of internal flash in the
// current part.  No adjustment is made for any sections reserved via
// options defined in bl_config.h.
//
// \return Returns the total number of bytes of internal flash.
//
//*****************************************************************************
uint32_t
BLInternalFlashSizeGet(void)
{
	uint32_t flashSize;
	flashSize = (uint32_t)flash_sector[NUMBEROFSECTORS-1].start + flash_sector[NUMBEROFSECTORS-1].length;
    return (flashSize);
}

//*****************************************************************************
//
//! Checks whether a given start address is valid for a download.
//!
//! This function checks to determine whether the given address is a valid
//! download image start address given the options defined in bl_config.h.
//!
//! \return Returns true if the address is valid or false otherwise.
//
//*****************************************************************************
bool
BLInternalFlashStartAddrCheck(uint32_t ulAddr, uint32_t ulImgSize)
{
    uint32_t count=0, i;

	if (ulImgSize == 0) {
	    return false;
	}

	/* The start address must be at the begining of the sector */
    for (i = 0; i < NUMBEROFSECTORS; i++){
		if ((ulAddr >= (uint32_t)(flash_sector[i].start)) && (ulAddr < ((uint32_t)flash_sector[i].start + flash_sector[i].length)))
		{
			count++;
		}
	}
    if (count == 0){
    	return false;
    }

    // Is the address we were passed a valid start address?  We allow:
    // Must be greater than GOLD_MINIMUM_ADDR
    // If flashing to the golden image bank, the size of the image may not write higher than 0x00200000. Which is the start of bank 1
    // if flashing to the application image bank, the image may not write higher than 0x003FFFFF. Which is the end of bank 1
    if (ulAddr <= GOLD_MINIMUM_ADDR) {
        return false;
    } else if (ulAddr < APP_MINIMUM_ADDR  && ulAddr + ulImgSize <= 0x00200000) {
        return true;
    } else if (ulAddr >= APP_MINIMUM_ADDR && ulAddr + ulImgSize <= 0x003FFFFF) {
        return true;
    } else {
        return false;
    }
}


//#pragma CODE_SECTION (Fapi_BlockErase, ".myTest")
uint32_t Fapi_BlockErase(uint32_t ulAddr, uint32_t Size)
{
	uint8_t  i=0, ucStartBank, ucEndBank, ucStartSector, ucEndSector;
    uint32_t EndAddr, status;

	EndAddr = ulAddr + Size;
	for (i = 0; i < NUMBEROFSECTORS; i++){
		if ((ulAddr >= (uint32_t)(flash_sector[i].start)) && (ulAddr < ((uint32_t)flash_sector[i].start + flash_sector[i].length)))
		{
			ucStartBank     = flash_sector[i].bankNumber;
		    ucStartSector   = i;
		    break;
		}
	}

	for (i = ucStartSector; i < NUMBEROFSECTORS; i++){
		if (EndAddr <= (((uint32_t)flash_sector[i].start) + flash_sector[i].length))
		{
			ucEndBank   = flash_sector[i].bankNumber;
			ucEndSector = i;
		    break;
		}
	}

	status=Fapi_initializeFlashBanks((uint32_t)SYS_CLK_FREQ); /* used for API Rev2.01 */
	Fapi_enableAutoEccCalculation();

    for (i = ucStartBank; i < (ucEndBank + 1); i++){
        Fapi_setActiveFlashBank((Fapi_FlashBankType)i);
        Fapi_enableMainBankSectors(0xFFFF);                 /* used for API 2.01*/
        while( FAPI_CHECK_FSM_READY_BUSY != Fapi_Status_FsmReady );
    }

    for (i=ucStartSector; i<(ucEndSector+1); i++){
		Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector, flash_sector[i].start);
    	while( FAPI_CHECK_FSM_READY_BUSY == Fapi_Status_FsmBusy );
    	while(FAPI_GET_FSM_STATUS != Fapi_Status_Success);
    }

//    status =  Flash_Erase_Check((uint32_t)ulAddr, Size);

	return (status);
}

//Bank here is not used. We calculate the bank in the function based on the Flash-Start-addr
uint32_t Fapi_BlockProgram( uint32_t Bank, uint32_t Flash_Address, uint32_t Data_Address, uint32_t SizeInBytes)
{
	register uint32_t src = Data_Address;
	register uint32_t dst = Flash_Address;
	uint32_t bytes;

	if (SizeInBytes < 32)
		bytes = SizeInBytes;
	else
		bytes = 32;

	if ((Fapi_initializeFlashBanks((uint32_t)SYS_CLK_FREQ)) == Fapi_Status_Success){
		 (void)Fapi_enableAutoEccCalculation();
		 (void)Fapi_setActiveFlashBank((Fapi_FlashBankType)Bank);
	     (void)Fapi_enableMainBankSectors(0xFFFF);                    /* used for API 2.01*/
	}else {
         return (1);
	}

	while( FAPI_CHECK_FSM_READY_BUSY != Fapi_Status_FsmReady );
	while( FAPI_GET_FSM_STATUS != Fapi_Status_Success );

    while( SizeInBytes > 0)
	{
		Fapi_issueProgrammingCommand((uint32_t *)dst,
									 (uint8_t *)src,
									 (uint32_t) bytes,
									 0,
									 0,
									 Fapi_AutoEccGeneration);

 		while( FAPI_CHECK_FSM_READY_BUSY == Fapi_Status_FsmBusy );
        while(FAPI_GET_FSM_STATUS != Fapi_Status_Success);

		src += bytes;
		dst += bytes;
		SizeInBytes -= bytes;
        if ( SizeInBytes < 32){
           bytes = SizeInBytes;
        }
    }
	return (0);
}


uint32_t Fapi_UpdateStatusProgram( uint32_t Bank, uint32_t Flash_Start_Address, uint32_t Data_Start_Address, uint32_t Size_In_Bytes)
{
	register uint32_t src = Data_Start_Address;
	register uint32_t dst = Flash_Start_Address;
	unsigned int bytes, status;

	if (Size_In_Bytes < 16)
		bytes = Size_In_Bytes;
	else
		bytes = 16;

	Fapi_initializeAPI((Fapi_FmcRegistersType *)F021_CPU0_REGISTER_ADDRESS, (uint32_t)SYS_CLK_FREQ);
	Fapi_setActiveFlashBank((Fapi_FlashBankType)Bank);
	Fapi_issueProgrammingCommand((uint32_t *)dst,
									 (uint8_t *)src,
									 (uint32_t) bytes,   //8,
									 0,
									 0,
									 Fapi_AutoEccGeneration);

 	while( Fapi_checkFsmForReady() == Fapi_Status_FsmBusy );
	status =  Flash_Program_Check(Flash_Start_Address, Data_Start_Address, Size_In_Bytes);
	return (status);
}



uint32_t Flash_Program_Check(uint32_t Program_Start_Address, uint32_t Source_Start_Address, uint32_t No_Of_Bytes)
{
	register uint32_t *src1 = (uint32_t *) Source_Start_Address;
	register uint32_t *dst1 = (uint32_t *) Program_Start_Address;
	register uint32_t bytes = No_Of_Bytes;

	while(bytes > 0)
	{	
		if(*dst1++ != *src1++)
			return (1);   //error

		bytes -= 0x4;
	}
	return(0);
}	


uint32_t Flash_Erase_Check(uint32_t Start_Address, uint32_t Bytes)
{
	uint32_t error=0;
	register uint32_t *dst1 = (uint32_t *) Start_Address;
	register uint32_t bytes = Bytes;

	while(bytes > 0)
	{	
		if(*dst1++ != 0xFFFFFFFF){
			error = 2;
		}
		bytes -= 0x4;
	}
	return(error);
}



uint32_t Fapi_BlockRead( uint32_t Bank, uint32_t Flash_Start_Address, uint32_t Data_Start_Address, uint32_t Size_In_Bytes)
{
	register uint32_t src = Data_Start_Address;
	register uint32_t dst = Flash_Start_Address;
	register uint32_t bytes_remain = Size_In_Bytes;
	int bytes;

	if (Size_In_Bytes < 16)
		bytes = Size_In_Bytes;
	else
		bytes = 16;
	Fapi_initializeAPI((Fapi_FmcRegistersType *)F021_CPU0_REGISTER_ADDRESS, (uint32_t)SYS_CLK_FREQ);

 	while( bytes_remain > 0)
	{
		Fapi_doMarginReadByByte((uint8_t *)src,
								(uint8_t *)dst,
								(uint32_t) bytes,                //16
								Fapi_NormalRead);
		src += bytes;
		dst += bytes;
        bytes_remain -= bytes;
    }
	return (0);
}
