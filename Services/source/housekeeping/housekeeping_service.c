/*
 * Copyright (C) 2021  University of Alberta
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
 * @file housekeeping_service.c
 * @author Haoran Qi, Andrew Rooney, Yuan Wang, Dustin Wagner, Grace Yi
 * @date 2020-07-07
 */
#include "housekeeping_service.h"

#include <FreeRTOS.h>
#include <os_task.h>
#include <os_semphr.h>
#include <os_projdefs.h>
#include <os_portmacro.h>
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <semphr.h> //for semaphore lock

#include <redposix.h> //include for file system

#include "rtcmk.h" //to get time from RTC
#include "task_manager.h"
#include "service_utilities.h"
#include "services.h"
#include "beacon_task.h"
#include "tempsense_athena.h"

uint16_t MAX_FILES = 20160; // value is 20160 (7 days) based on 30 second period
char fileName[] = "VOL0:/tempHKdata.TMP";
uint16_t current_file = 1; // Increments after file write. loops back at MAX_FILES
                           // 1 indexed
char hk_config[] = "VOL0:/HKconfig.TMP";
static uint8_t config_loaded = 0; // set to 1 after config is loaded

uint32_t *timestamps = 0;             // This is a dynamic array to handle file search by timestamp
uint16_t hk_timestamp_array_size = 0; // NOT BYTES. stored as number of items. 1 indexed. 0 element unused

SemaphoreHandle_t f_count_lock = NULL;

static uint32_t svc_wdt_counter = 0;
static uint32_t get_svc_wdt_counter() { return svc_wdt_counter; }

/**
 * @brief
 *      gets the hk file id that holds a timestamp closest to that given
 * @attention
 *      Complex algorithm. should be thoroughly tested when possible
 * @param timestamp
 *      This is the time from which the file is desired
 * @return uint16_t
 *      File ID if found. 0 if no file found
 */
uint16_t get_file_id_from_timestamp(uint32_t timestamp) {
    uint32_t threshold =
        15; // How many seconds timestamps need to be within. Currently assumes 30 second hk intervals
    // perform leftmost binary search
    uint32_t left;
    uint32_t right;
    uint32_t middle;
    uint32_t offset = 0;
    if (timestamps == NULL) {
        return 0;
    }
    if (timestamps[current_file] == 0) { // haven't made full loop of storage
        if (current_file == 1) {         // base case. no files written
            return 0;
        }
        // not enough files written to use loop portion yet
        left = 1;
        right = current_file - 1;
    } else {
        // These accomodate circular structure
        left = current_file;
        right = left + hk_timestamp_array_size - 1;
        offset = current_file - 1;
    }

    while (left < right) {
        middle = (left + right) / 2;
        if (timestamps[middle - offset] < timestamp) {
            left = middle + 1;
        } else {
            right = middle;
        }
    }
    uint32_t true_position = left - offset;
    if (true_position > 1) {
        if (timestamp - timestamps[true_position - 1] <= threshold) { // check lower neighbour if exists
            return true_position - 1;
        }
        if (true_position != hk_timestamp_array_size) { // check self if won't cause underflow
            if (timestamps[true_position] - timestamp <= threshold) {
                return true_position;
            }
        } else { // left must be max index
            if (timestamp > timestamps[true_position] &&
                timestamp - timestamps[true_position] <=
                    threshold) { // edge case left is max index. bigger value than at max
                return true_position;
            } else if (timestamps[true_position] - timestamp <=
                       threshold) { // edge case left is max index. smaller value than at max
                return true_position;
            }
        }
    } else {                                                      // left must be min index
        if (timestamps[true_position] - timestamp <= threshold) { // edge case left is min index. smaller than min
            return true_position;
        }
    }
    return 0;
}

/**
 * @brief
 *      Handles malloc, realloc, and free for array holding hk timestamps
 * @param num_items
 *      This is how many items should have space malloced
 * @return Result
 *      FAILURE or SUCCESS
 */
Result dynamic_timestamp_array_handler(uint16_t num_items) {
    if (num_items == 0) {
        if (timestamps != NULL) {
            vPortFree(timestamps);
        }
        hk_timestamp_array_size = 0;
        return SUCCESS;
    }
    if (num_items != hk_timestamp_array_size) {
        uint32_t size = sizeof(*timestamps) * (num_items + 1); //+1 to allow non-zero index room
        uint32_t *tmp = (uint32_t *)pvPortMalloc(size);
        if (tmp == NULL) {
            return FAILURE;
        }
        if (timestamps != NULL &&
            hk_timestamp_array_size < num_items) { // check if growing because we delete everything if shrinking
            memcpy(tmp, timestamps, sizeof(*timestamps) * hk_timestamp_array_size);
            vPortFree(timestamps);
        }
        timestamps = tmp;

        uint16_t i;
        for (i = (hk_timestamp_array_size + 1); i <= num_items; i++) { // ensure new entries are clean
            timestamps[i] = 0;
        }
        hk_timestamp_array_size = num_items;
    }
    return SUCCESS;
}

