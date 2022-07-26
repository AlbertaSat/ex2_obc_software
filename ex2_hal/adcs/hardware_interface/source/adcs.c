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
 * @file adcs.c
 * @author Arash Yazdan, Trung Tran, Thomas Ganley, Grace Yi
 * @date 2021-11-05
 */

#include "adcs.h"

ADCS_returnState HAL_ADCS_download_file_list_to_OBC(void) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    return ADCS_get_file_list();
#endif
}

ADCS_returnState HAL_ADCS_download_file_to_OBC(adcs_file_download_id *id) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    // Spawn high-priority file download task
    TaskHandle_t xHandle;
    if (xTaskCreate(ADCS_download_file_task, "ADCS_download_file_task", ADCS_QUEUE_GET_TASK_SIZE, (void *)id,
                    ADCS_QUEUE_GET_TASK_PRIO, &xHandle) == pdPASS) {
        return ADCS_OK;
    } else {
        vPortFree(id);
        return ADCS_TASK_FAIL;
    }
#endif
}

ADCS_returnState HAL_ADCS_reset() {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "ADCS_reset called");
    return ADCS_reset();
#endif
}

ADCS_returnState HAL_ADCS_reset_log_pointer() {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "ADCS_reset_log_pointer called");
    return ADCS_reset_log_pointer();
#endif
}

ADCS_returnState HAL_ADCS_advance_log_pointer() {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_advance_log_pointer called");
    return ADCS_advance_log_pointer();
#endif
}

ADCS_returnState HAL_ADCS_reset_boot_registers() {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_reset_boot_registers called");
    return ADCS_reset_boot_registers();
#endif
}

ADCS_returnState HAL_ADCS_format_sd_card() {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_format_sd_card called");
    return ADCS_format_sd_card();
#endif
}

ADCS_returnState HAL_ADCS_erase_file(uint8_t file_type, uint8_t file_counter, bool erase_all) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_erase_file called");
    return ADCS_erase_file(file_type, file_counter, erase_all);
#endif
}

ADCS_returnState HAL_ADCS_load_file_download_block(uint8_t file_type, uint8_t counter, uint32_t offset,
                                                   uint16_t block_length) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_load_file_download_block called");
    return ADCS_load_file_download_block(file_type, counter, offset, block_length);
#endif
}

ADCS_returnState HAL_ADCS_advance_file_list_read_pointer() {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_advance_file_list_read_pointer called");
    return ADCS_advance_file_list_read_pointer();
#endif
}

ADCS_returnState HAL_ADCS_initiate_file_upload(uint8_t file_dest, uint8_t block_size) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_initiate_file_upload called");
    return ADCS_initiate_file_upload(file_dest, block_size);
#endif
}

ADCS_returnState HAL_ADCS_file_upload_packet(uint16_t packet_number, uint8_t *file_bytes, int packet_size) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_file_upload_packet called");
    return ADCS_file_upload_packet(packet_number, file_bytes, packet_size);
#endif
}

ADCS_returnState HAL_ADCS_finalize_upload_block(uint8_t file_dest, uint32_t offset, uint16_t block_length) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_finalize_upload_block called");
    return ADCS_finalize_upload_block(file_dest, offset, block_length);
#endif
}

ADCS_returnState HAL_ADCS_reset_upload_block() {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_reset_upload_block called");
    return ADCS_reset_upload_block();
#endif
}

struct {
    char border1[32];
    ADCS_returnState state;
    char border2[32];
} border;

