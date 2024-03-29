/*
 * Copyright (C) 2015  University of Alberta
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "iris_gio.h"
#include "HL_gio.h"
#include "HL_reg_gio.h"
#include "HL_reg_het.h"

/**
 * @brief
 *   Initialize GPIO directions
 **/
void iris_gio_init() {
    gioSetDirection(hetPORT1, 0xFFFFFFFF);
    gioSetDirection(gioPORTB, 0xFFFFFFFF);
}

/**
 * @brief
 *   Pull slave select high via GPIO pin
 **/
void iris_nss_high() { gioSetBit(hetPORT1, IRIS_NSS_PIN, 1); }

/**
 * @brief
 *   Pull slave select low via GPIO pin
 **/
void iris_nss_low() { gioSetBit(hetPORT1, IRIS_NSS_PIN, 0); }

#if IS_ATHENA == 1
/**
 * @brief
 *   Pull ~RESET high via GPIO pin
 **/
void iris_reset_high() { gioSetBit(gioPORTB, IRIS_nRST_PIN, 1); }

/**
 * @brief
 *   Pull ~RESET low via GPIO pin
 **/
void iris_reset_low() { gioSetBit(gioPORTB, IRIS_nRST_PIN, 0); }

/**
 * @brief
 *   Pull BOOT high via GPIO pin
 **/
void iris_boot_high() { gioSetBit(hetPORT1, IRIS_BOOT_PIN, 1); }

/**
 * @brief
 *   Pull BOOT low via GPIO pin
 **/
void iris_boot_low() { gioSetBit(hetPORT1, IRIS_BOOT_PIN, 0); }
#else
/**
 * @brief
 *   Pull ~RESET high via GPIO pin
 **/
void iris_reset_high() { gioSetBit(hetPORT1, IRIS_nRST_PIN, 1); }

/**
 * @brief
 *   Pull ~RESET low via GPIO pin
 **/
void iris_reset_low() { gioSetBit(hetPORT1, IRIS_nRST_PIN, 0); }

/**
 * @brief
 *   Pull BOOT high via GPIO pin
 **/
void iris_boot_high() { gioSetBit(hetPORT1, IRIS_BOOT_PIN, 1); }

/**
 * @brief
 *   Pull BOOT low via GPIO pin
 **/
void iris_boot_low() { gioSetBit(hetPORT1, IRIS_BOOT_PIN, 0); }
#endif
