/*
 * housekeeping_mocks.c
 *
 *  Created on: Sep. 7, 2022
 *      Author: Robert
 */
#include "housekeeping_mocks.h"

// universal temps
// temp function for testing. not for final project build
#define t 45
static const long tempLong = (long)t;
static const uint8_t tempu8 = (uint8_t)t;
static const int8_t temp8 = (int8_t)t;
static const double tempDouble = (t * 1.0);
static const uint16_t tempu16 = (uint16_t)t;
static const uint32_t tempu32 = (uint32_t)t;
static const float tempFloat = (t * 1.0);

uint32_t mock_time() { return 1662581260; }

void mock_adcs(ADCS_HouseKeeping *hk) {
    hk->Estimated_Angular_Rate_X = tempFloat;
    hk->Estimated_Angular_Rate_Y = tempFloat;
    hk->Estimated_Angular_Rate_Z = tempFloat;
    hk->Estimated_Angular_Angle_X = tempFloat;
    hk->Estimated_Angular_Angle_Y = tempFloat;
    hk->Estimated_Angular_Angle_Z = tempFloat;
    hk->Sat_Position_ECI_X = tempFloat;
    hk->Sat_Position_ECI_Y = tempFloat;
    hk->Sat_Position_ECI_Z = tempFloat;
    hk->Sat_Velocity_ECI_X = tempFloat;
    hk->Sat_Velocity_ECI_Y = tempFloat;
    hk->Sat_Velocity_ECI_Z = tempFloat;
    hk->Sat_Position_LLH_X = tempFloat;
    hk->Sat_Position_LLH_Y = tempFloat;
    hk->Sat_Position_LLH_Z = tempFloat;
    hk->ECEF_Position_X = tempFloat;
    hk->ECEF_Position_Y = tempFloat;
    hk->ECEF_Position_Z = tempFloat;
    hk->Coarse_Sun_Vector_X = tempFloat;
    hk->Coarse_Sun_Vector_Y = tempFloat;
    hk->Coarse_Sun_Vector_Z = tempFloat;
    hk->Fine_Sun_Vector_X = tempFloat;
    hk->Fine_Sun_Vector_Y = tempFloat;
    hk->Fine_Sun_Vector_Z = tempFloat;
    hk->Nadir_Vector_X = tempFloat;
    hk->Nadir_Vector_Y = tempFloat;
    hk->Nadir_Vector_Z = tempFloat;
    hk->Wheel_Speed_X = tempFloat;
    hk->Wheel_Speed_Y = tempFloat;
    hk->Wheel_Speed_Z = tempFloat;
    hk->Mag_Field_Vector_X = tempFloat;
    hk->Mag_Field_Vector_Y = tempFloat;
    hk->Mag_Field_Vector_Z = tempFloat;
    hk->TC_num = tempFloat;
    hk->TM_num = tempFloat;
    hk->CommsStat_flags[0] = tempFloat;
    hk->CommsStat_flags[1] = tempFloat;
    hk->CommsStat_flags[2] = tempFloat;
    hk->CommsStat_flags[3] = tempFloat;
    hk->CommsStat_flags[4] = tempFloat;
    hk->CommsStat_flags[5] = tempFloat;
    hk->Wheel1_Current = tempFloat;
    hk->Wheel2_Current = tempFloat;
    hk->Wheel3_Current = tempFloat;
    hk->CubeSense1_Current = tempFloat;
    hk->CubeSense2_Current = tempFloat;
    hk->CubeControl_Current3v3 = tempFloat;
    hk->CubeControl_Current5v0 = tempFloat;
    hk->CubeStar_Current = tempFloat;
    hk->CubeStar_Temp = tempFloat;
    hk->Magnetorquer_Current = tempFloat;
    hk->MCU_Temp = tempFloat;
    hk->Rate_Sensor_Temp_X = tempFloat;
    hk->Rate_Sensor_Temp_Y = tempFloat;
    hk->Rate_Sensor_Temp_Z = tempFloat;
}

