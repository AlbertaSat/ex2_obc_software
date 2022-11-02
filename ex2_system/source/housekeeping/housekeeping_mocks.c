/*
 * housekeeping_mocks.c
 *
 *  Created on: Sep. 7, 2022
 *      Author: Robert
 */
#include "housekeeping_mocks.h"

uint32_t mock_time() { return 1662581260; }

void mock_adcs(ADCS_HouseKeeping *hk) {
    int fake = 0;
    hk->att_estimate_mode = fake++;
    hk->att_control_mode = fake++;
    hk->run_mode = fake++;
    for (int i = 0; i < 52; i++) {
        hk->flags_arr[i] = fake++;
    }
    hk->longlatalt.x = fake++;
    hk->longlatalt.y = fake++;
    hk->longlatalt.z = fake++;
    hk->Estimated_Angular_Rate_X = fake++;
    hk->Estimated_Angular_Rate_Y = fake++;
    hk->Estimated_Angular_Rate_Z = fake++;
    hk->Estimated_Angular_Angle_X = fake++;
    hk->Estimated_Angular_Angle_Y = fake++;
    hk->Estimated_Angular_Angle_Z = fake++;
    hk->Sat_Position_ECI_X = fake++;
    hk->Sat_Position_ECI_Y = fake++;
    hk->Sat_Position_ECI_Z = fake++;
    hk->Sat_Velocity_ECI_X = fake++;
    hk->Sat_Velocity_ECI_Y = fake++;
    hk->Sat_Velocity_ECI_Z = fake++;
    hk->Sat_Position_LLH_X = fake++;
    hk->Sat_Position_LLH_Y = fake++;
    hk->Sat_Position_LLH_Z = fake++;
    hk->ECEF_Position_X = fake++;
    hk->ECEF_Position_Y = fake++;
    hk->ECEF_Position_Z = fake++;
    hk->Coarse_Sun_Vector_X = fake++;
    hk->Coarse_Sun_Vector_Y = fake++;
    hk->Coarse_Sun_Vector_Z = fake++;
    hk->Fine_Sun_Vector_X = fake++;
    hk->Fine_Sun_Vector_Y = fake++;
    hk->Fine_Sun_Vector_Z = fake++;
    hk->Nadir_Vector_X = fake++;
    hk->Nadir_Vector_Y = fake++;
    hk->Nadir_Vector_Z = fake++;
    hk->Wheel_Speed_X = fake++;
    hk->Wheel_Speed_Y = fake++;
    hk->Wheel_Speed_Z = fake++;
    hk->Mag_Field_Vector_X = fake++;
    hk->Mag_Field_Vector_Y = fake++;
    hk->Mag_Field_Vector_Z = fake++;
    hk->TC_num = fake++;
    hk->TM_num = fake++;
    hk->CommsStat_flags[0] = fake++;
    hk->CommsStat_flags[1] = fake++;
    hk->CommsStat_flags[2] = fake++;
    hk->CommsStat_flags[3] = fake++;
    hk->CommsStat_flags[4] = fake++;
    hk->CommsStat_flags[5] = fake++;
    hk->Wheel1_Current = fake++;
    hk->Wheel2_Current = fake++;
    hk->Wheel3_Current = fake++;
    hk->CubeSense1_Current = fake++;
    hk->CubeSense2_Current = fake++;
    hk->CubeControl_Current3v3 = fake++;
    hk->CubeControl_Current5v0 = fake++;
    hk->CubeStar_Current = fake++;
    hk->CubeStar_Temp = fake++;
    hk->Magnetorquer_Current = fake++;
    hk->MCU_Temp = fake++;
    hk->Rate_Sensor_Temp_X = fake++;
    hk->Rate_Sensor_Temp_Y = fake++;
    hk->Rate_Sensor_Temp_Z = fake++;
}

void mock_athena(athena_housekeeping *hk) {
    int fake = 0;
    hk->version_major = fake++;
    hk->version_minor = fake++;
    hk->version_patch = fake++;
    hk->MCU_core_temp = fake++;
    hk->converter_temp = fake++;
    hk->OBC_uptime = fake++;
    hk->vol0_usage_percent = fake++;
    hk->vol1_usage_percent = fake++;
    hk->boot_cnt = fake++;
    hk->boot_src = fake++;
    hk->last_reset_reason = fake++;
    hk->solar_panel_supply_curr = fake++;
    hk->commands_received = fake++;
    hk->heap_free = fake++;
    hk->lowest_heap_free = fake++;
}

