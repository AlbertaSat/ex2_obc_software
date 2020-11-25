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
/**
 * @file uhf_hal.c
 * @author Arash Yazdani
 * @date 2020-10-26
 */

/*
 * When TRX connected, the stubbed blocks can be used for TRX = off situation.
 */
#include <FreeRTOS.h>

#include "uhf_hal.h"
#include "services.h"
#include "queue.h"
#include <stdio.h>
#include <string.h>

#ifdef UHF_IS_STUBBED
    // Arbitrary values for testing
    static UHF_Status U_status_reg = {.uptime = 12, .pckts_out = 100, .pckts_in = 70, .pckts_in_crc16 = 10, .temperature = 18.4, .firmware_ver = 2, .payload_size = 127, .secure_key = 32};
#else
    static UHF_Status U_status_reg;
#endif
static UHF_Call_Sign U_call_reg;
static UHF_Beacon U_beacon_reg;
static UHF_framStruct U_FRAM_reg;


UHF_return HAL_UHF_setStatus (uint8_t * U_stat_ctrl){
    memcpy(&U_status_reg.status_ctrl, U_stat_ctrl, 12);
    #ifdef UHF_IS_STUBBED
        return IS_STUBBED_U;
    #else
        return UHF_genericWrite(0, &U_status_reg.status_ctrl);
    #endif
}

UHF_return HAL_UHF_setFreq (uint32_t U_freq){
    U_status_reg.set.freq = U_freq;
    #ifdef UHF_IS_STUBBED
        return IS_STUBBED_U;
    #else
        return UHF_genericWrite(1, &U_status_reg.set.freq);
    #endif
}

UHF_return HAL_UHF_setPipeT (uint16_t U_pipe_t){
    U_status_reg.set.pipe_t = U_pipe_t;
    #ifdef UHF_IS_STUBBED
        return IS_STUBBED_U;
    #else
        return UHF_genericWrite(6, &U_status_reg.set.pipe_t);
    #endif
}

UHF_return HAL_UHF_setBeaconT (uint16_t U_beacon_t){
    U_status_reg.set.beacon_t = U_beacon_t;
    #ifdef UHF_IS_STUBBED
        return IS_STUBBED_U;
    #else
        return UHF_genericWrite(7, &U_status_reg.set.beacon_t);
    #endif
}

UHF_return HAL_UHF_setAudioT (uint16_t U_audio_t){
    U_status_reg.set.audio_t = U_audio_t;
    #ifdef UHF_IS_STUBBED
        return IS_STUBBED_U;
    #else
        return UHF_genericWrite(8, &U_status_reg.set.audio_t);
    #endif
}

UHF_return HAL_UHF_restore (UHF_Confirm * U_restore){
    #ifdef UHF_IS_STUBBED
        U_restore->confirm = 5;
        return IS_STUBBED_U;
    #else
        return UHF_genericWrite(9, &U_restore->confirm);//doublecheck
    #endif
}

UHF_return HAL_UHF_lowPwr (uint8_t * U_low_pwr){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericWrite(244, &U_status_reg.low_pwr_stat);
    #else
        U_status_reg.low_pwr_stat = 5;
        status = IS_STUBBED_U;
    #endif
    *U_low_pwr = U_status_reg.low_pwr_stat;
    return status;
}

UHF_return HAL_UHF_setDestination (UHF_configStruct U_dest){
    U_call_reg.dest = U_dest;
    #ifdef UHF_IS_STUBBED
        return IS_STUBBED_U;
    #else
        return UHF_genericWrite(245, &U_call_reg.dest);
    #endif
}

UHF_return HAL_UHF_setSource (UHF_configStruct U_src){
    U_call_reg.src = U_src;
    #ifdef UHF_IS_STUBBED
        return IS_STUBBED_U;
    #else
        return UHF_genericWrite(246, &U_call_reg.src);
    #endif
}

UHF_return HAL_UHF_setMorse (UHF_configStruct U_morse){
    U_beacon_reg.morse = U_morse;
    #ifdef UHF_IS_STUBBED
        return IS_STUBBED_U;
    #else
        return UHF_genericWrite(247, &U_beacon_reg.morse);
    #endif
}

UHF_return HAL_UHF_setMIDI (UHF_configStruct U_MIDI){
    U_beacon_reg.MIDI = U_MIDI;
    #ifdef UHF_IS_STUBBED
        return IS_STUBBED_U;
    #else
        return UHF_genericWrite(248, &U_beacon_reg.MIDI);
    #endif
}

UHF_return HAL_UHF_setBeaconMsg (UHF_configStruct U_beacon_msg){
    U_beacon_reg.message = U_beacon_msg;
    #ifdef UHF_IS_STUBBED
        return IS_STUBBED_U;
    #else
        return UHF_genericWrite(251, &U_beacon_reg.message);
    #endif
}

UHF_return HAL_UHF_setI2C (uint8_t U_I2C_add){
    #ifdef UHF_IS_STUBBED
        return IS_STUBBED_U;
    #else
        return UHF_genericWrite(252, &U_I2C_add);//doublecheck
    #endif
}

UHF_return HAL_UHF_setFRAM (UHF_framStruct U_FRAM){
    U_FRAM_reg = U_FRAM;
    #ifdef UHF_IS_STUBBED
        return IS_STUBBED_U;
    #else
        return UHF_genericWrite(253, &U_FRAM_reg);
    #endif
}

UHF_return HAL_UHF_secure (UHF_Confirm * U_secure){
    #ifdef UHF_IS_STUBBED
        U_secure->confirm = 0;
        return IS_STUBBED_U;
    #else
        return UHF_genericWrite(255, U_secure);//doublecheck
    #endif
}

