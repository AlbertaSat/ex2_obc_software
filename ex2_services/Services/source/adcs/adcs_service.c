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
 * @file adcs_service.c
 * @author Quoc Trung Tran
 * @date Jul 08, 2021
 */

#include <string.h>
#include "adcs/adcs_service.h"
#include "logger/logger.h"

SAT_returnState adcs_service_app(csp_packet_t *packet) {
    increment_commands_recv();
    uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];
    int8_t status;
    SAT_returnState return_state = SATR_OK; // temporarily OK

    switch (ser_subtype) {
    case ADCS_RESET:
        status = HAL_ADCS_reset();
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;

    case ADCS_RESET_LOG_POINTER:
        status = HAL_ADCS_reset_log_pointer();
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;

    case ADCS_ADVANCE_LOG_POINTER:
        status = HAL_ADCS_advance_log_pointer();
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;

    case ADCS_RESET_BOOT_REGISTERS:
        status = HAL_ADCS_reset_boot_registers();
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;

    case ADCS_FORMAT_SD_CARD:
        status = HAL_ADCS_format_sd_card();
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;

    case ADCS_ERASE_FILE: {
        uint8_t file_type = packet->data[IN_DATA_BYTE];
        uint8_t file_counter = packet->data[IN_DATA_BYTE + 1];
        uint8_t erase_all = packet->data[IN_DATA_BYTE + 2];
        status = HAL_ADCS_erase_file(file_type, file_counter, erase_all);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case ADCS_LOAD_FILE_DOWNLOAD_BLOCK: {
        uint8_t file_type = packet->data[IN_DATA_BYTE];
        uint8_t counter = packet->data[IN_DATA_BYTE + 1];

        uint32_t offset;
        cnv8_32(&packet->data[IN_DATA_BYTE + 2], &offset);
        offset = csp_ntoh32(offset);

        uint16_t block_length;
        cnv8_16(&packet->data[IN_DATA_BYTE + 6], &block_length);
        block_length = csp_ntoh16(block_length);

        status = HAL_ADCS_load_file_download_block(file_type, counter, offset, block_length);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case ADCS_ADVANCE_FILE_LIST_READ_POINTER:
        status = HAL_ADCS_advance_file_list_read_pointer();
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;

    case ADCS_INITIATE_FILE_UPLOAD: {
        uint8_t file_dest = packet->data[IN_DATA_BYTE];
        uint8_t block_size = packet->data[IN_DATA_BYTE + 1];
        status = HAL_ADCS_initiate_file_upload(file_dest, block_size);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case ADCS_FILE_UPLOAD_PACKET: {
        uint16_t packet_number;
        cnv8_16(&packet->data[IN_DATA_BYTE], &packet_number);
        packet_number = csp_ntoh16(packet_number);

        char file_bytes;
        status = HAL_ADCS_file_upload_packet(packet_number, &file_bytes);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &file_bytes, sizeof(file_bytes));
        set_packet_length(packet, sizeof(file_bytes) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_FINALIZE_UPLOAD_BLOCK: {
        uint8_t file_dest = packet->data[IN_DATA_BYTE];

        uint32_t offset;
        cnv8_32(&packet->data[IN_DATA_BYTE + 1], &offset);
        offset = csp_ntoh32(offset);

        uint16_t block_length;
        cnv8_16(&packet->data[IN_DATA_BYTE + 5], &block_length);
        block_length = csp_ntoh16(block_length);

        status = HAL_ADCS_finalize_upload_block(file_dest, offset, block_length);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case ADCS_RESET_UPLOAD_BLOCK:
        status = HAL_ADCS_reset_upload_block();
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;

    case ADCS_RESET_FILE_LIST_READ_POINTER:
        status = HAL_ADCS_reset_file_list_read_pointer();
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;

    case ADCS_INITIATE_DOWNLOAD_BURST: {
        uint8_t msg_length = packet->data[IN_DATA_BYTE];
        bool ignore_hole_map = packet->data[IN_DATA_BYTE + 1];
        status = HAL_ADCS_initiate_download_burst(msg_length, ignore_hole_map);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_NODE_IDENTIFICATION: {
        ADCS_node_identification node_id;
        status = HAL_ADCS_get_node_identification(&node_id);

        if (sizeof(node_id) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        node_id.node_type = csp_hton32((uint32_t)node_id.node_type);
        node_id.interface_ver = csp_hton32((uint32_t)node_id.interface_ver);
        node_id.major_firm_ver = csp_hton32((uint32_t)node_id.major_firm_ver);
        node_id.minor_firm_ver = csp_hton32((uint32_t)node_id.minor_firm_ver);
        node_id.runtime_s = csp_hton32((uint32_t)node_id.runtime_s);
        node_id.runtime_ms = csp_hton32((uint32_t)node_id.runtime_ms);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &node_id, sizeof(node_id));
        set_packet_length(packet, sizeof(node_id) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_BOOT_PROGRAM_STAT: {

        ADCS_boot_program_stat boot_program_stat;
        status = HAL_ADCS_get_boot_program_stat(&boot_program_stat);

        if (sizeof(boot_program_stat) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        boot_program_stat.mcu_reset_cause = csp_hton32((uint32_t)boot_program_stat.mcu_reset_cause);
        boot_program_stat.boot_cause = csp_hton32((uint32_t)boot_program_stat.boot_cause);
        boot_program_stat.boot_count = csp_hton32((uint32_t)boot_program_stat.boot_count);
        boot_program_stat.boot_idx = csp_hton32((uint32_t)boot_program_stat.boot_idx);
        boot_program_stat.major_firm_version = csp_hton32((uint32_t)boot_program_stat.major_firm_version);
        boot_program_stat.minor_firm_version = csp_hton32((uint32_t)boot_program_stat.minor_firm_version);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &boot_program_stat, sizeof(boot_program_stat));
        set_packet_length(packet, sizeof(boot_program_stat) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_BOOT_INDEX: {
        ADCS_boot_index boot_index;
        status = HAL_ADCS_get_boot_index(&boot_index);

        if (sizeof(boot_index) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        boot_index.program_idx = csp_hton32((uint32_t)boot_index.program_idx);
        boot_index.boot_stat = csp_hton32((uint32_t)boot_index.boot_stat);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &boot_index, sizeof(boot_index));
        set_packet_length(packet, sizeof(boot_index) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_LAST_LOGGED_EVENT: {

        ADCS_last_logged_event last_logged_event;
        status = HAL_ADCS_get_last_logged_event(&last_logged_event);
        if (sizeof(last_logged_event) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        last_logged_event.time = csp_hton32((uint32_t)last_logged_event.time);
        last_logged_event.event_id = csp_hton32((uint32_t)last_logged_event.event_id);
        last_logged_event.event_param = csp_hton32((uint32_t)last_logged_event.event_param);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &last_logged_event, sizeof(last_logged_event));
        set_packet_length(packet, sizeof(last_logged_event) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_SD_FORMAT_PROCESS: {
        bool format_busy = false;
        bool erase_all_busy = false;
        status = HAL_ADCS_get_SD_format_progress(&format_busy, &erase_all_busy);

        if (sizeof(format_busy) + sizeof(erase_all_busy) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        format_busy = csp_hton32((uint32_t)format_busy);
        erase_all_busy = csp_hton32((uint32_t)erase_all_busy);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &format_busy, sizeof(format_busy));
        memcpy(&packet->data[OUT_DATA_BYTE + 1], &erase_all_busy, sizeof(erase_all_busy));

        set_packet_length(packet, sizeof(format_busy) + sizeof(erase_all_busy) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_TC_ACK: {

        ADCS_TC_ack TC_ack;
        status = HAL_ADCS_get_TC_ack(&TC_ack);

        if (sizeof(TC_ack) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        TC_ack.last_tc_id = csp_hton32((uint32_t)TC_ack.last_tc_id);
        TC_ack.tc_processed = csp_hton32((uint32_t)TC_ack.tc_processed);
        TC_ack.tc_err_idx = csp_hton32((uint32_t)TC_ack.tc_err_idx);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &TC_ack, sizeof(TC_ack));
        set_packet_length(packet, sizeof(TC_ack) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_FILE_DOWNLOAD_BUFFER: {
        ADCS_file_download_buffer file_download_buffer;
        status = HAL_ADCS_get_file_download_buffer(&file_download_buffer.packet_count, file_download_buffer.file);

        if (sizeof(file_download_buffer) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &file_download_buffer, sizeof(file_download_buffer));
        set_packet_length(packet, sizeof(file_download_buffer) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_FILE_DOWNLOAD_BLOCK_STAT: {

        ADCS_file_download_block_stat file_download_block_stat;

        status = HAL_ADCS_get_file_download_block_stat(&file_download_block_stat);

        if (sizeof(file_download_block_stat) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        file_download_block_stat.ready = csp_hton32((uint32_t)file_download_block_stat.ready);
        file_download_block_stat.param_err = csp_hton32((uint32_t)file_download_block_stat.param_err);
        file_download_block_stat.crc16_checksum = csp_hton32((uint32_t)file_download_block_stat.crc16_checksum);
        file_download_block_stat.length = csp_hton32((uint32_t)file_download_block_stat.length);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &file_download_block_stat, sizeof(file_download_block_stat));
        set_packet_length(packet, sizeof(file_download_block_stat) + sizeof(int8_t) + 1);

        break;
    }
    case ADCS_GET_FILE_INFO: {
        ADCS_file_info file_info;

        status = HAL_ADCS_get_file_info(&file_info);

        if (sizeof(file_info) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        file_info.type = csp_hton32((uint32_t)file_info.type);
        file_info.updating = csp_hton32((uint32_t)file_info.updating);
        file_info.counter = csp_hton32((uint32_t)file_info.counter);
        file_info.size = csp_hton32((uint32_t)file_info.size);
        file_info.time = csp_hton32((uint32_t)file_info.time);
        file_info.crc16_checksum = csp_hton32((uint32_t)file_info.crc16_checksum);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &file_info, sizeof(file_info));
        set_packet_length(packet, sizeof(file_info) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_INIT_UPLOAD_STAT: {
        bool busy = false;
        status = HAL_ADCS_get_init_upload_stat(&busy);

        if (sizeof(busy) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        busy = csp_hton32((uint32_t)busy);

        // only write the result if status is okay
        if (status == SATR_OK) {
            memcpy(&packet->data[OUT_DATA_BYTE], &busy, sizeof(busy));
        }
        set_packet_length(packet, sizeof(busy) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_FINALIZE_UPLOAD_STAT: {

        bool busy = false;
        bool err = false;
        status = HAL_ADCS_get_finalize_upload_stat(&busy, &err);
        if (sizeof(busy) + sizeof(err) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        busy = csp_hton32((uint32_t)busy);
        err = csp_hton32((uint32_t)err);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &busy, sizeof(busy));
        memcpy(&packet->data[OUT_DATA_BYTE + 1], &err, sizeof(err));
        set_packet_length(packet, sizeof(busy) + sizeof(err) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_UPLOAD_CRC16_CHECKSUM: {
        uint16_t checksum;
        status = HAL_ADCS_get_upload_crc16_checksum(&checksum);
        if (sizeof(checksum) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        checksum = csp_hton32((uint32_t)checksum);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &checksum, sizeof(checksum));
        set_packet_length(packet, sizeof(checksum) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_SRAM_LATCHUP_COUNT: {
        ADCS_SRAM_latchup_count SRAM_latchup_count;
        status = HAL_ADCS_get_SRAM_latchup_count(&SRAM_latchup_count);

        if (sizeof(SRAM_latchup_count) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        SRAM_latchup_count.sram1 = csp_hton32((uint32_t)SRAM_latchup_count.sram1);
        SRAM_latchup_count.sram2 = csp_hton32((uint32_t)SRAM_latchup_count.sram2);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &SRAM_latchup_count, sizeof(SRAM_latchup_count));
        set_packet_length(packet, sizeof(SRAM_latchup_count) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_EDAC_ERR_COUNT: {
        ADCS_EDAC_err_count EDAC_err_count;

        status = HAL_ADCS_get_EDAC_err_count(&EDAC_err_count);
        if (sizeof(EDAC_err_count) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        EDAC_err_count.single_sram = csp_hton32((uint32_t)EDAC_err_count.single_sram);
        EDAC_err_count.double_sram = csp_hton32((uint32_t)EDAC_err_count.double_sram);
        EDAC_err_count.multi_sram = csp_hton32((uint32_t)EDAC_err_count.multi_sram);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &EDAC_err_count, sizeof(EDAC_err_count));
        set_packet_length(packet, sizeof(EDAC_err_count) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_COMMS_STAT: {
        uint16_t comm_status[5];
        status = HAL_ADCS_get_comms_stat(comm_status);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));

        if (sizeof(comm_status) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        // only write the result if status is okay
        if (status == SATR_OK) {
            comm_status[0] = csp_hton16(comm_status[0]);
            comm_status[1] = csp_hton16(comm_status[1]);
            comm_status[2] = csp_hton16(comm_status[2]);
            comm_status[3] = csp_hton16(comm_status[3]);
            comm_status[4] = csp_hton16(comm_status[4]);
            memcpy(&packet->data[OUT_DATA_BYTE], comm_status, sizeof(comm_status));
        }
        set_packet_length(packet, sizeof(comm_status) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_SET_CACHE_EN_STATE:
        status = HAL_ADCS_set_cache_en_state((bool)packet->data[IN_DATA_BYTE]);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;

    case ADCS_SET_SRAM_SCRUB_SIZE: {
        uint16_t size = 0;
        cnv8_16(&packet->data[IN_DATA_BYTE], &size);
        status = HAL_ADCS_set_sram_scrub_size(size);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case ADCS_SET_UNIXTIME_SAVE_CONFIG: {
        uint8_t when = packet->data[IN_DATA_BYTE];
        uint8_t period = packet->data[IN_DATA_BYTE + 1];
        status = HAL_ADCS_set_UnixTime_save_config(when, period);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case ADCS_SET_HOLE_MAP: {
        uint8_t hole_map = packet->data[IN_DATA_BYTE];
        uint8_t num = packet->data[IN_DATA_BYTE + 1];

        status = HAL_ADCS_set_hole_map(&hole_map, num);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &hole_map, sizeof(hole_map));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case ADCS_SET_UNIX_T: {
        uint32_t unix_t;
        cnv8_32(&packet->data[IN_DATA_BYTE], &unix_t);
        unix_t = csp_ntoh32(unix_t);

        uint16_t count_ms;
        cnv8_16(&packet->data[IN_DATA_BYTE + 4], &count_ms);
        count_ms = csp_ntoh16(count_ms);

        status = HAL_ADCS_set_unix_t(unix_t, count_ms);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_CACHE_EN_STATE: {
        bool en_state = false;
        status = HAL_ADCS_get_cache_en_state(&en_state);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));

        if (sizeof(en_state) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        // only write the result if status is okay
        if (status == SATR_OK) {
            en_state = csp_hton32((uint32_t)en_state);
            memcpy(&packet->data[OUT_DATA_BYTE], &en_state, sizeof(en_state));
        }
        set_packet_length(packet, sizeof(en_state) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_SRAM_SCRUB_SIZE: {
        uint16_t size;
        status = HAL_ADCS_get_sram_scrub_size(&size);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));

        if (sizeof(size) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        if (status == SATR_OK) {
            size = csp_hton32((uint32_t)size);
            memcpy(&packet->data[OUT_DATA_BYTE], &size, sizeof(size));
        }
        set_packet_length(packet, sizeof(size) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_UNIXTIME_SAVE_CONFIG: {
        ADCS_Unixtime_save_config Unixtime_save_config;

        status = HAL_ADCS_get_UnixTime_save_config(&Unixtime_save_config);

        if (sizeof(Unixtime_save_config) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        Unixtime_save_config.when = csp_hton32((uint32_t)Unixtime_save_config.when);
        Unixtime_save_config.period = csp_hton32((uint32_t)Unixtime_save_config.period);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &Unixtime_save_config, sizeof(Unixtime_save_config));
        set_packet_length(packet, sizeof(Unixtime_save_config) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_HOLE_MAP: {
        uint8_t hole_map;
        uint8_t num = packet->data[IN_DATA_BYTE];
        status = HAL_ADCS_get_hole_map(&hole_map, num);

        if (sizeof(hole_map) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &hole_map, sizeof(uint8_t));
        set_packet_length(packet, sizeof(uint8_t) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_UNIX_T: {
        ADCS_unix_t A_unix_t;

        status = HAL_ADCS_get_unix_t(&A_unix_t);

        if (sizeof(A_unix_t) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        A_unix_t.unix_t = csp_hton32((uint32_t)A_unix_t.unix_t);
        A_unix_t.count_ms = csp_hton32((uint32_t)A_unix_t.count_ms);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &A_unix_t, sizeof(A_unix_t));
        set_packet_length(packet, sizeof(A_unix_t) + sizeof(int8_t) + 1);
        break;
    }
    case ADCS_CLEAR_ERR_FLAGS:
        status = HAL_ADCS_clear_err_flags();
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    case ADCS_SET_BOOT_INDEX:
        status = HAL_ADCS_set_boot_index(packet->data[IN_DATA_BYTE]);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;

    case ADCS_RUN_SELECTED_PROGRAM:
        status = HAL_ADCS_run_selected_program();
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;

    case ADCS_READ_PROGRAM_INFO:
        status = HAL_ADCS_read_program_info(packet->data[IN_DATA_BYTE]);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;

    case ADCS_COPY_PROGRAM_INTERNAL_FLASH: {
        uint8_t index = packet->data[IN_DATA_BYTE];
        uint8_t overwrite_flag = packet->data[IN_DATA_BYTE + 1];

        status = HAL_ADCS_copy_program_internal_flash(index, overwrite_flag);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_BOOTLOADER_STATE: {
        ADCS_bootloader_state bootloader_state;

        status = HAL_ADCS_get_bootloader_state(&bootloader_state);

        if (sizeof(bootloader_state) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &bootloader_state, sizeof(bootloader_state));
        set_packet_length(packet, sizeof(bootloader_state) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_PROGRAM_INFO: {

        ADCS_program_info program_info;

        status = HAL_ADCS_get_program_info(&program_info);
        if (sizeof(program_info) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        program_info.index = csp_hton32((uint32_t)program_info.index);
        program_info.busy = csp_hton32((uint32_t)program_info.busy);
        program_info.file_size = csp_hton32((uint32_t)program_info.file_size);
        program_info.crc16_checksum = csp_hton32((uint32_t)program_info.crc16_checksum);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &program_info, sizeof(program_info));
        set_packet_length(packet, sizeof(program_info) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_COPY_INTERNAL_FLASH_PROGRESS: {
        bool busy = false;
        bool err = false;

        status = HAL_ADCS_copy_internal_flash_progress(&busy, &err);
        if (sizeof(busy) + sizeof(err) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        busy = csp_hton32((uint32_t)busy);
        err = csp_hton32((uint32_t)err);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &busy, sizeof(busy));
        memcpy(&packet->data[OUT_DATA_BYTE + 1], &err, sizeof(err));
        set_packet_length(packet, sizeof(busy) + sizeof(err) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_DEPLOY_MAGNETOMETER_BOOM:
        status = HAL_ADCS_deploy_magnetometer_boom(packet->data[IN_DATA_BYTE]);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;

    case ADCS_SET_ENABLED_STATE:
        status = HAL_ADCS_set_enabled_state(packet->data[IN_DATA_BYTE]);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;

    case ADCS_CLEAR_LATCHED_ERRS: {
        status = HAL_ADCS_clear_latched_errs(packet->data[IN_DATA_BYTE], packet->data[IN_DATA_BYTE + 1]);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_ATTITUDE_CONTROL_MODE: {
        uint8_t ctrl_mode = packet->data[IN_DATA_BYTE];
        uint16_t timeout;
        cnv8_16(&packet->data[IN_DATA_BYTE + 1], &timeout);
        timeout = csp_ntoh16(timeout);
        status = HAL_ADCS_set_attitude_ctrl_mode(ctrl_mode, timeout);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_ATTITUDE_ESTIMATE_MODE: {

        status = HAL_ADCS_set_attitude_estimate_mode(packet->data[IN_DATA_BYTE]);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case ADCS_TRIGGER_ADCS_LOOP: {

        status = HAL_ADCS_trigger_adcs_loop();
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_TRIGGER_ADCS_LOOP_SIM: {
        sim_sensor_data data;
        memcpy(&data, &packet->data[IN_DATA_BYTE], sizeof(sim_sensor_data));
        status = HAL_ADCS_trigger_adcs_loop_sim(data);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_ASGP4_RUNE_MODE: {
        status = HAL_ADCS_set_ASGP4_rune_mode(packet->data[IN_DATA_BYTE]);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_TRIGGER_ASGP4: {
        status = HAL_ADCS_trigger_ASGP4();
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_MTM_OP_MODE: {
        status = HAL_ADCS_set_MTM_op_mode(packet->data[IN_DATA_BYTE]);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_CNV2JPG: {
        status = HAL_ADCS_cnv2jpg(packet->data[IN_DATA_BYTE], packet->data[IN_DATA_BYTE + 1],
                                  packet->data[IN_DATA_BYTE + 2]);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SAVE_IMG: {
        status = HAL_ADCS_save_img(packet->data[IN_DATA_BYTE], packet->data[IN_DATA_BYTE + 1]);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case ADCS_SET_MAGNETORQUER_OUTPUT: {
        xyz16 coordinate;
        memcpy(&coordinate, &packet->data[IN_DATA_BYTE], sizeof(xyz16));
        status = HAL_ADCS_set_magnetorquer_output(coordinate);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_WHEEL_SPEED: {
        xyz16 speed;
        memcpy(&speed, &packet->data[IN_DATA_BYTE], sizeof(xyz16));
        status = HAL_ADCS_set_wheel_speed(speed);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SAVE_CONFIG: {
        status = HAL_ADCS_save_config();
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SAVE_ORBIT_PARAMS: {
        status = HAL_ADCS_save_orbit_params();
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_CURRENT_STATE: {
        adcs_state data;
        status = HAL_ADCS_get_current_state(&data);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &data, sizeof(adcs_state));
        set_packet_length(packet, sizeof(data) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_JPG_CNV_PROGESS: {

        ADCS_jpg_cnv_progress jpg_cnv_progress;

        status = HAL_ADCS_get_jpg_cnv_progress(&jpg_cnv_progress);
        if (sizeof(jpg_cnv_progress) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        jpg_cnv_progress.percentage = csp_hton32((uint32_t)jpg_cnv_progress.percentage);
        jpg_cnv_progress.result = csp_hton32((uint32_t)jpg_cnv_progress.result);
        jpg_cnv_progress.file_counter = csp_hton32((uint32_t)jpg_cnv_progress.file_counter);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &jpg_cnv_progress, sizeof(jpg_cnv_progress));
        set_packet_length(packet, sizeof(jpg_cnv_progress) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_CUBEACP_STATE: {
        uint8_t flags_arr[6];
        status = HAL_ADCS_get_cubeACP_state(flags_arr);
        if (sizeof(flags_arr) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &flags_arr, sizeof(flags_arr));
        set_packet_length(packet, sizeof(flags_arr) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_SAT_POS_LLH: {
        LLH target;
        status = HAL_ADCS_get_sat_pos_LLH(&target);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &target, sizeof(LLH));
        set_packet_length(packet, sizeof(LLH) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_EXECUTION_TIMES: {
        ADCS_execution_times execution_times;

        status = HAL_ADCS_get_execution_times(&execution_times);
        if (sizeof(execution_times) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        execution_times.adcs_update = csp_hton32((uint32_t)execution_times.adcs_update);
        execution_times.sensor_comms = csp_hton32((uint32_t)execution_times.sensor_comms);
        execution_times.sgp4_propag = csp_hton32((uint32_t)execution_times.sgp4_propag);
        execution_times.igrf_model = csp_hton32((uint32_t)execution_times.igrf_model);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &execution_times, sizeof(execution_times));
        set_packet_length(packet, sizeof(execution_times) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_ACP_LOOP_STAT: {
        ADCS_ACP_loop_stat ACP_loop_stat;

        status = HAL_ADCS_get_ACP_loop_stat(&ACP_loop_stat);
        if (sizeof(ACP_loop_stat) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        ACP_loop_stat.time = csp_hton32((uint32_t)ACP_loop_stat.time);
        ACP_loop_stat.execution_point = csp_hton32((uint32_t)ACP_loop_stat.execution_point);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &ACP_loop_stat, sizeof(ACP_loop_stat));
        set_packet_length(packet, sizeof(ACP_loop_stat) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_IMG_SAVE_PROGRESS: {
        ADCS_img_save_progress img_save_progress;

        status = HAL_ADCS_get_img_save_progress(&img_save_progress);
        if (sizeof(img_save_progress) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        img_save_progress.percentage = csp_hton32((uint32_t)img_save_progress.percentage);
        img_save_progress.status = csp_hton32((uint32_t)img_save_progress.status);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &img_save_progress, sizeof(img_save_progress));
        set_packet_length(packet, sizeof(img_save_progress) + sizeof(int8_t) + 1);

        break;
    }

    case ADCS_GET_MEASUREMENTS: {

        adcs_measures mes;
        status = HAL_ADCS_get_measurements(&mes);
        if (sizeof(mes) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &mes, sizeof(mes));
        set_packet_length(packet, sizeof(mes) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_ACTUATOR: {
        adcs_actuator cmd;
        status = HAL_ADCS_get_actuator(&cmd);
        if (sizeof(cmd) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &cmd, sizeof(cmd));
        set_packet_length(packet, sizeof(cmd) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_ESTIMATION: {
        adcs_estimate data;
        status = HAL_ADCS_get_estimation(&data);
        if (sizeof(data) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &data, sizeof(data));
        set_packet_length(packet, sizeof(data) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_ASGP4: {
        bool complete = false;
        uint8_t err = 0;
        adcs_asgp4 asgp4;
        status = HAL_ADCS_get_ASGP4(&complete, &err, &asgp4);
        if (sizeof(complete) + sizeof(err) + sizeof(asgp4) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &complete, sizeof(complete));
        memcpy(&packet->data[OUT_DATA_BYTE + 1], &err, sizeof(err));
        memcpy(&packet->data[OUT_DATA_BYTE + 2], &asgp4, sizeof(asgp4));
        set_packet_length(packet, sizeof(complete) + sizeof(err) + sizeof(asgp4) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_RAW_SENSOR: {
        adcs_raw_sensor raw;
        status = HAL_ADCS_get_raw_sensor(&raw);
        if (sizeof(raw) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &raw, sizeof(raw));
        set_packet_length(packet, sizeof(raw) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_RAW_GPS: {
        adcs_raw_gps mes;
        status = HAL_ADCS_get_raw_GPS(&mes);
        if (sizeof(mes) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &mes, sizeof(mes));
        set_packet_length(packet, sizeof(mes) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_STAR_TRACKER: {
        adcs_star_track mes;
        status = HAL_ADCS_get_star_tracker(&mes);
        if (sizeof(mes) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &mes, sizeof(mes));
        set_packet_length(packet, sizeof(mes) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_MTM2_MEASUREMENTS: {
        xyz16 mag;
        status = HAL_ADCS_get_MTM2_measurements(&mag);
        if (sizeof(mag) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &mag, sizeof(mag));
        set_packet_length(packet, sizeof(mag) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_POWER_TEMP: {
        adcs_pwr_temp mes;
        status = HAL_ADCS_get_power_temp(&mes);
        if (sizeof(mes) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &mes, sizeof(mes));
        set_packet_length(packet, sizeof(mes) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_POWER_CONTROL: {
        uint8_t control[10];
        memcpy(control, &packet->data[IN_DATA_BYTE], 10);
        status = HAL_ADCS_set_power_control(control);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_POWER_CONTROL: {
        uint8_t control[10];
        status = HAL_ADCS_get_power_control(control);
        if (sizeof(control) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], control, sizeof(control));
        set_packet_length(packet, sizeof(control) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_ATTITUDE_ANGLE: {
        xyz angle;
        memcpy(&angle, &packet->data[IN_DATA_BYTE], sizeof(xyz));
        status = ADCS_set_attitude_angle(angle);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_ATTITUDE_ANGLE: {
        xyz angle;
        status = HAL_ADCS_get_attitude_angle(&angle);
        if (sizeof(angle) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &angle, sizeof(angle));
        set_packet_length(packet, sizeof(angle) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_TRACK_CONTROLLER: {
        xyz target;
        memcpy(&target, &packet->data[IN_DATA_BYTE], sizeof(xyz));
        status = ADCS_set_track_controller(target);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_TRACK_CONTROLLER: {
        xyz target;
        status = HAL_ADCS_get_track_controller(&target);
        if (sizeof(target) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &target, sizeof(target));
        set_packet_length(packet, sizeof(target) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_LOG_CONFIG: {
        uint16_t period;
        memcpy(&period, &packet->data[IN_DATA_BYTE], 2);
        uint8_t dest = packet->data[IN_DATA_BYTE + 2];
        uint8_t log = packet->data[IN_DATA_BYTE + 3];
        char file_name[30];
        memcpy(file_name, &packet->data[IN_DATA_BYTE + 4], 30);

        strncat(file_name, ".txt", 4);
        int32_t iErr = red_chdir("VOL0:/adcs");
        if (iErr == -1) {
            sys_log(ERROR, "Error %d trying to change into adcs directory\r\n", red_errno);
            status = ADCS_FILESYSTEM_FAIL;
            memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
            set_packet_length(packet, sizeof(int8_t) + 1);
            break;
        }

        int32_t file1 = red_open(file_name, RED_O_RDONLY);
        if (file1 == -1) {
            uint16_t error = red_errno;
            sys_log(ERROR, "ADCS_SET_LOG_CONFIG file error %d\r\n", red_errno);
            status = ADCS_FILE_DNE;
            memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
            set_packet_length(packet, sizeof(int8_t) + 1);
            break;
        }

        char buf[160];
        iErr = red_read(file1, buf, 160);
        if (iErr == -1) {
            sys_log(ERROR, "Error %d trying to read from file %s\r\n", red_errno, file_name);
            status = ADCS_FILESYSTEM_FAIL;
            memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
            set_packet_length(packet, sizeof(int8_t) + 1);
            break;
        }

        char *token;
        token = strtok(buf, "\n");
        uint8_t flags_arr[80];
        for (uint8_t flags_index = 0; flags_index < 80; flags_index++) {
            flags_arr[flags_index] = atoi(token);
            token = strtok(NULL, "\n");
        }

        status = HAL_ADCS_set_log_config(flags_arr, period, dest, log);

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_LOG_CONFIG: {
        uint8_t flags_arr[80];
        uint16_t period;
        uint8_t dest;
        uint8_t log = packet->data[IN_DATA_BYTE];
        status = HAL_ADCS_get_log_config(flags_arr, &period, &dest, log);
        if (sizeof(flags_arr) + sizeof(period) + sizeof(dest) + sizeof(log) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &flags_arr, sizeof(flags_arr));
        memcpy(&packet->data[OUT_DATA_BYTE + 80], &period, sizeof(period));
        memcpy(&packet->data[OUT_DATA_BYTE + 82], &dest, sizeof(dest));
        memcpy(&packet->data[OUT_DATA_BYTE + 83], &log, sizeof(log));

        set_packet_length(packet,
                          sizeof(flags_arr) + sizeof(period) + sizeof(dest) + sizeof(log) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_INERTIAL_REF: {
        xyz iner_ref;
        memcpy(&iner_ref, &packet->data[IN_DATA_BYTE], sizeof(xyz));
        status = ADCS_set_inertial_ref(iner_ref);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_INERTIAL_REF: {
        xyz iner_ref;
        status = HAL_ADCS_get_inertial_ref(&iner_ref);
        if (sizeof(iner_ref) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &iner_ref, sizeof(iner_ref));
        set_packet_length(packet, sizeof(iner_ref) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_SGP4_ORBIT_PARAMS: {
        adcs_sgp4 sgp;
        memcpy(&sgp, &packet->data[IN_DATA_BYTE], sizeof(adcs_sgp4));
        status = ADCS_set_sgp4_orbit_params(sgp);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_SGP4_ORBIT_PARAMS: {
        adcs_sgp4 sgp;
        status = HAL_ADCS_get_sgp4_orbit_params(&sgp);
        if (sizeof(sgp) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &sgp, sizeof(sgp));
        set_packet_length(packet, sizeof(sgp) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_SYSTEM_CONFIG: {
        adcs_sysConfig config;
        memcpy(&config, &packet->data[IN_DATA_BYTE], sizeof(adcs_sysConfig));
        status = ADCS_set_system_config(config);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_SYSTEM_CONFIG: {
        adcs_sysConfig config;
        status = HAL_ADCS_get_system_config(&config);
        if (sizeof(config) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &config, sizeof(config));
        set_packet_length(packet, sizeof(config) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_MTQ_CONFIG: {
        xyzu8 params;
        memcpy(&params, &packet->data[IN_DATA_BYTE], sizeof(xyzu8));
        status = HAL_ADCS_set_MTQ_config(params);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_RW_CONFIG: {
        uint8_t RW[4];
        memcpy(RW, &packet->data[IN_DATA_BYTE], 4);
        status = HAL_ADCS_set_RW_config(RW);
        if (sizeof(RW) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);

        break;
    }

    case ADCS_SET_RATE_GYRO: {
        rate_gyro_config params;
        memcpy(&params, &packet->data[IN_DATA_BYTE], sizeof(rate_gyro_config));
        status = ADCS_set_rate_gyro(params);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_CSS_CONFIG: {
        css_config config;
        memcpy(&config, &packet->data[IN_DATA_BYTE], sizeof(css_config));
        status = ADCS_set_css_config(config);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_STAR_TRACK_CONFIG: {
        cubestar_config config;
        memcpy(&config, &packet->data[IN_DATA_BYTE], sizeof(cubestar_config));
        status = ADCS_set_star_track_config(config);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_CUBESENSE_CONFIG: {
        cubesense_config cs_config;
        char file_name[30];
        memcpy(file_name, &packet->data[IN_DATA_BYTE], 30);

        strncat(file_name, ".txt", 4);
        int32_t iErr = red_chdir("VOL0:/adcs");
        if (iErr == -1) {
            sys_log(ERROR, "Error %d trying to change into adcs directory\r\n", red_errno);
            status = ADCS_FILESYSTEM_FAIL;
            memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
            set_packet_length(packet, sizeof(int8_t) + 1);
            break;
        }

        // Open file for reading config
        int32_t file1 = red_open(file_name, RED_O_RDONLY);
        if (file1 == -1) {
            sys_log(ERROR, "Error %d trying to open file %s\r\n", red_errno, file_name);
            status = ADCS_FILE_DNE;
            memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
            set_packet_length(packet, sizeof(int8_t) + 1);
            break;
        }

        char buf[512];
        iErr = red_read(file1, buf, 512);
        if (iErr == -1) {
            sys_log(ERROR, "Error %d trying to read from file %s\r\n", red_errno, file_name);
            status = ADCS_FILESYSTEM_FAIL;
            memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
            set_packet_length(packet, sizeof(int8_t) + 1);
            break;
        }

        char *token;
        token = strtok(buf, "\n");

        for (uint8_t camsensor_counter = 0; camsensor_counter < 2; camsensor_counter++) {
            camsensor_config conf;
            conf.mounting_angle.x = atof(token);
            token = strtok(NULL, "\n");
            conf.mounting_angle.y = atof(token);
            token = strtok(NULL, "\n");
            conf.mounting_angle.z = atof(token);
            token = strtok(NULL, "\n");
            conf.detect_th = atoi(token);
            token = strtok(NULL, "\n");
            conf.auto_adjust = atoi(token);
            token = strtok(NULL, "\n");
            conf.exposure_t = atoi(token);
            token = strtok(NULL, "\n");
            conf.boresight_x = atof(token);
            token = strtok(NULL, "\n");
            conf.boresight_y = atof(token);
            token = strtok(NULL, "\n");
            if (camsensor_counter == 0) {
                cs_config.cam1_sense = conf;
            } else {
                cs_config.cam2_sense = conf;
            }
        }

        cs_config.nadir_max_deviate = atoi(token);
        token = strtok(NULL, "\n");
        cs_config.nadir_max_bad_edge = atoi(token);
        token = strtok(NULL, "\n");
        cs_config.nadir_max_radius = atoi(token);
        token = strtok(NULL, "\n");
        cs_config.nadir_min_radius = atoi(token);
        token = strtok(NULL, "\n");

        for (uint8_t area_counter = 0; area_counter < 2; area_counter++) {
            cam_area temp;
            temp.area1.x.min = atoi(token);
            token = strtok(NULL, "\n");
            temp.area1.x.max = atoi(token);
            token = strtok(NULL, "\n");
            temp.area1.y.min = atoi(token);
            token = strtok(NULL, "\n");
            temp.area1.y.max = atoi(token);
            token = strtok(NULL, "\n");
            temp.area2.x.min = atoi(token);
            token = strtok(NULL, "\n");
            temp.area2.x.max = atoi(token);
            token = strtok(NULL, "\n");
            temp.area2.y.min = atoi(token);
            token = strtok(NULL, "\n");
            temp.area2.y.max = atoi(token);
            token = strtok(NULL, "\n");
            temp.area3.x.min = atoi(token);
            token = strtok(NULL, "\n");
            temp.area3.x.max = atoi(token);
            token = strtok(NULL, "\n");
            temp.area3.y.min = atoi(token);
            token = strtok(NULL, "\n");
            temp.area3.y.max = atoi(token);
            token = strtok(NULL, "\n");
            temp.area4.x.min = atoi(token);
            token = strtok(NULL, "\n");
            temp.area4.x.max = atoi(token);
            token = strtok(NULL, "\n");
            temp.area4.y.min = atoi(token);
            token = strtok(NULL, "\n");
            temp.area4.y.max = atoi(token);
            token = strtok(NULL, "\n");
            temp.area5.x.min = atoi(token);
            token = strtok(NULL, "\n");
            temp.area5.x.max = atoi(token);
            token = strtok(NULL, "\n");
            temp.area5.y.min = atoi(token);
            token = strtok(NULL, "\n");
            temp.area5.y.max = atoi(token);
            token = strtok(NULL, "\n");
            if (area_counter == 0) {
                cs_config.cam1_area = temp;
            } else {
                cs_config.cam2_area = temp;
            }

            status = ADCS_set_cubesense_config(cs_config);
        }

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_MTM_CONFIG: {
        mtm_config config;
        memcpy(&config, &packet->data[IN_DATA_BYTE], sizeof(mtm_config));
        status = HAL_ADCS_set_mtm_config(config, 1); // We only have one mag so the mtm param is redundant
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_DETUMBLE_CONFIG: {
        detumble_config config;
        memcpy(&config, &packet->data[IN_DATA_BYTE], sizeof(detumble_config));
        status = HAL_ADCS_set_detumble_config(&config);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_YWHEEL_CONFIG: {
        ywheel_ctrl_config config;
        memcpy(&config, &packet->data[IN_DATA_BYTE], sizeof(ywheel_ctrl_config));
        status = HAL_ADCS_set_ywheel_config(config);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }
    case ADCS_SET_RWHEEL_CONFIG: {
        rwheel_ctrl_config config;
        memcpy(&config, &packet->data[IN_DATA_BYTE], sizeof(ywheel_ctrl_config));
        status = HAL_ADCS_set_rwheel_config(config);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }
    case ADCS_SET_TRACKING_CONFIG: {
        track_ctrl_config config;
        memcpy(&config, &packet->data[IN_DATA_BYTE], sizeof(track_ctrl_config));
        status = HAL_ADCS_set_tracking_config(config);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_MOI_MAT: {
        moment_inertia_config config;
        memcpy(&config, &packet->data[IN_DATA_BYTE], sizeof(moment_inertia_config));
        status = HAL_ADCS_set_MoI_mat(config);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_ESTIMATION_CONFIG: {
        estimation_config config;
        memcpy(&config, &packet->data[IN_DATA_BYTE], sizeof(estimation_config));
        status = HAL_ADCS_set_estimation_config(config);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_USERCODED_SETTING: {
        usercoded_setting setting;
        memcpy(&setting, &packet->data[IN_DATA_BYTE], sizeof(usercoded_setting));
        status = HAL_ADCS_set_usercoded_setting(setting);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_SET_ASGP4_SETTING: {
        aspg4_setting setting;
        memcpy(&setting, &packet->data[IN_DATA_BYTE], sizeof(aspg4_setting));
        status = HAL_ADCS_set_asgp4_setting(setting);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_GET_FULL_CONFIG: {
        adcs_config config;
        status = HAL_ADCS_get_full_config(&config);
        if (sizeof(config) + 1 > csp_buffer_data_size()) {
            return_state = SATR_ERROR;
        }

        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&packet->data[OUT_DATA_BYTE], &config, sizeof(adcs_config));
        set_packet_length(packet, sizeof(adcs_config) + sizeof(int8_t) + 1);
        break;
    }

    case ADCS_DOWNLOAD_FILE_LIST_TO_OBC: {
        status = HAL_ADCS_download_file_list_to_OBC();
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    case ADCS_DOWNLOAD_FILE_TO_OBC: {
        adcs_file_download_id *id = (adcs_file_download_id *)pvPortMalloc(sizeof(adcs_file_download_id));
        memcpy(id, &packet->data[IN_DATA_BYTE], sizeof(adcs_file_download_id));
        status = HAL_ADCS_download_file_to_OBC(id);
        memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(packet, sizeof(int8_t) + 1);
        break;
    }

    default:
        break;
    }

    return return_state;
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
    int32_t iErr = red_mkdir("adcs");
    if (iErr == -1 && red_errno != 17) {
        sys_log(ERROR, "Unexpected error %d from creating adcs directory", red_errno);
    } else {
        sys_log(INFO, "Successfully created adcs directory");
    }

    // create socket
    csp_socket_t *sock = csp_socket(CSP_SO_HMACREQ);

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
        if ((conn = csp_accept(sock, DELAY_WAIT_TIMEOUT)) == NULL) {

            /* timeout */
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
    if (xTaskCreate((TaskFunction_t)adcs_service, "adcs_service", ADCS_SVC_SIZE, NULL, NORMAL_SERVICE_PRIO,
                    NULL) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK start_adcs_service\n");
        return SATR_ERROR;
    }

    ex2_log("ADCS service started\n");

    return SATR_OK;
}
