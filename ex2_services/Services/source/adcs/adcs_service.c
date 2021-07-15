/*
 * adcs_service.c
 *
 *  Created on: Jul 08, 2021
 *      Author: tttru
 */

#include "adcs/adcs_service.h"

SAT_returnState adcs_service_app(csp_packet_t *packet) {
    uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];
    int8_t status;
    SAT_returnState return_state = SATR_OK; // temporarily OK

    switch (ser_subtype)
    {
    case ADCS_RESET:
        /* code */
        break;
    case ADCS_RESET_LOG_POINTER:
        break;
    case ADCS_ADVANCE_LOG_POINTER:
        break;
    case ADCS_RESET_BOOT_REGISTERS:
        break;
    case ADCS_FORMAT_SD_CARD:
        break;
    case ADCS_ERASE_FILE:
        break;
    case ADCS_LOAD_FILE_DOWNLOAD_BLOCK:
        break;
    case ADCS_ADVANCE_FILE_LIST_READ_POINTER:
        break;
    case ADCS_INITIATE_FILE_UPLOAD:
        break;
    case ADCS_FILE_UPLOAD_PACKET:
        break;
    case ADCS_FINALIZE_UPLOAD_BLOCK:
        break;
    case ADCS_RESET_UPLOAD_BLOCK:
        break;
    case ADCS_RESET_FILE_LIST_READ_POINTER:
        break;
    case ADCS_INITIATE_DOWNLOAD_BURST:
        break;
    case ADCS_GET_NODE_IDENTIFICATION:
        break;
    case ADCS_GET_BOOT_PROGRAM_STAT:
        break;
    case ADCS_GET_BOOT_INDEX:
        break;
    case ADCS_GET_LAST_LOGGED_EVENT:
        break;
    case ADCS_GET_SD_FORMAT_PROCESS:
        break;
    case ADCS_GET_TC_ACK:
        break;
    case ADCS_GET_FILE_DOWNLOAD_BUFFER:
        break;
    case ADCS_GET_FILE_DOWNLOAD_BLOCK_STAT:
        break;
    case ADCS_GET_FILE_INFO:
        break;
    case ADCS_GET_INIT_UPLOAD_STAT:
        break;
    case ADCS_GET_FINALIZE_UPLOAD_STAT:
        break;
    case ADCS_GET_UPLOAD_CRC16_CHECKSUM:
        break;
    case ADCS_GET_SRAM_LATCHUP_COUNT:
        break;
    case ADCS_GET_EDAC_ERR_COUNT:
        break;
    case ADCS_GET_COMMS_STAT:
        break;
    case ADCS_SET_CACHE_EN_STATE:
        break;
    case ADCS_SET_SRAM_SCRUB_SIZE:
        break;
    case ADCS_SET_UNIXTIME_SAVE_CONFIG:
        break;
    case ADCS_SET_HOLE_MAP:
        break;
    case ADCS_SET_UNIX_T:
        break;
    case ADCS_GET_CACHE_EN_STATE:
        break;
    case ADCS_GET_SRAM_SCRUB_SIZE:
        break;
    case ADCS_GET_UNIXTIME_SAVE_CONFIG:
        break;
    case ADCS_GET_HOLE_MAP:
        break;
    case ADCS_GET_UNIX_T:
        break;
    case ADCS_CLEAR_ERR_FLAGS:
        break;
    case ADCS_SET_BOOT_INDEX:
        break;
    case ADCS_RUN_SELECTED_PROGRAM:
        break;
    case ADCS_READ_PROGRAM_INFO:
        break;
    case ADCS_COPY_PROGRAM_INTERNAL_FLASH:
        break;
    case ADCS_GET_BOOTLOADER_STATE:
        break;
    case ADCS_GET_PROGRAM_INFO:
        break;
    case ADCS_COPY_INTERNAL_FLASH_PROGRESS:
        break;
    case ADCS_DEPLOY_MAGNETOMETER_BOOM:
        break;
    case ADCS_SET_ENABLED_STATE:
        break;
    case ADCS_CLEAR_LATCHED_ERRS:
        break;
    case ADCS_SET_ATTITUDE_CTR_MODE:
        break;
    case ADCS_SET_ATTITUDE_ESTIMATE_MODE:
        break;
    case ADCS_TRIGGER_ADCS_LOOP:
        break;
    case ADCS_TRIGGER_ADCS_LOOP_SIM:
        break;
    case ADCS_SET_ASGP4_RUNE_MODE:
        break;
    case ADCS_TRIGGER_ASGP4:
        break;
    case ADCS_SET_MTM_OP_MODE:
        break;
    case ADCS_CNV2JPG:
        break;
    case ADCS_SAVE_IMG:
        break;
    case ADCS_SET_MAGNETORQUER_OUTPUT:
        break;
    case ADCS_SET_WHEEL_SPEED:
        break;
    case ADCS_SAVE_CONFIG:
        break;
    case ADCS_SAVE_ORBIT_PARAMS:
        break;
    case ADCS_GET_CURRENT_STATE:
        break;
    case ADCS_GET_JPG_CNV_PROGESS:
        break;
    case ADCS_GET_CUBEACP_STATE:
        break;
    case ADCS_GET_SAT_POS_LLH:
        break;
    case ADCS_GET_EXECUTION_TIMES:
        break;
    case ADCS_GET_ACP_LOOP_STAT:
        break;
    case ADCS_GET_IMG_SAVE_PROGRESS:
        break;
    case ADCS_GET_MEASUREMENTS:
        break;
    case ADCS_GET_ACTUATOR:
        break;
    case ADCS_GET_ESTIMATION:
        break;
    case ADCS_GET_ASGP4:
        break;
    case ADCS_GET_RAW_SENSOR:
        break;
    case ADCS_GET_RAW_GPS:
        break;
    case ADCS_GET_STAR_TRACKER:
        break;
    case ADCS_GET_MTM2_MEASUREMENTS:
        break;
    case ADCS_GET_POWER_TEMP:
        break;
    case ADCS_SET_POWER_CONTROL:
        break;
    case ADCS_GET_POWER_CONTROL:
        break;
    case ADCS_SET_ATTITUDE_ANGLE:
        break;
    case ADCS_GET_ATTITUDE_ANGLE:
        break;
    case ADCS_SET_TRACK_CONTROLLER:
        break;
    case ADCS_GET_TRACK_CONTROLLER:
        break;
    case ADCS_SET_LOG_CONFIG:
        break;
    case ADCS_GET_LOG_CONFIG:
        break;
    case ADCS_SET_INERTIAL_REF:
        break;
    case ADCS_GET_INERTIAL_REF:
        break;
    case ADCS_SET_SGP4_ORBIT_PARAMS:
        break;
    case ADCS_GET_SGP4_ORBIT_PARAMS:
        break;
    case ADCS_SET_SYSTEM_CONFIG:
        break;
    case ADCS_GET_SYSTEM_CONFIG:
        break;
    case ADCS_SET_MTQ_CONFIG:
        break;
    case ADCS_SET_RW_CONFIG:
        break;
    case ADCS_SET_RATE_GYRO:
        break;
    case ADCS_SET_CSS_CONFIG:
        break;
    case ADCS_SET_STAR_TRACK_CONFIG:
        break;
    case ADCS_SET_CUBESENSE_CONFIG:
        break;
    case ADCS_SET_MTM_CONFIG:
        break;
    case ADCS_SET_DETUMBLE_CONFIG:
        break;
    case ADCS_SET_YWHEEL_CONFIG:
        break;
    case ADCS_SET_TRACKING_CONFIG:
        break;
    case ADCS_SET_MOI_MAT:
        break;
    case ADCS_SET_ESTIMATION_CONFIG:
        break;
    case ADCS_SET_USERCODED_SETTING:
        break;
    case ADCS_SET_ASGP4_SETTING:
        break;
    case ADCS_GET_FULL_CONFIG:
        break;

    default:
        break;
    }
}