int HAL_ADCS_firmware_upload(uint8_t file_dest, char *filename) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_firmware_upload called");
    // ADCS firmware is uploaded in 20kB blocks to the external flash, and each 20kB is uploaded in 20 byte packets
    // There are 7 external flashes, each with a size of 512kB

    //---------------------------------------Initialization----------------------------------------//
    ADCS_returnState state;
    // send initiate file upload
    state = ADCS_initiate_file_upload(file_dest, 0);
    if (state != ADCS_OK) {
        sys_log(ERROR, "HAL_ADCS_firmware_upload failed at ADCS_initiate_file_upload, state: %d", state);
        return state;
    }
    // wait for the flash erase procedure to finish
    vTaskDelay(8000);
    // check if the flash erase is complete
    bool init_busy_flag = TRUE;
    int try_again_flag = 0;
    while (init_busy_flag == TRUE) {
        state = ADCS_get_init_upload_stat(&init_busy_flag);
        try_again_flag++;
        vTaskDelay(1000);
        if (try_again_flag > 5) {
            sys_log(ERROR, "HAL_ADCS_firmware_upload failed at ADCS_get_init_upload_stat, flash was not erased");
            return -1;
        }
    }
    //-----------------------------------------Upload------------------------------------------//
    // open firware file from reliance edge
    int32_t fout, f_stat, f_read;
    fout = red_open(filename, RED_O_RDWR);
    if (fout < 0) {
        sys_log(ERROR, "ADCS firmware file not found, upload firmware");
        return ADCS_FIRMWARE_NA;
    }
    // read the size of the firmware file
    REDSTAT firmware_stat;
    f_stat = red_fstat(fout, &firmware_stat);
    if (firmware_stat.st_size <= 0) {
        sys_log(ERROR, "ADCS firmware size is <= 0, re-load firmware");
        red_close(fout);
        return ADCS_FIRMWARE_NA;
    }
    // close file
    red_close(fout);

    // divide the firmware into 20kB blocks
    int num_blocks = firmware_stat.st_size / FIRMWARE_BLOCK_SIZE;
    if (num_blocks > 0) {
        int CRC16_retries = 0;
        int ADCS_reset_retires = 0;
        uint8_t *firmware_buff = pvPortMalloc(FIRMWARE_BLOCK_SIZE);
        for (int i = 0; i < num_blocks; i++) {
            // reset upload block, ie. reset the hole-map
            border.state = ADCS_reset_upload_block();
            while (border.state != ADCS_OK) {
                vTaskDelay(500);
                border.state = ADCS_reset_upload_block();
                ADCS_reset_retires++;
                if (ADCS_reset_retires > 10) {
                    sys_log(ERROR, "HAL_ADCS_firmware_upload failed at ADCS_reset_upload_block, state: %d",
                            border.state);
                    //                    return border.state;
                }
            }
            // file upload packet
            memset(firmware_buff, 0, FIRMWARE_BLOCK_SIZE);
            if (firmware_buff == NULL) {
                sys_log(ERROR, "ADCS firmware_buff malloc failed, out of memory");
                return MALLOC_FAILED;
            }
            // open firmware file from reliance edge
            fout = red_open(filename, RED_O_RDWR);
            if (fout < 0) {
                sys_log(ERROR, "ADCS firmware file not found, upload firmware");
                vPortFree(firmware_buff);
                return ADCS_FIRMWARE_NA;
            }
            red_lseek(fout, i * FIRMWARE_BLOCK_SIZE, RED_SEEK_SET);
            f_read = red_read(fout, firmware_buff, FIRMWARE_BLOCK_SIZE);
            if (f_read != FIRMWARE_BLOCK_SIZE) {
                sys_log(ERROR, "ADCS firmware red_read failed, err #: %d", (int)red_errno);
                vPortFree(firmware_buff);
                red_close(fout);
                return RED_ERR;
            }
            // close file
            red_close(fout);
            //---------------------test code below-----------------------//
            //            uint16_t ADCS_CRC16_test, OBC_buffer_CRC16_test;
            //            int OBC_CRC16_test = CRC_Calc(firmware_buff, (uint32_t)FIRMWARE_BLOCK_SIZE);
            //            int CRC16_state_test = ADCS_get_upload_crc16_checksum(&ADCS_CRC16_test);
            //---------------------test code above-----------------------//
            // divide the firmware block into 20 byte packets
            uint16_t num_packets = FIRMWARE_BLOCK_SIZE / PACKET_SIZE;
            // TODO: find out if the first packet should be 0, or 1
            for (int packet_ctr = 0; packet_ctr < num_packets; packet_ctr++) {
                // upload firmware buffer to ADCS
                HAL_ADCS_file_upload_packet(packet_ctr, firmware_buff + packet_ctr * PACKET_SIZE, PACKET_SIZE);
            }
            // check hole map for missed packets
            int hole_map_num = 8;
            uint8_t *hole_map = pvPortMalloc(hole_map_num * HOLE_MAP_SIZE);
            if (hole_map == NULL) {
                sys_log(ERROR, "ADCS firmware aborted due to hole_map malloc failure, out of memory");
                vPortFree(firmware_buff);
                return MALLOC_FAILED;
            }
            int hole_map_complete = 0;
            try_again_flag = 0;
            while (hole_map_complete == 0) {
                memset(hole_map, 0, hole_map_num * HOLE_MAP_SIZE);
                hole_map_complete = 1;
                for (int j = 1; j <= hole_map_num; j++) {
                    HAL_ADCS_get_hole_map(hole_map + (j - 1) * HOLE_MAP_SIZE, j);
                }
                for (int j = 0; j < num_packets; j++) {
                    if ((hole_map[j >> 3] & (1 << (j & 0x07))) == 0) {
                        // resend missed packet
                        HAL_ADCS_file_upload_packet(j, firmware_buff + j * PACKET_SIZE, PACKET_SIZE);
                        hole_map_complete = 0;
                    }
                }
                try_again_flag++;
                if (try_again_flag > 3) {
                    sys_log(ERROR, "ADCS firmware aborted due to hole_map malloc failure, out of memory");
                    vPortFree(firmware_buff);
                    vPortFree(hole_map);
                    return HOLE_MAP_FAILED;
                }
            }
            //---------------------test code below-----------------------//
            //            OBC_CRC16_test = CRC_Calc(firmware_buff, (uint32_t)FIRMWARE_BLOCK_SIZE);
            //            CRC16_state_test = ADCS_get_upload_crc16_checksum(&ADCS_CRC16_test);
            //            HAL_ADCS_finalize_upload_block(file_dest, (uint32_t)num_blocks * FIRMWARE_BLOCK_SIZE,
            //            FIRMWARE_BLOCK_SIZE);
            //---------------------test code above-----------------------//

            // send finalized block
            HAL_ADCS_finalize_upload_block(file_dest, (uint32_t)i * FIRMWARE_BLOCK_SIZE, FIRMWARE_BLOCK_SIZE);
            // upload block complete?
            bool upload_busy, upload_err;
            upload_busy = 1;
            upload_err = 0;
            while (upload_busy != 0) {
                ADCS_get_finalize_upload_stat(&upload_busy, &upload_err);
                if (upload_err == 1) {
                    sys_log(ERROR, "ADCS_get_finalize_upload_stat error, upload failed");
                    vPortFree(firmware_buff);
                    vPortFree(hole_map);
                    return UPLOAD_FAILED;
                }
                vTaskDelay(500);
            }

            vPortFree(hole_map);
        }
        vPortFree(firmware_buff);
    }
    //--------------------------Upload any remaining block smaller than 20kB-----------------------------//
    int CRC16_retries = 0;
    int remaining_block_size = firmware_stat.st_size % FIRMWARE_BLOCK_SIZE;
    if (remaining_block_size != 0 && CRC16_retries < MAX_CRC16_ATTEMPTS) {
        // reset upload block, ie. reset the hole-map
        state = ADCS_reset_upload_block();
        if (state != ADCS_OK) {
            sys_log(ERROR, "HAL_ADCS_firmware_upload failed at ADCS_reset_upload_block, state: %d", state);
            return state;
        }
        // file upload packet
        uint8_t *firmware_buff = pvPortMalloc(remaining_block_size);
        memset(firmware_buff, 0, remaining_block_size);
        if (firmware_buff == NULL) {
            sys_log(ERROR, "ADCS firmware_buff malloc failed, out of memory");
            return MALLOC_FAILED;
        }
        // open firware file from reliance edge
        fout = red_open(filename, RED_O_RDWR);
        if (fout < 0) {
            sys_log(ERROR, "ADCS firmware file not found, upload firmware");
            vPortFree(firmware_buff);
            return ADCS_FIRMWARE_NA;
        }
        red_lseek(fout, num_blocks * FIRMWARE_BLOCK_SIZE, RED_SEEK_SET);
        f_read = red_read(fout, firmware_buff, remaining_block_size);
        if (f_read != remaining_block_size) {
            sys_log(ERROR, "ADCS firmware red_read failed, err #: %d", (int)red_errno);
            vPortFree(firmware_buff);
            red_close(fout);
            return RED_ERR;
        }
        // close file
        red_close(fout);

        // divide the firmware block into 20 byte packets
        uint16_t num_packets = remaining_block_size / PACKET_SIZE;
        // TODO: check if the first packet should be 0, or 1
        int packet_ctr = 0;
        for (packet_ctr; packet_ctr < (int)num_packets; packet_ctr++) {
            // upload firmware buffer to ADCS
            HAL_ADCS_file_upload_packet(packet_ctr, firmware_buff + packet_ctr * PACKET_SIZE, PACKET_SIZE);
        }
        // upload remaining bytes less than 20 bytes
        uint16_t remaining_bytes = remaining_block_size % PACKET_SIZE;
        if (remaining_bytes != 0) {
            HAL_ADCS_file_upload_packet(packet_ctr, firmware_buff + num_packets * PACKET_SIZE, remaining_bytes);
            packet_ctr++;
        }
        // check hole map for missed packets
        int hole_map_num = 0;
        if (packet_ctr % (16 * 8) != 0) {
            hole_map_num = packet_ctr / (16 * 8) + 1;
        } else {
            hole_map_num = packet_ctr / (16 * 8);
        }
        uint8_t *hole_map = pvPortMalloc(hole_map_num * HOLE_MAP_SIZE);
        memset(hole_map, 0, hole_map_num * HOLE_MAP_SIZE);
        if (hole_map == NULL) {
            sys_log(ERROR, "ADCS firmware aborted due to hole_map malloc failure, out of memory");
            vPortFree(firmware_buff);
            return MALLOC_FAILED;
        }
        int hole_map_complete = 0;
        while (hole_map_complete == 0) {
            hole_map_complete = 1;
            for (int j = 1; j <= hole_map_num; j++) {
                HAL_ADCS_get_hole_map(hole_map + (j - 1) * HOLE_MAP_SIZE, j);
            }
            for (int j = 0; j < packet_ctr; j++) {
                if ((hole_map[j >> 3] & (1 << (j & 0x07))) == 0) {
                    // resend missed packet
                    if ((j == packet_ctr - 1) && (remaining_bytes != 0)) {
                        // if this is the last packet with less than 20 bytes, resend remaining bytes
                        HAL_ADCS_file_upload_packet(j, firmware_buff + j * PACKET_SIZE, remaining_bytes);
                    } else {
                        HAL_ADCS_file_upload_packet(j, firmware_buff + j * PACKET_SIZE, PACKET_SIZE);
                    }
                    hole_map_complete = 0;
                }
            }
        }

        // send finalized block
        HAL_ADCS_finalize_upload_block(file_dest, (uint32_t)num_blocks * FIRMWARE_BLOCK_SIZE,
                                       remaining_block_size);
        // upload block complete?
        bool upload_busy, upload_err;
        upload_busy = 1;
        upload_err = 0;
        while (upload_busy != 0) {
            ADCS_get_finalize_upload_stat(&upload_busy, &upload_err);
            if (upload_err == 1) {
                sys_log(ERROR, "ADCS_get_finalize_upload_stat error, upload failed");
                vPortFree(firmware_buff);
                vPortFree(hole_map);
                return UPLOAD_FAILED;
            }
            vTaskDelay(500);
        }

        vPortFree(firmware_buff);
        vPortFree(hole_map);
    }

    // check CRC16 checksum for bit errors
    uint16_t OBC_CRC16;
    uint16_t ADCS_CRC16;
    uint32_t upload_file_size = (uint32_t)firmware_stat.st_size;
    bool get_program_busy = 1;
    OBC_CRC16 = 0;
    adcs_file_info *info;
    OBC_CRC16 = CRC_Calc(filename);
    // ADCS_returnState CRC16_state = ADCS_get_file_info(info);
    ADCS_returnState read_program_info = ADCS_read_program_info(file_dest);
    ADCS_returnState get_program_info =
        ADCS_get_program_info(&file_dest, &get_program_busy, &upload_file_size, &ADCS_CRC16);
    /*
    ADCS_CRC16 = info->crc16_checksum;
    if (CRC16_state != ADCS_OK) {
        sys_log(ERROR, "ADCS_get_upload_crc16_checksum state returned %d", CRC16_state);
        return CRC16_state;
    }
    */
    while (get_program_busy = 1) {
        vTaskDelay(2000);
        get_program_info = ADCS_get_program_info(&file_dest, &get_program_busy, &upload_file_size, &ADCS_CRC16);
    }

    if (ADCS_CRC16 != OBC_CRC16) {
        sys_log(ERROR, "CRC16 checksum mismatch");
        return CRC16_MISMATCH;
    }

    return ADCS_OK;