// temp function for testing. not for final project build
int32_t temp = 0;
uint32_t tempTime = 1000;
Result mock_everyone(All_systems_housekeeping *all_hk_data) {
    // universal temps
    long tempLong = (long)temp;
    uint8_t tempu8 = (uint8_t)temp;
    int8_t temp8 = (int8_t)temp;
    double tempDouble = (temp * 1.0);
    uint16_t tempu16 = (uint16_t)temp;
    uint32_t tempu32 = (uint32_t)temp;
    float tempFloat = (temp * 1.0);

    // Packet meta
    all_hk_data->hk_timeorder.UNIXtimestamp = tempTime;

    // ADCS
    all_hk_data->adcs_hk.Estimated_Angular_Rate_X = tempFloat;
    all_hk_data->adcs_hk.Estimated_Angular_Rate_Y = tempFloat;
    all_hk_data->adcs_hk.Estimated_Angular_Rate_Z = tempFloat;
    all_hk_data->adcs_hk.Estimated_Angular_Angle_X = tempFloat;
    all_hk_data->adcs_hk.Estimated_Angular_Angle_Y = tempFloat;
    all_hk_data->adcs_hk.Estimated_Angular_Angle_Z = tempFloat;
    all_hk_data->adcs_hk.Sat_Position_ECI_X = tempFloat;
    all_hk_data->adcs_hk.Sat_Position_ECI_Y = tempFloat;
    all_hk_data->adcs_hk.Sat_Position_ECI_Z = tempFloat;
    all_hk_data->adcs_hk.Sat_Velocity_ECI_X = tempFloat;
    all_hk_data->adcs_hk.Sat_Velocity_ECI_Y = tempFloat;
    all_hk_data->adcs_hk.Sat_Velocity_ECI_Z = tempFloat;
    all_hk_data->adcs_hk.Sat_Position_LLH_X = tempFloat;
    all_hk_data->adcs_hk.Sat_Position_LLH_Y = tempFloat;
    all_hk_data->adcs_hk.Sat_Position_LLH_Z = tempFloat;
    all_hk_data->adcs_hk.ECEF_Position_X = tempFloat;
    all_hk_data->adcs_hk.ECEF_Position_Y = tempFloat;
    all_hk_data->adcs_hk.ECEF_Position_Z = tempFloat;
    all_hk_data->adcs_hk.Coarse_Sun_Vector_X = tempFloat;
    all_hk_data->adcs_hk.Coarse_Sun_Vector_Y = tempFloat;
    all_hk_data->adcs_hk.Coarse_Sun_Vector_Z = tempFloat;
    all_hk_data->adcs_hk.Fine_Sun_Vector_X = tempFloat;
    all_hk_data->adcs_hk.Fine_Sun_Vector_Y = tempFloat;
    all_hk_data->adcs_hk.Fine_Sun_Vector_Z = tempFloat;
    all_hk_data->adcs_hk.Nadir_Vector_X = tempFloat;
    all_hk_data->adcs_hk.Nadir_Vector_Y = tempFloat;
    all_hk_data->adcs_hk.Nadir_Vector_Z = tempFloat;
    all_hk_data->adcs_hk.Wheel_Speed_X = tempFloat;
    all_hk_data->adcs_hk.Wheel_Speed_Y = tempFloat;
    all_hk_data->adcs_hk.Wheel_Speed_Z = tempFloat;
    all_hk_data->adcs_hk.Mag_Field_Vector_X = tempFloat;
    all_hk_data->adcs_hk.Mag_Field_Vector_Y = tempFloat;
    all_hk_data->adcs_hk.Mag_Field_Vector_Z = tempFloat;
    all_hk_data->adcs_hk.Comm_Status = tempFloat;
    all_hk_data->adcs_hk.Wheel1_Current = tempFloat;
    all_hk_data->adcs_hk.Wheel2_Current = tempFloat;
    all_hk_data->adcs_hk.Wheel3_Current = tempFloat;
    all_hk_data->adcs_hk.CubeSense1_Current = tempFloat;
    all_hk_data->adcs_hk.CubeSense2_Current = tempFloat;
    all_hk_data->adcs_hk.CubeControl_Current3v3 = tempFloat;
    all_hk_data->adcs_hk.CubeControl_Current5v0 = tempFloat;
    all_hk_data->adcs_hk.CubeStar_Current = tempFloat;
    all_hk_data->adcs_hk.CubeStar_Temp = tempFloat;
    all_hk_data->adcs_hk.Magnetorquer_Current = tempFloat;
    all_hk_data->adcs_hk.MCU_Temp = tempFloat;
    all_hk_data->adcs_hk.Rate_Sensor_Temp_X = tempFloat;
    all_hk_data->adcs_hk.Rate_Sensor_Temp_Y = tempFloat;
    all_hk_data->adcs_hk.Rate_Sensor_Temp_Z = tempFloat;

    // Athena
    uint8_t i;
    for (i = 0; i < NUM_TEMP_SENSOR; i++) {
        all_hk_data->Athena_hk.temparray[i] = tempLong;
    }
    all_hk_data->Athena_hk.boot_cnt = tempu16;
    all_hk_data->Athena_hk.OBC_mode = tempu8;
    all_hk_data->Athena_hk.OBC_uptime = tempu16;
    all_hk_data->Athena_hk.solar_panel_supply_curr = tempu8;
    all_hk_data->Athena_hk.OBC_software_ver = tempu8;
    all_hk_data->Athena_hk.cmds_received = tempu16;
    all_hk_data->Athena_hk.pckts_uncovered_by_FEC = tempu16;

    // EPS
    all_hk_data->EPS_hk.cmd = tempu8;
    all_hk_data->EPS_hk.status = temp8;
    all_hk_data->EPS_hk.timestampInS = tempDouble;
    all_hk_data->EPS_hk.uptimeInS = tempu32;
    all_hk_data->EPS_hk.bootCnt = tempu32;
    all_hk_data->EPS_hk.wdt_gs_time_left = tempu32;
    all_hk_data->EPS_hk.wdt_gs_counter = tempu32;
    all_hk_data->EPS_hk.vBatt = tempu16;
    all_hk_data->EPS_hk.curSolar = tempu16;
    all_hk_data->EPS_hk.curBattIn = tempu16;
    all_hk_data->EPS_hk.curBattOut = tempu16;
    all_hk_data->EPS_hk.outputConverterState = tempu8;
    all_hk_data->EPS_hk.outputStatus = tempu32;
    all_hk_data->EPS_hk.outputFaultStatus = tempu32;
    all_hk_data->EPS_hk.protectedOutputAccessCnt = tempu16;
    all_hk_data->EPS_hk.battMode = tempu8;
    all_hk_data->EPS_hk.mpptMode = tempu8;
    all_hk_data->EPS_hk.batHeaterMode = tempu8;
    all_hk_data->EPS_hk.batHeaterState = tempu8;
    all_hk_data->EPS_hk.PingWdt_toggles = tempu16;
    all_hk_data->EPS_hk.PingWdt_turnOffs = tempu8;

    for (i = 0; i < 2; i++) {
        all_hk_data->EPS_hk.AOcurOutput[i] = tempu16;
    }
    for (i = 0; i < 4; i++) {
        all_hk_data->EPS_hk.mpptConverterVoltage[i] = tempu16;
    }
    for (i = 0; i < 8; i++) {
        all_hk_data->EPS_hk.curSolarPanels[i] = tempu16;
        all_hk_data->EPS_hk.OutputConverterVoltage[i] = tempu16;
    }
    for (i = 0; i < 18; i++) {
        all_hk_data->EPS_hk.curOutput[i] = tempu16;
        all_hk_data->EPS_hk.outputOnDelta[i] = tempu16;
        all_hk_data->EPS_hk.outputOffDelta[i] = tempu16;
        all_hk_data->EPS_hk.outputFaultCnt[i] = tempu8;
    }
    for (i = 0; i < 14; i++) {
        all_hk_data->EPS_hk.temp[i] = temp8;
    }

    // UHF
    all_hk_data->UHF_hk.freq = tempu32;
    all_hk_data->UHF_hk.pipe_t = tempu32;
    all_hk_data->UHF_hk.beacon_t = tempu32;
    all_hk_data->UHF_hk.audio_t = tempu32;
    all_hk_data->UHF_hk.uptime = tempu32;
    all_hk_data->UHF_hk.pckts_out = tempu32;
    all_hk_data->UHF_hk.pckts_in = tempu32;
    all_hk_data->UHF_hk.pckts_in_crc16 = tempu32;
    all_hk_data->UHF_hk.temperature = tempFloat;

    for (i = 0; i < SCW_LEN; i++) {
        all_hk_data->UHF_hk.scw[i] = tempu8;
    }

    // Sband
    all_hk_data->S_band_hk.Output_Power = tempFloat;
    all_hk_data->S_band_hk.PA_Temp = tempFloat;
    all_hk_data->S_band_hk.Top_Temp = tempFloat;
    all_hk_data->S_band_hk.Bottom_Temp = tempFloat;
    all_hk_data->S_band_hk.Bat_Current = tempFloat;
    all_hk_data->S_band_hk.Bat_Voltage = tempFloat;
    all_hk_data->S_band_hk.PA_Current = tempFloat;
    all_hk_data->S_band_hk.PA_Voltage = tempFloat;

    // Hyperion
    all_hk_data->hyperion_hk.Nadir_Temp1 = tempFloat;
    all_hk_data->hyperion_hk.Port_Temp1 = tempFloat;
    all_hk_data->hyperion_hk.Port_Temp2 = tempFloat;
    all_hk_data->hyperion_hk.Port_Temp3 = tempFloat;
    all_hk_data->hyperion_hk.Port_Temp_Adc = tempFloat;
    all_hk_data->hyperion_hk.Port_Dep_Temp1 = tempFloat;
    all_hk_data->hyperion_hk.Port_Dep_Temp2 = tempFloat;
    all_hk_data->hyperion_hk.Port_Dep_Temp3 = tempFloat;
    all_hk_data->hyperion_hk.Port_Dep_Temp_Adc = tempFloat;
    all_hk_data->hyperion_hk.Star_Temp1 = tempFloat;
    all_hk_data->hyperion_hk.Star_Temp2 = tempFloat;
    all_hk_data->hyperion_hk.Star_Temp3 = tempFloat;
    all_hk_data->hyperion_hk.Star_Temp_Adc = tempFloat;
    all_hk_data->hyperion_hk.Star_Dep_Temp1 = tempFloat;
    all_hk_data->hyperion_hk.Star_Dep_Temp2 = tempFloat;
    all_hk_data->hyperion_hk.Star_Dep_Temp3 = tempFloat;
    all_hk_data->hyperion_hk.Star_Dep_Temp_Adc = tempFloat;
    all_hk_data->hyperion_hk.Zenith_Temp1 = tempFloat;
    all_hk_data->hyperion_hk.Zenith_Temp2 = tempFloat;
    all_hk_data->hyperion_hk.Zenith_Temp3 = tempFloat;
    all_hk_data->hyperion_hk.Zenith_Temp_Adc = tempFloat;
    all_hk_data->hyperion_hk.Nadir_Pd1 = tempFloat;
    all_hk_data->hyperion_hk.Port_Pd1 = tempFloat;
    all_hk_data->hyperion_hk.Port_Pd2 = tempFloat;
    all_hk_data->hyperion_hk.Port_Pd3 = tempFloat;
    all_hk_data->hyperion_hk.Port_Dep_Pd1 = tempFloat;
    all_hk_data->hyperion_hk.Port_Dep_Pd2 = tempFloat;
    all_hk_data->hyperion_hk.Port_Dep_Pd3 = tempFloat;
    all_hk_data->hyperion_hk.Star_Pd1 = tempFloat;
    all_hk_data->hyperion_hk.Star_Pd2 = tempFloat;
    all_hk_data->hyperion_hk.Star_Pd3 = tempFloat;
    all_hk_data->hyperion_hk.Star_Dep_Pd1 = tempFloat;
    all_hk_data->hyperion_hk.Star_Dep_Pd2 = tempFloat;
    all_hk_data->hyperion_hk.Star_Dep_Pd3 = tempFloat;
    all_hk_data->hyperion_hk.Zenith_Pd1 = tempFloat;
    all_hk_data->hyperion_hk.Zenith_Pd2 = tempFloat;
    all_hk_data->hyperion_hk.Zenith_Pd3 = tempFloat;
    all_hk_data->hyperion_hk.Port_Voltage = tempFloat;
    all_hk_data->hyperion_hk.Port_Dep_Voltage = tempFloat;
    all_hk_data->hyperion_hk.Star_Voltage = tempFloat;
    all_hk_data->hyperion_hk.Star_Dep_Voltage = tempFloat;
    all_hk_data->hyperion_hk.Zenith_Voltage = tempFloat;
    all_hk_data->hyperion_hk.Port_Current = tempFloat;
    all_hk_data->hyperion_hk.Port_Dep_Current = tempFloat;
    all_hk_data->hyperion_hk.Star_Current = tempFloat;
    all_hk_data->hyperion_hk.Star_Dep_Current = tempFloat;
    all_hk_data->hyperion_hk.Zenith_Current = tempFloat;

    temp++;
    tempTime += 30;
    return SUCCESS;
}

