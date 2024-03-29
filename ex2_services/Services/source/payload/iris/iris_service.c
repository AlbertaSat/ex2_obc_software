/*
 * Copyright (C) 2015  University of Alberta
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributeds in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <FreeRTOS.h>
#include <os_task.h>

#include "rtcmk.h"
#include "services.h"

#include "payload/iris/iris_service.h"
#include "util/service_utilities.h"
#include "logger.h"
#include "redconf.h"
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <main/system.h>
#include <stdio.h>

#include "iris.h"
#include "iris_bootloader_cmds.h"
#include "eps.h"

SAT_returnState iris_service_app(csp_packet_t *packet);

/**
 * @brief
 *      Initialize and bind socket, and listen for packets in an infinite loop
 * @details
 *      Accepts incoming iris service packets and executes the application
 * @param void* param
 * @return None
 */
void iris_service(void *param) {
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_HMACREQ); // require RDP connection
    csp_bind(sock, TC_IRIS_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN);

    for (;;) {

        csp_conn_t *conn;
        csp_packet_t *packet;
        if ((conn = csp_accept(sock, DELAY_WAIT_TIMEOUT)) == NULL) {

            /* timeout */
            continue;
        }

        while ((packet = csp_read(conn, 50)) != NULL) {
            if (iris_service_app(packet) != SATR_OK) {
                // something went wrong, this shouldn't happen
                csp_buffer_free(packet);
            } else {
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
 *      Start the iris server task
 * @details
 *      Starts the FreeRTOS task responsible for accepting incoming
 *      iris packets
 * @param None
 * @return SAT_returnState
 *      success report
 */
SAT_returnState start_iris_service(void) {

    if (xTaskCreate((TaskFunction_t)iris_service, "iris_service", IRIS_SVC_SIZE, NULL, NORMAL_SERVICE_PRIO,
                    NULL) != pdPASS) {
        sys_log(ERROR, "FAILED TO CREATE TASK iris_service");
        return SATR_ERROR;
    }
    return SATR_OK;
}

/**
 * @brief
 *      Handle incoming csp_packet_t
 * @details
 *      Takes a csp packet destined for the iris service,
 *              and will handle the packet based on it's subservice type.
 * @param csp_packet_t *packet
 *              Incoming CSP packet - we can be sure that this packet is
 *              valid and destined for this service.
 * @return SAT_returnState
 *      success report
 */
SAT_returnState iris_service_app(csp_packet_t *packet) {
    increment_commands_recv();
    uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];
    int8_t status = 0;

    switch (ser_subtype) {
    case IRIS_POWER_ON:
#if IS_ATHENA == 1 && IS_FLATSAT == 0
        status = eps_set_pwr_chnl(PYLD_3V3_PWR_CHNL, ON);
        status += eps_set_pwr_chnl(PYLD_5V0_PWR_CHNL, ON);
#endif
        status += iris_init();

        // Return success/failure report
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(uint8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    case IRIS_POWER_OFF:
#if IS_ATHENA == 1 && IS_FLATSAT == 0
        status = eps_set_pwr_chnl(PYLD_3V3_PWR_CHNL, OFF);
        status += eps_set_pwr_chnl(PYLD_5V0_PWR_CHNL, OFF);
#endif
        // Return success/failure report
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(uint8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    case IRIS_TURN_ON_IMAGE_SENSORS:
        status = iris_toggle_sensor(1);

        // Return success/failure report
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(uint8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    case IRIS_TURN_OFF_IMAGE_SENSORS:
        status = iris_toggle_sensor(0);

        // Return success/failure report
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(uint8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    case IRIS_TAKE_IMAGE: {
        status = iris_take_pic();

        // Return success/failure report
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(uint8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }
    case IRIS_DELIVER_IMAGE: {
        /*
         * HAL function execution path
         * 1. Get image length
         * 2. Transfer image
         */
        uint32_t image_length = 0;
        uint16_t image_count = 0;
        char filename[REDCONF_NAME_MAX];

        status = iris_get_image_count(&image_count);
        if (status == IRIS_HAL_OK) {
            for (int i = 0; i < image_count; i++) {
                status = iris_get_image_length(&image_length);

                if (status == IRIS_HAL_OK && image_length != NULL) {
                    sprintf(filename, "iris_image_%d.jpg", i);
                    status = iris_transfer_image(image_length, filename);
                }
            }
        }

        // Return success/failure report
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(uint8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }
    case IRIS_COUNT_IMAGES: {
        uint16_t image_count;

        status = iris_get_image_count(&image_count);

        // Return success/failure report
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(uint8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }
    case IRIS_PROGRAM_FLASH: {
        status = iris_program();

        // Return success/failure report
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(uint8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }
    case IRIS_GET_HK: {
        // Get Iris housekeeping data
        IRIS_Housekeeping HK = {0};
        status = iris_get_housekeeping(&HK);

        HK.vis_temp = csp_hton16(HK.vis_temp);
        HK.nir_temp = csp_hton16(HK.nir_temp);
        HK.flash_temp = csp_hton16(HK.flash_temp);
        HK.gate_temp = csp_hton16(HK.gate_temp);
        HK.imagenum = HK.imagenum;
        HK.software_version = HK.software_version;
        HK.errornum = HK.errornum;
        HK.MAX_5V_voltage = csp_hton16(HK.MAX_5V_voltage);
        HK.MAX_5V_power = csp_hton16(HK.MAX_5V_power);
        HK.MAX_3V_voltage = csp_hton16(HK.MAX_3V_voltage);
        HK.MAX_3V_power = csp_hton16(HK.MAX_3V_power);
        HK.MIN_5V_voltage = csp_hton16(HK.MIN_5V_voltage);
        HK.MIN_3V_voltage = csp_hton16(HK.MIN_3V_voltage);

        // Return success/failure report and Iris housekeeping data
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &HK, sizeof(HK));
        set_packet_length(packet, sizeof(int8_t) + sizeof(HK) + 1);
        break;
    }
    case IRIS_DELIVER_LOG: {
        status = iris_transfer_log();

        // Return success/failure report
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(uint8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }
    case IRIS_SET_TIME: {
        uint32_t unix_time;

        cnv8_32(&packet->data[IN_DATA_BYTE], &unix_time);
        unix_time = csp_ntoh32(unix_time);

        status = iris_update_rtc(unix_time);

        // Return success/failure report
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(uint8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }
    case IRIS_SET_CONFIG: {
        Iris_config config = {0};
        memcpy(&config, &packet->data[IN_DATA_BYTE], sizeof(config));
        status = iris_update_config(config);

        // Return success/failure report
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(uint8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }
    default:
        sys_log(WARN, "No such sub-service %d", ser_subtype);
        return SATR_PKT_ILLEGAL_SUBSERVICE;
    }
    return SATR_OK;
}
