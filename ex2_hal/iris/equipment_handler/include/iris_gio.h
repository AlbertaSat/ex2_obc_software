/*
 * iris_gio.h
 *
 *  Created on: Jul 5, 2022
 *      Author: jenish
 */

#ifndef EX2_HAL_IRIS_EQUIPMENT_HANDLER_INCLUDE_IRIS_GIO_H_
#define EX2_HAL_IRIS_EQUIPMENT_HANDLER_INCLUDE_IRIS_GIO_H_

#include "FreeRTOS.h"

#define IRIS_BOOT_PIN 20
#define IRIS_nRST_PIN 0
#define IRIS_NSS_PIN 1

#if IS_ATHENA == 1
#define IRIS_BOOT_PIN 20 // H2-40
#define IRIS_nRST_PIN 0  // H2-44
#define IRIS_NSS_PIN 7   // H1-34
#else
#define IRIS_BOOT_PIN 16
#define IRIS_nRST_PIN 18
#define IRIS_NSS_PIN 8
#endif

void iris_gio_init();
void iris_nss_high();
void iris_nss_low();
void iris_reset_high();
void iris_reset_low();
void iris_boot_high();
void iris_boot_low();

#endif /* EX2_HAL_IRIS_EQUIPMENT_HANDLER_INCLUDE_IRIS_GIO_H_ */