/**
 * @brief
 *      Private. Collect housekeeping information from each device in system
 * @attention
 *      Error testing and review recommended to check for possible shallow
 *      copies and resultant data loss and concurrency errors
 * @param all_hk_data
 *      pointer to struct of all the housekeeping data collected from components
 * @return Result
 *      FAILURE or SUCCESS
 */
Result collect_hk_from_devices(All_systems_housekeeping *all_hk_data) {
/*populate struct by calling appropriate functions*/
#ifndef ADCS_IS_STUBBED
    ADCS_returnState ADCS_return_code = HAL_ADCS_getHK(&all_hk_data->adcs_hk); // ADCS get housekeeeing
#endif                                                                         /* ADCS_IS_STUBBED */

#ifndef ATHENA_IS_STUBBED
    int Athena_return_code = Athena_getHK(&all_hk_data->Athena_hk); // Athena get temperature
#endif                                                              /* ATHENA_IS_STUBBED */

#ifndef EPS_IS_STUBBED
    EPS_getHK(&all_hk_data->EPS_hk, &all_hk_data->EPS_startup_hk); // EPS get housekeeping
#endif                                                             /* EPS_IS_STUBBED */

#ifndef UHF_IS_STUBBED
    UHF_return UHF_return_code = UHF_getHK(&all_hk_data->UHF_hk); // UHF get housekeeping
#endif                                                            /* UHF_IS_STUBBED */

#ifndef SBAND_IS_STUBBED
    STX_return STX_return_code = HAL_S_getHK(&all_hk_data->S_band_hk); // S_band get housekeeping
#endif                                                                 /* SBAND_IS_STUBBED */

#ifdef HYPERION_PANEL_3U
    Hyperion_config1_getHK(&all_hk_data->hyperion_hk);
#endif /* HYPERION_PANEL_3U */

#ifdef HYPERION_PANEL_2U
    Hyperion_config3_getHK(&all_hk_data->hyperion_hk);
#endif /* HYPERION_PANEL_2U */

    /*consider if struct should hold error codes returned from these functions*/
    return SUCCESS;
}