void mock_athena(athena_housekeeping *hk) {
    hk->MCU_core_temp = tempLong;
    hk->converter_temp = tempLong;
    hk->boot_cnt = tempu16;
    hk->OBC_uptime = tempu16;
    hk->solar_panel_supply_curr = tempu8;
    hk->version_major = tempu16;
    hk->version_minor = tempu16;
    hk->version_patch = tempu16;
    hk->commands_received = tempu16;
}

void mock_eps_instantaneous(eps_instantaneous_telemetry_t *hk) {
    hk->cmd = tempu8;
    hk->status = temp8;
    hk->timestampInS = tempDouble;
    hk->uptimeInS = tempu32;
    hk->bootCnt = tempu32;
    hk->wdt_gs_time_left = tempu32;
    hk->wdt_gs_counter = tempu32;
    hk->vBatt = tempu16;
    hk->curSolar = tempu16;
    hk->curBattIn = tempu16;
    hk->curBattOut = tempu16;
    hk->outputConverterState = tempu8;
    hk->outputStatus = tempu32;
    hk->outputFaultStatus = tempu32;
    hk->protectedOutputAccessCnt = tempu16;
    hk->battMode = tempu8;
    hk->mpptMode = tempu8;
    hk->batHeaterMode = tempu8;
    hk->batHeaterState = tempu8;
    hk->PingWdt_toggles = tempu16;
    hk->PingWdt_turnOffs = tempu8;

    for (int i = 0; i < 2; i++) {
        hk->AOcurOutput[i] = tempu16;
    }
    for (int i = 0; i < 4; i++) {
        hk->mpptConverterVoltage[i] = tempu16;
    }
    for (int i = 0; i < 8; i++) {
        hk->curSolarPanels[i] = tempu16;
        hk->OutputConverterVoltage[i] = tempu16;
    }
    for (int i = 0; i < 18; i++) {
        hk->curOutput[i] = tempu16;
        hk->outputOnDelta[i] = tempu16;
        hk->outputOffDelta[i] = tempu16;
        hk->outputFaultCnt[i] = tempu8;
    }
    for (int i = 0; i < 14; i++) {
        hk->temp[i] = temp8;
    }
}

void mock_eps_startup(eps_startup_telemetry_t *hk) { memset(hk, 45, sizeof(eps_startup_telemetry_t)); }

void mock_uhf(UHF_housekeeping *hk) {
    hk->freq = tempu32;
    hk->pipe_t = tempu32;
    hk->beacon_t = tempu32;
    hk->audio_t = tempu32;
    hk->uptime = tempu32;
    hk->pckts_out = tempu32;
    hk->pckts_in = tempu32;
    hk->pckts_in_crc16 = tempu32;
    hk->temperature = tempFloat;

    for (int i = 0; i < SCW_LEN; i++) {
        hk->scw[i] = tempu8;
    }
}

void mock_sband(Sband_Housekeeping *hk) {
    hk->Output_Power = tempFloat;
    hk->PA_Temp = tempFloat;
    hk->Top_Temp = tempFloat;
    hk->Bottom_Temp = tempFloat;
    hk->Bat_Current = tempFloat;
    hk->Bat_Voltage = tempFloat;
    hk->PA_Current = tempFloat;
    hk->PA_Voltage = tempFloat;
}