UHF_return HAL_UHF_getStatus (uint8_t * U_stat_ctrl){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(0, &U_status_reg.status_ctrl);
    #else
        status = IS_STUBBED_U;
    #endif
    memcpy(U_stat_ctrl,&U_status_reg.status_ctrl,12);
    return status;
}

UHF_return HAL_UHF_getFreq (uint32_t * U_freq){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(1, &U_status_reg.set.freq);
    #else
        status = IS_STUBBED_U;
    #endif
    *U_freq = U_status_reg.set.freq;
    return status;
}

UHF_return HAL_UHF_getUptime (uint32_t * U_uptime){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(2, &U_status_reg.uptime);
    #else
        status = IS_STUBBED_U;
    #endif
    *U_uptime = U_status_reg.uptime;
    return status;
}

UHF_return HAL_UHF_getPcktsOut (uint32_t * U_pckts_out){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(3, &U_status_reg.pckts_out);
    #else
        status = IS_STUBBED_U;
    #endif
    *U_pckts_out = U_status_reg.pckts_out;
    return status;
}

UHF_return HAL_UHF_getPcktsIn (uint32_t * U_pckts_in){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(4, &U_status_reg.pckts_in);
    #else
        status = IS_STUBBED_U;
    #endif
    *U_pckts_in = U_status_reg.pckts_in;
    return status;
}

UHF_return HAL_UHF_getPcktsInCRC16 (uint32_t * U_pckts_in_crc16){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(5, &U_status_reg.pckts_in_crc16);
    #else
        status = IS_STUBBED_U;
    #endif
    *U_pckts_in_crc16 = U_status_reg.pckts_in_crc16;
    return status;
}

UHF_return HAL_UHF_getPipeT (uint16_t * U_pipe_t){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(6, &U_status_reg.set.pipe_t);
    #else
        status = IS_STUBBED_U;
    #endif
    *U_pipe_t = U_status_reg.set.pipe_t;
    return status;
}

UHF_return HAL_UHF_getBeaconT (uint16_t * U_beacon_t){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(7, &U_status_reg.set.beacon_t);
    #else
        status = IS_STUBBED_U;
    #endif
    *U_beacon_t = U_status_reg.set.beacon_t;
    return status;
}

UHF_return HAL_UHF_getAudioT (uint16_t * U_audio_t){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(8, &U_status_reg.set.audio_t);
    #else
        status = IS_STUBBED_U;
    #endif
    *U_audio_t = U_status_reg.set.audio_t;
    return status;
}

UHF_return HAL_UHF_getTemp (float * U_temperature){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(10, &U_status_reg.temperature);
    #else
        status = IS_STUBBED_U;
    #endif
    *U_temperature = U_status_reg.temperature;
    return status;
}

UHF_return HAL_UHF_getLowPwr (uint8_t * U_low_pwr){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(244, &U_status_reg.low_pwr_stat);
    #else
        status = IS_STUBBED_U;
    #endif
    *U_low_pwr = U_status_reg.low_pwr_stat;
    return status;
}

UHF_return HAL_UHF_getFV (uint8_t * U_firmware_ver){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(249, &U_status_reg.firmware_ver);
    #else
        status = IS_STUBBED_U;
    #endif
    *U_firmware_ver = U_status_reg.firmware_ver;
    return status;
}

UHF_return HAL_UHF_getPayload (uint16_t * U_payload_size){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(250, &U_status_reg.payload_size);
    #else
        status = IS_STUBBED_U;
    #endif
    *U_payload_size = U_status_reg.payload_size;
    return status;
}

UHF_return HAL_UHF_getSecureKey (uint32_t * U_secure){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(255, &U_status_reg.secure_key);
    #else
        status = IS_STUBBED_U;
    #endif
    *U_secure = U_status_reg.secure_key;
    return status;
}

UHF_return HAL_UHF_getDestination (UHF_configStruct * U_dest){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(245, &U_call_reg.dest);
    #else
        status = IS_STUBBED_U;
    #endif
    *U_dest = U_call_reg.dest;
    return status;
}

UHF_return HAL_UHF_getSource (UHF_configStruct * U_src){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(246, &U_call_reg.src);
    #else
        status = IS_STUBBED_U;
    #endif
    *U_src = U_call_reg.src;
    return status;
}

UHF_return HAL_UHF_getMorse (UHF_configStruct * U_morse){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(247, &U_beacon_reg.morse);
    #else
        status = IS_STUBBED_U;
    #endif
    *U_morse = U_beacon_reg.morse;
    return status;
}

UHF_return HAL_UHF_getMIDI (UHF_configStruct * U_MIDI){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(248, &U_beacon_reg.MIDI);
    #else
        status = IS_STUBBED_U;
    #endif
    *U_MIDI = U_beacon_reg.MIDI;
    return status;
}

UHF_return HAL_UHF_getBeaconMsg (UHF_configStruct * U_beacon_msg){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(251, &U_beacon_reg.message);
    #else
        status = IS_STUBBED_U;
    #endif
    *U_beacon_msg = U_beacon_reg.message;
    return status;
}

UHF_return HAL_UHF_getFRAM (UHF_framStruct * U_FRAM){
    UHF_return status;
    #ifndef UHF_IS_STUBBED
        status = UHF_genericRead(253, &U_FRAM_reg);
    #else
        status = IS_STUBBED_U;
    #endif
    *U_FRAM = U_FRAM_reg;
    return status;
}