/**
 * @brief
 *      Check if file with given name exists
 * @param filename
 *      const char * to name of file to check
 * @return Found_file
 *      FILE_EXISTS or FILE_NOT_EXIST
 */
static Found_file exists(const char *filename) {
    int32_t file;
    red_errno = 0;
    file = red_open(filename, RED_O_CREAT | RED_O_EXCL | RED_O_RDWR); // attempt to create file
    if (red_errno == RED_EEXIST) {                                    // does file already exist?
        return FILE_EXISTS;
    }
    red_close(file);      // didn't exist. was created. now close it
    red_unlink(filename); // delete file. file creation would be a side affect
    return FILE_NOT_EXIST;
}

Result store_config(uint8_t rewrite_all) {
    uint8_t exist = 0;
    if (exists(hk_config) == FILE_EXISTS) {
        exist = 1;
    }
    int32_t fout = red_open(hk_config, RED_O_CREAT | RED_O_RDWR); // open or create file to write binary
    if (fout == -1) {
        ex2_log("Failed to open or create file to write: '%s'\n", hk_config);
        return FAILURE;
    }
    red_write(fout, &MAX_FILES, sizeof(MAX_FILES));
    red_write(fout, &current_file, sizeof(current_file));
    red_write(fout, &tempTime, sizeof(tempTime)); // for debugging
    if (exist == 0 || rewrite_all == 1) {
        red_write(fout, timestamps, ((hk_timestamp_array_size + 1) * sizeof(uint32_t)));
    } else {
        red_lseek(fout, current_file * sizeof(uint32_t), RED_SEEK_CUR);
        red_write(fout, &timestamps[current_file], sizeof(uint32_t));
    }
    red_close(fout);
    return SUCCESS;
}