#endif
}

uint16_t CRC_Calc(char *filename) {
    // initialize variables
    uint32_t fout = red_open(filename, RED_O_RDONLY);
    REDSTAT firmware_stat;
    uint32_t f_stat = red_fstat(fout, &firmware_stat);
    int buff_len = 4608; // 512*9 bytes of the file will be used to calculate each additive CRC16 checksum
    int read_len = 5120; // 512*10 bytes will be read from file to provide extra room for bitwise shifts
    int remaining_length = buff_len + firmware_stat.st_size % buff_len;
    int num_blocks = firmware_stat.st_size / buff_len;
    uint8_t *file_buff = pvPortMalloc(read_len);
    memset(file_buff, 0, read_len);
    uint16_t crc = 0;

    // initialize start/end
    uint32_t f_read, f_close;
    uint8_t *data, *end;
    data = file_buff;
    end = file_buff + buff_len;

    for (int i = 0; i < num_blocks; i++) {

        // initialize/shift to the next additive CRC16 calculation
        memset(file_buff, 0, read_len);
        red_lseek(fout, i * buff_len, RED_SEEK_SET);
        f_read = red_read(fout, file_buff, read_len);
        // data = file_buff;
        end = file_buff + buff_len;

        for (data = file_buff; data < end; data++) {
            crc = (crc >> 8) | (crc << 8);
            crc ^= *data;
            crc ^= (crc & 0xff) >> 4;
            crc ^= crc << 12;
            crc ^= (crc & 0xff) << 5;
        }
    }

    // calculate the CRC16 of the last block and remaining bytes together
    // shift to the next additive CRC16 calculation
    vPortFree(file_buff);
    if (remaining_length > 0) {
        file_buff = pvPortMalloc(remaining_length);
        memset(file_buff, 0, remaining_length);
        red_lseek(fout, num_blocks * buff_len, RED_SEEK_SET);
        f_read = red_read(fout, file_buff, remaining_length);
        // data = file_buff;
        end = file_buff + remaining_length;

        for (data = file_buff; data < end; data++) {
            crc = (crc >> 8) | (crc << 8);
            crc ^= *data;
            crc ^= (crc & 0xff) >> 4;
            crc ^= crc << 12;
            crc ^= (crc & 0xff) << 5;
        }

        vPortFree(file_buff);
    }
    f_close = red_close(fout);

    return crc;
}

