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
 * @file ns_service.c
 * @author Daniel Sacro, Thomas Ganley
 * @date 2022-06-24
 */
#include "northern_spirit/ns_service.h"

/**
 * @brief
 *      FreeRTOS 2U Payload File Transferring (FT) server task
 * @details
 *      Accepts incoming 2U Payload FT service packets and executes
 *      the application
 * @param void* param
 * @return None
 */
void ns_payload_service(void *param) {
    // socket initialization
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_HMACREQ);
    csp_bind(sock, TC_NORTHERN_SPIRIT_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN);

    for (;;) {
        // establish a connection
        csp_packet_t *packet;
        csp_conn_t *conn;
        if ((conn = csp_accept(sock, DELAY_WAIT_TIMEOUT)) == NULL) {

            /* timeout */
            continue;
        }

        // read and process packets
        while ((packet = csp_read(conn, 50)) != NULL) {
            if (ns_payload_service_app(packet) != SATR_OK) {
                // something went wrong in the subservice
                csp_buffer_free(packet);
            } else {
                // subservice was successful
                if (!csp_send(conn, packet, 50)) {
                    csp_buffer_free(packet);
                }
            }
        }
        csp_close(conn);
    }
}

/**
 * @brief
 *      Starts the northern spirit payload server task
 * @details
 *      Starts the FreeRTOS task responsible for accepting incoming
 *      northern spirit service requests
 * @param None
 * @return SAT_returnState
 *      Success report
 */
SAT_returnState start_ns_payload_service(void) {

    if (xTaskCreate((TaskFunction_t)ns_payload_service, "ns_payload_service", NS_SVC_SIZE, NULL,
                    NORMAL_SERVICE_PRIO, NULL) != pdPASS) {
        sys_log(ERROR, "FAILED TO CREATE TASK ns_payload_service\n");
        return SATR_ERROR;
    }

    sys_log(INFO, "ns payload service started\n");
    return SATR_OK;
}

/**
 * @brief
 *      Takes a CSP packet and switches based on the subservice command
 * @details
 *      Transfers files between the OBC and GS as well as the OBC and 2U
 *      Payload
 * @param *packet
 *      The CSP packet
 * @return SAT_returnState
 *      Success or failure
 */
SAT_returnState ns_payload_service_app(csp_packet_t *packet) {
    ns_payload_service_subtype ser_subtype = (ns_payload_service_subtype)packet->data[SUBSERVICE_BYTE];
    int8_t status;
    SAT_returnState return_state = SATR_OK; // OK until an error is encountered

    switch (ser_subtype) {

    case NS_UPLOAD_ARTWORK: {
        char filename[11]; // File name is supposed to be 7 bytes long
        memcpy(filename, &packet->data[IN_DATA_BYTE], 11);
        status = HAL_NS_upload_artwork(filename);
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case NS_CAPTURE_IMAGE: {
        status = HAL_NS_capture_image();
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }
    case NS_CONFIRM_DOWNLINK: {
        uint8_t conf;
        status = HAL_NS_confirm_downlink(&conf);
        memcpy(&packet->data[OUT_DATA_BYTE], &conf, sizeof(conf));
        set_packet_length(packet, sizeof(int8_t) + sizeof(conf) + 1);
        break;
    }

    case NS_GET_HEARTBEAT: {
        uint8_t heartbeat;
        status = HAL_NS_get_heartbeat(&heartbeat);
        memcpy(&packet->data[OUT_DATA_BYTE], &heartbeat, sizeof(heartbeat));
        set_packet_length(packet, sizeof(int8_t) + sizeof(heartbeat) + 1);
        break;
    }

    case NS_GET_FLAG: {
        char flag;
        bool flag_stat;
        memcpy(&flag, &packet->data[IN_DATA_BYTE], sizeof(flag));
        status = HAL_NS_get_flag(flag, &flag_stat);
        memcpy(&packet->data[OUT_DATA_BYTE], &flag_stat, sizeof(flag_stat));
        set_packet_length(packet, sizeof(int8_t) + sizeof(flag_stat) + 1);
        break;
    }

    case NS_GET_FILENAME: {
        char subcode;
        char filename[11];
        memcpy(&subcode, &packet->data[IN_DATA_BYTE], sizeof(subcode));
        status = HAL_NS_get_filename(subcode, filename);
        memcpy(&packet->data[OUT_DATA_BYTE], filename, 11);
        set_packet_length(packet, sizeof(int8_t) + 11 + 1);
        break;
    }

    case NS_GET_TELEMETRY: {
        ns_telemetry tlm;
        status = HAL_NS_get_telemetry(&tlm);
        memcpy(&packet->data[OUT_DATA_BYTE], &tlm, sizeof(tlm));
        set_packet_length(packet, sizeof(int8_t) + sizeof(tlm) + 1);
        break;
    }

    case NS_GET_SW_VERSION: {
        uint8_t sw_version[7];
        status = HAL_NS_get_software_version(sw_version);
        memcpy(&packet->data[OUT_DATA_BYTE], sw_version, 7);
        set_packet_length(packet, sizeof(int8_t) + 7 + 1);
        break;
    }
    case NS_DOWNLOAD_IMAGE: {
        status = HAL_NS_download_image();
        set_packet_length(packet, sizeof(int8_t) * 2);
        break;
    }
    case NS_CLEAR_SD_CARD: {
        status = HAL_NS_clear_sd_card();
        set_packet_length(packet, sizeof(int8_t) * 2);
        break;
    }

    default:
        ex2_log("No such subservice!\n");
        return_state = SATR_PKT_ILLEGAL_SUBSERVICE;
    }
    status = status * -1;
    memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
    return return_state;
}