Result load_config() {
    if (exists(hk_config) == FILE_NOT_EXIST) {
        ex2_log("Config file: '%s' does not exist\n", hk_config);
        return FAILURE;
    }
    int32_t fin = red_open(hk_config, RED_O_RDONLY); // open file to read binary
    if (fin == -1) {
        ex2_log("Failed to open file to read: '%s'\n", hk_config);
        return FAILURE;
    }
    red_read(fin, &MAX_FILES, sizeof(MAX_FILES));
    red_read(fin, &current_file, sizeof(current_file));
    red_read(fin, &tempTime, sizeof(tempTime)); // for debugging
    if (dynamic_timestamp_array_handler(MAX_FILES) == SUCCESS) {
        red_read(fin, timestamps, ((hk_timestamp_array_size + 1) * sizeof(uint32_t)));
    } else {
        return FAILURE;
    }
    red_close(fin);

    ++current_file;
    if (current_file > MAX_FILES) {
        current_file = 1;
    }

    return SUCCESS;
}

/**
 * @brief
 *    get the size of the entire housekeeping struct
 * @param all_hk_data
 *    a pointer to an instance of the housekeeping struct to measure size of
 * @return needed_size
 *    uint16_t of the size of the structure
 */
uint16_t get_size_of_housekeeping(All_systems_housekeeping *all_hk_data) {
    // needed_size is currently 713 bytes as of 2021/08/24
    uint16_t needed_size = sizeof(all_hk_data->hk_timeorder) + // currently 7U
                           sizeof(all_hk_data->Athena_hk) +    // currently 24U
                           sizeof(all_hk_data->EPS_hk) +       // currently 236U
                           sizeof(all_hk_data->UHF_hk) +       // currently 48U
                           sizeof(all_hk_data->S_band_hk) +    // currently 32U
                           sizeof(all_hk_data->adcs_hk) +      // currently 178U
                           sizeof(all_hk_data->hyperion_hk);   // currently 188U
    return needed_size;
}

/**
 * @brief
 *      Write housekeeping data to the given file location
 * @details
 *      Writes one struct to file for each subsystem present
 *      Order of writes must match the appropriate read function
 * @param filenumber
 *     uint16_t number to seek to in file
 * @param all_hk_data
 *      Struct containing structs of other hk data
 * @return Result
 *      FAILURE or SUCCESS
 */
Result write_hk_to_file(uint16_t filenumber, All_systems_housekeeping *all_hk_data) {
    int32_t fout = red_open(fileName, RED_O_CREAT | RED_O_RDWR); // open or create file to write binary
    if (fout == -1) {
        printf("Unexpected error %d from red_open()\r\n", (int)red_errno);
        ex2_log("Failed to open or create file to write: '%s'\n", fileName);
        return FAILURE;
    }
    uint16_t needed_size = get_size_of_housekeeping(all_hk_data);

    red_lseek(fout, (filenumber - 1) * needed_size, RED_SEEK_SET);

    red_errno = 0;
    /*The order of writes and subsequent reads must match*/
    red_write(fout, &all_hk_data->adcs_hk, sizeof(all_hk_data->adcs_hk));
    red_write(fout, &all_hk_data->hk_timeorder, sizeof(all_hk_data->hk_timeorder));
    red_write(fout, &all_hk_data->Athena_hk, sizeof(all_hk_data->Athena_hk));
    red_write(fout, &all_hk_data->EPS_hk, sizeof(all_hk_data->EPS_hk));
    red_write(fout, &all_hk_data->UHF_hk, sizeof(all_hk_data->UHF_hk));
    red_write(fout, &all_hk_data->S_band_hk, sizeof(all_hk_data->S_band_hk));
    red_write(fout, &all_hk_data->hyperion_hk, sizeof(all_hk_data->hyperion_hk));

    if (red_errno != 0) {
        ex2_log("Failed to write to file: '%s'\n", fileName);
        red_close(fout);
        return FAILURE;
    }
    red_close(fout);
    return SUCCESS;
}

