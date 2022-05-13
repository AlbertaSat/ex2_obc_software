/*
 * Copyright (C) 2020  University of Alberta
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

#ifndef ADCS_TYPES_H
#define ADCS_TYPES_H

#define ADCS_Telemetery_Request 1;
#define ADCS_Telecommand 0;
#define ADCS_TC_TR_Mask 0x80;

#define ADCS_MAGIC_NUMBER 0x5A;
#define ADCS_ESC_CHAR 0x1F;
#define ADCS_SOM 0x7F;
#define ADCS_EOM 0xFF;

#define ADCS_UART_FILE_DOWNLOAD_PKT_LEN 27
#define ADCS_UART_FILE_DOWNLOAD_PKT_DATA_LEN 20
#define ADCS_UART_FILE_DOWNLOAD_PKT_RETRIES 10
#define ADCS_HOLE_MAP_SIZE 128

typedef enum ADCS_returnState {
    ADCS_OK = 0,
    ADCS_INVALID_ID = 1,
    ADCS_INCORRECT_LENGTH = 2,
    ADCS_INVALID_PARAMETERS = 3,
    ADCS_CRC_ERROR = 4,
    ADCS_MALLOC_FAILED = 5,
    ADCS_UART_FAILED = 6,
    ADCS_UART_BUSY = 7,

    IS_STUBBED_A = 0 // Used for stubbed ADCS in hardware interface
} ADCS_returnState;

typedef enum Common_Telecommand_IDs {
    RESET_ID = 1,             // perform a reset
    RESET_LOG_POINTER_ID = 4, // reset pointer to log buffer
    ADVANCE_LOG_POINTER_ID = 5,
    RESET_BOOT_REGISTERS_ID = 6,
    FORMAT_SD_CARD_ID = 33,
    ERASE_FILE_ID = 108,
    LOAD_FILE_DOWNLOAD_BLOCK_ID = 112, // fill download buffer with file contents
    ADVANCE_FILE_LIST_READ_POINTER_ID = 113,
    INITIATE_FILE_UPLOAD_ID = 114,
    FILE_UPLOAD_PACKET_ID = 115,
    FINALIZE_UPLOAD_BLOCK_ID = 116,
    RESET_UPLOAD_BLOCK_ID = 117, // reset hole map for upload block
    RESET_FILE_LIST_READ_POINTER_ID = 118,
    INITIATE_DOWNLOAD_BURST_ID = 119
} Common_Telecommand_IDs;

typedef enum Common_Telemetry_IDs {
    NODE_IDENTIFICATION_ID = 128,
    BOOT_RUNNING_STAT = 129,
    BOOT_IDX_STAT = 130,
    LAST_LOGGED_EVENT_ID = 141,
    SD_FORMAT_PROGRESS = 234,
    LAST_TC_ACK_ID = 240,
    FILE_DL_BUFFER_ID = 241,
    DL_BLOCK_STAT_ID = 242,
    FILE_INFO_ID = 243,
    INIT_UPLOAD_STAT_ID = 244,
    FINIALIZE_UPLOAD_STAT_ID = 245,
    UPLOAD_CRC16_ID = 246,
    SRAM_LATCHUP_COUNT_ID = 142,
    EDAC_ERR_COUNT_ID = 143,
    COMMS_STAT_ID = 144
} Common_Telemetry_IDs;

typedef enum Common_ConfigMsgs_IDs {
    SET_CACHE_EN_STATE_ID = 3,
    SET_SRAM_SCRUB_PARAM_ID = 8,
    SET_UNIX_TIME_SAVE_ID = 9,
    SET_HOLE_MAP_ID = 119, // 119 = hole_map1 - 1 (See the function)
    SET_CURRENT_UNIX_TIME = 2,

    GET_CACHE_EN_STATE_ID = 131,
    GET_SRAM_SCRUB_PARAM_ID = 134,
    GET_UNIX_TIME_SAVE_ID = 145,
    GET_HOLE_MAP_ID = 246, // 246 = hole_map1 - 1 (See the function)
    GET_CURRENT_UNIX_TIME = 140
} Common_ConfigMsgs_IDs;

typedef enum BootLoader_Telecommand_IDs {
    CLEAR_ERR_FLAGS_ID = 7,
    SET_BOOT_INDEX_ID = 100,
    RUN_SELECTED_PROGRAM_ID = 101,
    READ_PROGRAM_INFO_ID = 102,
    COPY_PROGRAM_INTERNAL_FLASH_ID = 103
} BootLoader_Telecommand_IDs;

typedef enum BootLoader_Telemetry_IDs {
    GET_BOOTLOADER_STATE_ID = 132,
    GET_PROGRAM_INFO_ID = 232,
    COPY_INTERNAL_FLASH_PROGRESS_ID = 233
} BootLoader_Telemetry_IDs;

typedef enum ACP_Telecommand_IDs {
    DEPLOY_MAGNETOMETER_BOOM_ID = 7, //* The ID is the same is CLEAR_ERR_FLAGS! how could it be?
    ADCS_RUN_MODE_ID = 10,
    CLEAR_LATCHED_ERRS_ID = 12,
    SET_ATT_CONTROL_MODE_ID = 13, // attitude
    SET_ATT_ESTIMATE_MODE_ID = 14,
    TRIGGER_ADCS_LOOP_ID = 18,
    TRIGGER_ADCS_LOOP_SIM_ID = 19,
    ASGP4_RUN_MODE_ID = 31,
    ASGP4_TRIGGER_ID = 32,
    SET_MTM_OP_MODE_ID = 56,
    CNV2JPG_ID = 57,
    SAVE_IMG_ID = 80,
    SET_MAGNETORQUER_OUTPUT_ID = 16,
    SET_WHEEL_SPEED_ID = 17,
    SAVE_CONFIG_ID = 63,
    SAVE_ORBIT_PARAMS = 64
} ACP_Telecommand_IDs;

typedef enum ACP_Telemetry_IDs {
    // ADCS state
    ADCS_STATE = 190, // Includes 132, 146:150, 201, 218-219, 224
    SATELLITE_POSITION_LLH_ID = 150,
    // General
    JPG_CNV_PROGRESS_ID = 133,
    CUBEACP_STATE_FLAGS_ID = 135,
    ADCS_EXE_TIMES_ID = 196,
    ACP_EXE_STATE_ID = 220,
    IMG_CAPTURE_SAVE_OP_STAT = 233,
    // ADCS measurements
    ADCS_MEASUREMENTS_ID = 191, // Includes 151:156, 181:186
    // Actuator
    ACTUATOR_ID = 192, // Includes 157,158
    // Estimation
    ESTIMATION_ID = 193, // Includes 159:165
    ASGP4_TLEs_ID = 228,
    // Raw sensor
    RAW_SENSOR_MEASUREMENTS_ID = 194, // Includes 166:170, 216
    RAW_GPS_MEASUREMENTS_ID = 210,    // Includes 176:180
    RAW_STAR_TRACKER_ID = 211,        // Includes 187:189, 212:214, 229:231
    MTM2_MEASUREMENTS_ID = 215,
    // Power
    POWER_TEMP_ID = 195 // Includes 171:175, 198, 232
} ACP_Telemetry_IDs;

typedef enum ACP_ConfigMsgs_IDs {
    // General
    SET_POWER_CONTROL_ID = 11,
    SET_ATT_ANGLE_ID = 15,
    SET_TRACK_CTRLER_TARGET_REF_ID = 55,
    SET_SD_LOG1_CONFIG_ID = 104,
    SET_SD_LOG2_CONFIG_ID = 105,
    SET_UART_LOG_CONFIG_ID = 106,
    SET_INERTIAL_POINT_ID = 34,

    GET_POWER_CONTROL_ID = 197,
    GET_ATT_ANGLE_ID = 199,
    GET_TRACK_CTRLER_TARGET_REF_ID = 200,
    GET_SD_LOG1_CONFIG_ID = 235,
    GET_SD_LOG2_CONFIG_ID = 236,
    GET_UART_LOG_CONFIG_ID = 237,
    GET_INERTIAL_POINT_ID = 238,

    // Configuration
    SET_SGP4_ORBIT_PARAMS_ID = 45,
    SET_SYSTEM_CONFIG_ID = 30,
    SET_MTQ_CONFIG_ID = 21,
    SET_WHEEL_CONFIG_ID = 22,
    SET_RATE_GYRO_CONFIG_ID = 23,
    SET_CSS_CONFIG_ID = 24,
    SET_STAR_TRACK_CONFIG_ID = 37,
    SET_CUBESENSE_CONFIG_ID = 25,
    SET_MTM_CONFIG_ID = 26,
    SET_MTM2_CONFIG_ID = 36,
    SET_DETUMBLE_PARAM_ID = 38,
    SET_YWHEEL_CTRL_PARAM_ID = 39,
    SET_RWHEEL_CTRL_PARAM_ID = 40,
    SET_TRACK_CTRL_ID = 54,
    SET_MOMENT_INERTIA_MAT_ID = 41,
    SET_ESTIMATE_PARAM = 27,
    SET_USERCODED_PARAM_ID = 29,
    SET_ASGP4_PARAM_ID = 28,

    GET_CUBESENSE_CONFIG_ID = 203,
    GET_SGP4_ORBIT_PARAMS_ID = 207,
    GET_SYSTEM_CONFIG_ID = 225,
    GET_FULL_CONFIG_ID = 206 // Includes 136:139, 202:205, 208-209, 217, 221:223, 226-227
} ACP_ConfigMsgs_IDs;

typedef enum PowerCTRL_IDs {
    Set_CubeCTRLSgn_Power = 0,
    Set_CubeCTRLMtr_Power  = 1,
    Set_CubeSense1_Power = 2,
    Set_CubeSense2_Power = 3,
    Set_CubeStar_Power = 4,
    Set_CubeWheel1_Power = 5,
    Set_CubeWheel2_Power = 6,
    Set_CubeWheel3_Power = 7,
    Set_Motor_Power = 8,
    Set_GPS_Power = 9

} PowerCTRL_IDs;

typedef enum TelemetryLOG_IDs {
    COMM_Status_ID = 7,
    EDAC_Err_Counter = 6,
    Last_Logged_Event = 4,
    SRAM_Latchup_Counter = 3,
    MTQ_Command = 0,
    Wheel_Speed_Command = 15, //Commanded speed for all 3 reaction wheels
    MAG_Field_Vector = 14,
    Coarse_Sun_Vector = 13,
    Fine_Sun_Vector = 12,
    Nadir_Vector = 11,
    Rate_Sensor_Rates = 10,
    Wheel_Speed = 9, //Speed of all 3 reaction wheels
    Star1_Body_Vector = 8,
    Star1_Orbit_Vector = 23,
    Star2_Body_Vector = 22,
    Star2_Orbit_Vector = 21,
    Star3_Body_Vector = 20,
    Star3_Orbit_Vector = 19,
    CubeSense1_Current = 18,
    CubeCTRL_CUrrent = 17,
    Wheel_Current = 16,
    ADCS_Misc_Current = 31,
    ADCS_Temp = 30,
    Rate_Sensor_Temp = 29,
    ADCS_State = 28,
    EST_ATT_Angles = 27,
    EST_Quaternion = 26,
    EST_Angular_Rates = 25,
    ECI_Position = 24,
    ECI_Velocity = 39,
    LLH_Position = 38,
    ECEF_Position = 37,
    IGRF_MAG_Field_Vector = 36,
    Modelled_Sun_Vector = 35,
    EST_Gyro_Bias = 34,
    EST_Innovation_Vector = 33,
    Quaternion_Err_Vector = 32,
    Quaternion_Covariance = 47,
    Angular_Rate_Covariance = 46,
    Raw_GPS_Status = 45,
    Raw_GPS_Time = 44,
    Raw_GPS_X = 43,
    Raw_GPS_Y = 42,
    Raw_GPS_Z = 41,
    Raw_CAM2_Sensor = 40,
    Raw_CAM1_Sensor = 55,
    Raw_CSS_1to6 = 54,
    Raw_CSS_7to10 = 53,
    Raw_MAG = 52,
    Star_Peformance1 = 51,
    Star_Magnitude = 50,
    Star1_Raw_Data = 49,
    Star2_Raw_Data = 48,
    Star3_Raw_Data = 63,
    Star_Timing = 62,
    Secondary_MAG_Raw_Measurements = 61,
    Fine_EST_Angular_Rates = 60,
    Raw_Rate_Sensor = 59,
    Current_ADCS_State2 = 58,
    CubeStar_EST_Rates = 57,
    CubeStar_EST_Quaternion = 56,
    Star_Performance2 = 71,
    CubeSense2_Current = 70

} TelemetryLOG_IDs;

#endif /* ADCS_TYPES_H */
