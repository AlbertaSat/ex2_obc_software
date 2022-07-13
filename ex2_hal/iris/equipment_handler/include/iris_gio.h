/*
 * iris_gio.h
 *
 *  Created on: Jul 5, 2022
 *      Author: jenish
 */

#ifndef EX2_HAL_IRIS_EQUIPMENT_HANDLER_INCLUDE_IRIS_GIO_H_
#define EX2_HAL_IRIS_EQUIPMENT_HANDLER_INCLUDE_IRIS_GIO_H_

#include "FreeRTOS.h"

#if IS_ATHENA == 1
#define IRIS_BOOT_PIN 20
#define IRIS_nRST_PIN 0
#define IRIS_NSS_PIN 1
#else
#define IRIS_BOOT_PIN 16
#define IRIS_nRST_PIN 18
#define IRIS_NSS_PIN 8
#endif

void iris_gio_init();
void IRIS_NSS_HIGH();
void IRIS_NSS_LOW();
void IRIS_nRST_HIGH();
void IRIS_nRST_LOW();
void IRIS_BOOT_HIGH();
void IRIS_BOOT_LOW();

#endif /* EX2_HAL_IRIS_EQUIPMENT_HANDLER_INCLUDE_IRIS_GIO_H_ */