/**
 * @brief
 *      Read housekeeping data from given file
 * @details
 *      Reads one struct from file for each subsystem present
 *      Order of reads must match the appropriate write function
 * @param filenumber
 *      uint16_t position to seek to in file
 * @param all_hk_data
 *      Struct containing structs of other hk data
 * @return Result
 *      FAILURE or SUCCESS
 */
Result read_hk_from_file(uint16_t filenumber, All_systems_housekeeping *all_hk_data) {
    if (exists(fileName) == FILE_NOT_EXIST) {
        ex2_log("Attempted to read file that doesn't exist: '%s'\n", fileName);
        return FAILURE;
    }
    int32_t fin = red_open(fileName, RED_O_RDONLY); // open file to read binary
    if (fin == -1) {
        ex2_log("Failed to open file to read: '%c'\n", fileName);
        return FAILURE;
    }

    uint16_t needed_size = get_size_of_housekeeping(all_hk_data);

    red_lseek(fin, (filenumber - 1) * needed_size, RED_SEEK_SET);

    red_errno = 0;
    /*The order of writes and subsequent reads must match*/
    red_read(fin, &all_hk_data->adcs_hk, sizeof(all_hk_data->adcs_hk));
    red_read(fin, &all_hk_data->hk_timeorder, sizeof(all_hk_data->hk_timeorder));
    red_read(fin, &all_hk_data->Athena_hk, sizeof(all_hk_data->Athena_hk));
    red_read(fin, &all_hk_data->EPS_hk, sizeof(all_hk_data->EPS_hk));
    red_read(fin, &all_hk_data->UHF_hk, sizeof(all_hk_data->UHF_hk));
    red_read(fin, &all_hk_data->S_band_hk, sizeof(all_hk_data->S_band_hk));
    red_read(fin, &all_hk_data->hyperion_hk, sizeof(all_hk_data->hyperion_hk));

    if (red_errno != 0) {
        ex2_log("Failed to read: '%c'\n", fileName);
        red_close(fin);
        return FAILURE;
    }
    red_close(fin);
    return SUCCESS;
}

/*Helper function to find number of digits in number*/
int num_digits(int num) {
    uint16_t count = 0;

    while (num != 0) {
        num /= 10;
        ++count;
    }
    return count;
}

static inline void prv_get_lock(SemaphoreHandle_t *lock) {
    if (*lock == NULL) {
        *lock = xSemaphoreCreateMutex();
    }
    xSemaphoreTake(*lock, portMAX_DELAY);
}

static inline void prv_give_lock(SemaphoreHandle_t *lock) { xSemaphoreGive(*lock); }

/**
 * @brief
 *      Public. Performs all calls and operations to retrieve hk data and store it
 * @return
 *      enum for SUCCESS or FAILURE
 */
Result populate_and_store_hk_data(void) {
    All_systems_housekeeping temp_hk_data;

    if (collect_hk_from_devices(&temp_hk_data) == FAILURE) {
        ex2_log("Error collecting hk data from peripherals\n");
    }

    // RTC_get_unix_time(&temp_hk_data.hk_timeorder.UNIXtimestamp);

    prv_get_lock(&f_count_lock); // lock

    if (config_loaded == 0) {
        if (load_config() == FAILURE) {
            ex2_log("couldn't load config");
        }
    }
    config_loaded = 1;

    // TEMP mock hk
    // mock_everyone(&temp_hk_data); //not permanent

    temp_hk_data.hk_timeorder.dataPosition = current_file;

    if (write_hk_to_file(current_file, &temp_hk_data) != SUCCESS) {
        ex2_log("Housekeeping data lost\n");
        prv_give_lock(&f_count_lock); // unlock
        return FAILURE;
    }

    if (dynamic_timestamp_array_handler(MAX_FILES) == SUCCESS) {
        timestamps[current_file] = temp_hk_data.hk_timeorder.UNIXtimestamp;
    } else {
        ex2_log("Warning, failed to malloc for secondary data structure\n");
    }
    store_config(0);

    ex2_log("%zu written to disk", current_file);

    ++current_file;
    if (current_file > MAX_FILES) {
        current_file = 1;
    }

    prv_give_lock(&f_count_lock); // unlock
    return SUCCESS;
}

/**
 * @brief
 *      Performs all calls and operations to load hk data from disk
 * @param file_num
 *      The id of the file to be retrieved. will be combined into full
 *      file name and is checked to ensure request is valid
 * @param all_hk_data
 *      Struct containing structs of other hk data
 * @return
 *      enum for SUCCESS or FAILURE
 */
Result load_historic_hk_data(uint16_t file_num, All_systems_housekeeping *all_hk_data) {
    if (read_hk_from_file(file_num, all_hk_data) != SUCCESS) {
        ex2_log("Housekeeping data could not be retrieved\n");
        return FAILURE;
    }

    return SUCCESS;
}