void mock_eps_instantaneous(eps_instantaneous_telemetry_t *hk) {
    int fake = 0;
    hk->cmd = fake++;
    hk->status = fake++;
    hk->timestampInS = fake++;
    hk->uptimeInS = fake++;
    hk->bootCnt = fake++;
    hk->wdt_gs_time_left = fake++;
    hk->wdt_gs_counter = fake++;
    hk->vBatt = fake++;
    hk->curSolar = fake++;
    hk->curBattIn = fake++;
    hk->curBattOut = fake++;
    hk->outputConverterState = fake++;
    hk->outputStatus = fake++;
    hk->outputFaultStatus = fake++;
    hk->protectedOutputAccessCnt = fake++;
    hk->battMode = fake++;
    hk->mpptMode = fake++;
    hk->batHeaterMode = fake++;
    hk->batHeaterState = fake++;
    hk->PingWdt_toggles = fake++;
    hk->PingWdt_turnOffs = fake++;

    for (int i = 0; i < 2; i++) {
        hk->AOcurOutput[i] = fake++;
    }
    for (int i = 0; i < 4; i++) {
        hk->mpptConverterVoltage[i] = fake++;
    }
    for (int i = 0; i < 8; i++) {
        hk->curSolarPanels[i] = fake++;
        hk->OutputConverterVoltage[i] = fake++;
    }
    for (int i = 0; i < 18; i++) {
        hk->curOutput[i] = fake++;
        hk->outputOnDelta[i] = fake++;
        hk->outputOffDelta[i] = fake++;
        hk->outputFaultCnt[i] = fake++;
    }
    for (int i = 0; i < 14; i++) {
        hk->temp[i] = fake++;
    }
}

void mock_eps_startup(eps_startup_telemetry_t *hk) {
    int fake = 0;
    hk->cmd = fake++;
    hk->status = fake++;
    hk->timestamp = fake++;
    hk->last_reset_reason_reg = fake++;
    hk->bootCnt = fake++;
    hk->FallbackConfigUsed = fake++;
    hk->rtcInit = fake++;
    hk->rtcClkSourceLSE = fake++;
    hk->flashAppInit = fake++;
    hk->Fram4kPartitionInit = fake++;
    hk->Fram520kPartitionInit = fake++;
    hk->intFlashPartitionInit = fake++;
    hk->fwUpdInit = fake++;
    hk->FSInit = fake++;
    hk->FTInit = fake++;
    hk->supervisorInit = fake++;
    hk->uart1App = fake++;
    hk->uart2App = fake++;
    hk->tmp107Init = fake++;
}

void mock_uhf(UHF_housekeeping *hk) {
    int fake = 0;
    for (int i = 0; i < SCW_LEN; i++) {
        hk->scw[i] = fake++;
    }
    hk->freq = fake++;
    hk->pipe_t = fake++;
    hk->beacon_t = fake++;
    hk->audio_t = fake++;
    hk->uptime = fake++;
    hk->pckts_out = fake++;
    hk->pckts_in = fake++;
    hk->pckts_in_crc16 = fake++;
    hk->temperature = fake++;
}

void mock_sband(Sband_Housekeeping *hk) {
    int fake = 0;
    hk->mode = fake++;
    hk->pa_status = fake++;
    hk->frequency = fake++;
    hk->scrambler = fake++;
    hk->filter = fake++;
    hk->modulation = fake++;
    hk->rate = fake++;
    hk->bit_order = fake++;
    hk->PWRGD = fake++;
    hk->TXL = fake++;
    hk->Output_Power = fake++;
    hk->PA_Temp = fake++;
    hk->Top_Temp = fake++;
    hk->Bottom_Temp = fake++;
    hk->Bat_Current = fake++;
    hk->Bat_Voltage = fake++;
    hk->PA_Current = fake++;
    hk->PA_Voltage = fake++;
}