ADCS_returnState HAL_ADCS_reset_file_list_read_pointer() {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_reset_file_list_read_pointer called");
    return ADCS_reset_file_list_read_pointer();
#endif
}

ADCS_returnState HAL_ADCS_initiate_download_burst(uint8_t msg_length, bool ignore_hole_map) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_initiate_download_burst called");
    return ADCS_initiate_download_burst(msg_length, ignore_hole_map);
#endif
}

ADCS_returnState HAL_ADCS_get_node_identification(ADCS_node_identification *node_id) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_node_identification called");
    return ADCS_get_node_identification(&node_id->node_type, &node_id->interface_ver, &node_id->major_firm_ver,
                                        &node_id->minor_firm_ver, &node_id->runtime_s, &node_id->runtime_ms);
#endif
}

ADCS_returnState HAL_ADCS_get_boot_program_stat(ADCS_boot_program_stat *boot_program_stat) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_boot_program_stat called");
    return ADCS_get_boot_program_stat(&boot_program_stat->mcu_reset_cause, &boot_program_stat->boot_cause,
                                      &boot_program_stat->boot_count, &boot_program_stat->boot_idx,
                                      &boot_program_stat->major_firm_version,
                                      &boot_program_stat->minor_firm_version);
#endif
}

ADCS_returnState HAL_ADCS_get_boot_index(ADCS_boot_index *boot_index) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_boot_index called");
    return ADCS_get_boot_index(&boot_index->program_idx, &boot_index->boot_stat);
#endif
}

ADCS_returnState HAL_ADCS_get_last_logged_event(ADCS_last_logged_event *last_logged_event) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_last_logged_event called");
    return ADCS_get_last_logged_event(&last_logged_event->time, &last_logged_event->event_id,
                                      &last_logged_event->event_param);
#endif
}

ADCS_returnState HAL_ADCS_get_SD_format_progress(bool *format_busy, bool *erase_all_busy) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_SD_format_progress called");
    return ADCS_get_SD_format_progress(format_busy, erase_all_busy);
#endif
}

ADCS_returnState HAL_ADCS_get_TC_ack(ADCS_TC_ack *TC_ack) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_TC_ack called");
    return ADCS_get_TC_ack(&TC_ack->last_tc_id, &TC_ack->tc_processed, &TC_ack->tc_err_stat, &TC_ack->tc_err_idx);
#endif
}

ADCS_returnState HAL_ADCS_get_file_download_buffer(uint16_t *packet_count, uint8_t file[20]) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_file_download_buffer called");
    return ADCS_get_file_download_buffer(packet_count, file);
#endif
}

ADCS_returnState HAL_ADCS_get_file_download_block_stat(ADCS_file_download_block_stat *file_download_block_stat) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_file_download_block_stat called");
    return ADCS_get_file_download_block_stat(
        &file_download_block_stat->ready, &file_download_block_stat->param_err,
        &file_download_block_stat->crc16_checksum, &file_download_block_stat->length);
#endif
}

ADCS_returnState HAL_ADCS_get_file_info(ADCS_file_info *file_info) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_file_info called");
    return ADCS_get_file_info(file_info);
#endif
}

ADCS_returnState HAL_ADCS_get_init_upload_stat(bool *busy) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_init_upload_stat called");
    return ADCS_get_init_upload_stat(busy);
#endif
}

ADCS_returnState HAL_ADCS_get_finalize_upload_stat(bool *busy, bool *err) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_finalize_upload_stat called");
    return ADCS_get_finalize_upload_stat(busy, err);
#endif
}

ADCS_returnState HAL_ADCS_get_upload_crc16_checksum(uint16_t *checksum) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_upload_crc16_checksum called");
    return ADCS_get_upload_crc16_checksum(checksum);
#endif
}