/**
 * @brief
 *      Change the maximum number of files stored by housekeeping service
 * @attention
 *      If new_max is less than MAX_FILES, all historic housekeeping files will
 *      be destroyed immediately to prevent orphaned files and confusion of data
 *      order. The next file written to after this function will be file #1.
 *      If new_max is greater than MAX_FILES, the data flow will be unaffected.
 * @param new_max
 *      The new value to change the maximum value to
 * @return
 *      enum for SUCCESS or FAILURE
 */
Result set_max_files(uint16_t new_max) {
    // ensure number requested isn't garbage
    if (new_max < 1 || new_max > 20160)
        return FAILURE;

    prv_get_lock(&f_count_lock); // lock

    // adjust the array

    // ensure value set before cleanup
    uint16_t old_max = MAX_FILES;
    MAX_FILES = new_max;

    if (old_max < new_max) {
        dynamic_timestamp_array_handler(new_max);
        prv_give_lock(&f_count_lock); // unlock
        return SUCCESS;
    }

    current_file = 1;

    // Cleanup files code if number of files has been reduced
    if (exists(fileName)) {
        red_unlink(fileName);
    }
    if (dynamic_timestamp_array_handler(0) == FAILURE) {
        ex2_log("failed to realloc data structure\n");
    }

    store_config(1);
    prv_give_lock(&f_count_lock); // unlock
    return SUCCESS;
}

/**
 * @brief
 *      Is given a struct of all the housekeeping data and converts the
 *      endianness of each value to be sent over the network
 * @param hk
 *      A struct of all the housekeeping data
 * @return
 *      enum for SUCCESS or FAILURE
 */
Result convert_hk_endianness(All_systems_housekeeping *hk) {
    /*hk_time_and_order*/
    hk->hk_timeorder.UNIXtimestamp = csp_hton32(hk->hk_timeorder.UNIXtimestamp);
    hk->hk_timeorder.dataPosition = csp_hton16(hk->hk_timeorder.dataPosition);

    // TODO:
    // hk->ADCS_hk.

    /*athena_housekeeping*/
    Athena_hk_convert_endianness(&hk->Athena_hk);

    /*eps_instantaneous_telemetry_t*/
    prv_instantaneous_telemetry_letoh(&hk->EPS_hk);

    /*UHF_housekeeping*/
    UHF_convert_endianness(&hk->UHF_hk);

    /*Sband_Housekeeping*/
    HAL_S_hk_convert_endianness(&hk->S_band_hk);

    /* The endianness converters for ADCS and Hyperion were never created */

    return SUCCESS;
}

/**
 * @brief
 *      Paging function to retrieve sets of data so they can be transmitted
 * @param conn
 *      Pointer to the connection on which to send packets
 * @param limit
 *      Maximum number of housekeeping files to retrieve in this request
 * @param before_id
 *      The earliest file in time that the user received. (lowest id)
 *      Files older than before_id will be fetched.
 *      Functions like a typical web API for paging. Prevents page drift.
 *      0 value means ignore variable. retrieve from most recent
 * @return
 *      enum for success or failure
 */
Result fetch_historic_hk_and_transmit(csp_conn_t *conn, uint16_t limit, uint16_t before_id, uint32_t before_time) {
    prv_get_lock(&f_count_lock); // lock
    uint16_t locked_max = MAX_FILES;
    uint16_t locked_before_id = before_id;
    uint32_t locked_before_time = before_time;
    if (locked_before_time != 0) { // use timestamp if exists
        locked_before_id = get_file_id_from_timestamp(locked_before_time);
    }
    prv_give_lock(&f_count_lock);

    // error check and accomodate user input
    if (locked_before_id == 0 || locked_before_id > locked_max) {
        locked_before_id = current_file;
    }
    // Check for data limit ignorance
    if (limit > locked_max) {
        limit = (uint16_t)locked_max;
    } else if (limit == 0) {
        ex2_log("Successfully did nothing O_o");
        return SUCCESS;
    }
    All_systems_housekeeping all_hk_data = {0};
    // fetch each appropriate set of data from file
    while (limit > 0) {
        locked_before_id--;

        if (locked_before_id == 0) {
            locked_before_id = locked_max;
        }

        if (load_historic_hk_data(locked_before_id, &all_hk_data) != SUCCESS) {
            return FAILURE;
        }
        if (convert_hk_endianness(&all_hk_data) != SUCCESS) {
            return FAILURE;
        }
        int8_t status = 0;

        if (limit > 1) {
            all_hk_data.hk_timeorder.final = 1;
        }

        uint16_t needed_size = get_size_of_housekeeping(&all_hk_data) + 2; // +2 for subservice and error

        csp_packet_t *packet = csp_buffer_get((size_t)needed_size);
        uint8_t ser_subtype = GET_HK;

        memcpy(&packet->data[SUBSERVICE_BYTE], &ser_subtype, sizeof(int8_t));
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));

        uint16_t used_size = 0;
        memcpy(&packet->data[OUT_DATA_BYTE + used_size], &all_hk_data.hk_timeorder,
               sizeof(all_hk_data.hk_timeorder));
        used_size += sizeof(all_hk_data.hk_timeorder);
        memcpy(&packet->data[OUT_DATA_BYTE + used_size], &all_hk_data.adcs_hk, sizeof(all_hk_data.adcs_hk));
        used_size += sizeof(all_hk_data.adcs_hk);
        memcpy(&packet->data[OUT_DATA_BYTE + used_size], &all_hk_data.Athena_hk, sizeof(all_hk_data.Athena_hk));
        used_size += sizeof(all_hk_data.Athena_hk);
        memcpy(&packet->data[OUT_DATA_BYTE + used_size], &all_hk_data.EPS_hk, sizeof(all_hk_data.EPS_hk));
        used_size += sizeof(all_hk_data.EPS_hk);
        memcpy(&packet->data[OUT_DATA_BYTE + used_size], &all_hk_data.UHF_hk, sizeof(all_hk_data.UHF_hk));
        used_size += sizeof(all_hk_data.UHF_hk);
        memcpy(&packet->data[OUT_DATA_BYTE + used_size], &all_hk_data.S_band_hk, sizeof(all_hk_data.S_band_hk));
        used_size += sizeof(all_hk_data.S_band_hk);
        memcpy(&packet->data[OUT_DATA_BYTE + used_size], &all_hk_data.hyperion_hk,
               sizeof(all_hk_data.hyperion_hk));
        used_size += sizeof(all_hk_data.hyperion_hk);

        set_packet_length(packet, used_size + 2);

        if (!csp_send(conn, packet, 50)) { // why are we all using magic number?
            ex2_log("Failed to send packet");
            csp_buffer_free(packet);
            return FAILURE;
        }
        limit--;
    }
    return SUCCESS;
}

