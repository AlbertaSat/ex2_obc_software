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

#ifndef UHF_HAL_H
#define UHF_HAL_H

#include <inttypes.h>
#include <csp/csp.h>
#include "services.h"
#include "communication_service.h"

#ifdef UHF_IS_STUBBED
  typedef enum{
      IS_STUBBED_U = 0, //optimize
  } UHF_return;
#else
  #include "uTransceiver.h"
#endif

UHF_return HAL_UHF_setStatus (uint8_t * U_stat_ctrl);
UHF_return HAL_UHF_setFreq (uint32_t U_freq);
UHF_return HAL_UHF_setPIPEt (uint16_t U_PIPE_t);
UHF_return HAL_UHF_setBeaconT (uint16_t U_beacon_t);
UHF_return HAL_UHF_setAudioT (uint16_t U_audio_t);
UHF_return HAL_UHF_restore (UHF_Confirm * U_restore);
UHF_return HAL_UHF_lowPwr (uint8_t * U_low_pwr);
UHF_return HAL_UHF_setDestination (UHF_configStruct U_dest);
UHF_return HAL_UHF_setSource (UHF_configStruct U_src);
UHF_return HAL_UHF_setMorse (UHF_configStruct U_morse);
UHF_return HAL_UHF_setMIDI (UHF_configStruct U_MIDI);
UHF_return HAL_UHF_setBeaconMsg (UHF_configStruct U_beacon_msg);
UHF_return HAL_UHF_setI2C (uint8_t U_I2C_add);
UHF_return HAL_UHF_setFRAM (UHF_framStruct U_FRAM);
UHF_return HAL_UHF_secure (UHF_Confirm * U_secure);

UHF_return HAL_UHF_getStatus (uint8_t * U_stat_ctrl);
UHF_return HAL_UHF_getFreq (uint32_t * U_freq);
UHF_return HAL_UHF_getUptime (uint32_t * U_uptime);
UHF_return HAL_UHF_getPcktsOut (uint32_t * U_pckts_out);
UHF_return HAL_UHF_getPcktsIn (uint32_t * U_pckts_in);
UHF_return HAL_UHF_getPcktsInCRC16 (uint32_t * U_pckts_in_crc16);
UHF_return HAL_UHF_getPIPEt (uint16_t * U_PIPE_t);
UHF_return HAL_UHF_getBeaconT (uint16_t * U_beacon_t);
UHF_return HAL_UHF_getAudioT (uint16_t * U_audio_t);
UHF_return HAL_UHF_getTemp (float * U_temperature);
UHF_return HAL_UHF_getLowPwr (uint8_t * U_low_pwr);//probably can use _lowPwr arguments if saved.
UHF_return HAL_UHF_getDestination (UHF_configStruct * U_dest);
UHF_return HAL_UHF_getSource (UHF_configStruct * U_src);
UHF_return HAL_UHF_getMorse (UHF_configStruct * U_morse);
UHF_return HAL_UHF_getMIDI (UHF_configStruct * U_MIDI);
UHF_return HAL_UHF_getFV (uint8_t * U_firmware_ver);
UHF_return HAL_UHF_getPayload (uint16_t * U_payload_size);
UHF_return HAL_UHF_getBeaconMsg (UHF_configStruct * U_beacon_msg);
UHF_return HAL_UHF_getFRAM (UHF_framStruct * U_FRAM);
UHF_return HAL_UHF_getSecureKey (uint32_t * U_secure);
//UHF_return HAL_UHF_sendCmd (uint8_t format, uint8_t s_address, uint8_t len, uint8_t * data, uint8_t n_read_bytes);


#endif /* UHF_HAL_H */