ADCS_returnState HAL_ADCS_get_SRAM_latchup_count(ADCS_SRAM_latchup_count *SRAM_latchup_count) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_SRAM_latchup_count called");
    return ADCS_get_SRAM_latchup_count(&SRAM_latchup_count->sram1, &SRAM_latchup_count->sram2);
#endif
}

ADCS_returnState HAL_ADCS_get_EDAC_err_count(ADCS_EDAC_err_count *EDAC_err_count) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_EDAC_err_count called");
    return ADCS_get_EDAC_err_count(&EDAC_err_count->single_sram, &EDAC_err_count->double_sram,
                                   &EDAC_err_count->multi_sram);
#endif
}

ADCS_returnState HAL_ADCS_get_comms_stat(uint16_t *comm_status) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_comms_stat called");
    ADCS_returnState return_state;
    uint16_t TC_num = 0;
    uint16_t TM_num = 0;
    uint8_t flags_arr[6] = {0};
    return_state = ADCS_get_comms_stat(&TC_num, &TM_num, flags_arr);
    *(comm_status) = TC_num;
    *(comm_status + 1) = TM_num;
    *(comm_status + 2) = (flags_arr[0] << 8) | flags_arr[1];
    *(comm_status + 3) = (flags_arr[2] << 8) | flags_arr[3];
    *(comm_status + 4) = (flags_arr[4] << 8) | flags_arr[5];
    return return_state;
#endif
}

ADCS_returnState HAL_ADCS_set_cache_en_state(bool en_state) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_cache_en_state called");
    return ADCS_set_cache_en_state(en_state);
#endif
}

ADCS_returnState HAL_ADCS_set_sram_scrub_size(uint16_t size) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_sram_scrub_size called");
    return ADCS_set_sram_scrub_size(size);
#endif
}

ADCS_returnState HAL_ADCS_set_UnixTime_save_config(uint8_t when, uint8_t period) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_UnixTime_save_config called");
    return ADCS_set_UnixTime_save_config(when, period);
#endif
}

ADCS_returnState HAL_ADCS_set_hole_map(uint8_t *hole_map, uint8_t num) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_hole_map called");
    return ADCS_set_hole_map(hole_map, num);
#endif
}

ADCS_returnState HAL_ADCS_set_unix_t(uint32_t unix_t, uint16_t count_ms) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_unix_t called");
    return ADCS_set_unix_t(unix_t, count_ms);
#endif
}

ADCS_returnState HAL_ADCS_get_cache_en_state(bool *en_state) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_cache_en_state called");
    return ADCS_get_cache_en_state(en_state);
#endif
}

ADCS_returnState HAL_ADCS_get_sram_scrub_size(uint16_t *size) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_sram_scrub_size called");
    return ADCS_get_sram_scrub_size(size);
#endif
}

ADCS_returnState HAL_ADCS_get_UnixTime_save_config(ADCS_Unixtime_save_config *Unixtime_save_config) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_UnixTime_save_config called");
    return ADCS_get_UnixTime_save_config(&Unixtime_save_config->when, &Unixtime_save_config->period);
#endif
}

ADCS_returnState HAL_ADCS_get_hole_map(uint8_t *hole_map, uint8_t num) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_hole_map called");
    return ADCS_get_hole_map(hole_map, num);
#endif
}

ADCS_returnState HAL_ADCS_get_unix_t(ADCS_unix_t *A_unix_t) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_unix_t called");
    return ADCS_get_unix_t(&A_unix_t->unix_t, &A_unix_t->count_ms);
#endif
}

ADCS_returnState HAL_ADCS_clear_err_flags() {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_clear_err_flags called");
    return ADCS_clear_err_flags();
#endif
}

ADCS_returnState HAL_ADCS_set_boot_index(uint8_t index) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_boot_index called");
    return ADCS_set_boot_index(index);
#endif
}

ADCS_returnState HAL_ADCS_run_selected_program() {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_run_selected_program called");
    return ADCS_run_selected_program();
#endif
}

ADCS_returnState HAL_ADCS_read_program_info(uint8_t index) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_read_program_info called");
    return ADCS_read_program_info(index);
#endif
}

ADCS_returnState HAL_ADCS_copy_program_internal_flash(uint8_t index, uint8_t overwrite_flag) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_copy_program_internal_flash called");
    return ADCS_copy_program_internal_flash(index, overwrite_flag);
#endif
}

ADCS_returnState HAL_ADCS_get_bootloader_state(ADCS_bootloader_state *bootloader_state) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    return ADCS_get_bootloader_state(&bootloader_state->uptime, &bootloader_state->flags_arr[0]);
#endif
}

ADCS_returnState HAL_ADCS_get_program_info(ADCS_program_info *program_info) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_program_info called");
    return ADCS_get_program_info(&program_info->index, &program_info->busy, &program_info->file_size,
                                 &program_info->crc16_checksum);
#endif
}

ADCS_returnState HAL_ADCS_copy_internal_flash_progress(bool *busy, bool *err) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_copy_internal_flash_progress called");
    return HAL_ADCS_copy_internal_flash_progress(busy, err);
#endif
}

ADCS_returnState HAL_ADCS_deploy_magnetometer_boom(uint8_t actuation_timeout) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_deploy_magnetometer_boom called");
    return ADCS_deploy_magnetometer_boom(actuation_timeout);
#endif
}

ADCS_returnState HAL_ADCS_set_enabled_state(uint8_t state) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_enabled_state called");
    return ADCS_set_enabled_state(state);
#endif
}

ADCS_returnState HAL_ADCS_clear_latched_errs(bool adcs_flag, bool hk_flag) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_clear_latched_errs called");
    return ADCS_clear_latched_errs(adcs_flag, hk_flag);
#endif
}

ADCS_returnState HAL_ADCS_set_attitude_ctrl_mode(uint8_t ctrl_mode, uint16_t timeout) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_attitude_ctrl_mode called");
    return ADCS_set_attitude_ctrl_mode(ctrl_mode, timeout);