/**
 * @brief
 *      Processes the incoming requests to decide what response is needed
 * @param conn
 *      Pointer to the connection on which to receive and send packets
 * @param packet
 *      The packet that was sent from the ground station
 * @return
 *      enum for return state
 */
SAT_returnState hk_service_app(csp_conn_t *conn, csp_packet_t *packet) {
    uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];
    int8_t status;
    uint16_t new_max_files;
    uint16_t *data16;
    uint16_t limit;
    uint16_t before_id;
    uint32_t before_time;

    switch (ser_subtype) {
    case SET_MAX_FILES:
        cnv8_16(&packet->data[IN_DATA_BYTE], &new_max_files);
        new_max_files = csp_ntoh16(new_max_files);

        if (set_max_files(new_max_files) != SUCCESS) {
            status = -1;
            memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        } else {
            status = 0;
            memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        }

        set_packet_length(packet, sizeof(int8_t) + 1); // +1 for subservice

        if (!csp_send(conn, packet, 50)) {
            csp_buffer_free(packet);
        }
        break;

    case GET_MAX_FILES:
        new_max_files = MAX_FILES;
        new_max_files = csp_hton16(new_max_files);
        status = 0;
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &new_max_files, sizeof(new_max_files));

        set_packet_length(packet, sizeof(int8_t) + sizeof(new_max_files) + 1); // +1 for subservice

        if (!csp_send(conn, packet, 50)) {
            csp_buffer_free(packet);
        }
        break;

    case GET_HK:
        data16 = (uint16_t *)(packet->data + 1);
        limit = data16[0];
        before_id = data16[1];
        before_time = ((uint32_t *)data16)[1];

        if (!csp_send(conn, packet, 50)) {
            csp_buffer_free(packet);
            if (fetch_historic_hk_and_transmit(conn, limit, before_id, before_time) != SUCCESS) {
                return SATR_ERROR;
            }
        }
        break;

    default:
        ex2_log("No such subservice\n");
        return SATR_PKT_ILLEGAL_SUBSERVICE;
    }

    return SATR_OK;
}

SAT_returnState start_housekeeping_service(void);

/**
 * @brief
 *      FreeRTOS housekeeping server task
 * @details
 *      Accepts incoming housekeeping service packets and executes the application
 * @param void* param
 * @return None
 */
void housekeeping_service(void *param) {
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_RDPREQ);
    csp_bind(sock, TC_HOUSEKEEPING_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN);
    svc_wdt_counter++;

    for (;;) {
        csp_conn_t *conn;
        csp_packet_t *packet;
        if ((conn = csp_accept(sock, CSP_MAX_TIMEOUT)) == NULL) {
            /* timeout */
            continue;
        }
        svc_wdt_counter++;

        while ((packet = csp_read(conn, 50)) != NULL) {
            if (hk_service_app(conn, packet) != SATR_OK) {
                ex2_log("Error responding to packet");
            }
        }
        csp_close(conn); // frees buffers used
    }
}

/**
 * @brief
 *      Start the housekeeping server task
 * @details
 *      Starts the FreeRTOS task responsible for accepting incoming
 *      housekeeping service requests
 * @param None
 * @return SAT_returnState
 *      success report
 */
SAT_returnState start_housekeeping_service(void) {
    TaskHandle_t svc_tsk;
    taskFunctions svc_funcs = {0};
    svc_funcs.getCounterFunction = get_svc_wdt_counter;
    if (xTaskCreate((TaskFunction_t)housekeeping_service, "start_housekeeping_service", 400, NULL,
                    NORMAL_SERVICE_PRIO, &svc_tsk) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK start_housekeeping_service\n");
        return SATR_ERROR;
    }
    ex2_register(svc_tsk, svc_funcs);
    ex2_log("Service handlers started\n");
    return SATR_OK;
}
