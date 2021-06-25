//*****************************************************************************
//
// bl_flash.h - Flash programming functions used by the boot loader.
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
//
//*****************************************************************************


#ifndef __BL_FLASH_H__
#define __BL_FLASH_H__

#include "HL_sys_common.h"
#include <stdbool.h>

//*****************************************************************************
//
// Basic functions for erasing and programming internal flash.
//
//*****************************************************************************
extern uint32_t BLInternalFlashFirstSectorSizeGet(void);
extern uint32_t BLInternalFlashSizeGet(void);
extern bool BLInternalFlashStartAddrCheck(uint32_t ulAddr, uint32_t ulImgSize);

extern uint32_t Fapi_BlockProgram( uint32_t Bank, uint32_t Flash_Start_Address, uint32_t Data_Start_Address, uint32_t Size_In_Bytes);

extern uint32_t Fapi_BlockErase(uint32_t Flash_Start_Address, uint32_t Size_In_Bytes);

extern uint32_t Fapi_UpdateStatusProgram( uint32_t Bank, uint32_t Flash_Start_Address, uint32_t Data_Start_Address, uint32_t Size_In_Bytes);
                                
extern uint32_t Flash_Erase_Check(uint32_t Start_Address, uint32_t Bytes);

extern uint32_t Flash_Program_Check(uint32_t Program_Start_Address, uint32_t Source_Start_Address, uint32_t No_Of_Bytes);

#endif // __BL_FLASH_H__