#endif
}

ADCS_returnState HAL_ADCS_set_attitude_estimate_mode(uint8_t mode) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_attitude_estimate_mode called");
    return ADCS_set_attitude_estimate_mode(mode);
#endif
}

ADCS_returnState HAL_ADCS_trigger_adcs_loop() {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_trigger_adcs_loop called");
    return ADCS_trigger_adcs_loop();
#endif
}

ADCS_returnState HAL_ADCS_trigger_adcs_loop_sim(sim_sensor_data sim_data) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_trigger_adcs_loop_sim called");
    return ADCS_trigger_adcs_loop_sim(sim_data);
#endif
}

ADCS_returnState HAL_ADCS_set_ASGP4_rune_mode(uint8_t mode) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_ASGP4_rune_mode called");
    return ADCS_set_ASGP4_rune_mode(mode);
#endif
}

ADCS_returnState HAL_ADCS_trigger_ASGP4() {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_trigger_ASGP4 called");
    return ADCS_trigger_ASGP4();
#endif
}

ADCS_returnState HAL_ADCS_set_MTM_op_mode(uint8_t mode) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_MTM_op_mode called");
    return ADCS_set_MTM_op_mode(mode);
#endif
}

ADCS_returnState HAL_ADCS_cnv2jpg(uint8_t source, uint8_t QF, uint8_t white_balance) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_cnv2jpg called");
    return ADCS_cnv2jpg(source, QF, white_balance);
#endif
}

ADCS_returnState HAL_ADCS_save_img(uint8_t camera, uint8_t img_size) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_save_img called");
    return ADCS_save_img(camera, img_size);
#endif
}

ADCS_returnState HAL_ADCS_set_magnetorquer_output(xyz16 duty_cycle) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_magnetorquer_output called");
    return ADCS_set_magnetorquer_output(duty_cycle);
#endif
}

ADCS_returnState HAL_ADCS_set_wheel_speed(xyz16 speed) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_wheel_speed called");
    return ADCS_set_wheel_speed(speed);
#endif
}

ADCS_returnState HAL_ADCS_save_config() {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_save_config called");
    return ADCS_save_config();
#endif
}

ADCS_returnState HAL_ADCS_save_orbit_params() {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_save_orbit_params called");
    return ADCS_save_orbit_params();
#endif
}

ADCS_returnState HAL_ADCS_get_current_state(adcs_state *data) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_current_state called");
    return ADCS_get_current_state(data);
#endif
}

ADCS_returnState HAL_ADCS_get_jpg_cnv_progress(ADCS_jpg_cnv_progress *jpg_cnv_progress) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_jpg_cnv_progress called");
    return ADCS_get_jpg_cnv_progress(&jpg_cnv_progress->percentage, &jpg_cnv_progress->result,
                                     &jpg_cnv_progress->file_counter);
#endif
}

ADCS_returnState HAL_ADCS_get_cubeACP_state(uint8_t *flags_arr) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_cubeACP_state called");
    return ADCS_get_cubeACP_state(flags_arr);
#endif
}

ADCS_returnState HAL_ADCS_get_execution_times(ADCS_execution_times *execution_times) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_execution_times called");
    return ADCS_get_execution_times(&execution_times->adcs_update, &execution_times->sensor_comms,
                                    &execution_times->sgp4_propag, &execution_times->igrf_model);
#endif
}

ADCS_returnState HAL_ADCS_get_ACP_loop_stat(ADCS_ACP_loop_stat *ACP_loop_stat) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_ACP_loop_stat called");
    return ADCS_get_ACP_loop_stat(&ACP_loop_stat->time, &ACP_loop_stat->execution_point);
#endif
}

ADCS_returnState HAL_ADCS_get_sat_pos_LLH(LLH *target) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_sat_pos_LLH called");
    return ADCS_get_sat_pos_LLH(target);
#endif
}

ADCS_returnState HAL_ADCS_get_img_save_progress(ADCS_img_save_progress *img_save_progress) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_img_save_progress called");
    return ADCS_get_img_save_progress(&img_save_progress->percentage, &img_save_progress->status);
#endif
}

ADCS_returnState HAL_ADCS_get_measurements(adcs_measures *measurements) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_measurements called");
    return ADCS_get_measurements(measurements);
#endif
}

ADCS_returnState HAL_ADCS_get_actuator(adcs_actuator *commands) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_actuator called");
    return ADCS_get_actuator(commands);
#endif
}

ADCS_returnState HAL_ADCS_get_estimation(adcs_estimate *data) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_estimation called");
    return ADCS_get_estimation(data);
#endif
}

ADCS_returnState HAL_ADCS_get_ASGP4(bool *complete, uint8_t *err, adcs_asgp4 *asgp4) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_ASGP4 called");
    return ADCS_get_ASGP4(complete, err, asgp4);
#endif
}

ADCS_returnState HAL_ADCS_get_raw_sensor(adcs_raw_sensor *measurements) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_raw_sensor called");
    return ADCS_get_raw_sensor(measurements);
#endif
}

ADCS_returnState HAL_ADCS_get_raw_GPS(adcs_raw_gps *measurements) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_raw_GPS called");
    return ADCS_get_raw_GPS(measurements);
#endif
}

ADCS_returnState HAL_ADCS_get_star_tracker(adcs_star_track *measurements) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_star_tracker called");
    return ADCS_get_star_tracker(measurements);
#endif
}

ADCS_returnState HAL_ADCS_get_MTM2_measurements(xyz16 *Mag) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_MTM2_measurements called");
    return ADCS_get_MTM2_measurements(Mag);
#endif
}

ADCS_returnState HAL_ADCS_get_power_temp(adcs_pwr_temp *measurements) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_power_temp called");
    return ADCS_get_power_temp(measurements);