void mock_hyperion(Hyperion_HouseKeeping *hk) {
    int fake = 0;
    hk->Nadir_Temp1 = fake++;
    hk->Nadir_Temp_Adc = fake++;
    hk->Port_Temp1 = fake++;
    hk->Port_Temp2 = fake++;
    hk->Port_Temp3 = fake++;
    hk->Port_Temp_Adc = fake++;
    hk->Port_Dep_Temp1 = fake++;
    hk->Port_Dep_Temp2 = fake++;
    hk->Port_Dep_Temp3 = fake++;
    hk->Port_Dep_Temp_Adc = fake++;
    hk->Star_Temp1 = fake++;
    hk->Star_Temp2 = fake++;
    hk->Star_Temp3 = fake++;
    hk->Star_Temp_Adc = fake++;
    hk->Star_Dep_Temp1 = fake++;
    hk->Star_Dep_Temp2 = fake++;
    hk->Star_Dep_Temp3 = fake++;
    hk->Star_Dep_Temp_Adc = fake++;
    hk->Zenith_Temp1 = fake++;
    hk->Zenith_Temp2 = fake++;
    hk->Zenith_Temp3 = fake++;
    hk->Zenith_Temp_Adc = fake++;
    hk->Nadir_Pd1 = fake++;
    hk->Port_Pd1 = fake++;
    hk->Port_Pd2 = fake++;
    hk->Port_Pd3 = fake++;
    hk->Port_Dep_Pd1 = fake++;
    hk->Port_Dep_Pd2 = fake++;
    hk->Port_Dep_Pd3 = fake++;
    hk->Star_Pd1 = fake++;
    hk->Star_Pd2 = fake++;
    hk->Star_Pd3 = fake++;
    hk->Star_Dep_Pd1 = fake++;
    hk->Star_Dep_Pd2 = fake++;
    hk->Star_Dep_Pd3 = fake++;
    hk->Zenith_Pd1 = fake++;
    hk->Zenith_Pd2 = fake++;
    hk->Zenith_Pd3 = fake++;
    hk->Port_Voltage = fake++;
    hk->Port_Dep_Voltage = fake++;
    hk->Star_Voltage = fake++;
    hk->Star_Dep_Voltage = fake++;
    hk->Zenith_Voltage = fake++;
    hk->Port_Current = fake++;
    hk->Port_Dep_Current = fake++;
    hk->Star_Current = fake++;
    hk->Star_Dep_Current = fake++;
    hk->Zenith_Current = fake++;
}

void mock_charon(charon_housekeeping *hk) {
    int fake = 0;
    hk->crc = fake++;
    for (int i = 0; i < 8; i++) {
        hk->temparray[i] = fake++;
    }
}

void mock_dfgm(DFGM_Housekeeping *hk) {
    int fake = 0;
    hk->coreVoltage = fake++;
    hk->sensorTemp = fake++;
    hk->refTemp = fake++;
    hk->boardTemp = fake++;
    hk->posRailVoltage = fake++;
    hk->inputVoltage = fake++;
    hk->refVoltage = fake++;
    hk->inputCurrent = fake++;
    hk->reserved1 = fake++;
    hk->reserved2 = fake++;
    hk->reserved3 = fake++;
    hk->reserved4 = fake++;
}

void mock_ns(ns_telemetry *hk) {
    int fake = 0;
    hk->temp0 = fake++;
    hk->temp1 = fake++;
    hk->temp2 = fake++;
    hk->temp3 = fake++;
    hk->eNIM0 = fake++;
    hk->eNIM1 = fake++;
    hk->eNIM2 = fake++;
    hk->eNIM3 = fake++;
    hk->ram_avail = fake++;
    hk->lowest_img_num = fake++;
    hk->first_blank_img_num = fake++;
}

void mock_iris(IRIS_Housekeeping *hk) {
    int fake = 0;
    hk->vis_temp = fake++;
    hk->nir_temp = fake++;
    hk->flash_temp = fake++;
    hk->gate_temp = fake++;
    hk->imagenum = fake++;
    hk->software_version = fake++;
    hk->errornum = fake++;
    hk->MAX_5V_voltage = fake++;
    hk->MAX_5V_power = fake++;
    hk->MAX_3V_voltage = fake++;
    hk->MAX_3V_power = fake++;
    hk->MIN_5V_voltage = fake++;
    hk->MIN_3V_voltage = fake++;
}