void mock_hyperion(Hyperion_HouseKeeping *hk) {
    hk->Nadir_Temp1 = tempFloat;
    hk->Port_Temp1 = tempFloat;
    hk->Port_Temp2 = tempFloat;
    hk->Port_Temp3 = tempFloat;
    hk->Port_Temp_Adc = tempFloat;
    hk->Port_Dep_Temp1 = tempFloat;
    hk->Port_Dep_Temp2 = tempFloat;
    hk->Port_Dep_Temp3 = tempFloat;
    hk->Port_Dep_Temp_Adc = tempFloat;
    hk->Star_Temp1 = tempFloat;
    hk->Star_Temp2 = tempFloat;
    hk->Star_Temp3 = tempFloat;
    hk->Star_Temp_Adc = tempFloat;
    hk->Star_Dep_Temp1 = tempFloat;
    hk->Star_Dep_Temp2 = tempFloat;
    hk->Star_Dep_Temp3 = tempFloat;
    hk->Star_Dep_Temp_Adc = tempFloat;
    hk->Zenith_Temp1 = tempFloat;
    hk->Zenith_Temp2 = tempFloat;
    hk->Zenith_Temp3 = tempFloat;
    hk->Zenith_Temp_Adc = tempFloat;
    hk->Nadir_Pd1 = tempFloat;
    hk->Port_Pd1 = tempFloat;
    hk->Port_Pd2 = tempFloat;
    hk->Port_Pd3 = tempFloat;
    hk->Port_Dep_Pd1 = tempFloat;
    hk->Port_Dep_Pd2 = tempFloat;
    hk->Port_Dep_Pd3 = tempFloat;
    hk->Star_Pd1 = tempFloat;
    hk->Star_Pd2 = tempFloat;
    hk->Star_Pd3 = tempFloat;
    hk->Star_Dep_Pd1 = tempFloat;
    hk->Star_Dep_Pd2 = tempFloat;
    hk->Star_Dep_Pd3 = tempFloat;
    hk->Zenith_Pd1 = tempFloat;
    hk->Zenith_Pd2 = tempFloat;
    hk->Zenith_Pd3 = tempFloat;
    hk->Port_Voltage = tempFloat;
    hk->Port_Dep_Voltage = tempFloat;
    hk->Star_Voltage = tempFloat;
    hk->Star_Dep_Voltage = tempFloat;
    hk->Zenith_Voltage = tempFloat;
    hk->Port_Current = tempFloat;
    hk->Port_Dep_Current = tempFloat;
    hk->Star_Current = tempFloat;
    hk->Star_Dep_Current = tempFloat;
    hk->Zenith_Current = tempFloat;
}

void mock_charon(charon_housekeeping *hk) {
    hk->crc = tempu16;
    for (int i = 0; i < 8; i++) {
        hk->temparray[i] = tempu8;
    }
}

void mock_dfgm(DFGM_Housekeeping *hk) {
    hk->coreVoltage = tempFloat;
    hk->sensorTemp = tempFloat;
    hk->refTemp = tempFloat;
    hk->boardTemp = tempFloat;
    hk->posRailVoltage = tempFloat;
    hk->inputVoltage = tempFloat;
    hk->refVoltage = tempFloat;
    hk->inputCurrent = tempFloat;
    hk->reserved1 = tempFloat;
    hk->reserved2 = tempFloat;
    hk->reserved3 = tempFloat;
    hk->reserved4 = tempFloat;
}

void mock_ns(ns_telemetry *hk) {
    hk->temp0 = tempu16;
    hk->temp1 = tempu16;
    hk->temp2 = tempu16;
    hk->temp3 = tempu16;
    hk->eNIM0 = tempu16;
    hk->eNIM1 = tempu16;
    hk->eNIM2 = tempu16;
    hk->ram_avail = tempu16;
    hk->lowest_img_num = tempu16;
    hk->first_blank_img_num = tempu16;
}

void mock_iris(IRIS_Housekeeping *hk) {
    hk->vis_temp = tempFloat;
    hk->nir_temp = tempFloat;
    hk->flash_temp = tempFloat;
    hk->gate_temp = tempFloat;
    hk->imagenum = tempu8;
    hk->software_version = tempu8;
    hk->errornum = tempu8;
    hk->MAX_5V_voltage = tempu16;
    hk->MAX_5V_power = tempu16;
    hk->MAX_3V_voltage = tempu16;
    hk->MAX_3V_power = tempu16;
    hk->MIN_5V_voltage = tempu16;
    hk->MIN_3V_voltage = tempu16;
}