#endif
}

ADCS_returnState HAL_ADCS_set_power_control(uint8_t *control) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_power_control called");
    return ADCS_set_power_control(control);
#endif
}

ADCS_returnState HAL_ADCS_get_power_control(uint8_t *control) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_power_control called");
    return ADCS_get_power_control(control);
#endif
}

ADCS_returnState HAL_ADCS_set_attitude_angle(xyz att_angle) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_attitude_angle called");
    return ADCS_set_attitude_angle(att_angle);
#endif
}

ADCS_returnState HAL_ADCS_get_attitude_angle(xyz *att_angle) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_attitude_angle called");
    return ADCS_get_attitude_angle(att_angle);
#endif
}

ADCS_returnState HAL_ADCS_set_track_controller(xyz target) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_track_controller called");
    return ADCS_set_track_controller(target);
#endif
}

ADCS_returnState HAL_ADCS_get_track_controller(xyz *target) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_track_controller called");
    return ADCS_get_track_controller(target);
#endif
}

ADCS_returnState HAL_ADCS_set_log_config(uint8_t *flags_arr, uint16_t period, uint8_t dest, uint8_t log) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_log_config called");
    return ADCS_set_log_config(flags_arr, period, dest, log);
#endif
}

ADCS_returnState HAL_ADCS_get_log_config(uint8_t *flags_arr, uint16_t *period, uint8_t *dest, uint8_t log) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_log_config called");
    return ADCS_get_log_config(flags_arr, period, dest, log);
#endif
}

ADCS_returnState HAL_ADCS_set_inertial_ref(xyz iner_ref) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_inertial_ref called");
    return ADCS_set_inertial_ref(iner_ref);
#endif
}

ADCS_returnState HAL_ADCS_get_inertial_ref(xyz *iner_ref) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_inertial_ref called");
    return ADCS_get_inertial_ref(iner_ref);
#endif
}

ADCS_returnState HAL_ADCS_set_sgp4_orbit_params(adcs_sgp4 params) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_sgp4_orbit_params called");
    return ADCS_set_sgp4_orbit_params(params);
#endif
}

ADCS_returnState HAL_ADCS_get_sgp4_orbit_params(adcs_sgp4 *params) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_sgp4_orbit_params called");
    return ADCS_get_sgp4_orbit_params(params);
#endif
}

ADCS_returnState HAL_ADCS_set_system_config(adcs_sysConfig config) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_system_config called");
    return ADCS_set_system_config(config);
#endif
}

ADCS_returnState HAL_ADCS_get_system_config(adcs_sysConfig *config) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_system_config called");
    return ADCS_get_system_config(config);
#endif
}

ADCS_returnState HAL_ADCS_set_MTQ_config(xyzu8 params) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_MTQ_config called");
    return ADCS_set_MTQ_config(params);
#endif
}

ADCS_returnState HAL_ADCS_set_RW_config(uint8_t *RW) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_RW_config called");
    return ADCS_set_RW_config(RW);
#endif
}

ADCS_returnState HAL_ADCS_set_rate_gyro(rate_gyro_config params) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_rate_gyro called");
    return ADCS_set_rate_gyro(params);
#endif
}

ADCS_returnState HAL_ADCS_set_css_config(css_config config) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_css_config called");
    return ADCS_set_css_config(config);
#endif
}

ADCS_returnState HAL_ADCS_set_star_track_config(cubestar_config config) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_star_track_config called");
    return ADCS_set_star_track_config(config);
#endif
}

ADCS_returnState HAL_ADCS_set_cubesense_config(cubesense_config params) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_cubesense_config called");
    return ADCS_set_cubesense_config(params);
#endif
}

ADCS_returnState HAL_ADCS_set_mtm_config(mtm_config params, uint8_t mtm) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_mtm_config called");
    return ADCS_set_mtm_config(params, mtm);
#endif
}

ADCS_returnState HAL_ADCS_set_detumble_config(detumble_config *config) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_detumble_config called");
    return ADCS_set_detumble_config(config);
#endif
}

ADCS_returnState HAL_ADCS_set_ywheel_config(ywheel_ctrl_config params) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_ywheel_config called");
    return ADCS_set_ywheel_config(params);
#endif
}

ADCS_returnState HAL_ADCS_set_rwheel_config(rwheel_ctrl_config params) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_rwheel_config called");
    return ADCS_set_rwheel_config(params);
#endif
}

ADCS_returnState HAL_ADCS_set_tracking_config(track_ctrl_config params) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_tracking_config called");
    return ADCS_set_tracking_config(params);
#endif
}

ADCS_returnState HAL_ADCS_set_MoI_mat(moment_inertia_config cell) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_MoI_mat called");
    return ADCS_set_MoI_mat(cell);
#endif
}

ADCS_returnState HAL_ADCS_set_estimation_config(estimation_config config) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_estimation_config called");
    return ADCS_set_estimation_config(config);
#endif
}

ADCS_returnState HAL_ADCS_set_usercoded_setting(usercoded_setting setting) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_usercoded_setting called");
    return ADCS_set_usercoded_setting(setting);
#endif
}

ADCS_returnState HAL_ADCS_set_asgp4_setting(aspg4_setting setting) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_set_asgp4_setting called");
    return ADCS_set_asgp4_setting(setting);
#endif
}

ADCS_returnState HAL_ADCS_get_full_config(adcs_config *config) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_get_full_config called");
    return ADCS_get_full_config(config);
#endif
}