/**
 * @brief
 *      FreeRTOS adcs server task
 * @details
 *      Accepts incoming adcs service packets and executes the
 * application
 * @param void* param
 * @return None
 */
void adcs_service(void *param) {
    // create socket
    csp_socket_t *sock = csp_socket(CSP_SO_RDPREQ);

    // bind the adcs service to socket
    csp_bind(sock, TC_ADCS_SERVICE);

    // create connections backlog queue
    csp_listen(sock, SERVICE_BACKLOG_LEN);

    // pointer to current connection and packet
    csp_conn_t *conn;
    csp_packet_t *packet;
    // process incoming connection
    while (1) {

        // wait for connection, timeout
        if ((conn = csp_accept(sock, CSP_MAX_TIMEOUT)) == NULL) {
            continue;
        }

        // read packets. timeout is 50ms
        while ((packet = csp_read(conn, 50)) != NULL) {
        if (adcs_service_app(packet) != SATR_OK) {
            // something went wrong in the service
            ex2_log("Error");
            csp_buffer_free(packet);
        } else {
            if (!csp_send(conn, packet, 50)) {
            csp_buffer_free(packet);
            }
        }
        }

        // close current connection
        csp_close(conn);
    }
}

/**
 * @brief
 *      Start the adcs server task
 * @details
 *      Starts the FreeRTOS task responsible for accepting incoming
 *      adcs service requests
 * @param None
 * @return SAT_returnState
 *      success report
 */
SAT_returnState start_adcs_service(void) {
    // create adcs service
    if (xTaskCreate((TaskFunction_t)adcs_service,
                  "adcs_service", 1024, NULL, NORMAL_SERVICE_PRIO,
                  NULL) != pdPASS) {
    ex2_log("FAILED TO CREATE TASK start_adcs_service\n");
    return SATR_ERROR;
    }
    ex2_log("Service handlers started\n");
    return SATR_OK;
}