ADCS_returnState HAL_ADCS_getHK(ADCS_HouseKeeping *adcs_hk) {
#if ADCS_IS_STUBBED == 1
    return IS_STUBBED_A;
#else
    sys_log(INFO, "HAL_ADCS_getHK called");
    ADCS_returnState temp;
    ADCS_returnState return_state = ADCS_OK;
    adcs_state data;
    adcs_measures mes;
    adcs_pwr_temp pwr;
    LLH pos;

    if ((temp = HAL_ADCS_get_current_state(&data)) != ADCS_OK) {
        return_state = temp;
    } else {
        // adcs_hk->Estimated_Angular_Rate = data.est_angular_rate;
        adcs_hk->Estimated_Angular_Rate_X = data.est_angular_rate.x;
        adcs_hk->Estimated_Angular_Rate_Y = data.est_angular_rate.y;
        adcs_hk->Estimated_Angular_Rate_Z = data.est_angular_rate.z;
        // adcs_hk->Estimated_Angular_Angle = data.est_angle;
        adcs_hk->Estimated_Angular_Angle_X = data.est_angle.x;
        adcs_hk->Estimated_Angular_Angle_Y = data.est_angle.y;
        adcs_hk->Estimated_Angular_Angle_Z = data.est_angle.z;
        // adcs_hk->Sat_Position_ECI = data.ECI_pos;
        adcs_hk->Sat_Position_ECI_X = data.ECI_pos.x;
        adcs_hk->Sat_Position_ECI_Y = data.ECI_pos.y;
        adcs_hk->Sat_Position_ECI_Z = data.ECI_pos.z;
        // adcs_hk->Sat_Velocity_ECI = data.ECI_vel;
        adcs_hk->Sat_Velocity_ECI_X = data.ECI_vel.x;
        adcs_hk->Sat_Velocity_ECI_Y = data.ECI_vel.y;
        adcs_hk->Sat_Velocity_ECI_Z = data.ECI_vel.z;
        // adcs_hk->ECEF_Position = data.ecef_pos;
        adcs_hk->ECEF_Position_X = data.ecef_pos.x;
        adcs_hk->ECEF_Position_Y = data.ecef_pos.y;
        adcs_hk->ECEF_Position_Z = data.ecef_pos.z;
    }

    if ((temp = HAL_ADCS_get_measurements(&mes)) != ADCS_OK) {
        return_state = temp;
    } else {
        // adcs_hk->Coarse_Sun_Vector = mes.coarse_sun;
        adcs_hk->Coarse_Sun_Vector_X = mes.coarse_sun.x;
        adcs_hk->Coarse_Sun_Vector_Y = mes.coarse_sun.y;
        adcs_hk->Coarse_Sun_Vector_Z = mes.coarse_sun.z;
        // adcs_hk->Fine_Sun_Vector = mes.sun;
        adcs_hk->Fine_Sun_Vector_X = mes.sun.x;
        adcs_hk->Fine_Sun_Vector_Y = mes.sun.y;
        adcs_hk->Fine_Sun_Vector_Z = mes.sun.z;
        // adcs_hk->Nadir_Vector = mes.nadir;
        adcs_hk->Nadir_Vector_X = mes.nadir.x;
        adcs_hk->Nadir_Vector_Y = mes.nadir.y;
        adcs_hk->Nadir_Vector_Z = mes.nadir.z;
        // adcs_hk->Wheel_Speed = mes.wheel_speed;
        adcs_hk->Wheel_Speed_X = mes.wheel_speed.x;
        adcs_hk->Wheel_Speed_Y = mes.wheel_speed.y;
        adcs_hk->Wheel_Speed_Z = mes.wheel_speed.z;
        // adcs_hk->Mag_Field_Vector = mes.magnetic_field;
        adcs_hk->Mag_Field_Vector_X = mes.magnetic_field.x;
        adcs_hk->Mag_Field_Vector_Y = mes.magnetic_field.y;
        adcs_hk->Mag_Field_Vector_Z = mes.magnetic_field.z;
    }

    if ((temp = HAL_ADCS_get_power_temp(&pwr)) != ADCS_OK) {
        return_state = temp;
    } else {
        adcs_hk->Wheel1_Current = pwr.wheel1_I;
        adcs_hk->Wheel2_Current = pwr.wheel2_I;
        adcs_hk->Wheel3_Current = pwr.wheel3_I;
        adcs_hk->CubeSense1_Current = pwr.cubesense1_3v3_I;
        adcs_hk->CubeSense2_Current = pwr.cubesense2_3v3_I;
        adcs_hk->CubeControl_Current3v3 = pwr.cubecontrol_3v3_I;
        adcs_hk->CubeControl_Current5v0 = pwr.cubecontrol_5v_I;
        adcs_hk->CubeStar_Current = pwr.cubestar_I;
        adcs_hk->Magnetorquer_Current = pwr.magnetorquer_I;
        adcs_hk->CubeStar_Temp = pwr.cubestar_temp;
        adcs_hk->MCU_Temp = pwr.MCU_temp;
        // adcs_hk->Rate_Sensor_Temp = pwr.rate_sensor_temp;
        adcs_hk->Rate_Sensor_Temp_X = pwr.rate_sensor_temp.x;
        adcs_hk->Rate_Sensor_Temp_Y = pwr.rate_sensor_temp.y;
        adcs_hk->Rate_Sensor_Temp_Z = pwr.rate_sensor_temp.z;
    }

    if ((temp = HAL_ADCS_get_sat_pos_LLH(&pos)) != ADCS_OK) {
        return_state = temp;
    } else {
        adcs_hk->Sat_Position_LLH_X = pos.latitude;
        adcs_hk->Sat_Position_LLH_Y = pos.longitude;
        adcs_hk->Sat_Position_LLH_Z = pos.altitude;
    }

    uint16_t comms_stat[5];

    if ((temp = HAL_ADCS_get_comms_stat(comms_stat)) != ADCS_OK) {
        return_state = temp;
    } else {
        adcs_hk->TC_num = comms_stat[0];
        adcs_hk->TM_num = comms_stat[1];
        memcpy(&adcs_hk->CommsStat_flags, &comms_stat[2], 6);
    }

    return return_state;
#endif
}
