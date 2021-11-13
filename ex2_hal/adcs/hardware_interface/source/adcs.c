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
 * @author Arash Yazdan, Trung Tran, Thomas Ganley
 * @date 2021-11-05
 */

#include "adcs.h"

ADCS_returnState HAL_ADCS_reset() {
#ifdef ADCS_IS_STUBBED
    return IS_STUBBED_A;
#else
    return ADCS_reset();
#endif
}

ADCS_returnState HAL_ADCS_reset_log_pointer() {
#ifdef ADCS_IS_STUBBED
    return IS_STUBBED_A;
#else
    return ADCS_reset_log_pointer();
#endif
}

ADCS_returnState HAL_ADCS_advance_log_pointer() {
#ifdef ADCS_IS_STUBBED
    return IS_STUBBED_A;
#else
    return ADCS_advance_log_pointer();
#endif
}

ADCS_returnState HAL_ADCS_reset_boot_registers() {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_reset_boot_registers();
    #endif
}

ADCS_returnState HAL_ADCS_format_sd_card() {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_format_sd_card();
    #endif
}

ADCS_returnState HAL_ADCS_erase_file(uint8_t file_type, uint8_t file_counter, bool erase_all) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_erase_file(file_type, file_counter, erase_all);
    #endif
}

ADCS_returnState HAL_ADCS_load_file_download_block(uint8_t file_type, uint8_t counter, uint32_t offset,
    uint16_t block_length) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_load_file_download_block(file_type, counter, offset, block_length);
    #endif
}

ADCS_returnState HAL_ADCS_advance_file_list_read_pointer() {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_advance_file_list_read_pointer();
    #endif
}

ADCS_returnState HAL_ADCS_initiate_file_upload(uint8_t file_dest, uint8_t block_size) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_initiate_file_upload(file_dest, block_size);
    #endif
}

ADCS_returnState HAL_ADCS_file_upload_packet(uint16_t packet_number, char *file_bytes) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_file_upload_packet(packet_number, file_bytes);
    #endif
}

ADCS_returnState HAL_ADCS_finalize_upload_block(uint8_t file_dest, uint32_t offset, uint16_t block_length) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_finalize_upload_block(file_dest, offset, block_length);
    #endif
}

ADCS_returnState HAL_ADCS_reset_upload_block() {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_reset_upload_block();
    #endif
}

ADCS_returnState HAL_ADCS_reset_file_list_read_pointer() {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_reset_file_list_read_pointer();
    #endif
}

ADCS_returnState HAL_ADCS_initiate_download_burst(uint8_t msg_length, bool ignore_hole_map) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_initiate_download_burst(msg_length, ignore_hole_map);
    #endif
}

ADCS_returnState HAL_ADCS_get_node_identification(ADCS_node_identification *node_id) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_node_identification(&node_id->node_type, &node_id->interface_ver, &node_id->major_firm_ver,
                                        &node_id->minor_firm_ver, &node_id->runtime_s, &node_id->runtime_ms);
    #endif
}

ADCS_returnState HAL_ADCS_get_boot_program_stat(ADCS_boot_program_stat *boot_program_stat) {
    return ADCS_get_boot_program_stat(&boot_program_stat->mcu_reset_cause, &boot_program_stat->boot_cause,
                                      &boot_program_stat->boot_count, &boot_program_stat->boot_idx);
}

ADCS_returnState HAL_ADCS_get_boot_index(ADCS_boot_index *boot_index) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_boot_index(&boot_index->program_idx, &boot_index->boot_stat);
    #endif
}

ADCS_returnState HAL_ADCS_get_last_logged_event(ADCS_last_logged_event *last_logged_event) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_last_logged_event(&last_logged_event->time, &last_logged_event->event_id,
                                      &last_logged_event->event_param);
    #endif
}

ADCS_returnState HAL_ADCS_get_SD_format_progress(bool *format_busy, bool *erase_all_busy) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_SD_format_progress(format_busy, erase_all_busy);
    #endif
}

ADCS_returnState HAL_ADCS_get_TC_ack(ADCS_TC_ack *TC_ack) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_TC_ack(&TC_ack->last_tc_id, &TC_ack->tc_processed, &TC_ack->tc_err_stat, &TC_ack->tc_err_idx);
    #endif
}

ADCS_returnState HAL_ADCS_get_file_download_buffer(uint16_t *packet_count, uint8_t *file[20]) {
    return ADCS_get_file_download_buffer(packet_count, file);
}

ADCS_returnState HAL_ADCS_get_file_download_block_stat(ADCS_file_download_block_stat *file_download_block_stat) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_file_download_block_stat(
            &file_download_block_stat->ready, &file_download_block_stat->param_err,
            &file_download_block_stat->crc16_checksum, &file_download_block_stat->length);
    #endif
}

ADCS_returnState HAL_ADCS_get_file_info(adcs_file_info *file_info) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_file_info(file_info);
    #endif
}

ADCS_returnState HAL_ADCS_get_init_upload_stat(bool *busy) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_init_upload_stat(busy);
    #endif
}

ADCS_returnState HAL_ADCS_get_finalize_upload_stat(bool *busy, bool *err) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_finalize_upload_stat(busy, err);
    #endif
}

ADCS_returnState HAL_ADCS_get_upload_crc16_checksum(uint16_t *checksum) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_upload_crc16_checksum(checksum);
    #endif
}

ADCS_returnState HAL_ADCS_get_SRAM_latchup_count(ADCS_SRAM_latchup_count *SRAM_latchup_count) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_SRAM_latchup_count(&SRAM_latchup_count->sram1, &SRAM_latchup_count->sram2);
    #endif
}

ADCS_returnState HAL_ADCS_get_EDAC_err_count(ADCS_EDAC_err_count *EDAC_err_count) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_EDAC_err_count(&EDAC_err_count->single_sram, &EDAC_err_count->double_sram,
                                   &EDAC_err_count->multi_sram);
    #endif
}

ADCS_returnState HAL_ADCS_get_comms_stat(uint16_t *comm_status) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        ADCS_returnState return_state;
        uint16_t TC_num = 0;
        uint16_t TM_num = 0;
        uint8_t flags_arr[6] = {0};
        return_state = ADCS_get_comms_stat(&TC_num, &TM_num, flags_arr);
        *(comm_status) = TC_num;
        *(comm_status+1) = TM_num;
        *(comm_status+2) = (flags_arr[0] << 8) | flags_arr[1];
        *(comm_status+3) = (flags_arr[2] << 8) | flags_arr[3];
        *(comm_status+4) = (flags_arr[4] << 8) | flags_arr[5];
        return return_state;
    #endif
}

ADCS_returnState HAL_ADCS_set_cache_en_state(bool en_state) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_cache_en_state(en_state);
    #endif
}

ADCS_returnState HAL_ADCS_set_sram_scrub_size(uint16_t size) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_sram_scrub_size(size);
    #endif
}

ADCS_returnState HAL_ADCS_set_UnixTime_save_config(uint8_t when, uint8_t period) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_UnixTime_save_config(when, period);
    #endif
}

ADCS_returnState HAL_ADCS_set_hole_map(uint8_t *hole_map, uint8_t num) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_hole_map(hole_map, num);
    #endif
}

ADCS_returnState HAL_ADCS_set_unix_t(uint32_t unix_t, uint16_t count_ms) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_unix_t(unix_t, count_ms);
    #endif
}

ADCS_returnState HAL_ADCS_get_cache_en_state(bool *en_state) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_cache_en_state(en_state);
    #endif
}

ADCS_returnState HAL_ADCS_get_sram_scrub_size(uint16_t *size) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_sram_scrub_size(size);
    #endif
}

ADCS_returnState HAL_ADCS_get_UnixTime_save_config(ADCS_Unixtime_save_config *Unixtime_save_config) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_UnixTime_save_config(&Unixtime_save_config->when, &Unixtime_save_config->period);
    #endif
}

ADCS_returnState HAL_ADCS_get_hole_map(uint8_t *hole_map, uint8_t num) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_hole_map(hole_map, num);
    #endif
}

ADCS_returnState HAL_ADCS_get_unix_t(ADCS_unix_t *A_unix_t) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_unix_t(&A_unix_t->unix_t, &A_unix_t->count_ms);
    #endif
}

ADCS_returnState HAL_ADCS_clear_err_flags() {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_clear_err_flags();
    #endif
}

ADCS_returnState HAL_ADCS_set_boot_index(uint8_t index) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_boot_index(index);
    #endif
}

ADCS_returnState HAL_ADCS_run_selected_program() {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_run_selected_program();
    #endif
}

ADCS_returnState HAL_ADCS_read_program_info(uint8_t index) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_read_program_info(index);
    #endif
}

ADCS_returnState HAL_ADCS_copy_program_internal_flash(uint8_t index, uint8_t overwrite_flag) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_copy_program_internal_flash(index, overwrite_flag);
    #endif
}

ADCS_returnState HAL_ADCS_get_bootloader_state(ADCS_bootloader_state *bootloader_state) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_bootloader_state(&bootloader_state->uptime, &bootloader_state->flags_arr);
    #endif
}

ADCS_returnState HAL_ADCS_get_program_info(ADCS_program_info *program_info) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_program_info(&program_info->index, &program_info->busy, &program_info->file_size,
                                 &program_info->crc16_checksum);
    #endif
}

ADCS_returnState HAL_ADCS_copy_internal_flash_progress(bool *busy, bool *err) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return HAL_ADCS_copy_internal_flash_progress(busy, err);
    #endif
}

ADCS_returnState HAL_ADCS_deploy_magnetometer_boom(uint8_t actuation_timeout) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_deploy_magnetometer_boom(actuation_timeout);
    #endif
}

ADCS_returnState HAL_ADCS_set_enabled_state(uint8_t state) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_enabled_state(state);
    #endif
}

ADCS_returnState HAL_ADCS_clear_latched_errs(bool adcs_flag, bool hk_flag) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_clear_latched_errs(adcs_flag, hk_flag);
    #endif
}

ADCS_returnState HAL_ADCS_set_attitude_ctrl_mode(uint8_t ctrl_mode, uint16_t timeout) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_attitude_ctrl_mode(ctrl_mode, timeout);
    #endif
}

ADCS_returnState HAL_ADCS_set_attitude_estimate_mode(uint8_t mode) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_attitude_estimate_mode(mode);
    #endif
}

ADCS_returnState HAL_ADCS_trigger_adcs_loop() {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_trigger_adcs_loop();
    #endif
}

ADCS_returnState HAL_ADCS_trigger_adcs_loop_sim(sim_sensor_data sim_data) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_trigger_adcs_loop_sim(sim_data);
    #endif
}

ADCS_returnState HAL_ADCS_set_ASGP4_rune_mode(uint8_t mode) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_ASGP4_rune_mode(mode);
    #endif
}

ADCS_returnState HAL_ADCS_trigger_ASGP4() {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_trigger_ASGP4();
    #endif
}

ADCS_returnState HAL_ADCS_set_MTM_op_mode(uint8_t mode) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_MTM_op_mode(mode);
    #endif
}

ADCS_returnState HAL_ADCS_cnv2jpg(uint8_t source, uint8_t QF, uint8_t white_balance) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_cnv2jpg(source, QF, white_balance);
    #endif
}

ADCS_returnState HAL_ADCS_save_img(uint8_t camera, uint8_t img_size) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_save_img(camera, img_size);
    #endif
}

ADCS_returnState HAL_ADCS_set_magnetorquer_output(xyz16 duty_cycle) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_magnetorquer_output(duty_cycle);
    #endif
}

ADCS_returnState HAL_ADCS_set_wheel_speed(xyz16 speed) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_wheel_speed(speed);
    #endif
}

ADCS_returnState HAL_ADCS_save_config() {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_save_config();
    #endif
}

ADCS_returnState HAL_ADCS_save_orbit_params() {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_save_orbit_params();
    #endif
}

ADCS_returnState HAL_ADCS_get_current_state(adcs_state *data) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_current_state(data);
    #endif
}

ADCS_returnState HAL_ADCS_get_jpg_cnv_progress(ADCS_jpg_cnv_progress *jpg_cnv_progress) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_jpg_cnv_progress(&jpg_cnv_progress->percentage, &jpg_cnv_progress->result,
                                     &jpg_cnv_progress->file_counter);
    #endif
}

ADCS_returnState HAL_ADCS_get_cubeACP_state(uint8_t *flags_arr) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_cubeACP_state(flags_arr);
    #endif
}

ADCS_returnState HAL_ADCS_get_execution_times(ADCS_execution_times *execution_times) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_execution_times(&execution_times->adcs_update, &execution_times->sensor_comms,
                                    &execution_times->sgp4_propag, &execution_times->igrf_model);
    #endif
}

ADCS_returnState HAL_ADCS_get_ACP_loop_stat(ADCS_ACP_loop_stat *ACP_loop_stat) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_ACP_loop_stat(&ACP_loop_stat->time, &ACP_loop_stat->execution_point);
    #endif
}

ADCS_returnState HAL_ADCS_get_sat_pos_LLH(xyz *target) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_sat_pos_LLH(target);
    #endif
}

ADCS_returnState HAL_ADCS_get_img_save_progress(ADCS_img_save_progress *img_save_progress) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_img_save_progress(&img_save_progress->percentage, &img_save_progress->status);
    #endif
}

ADCS_returnState HAL_ADCS_get_measurements(adcs_measures *measurements) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_measurements(measurements);
    #endif
}

ADCS_returnState HAL_ADCS_get_actuator(adcs_actuator *commands) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_actuator(commands);
    #endif
}

ADCS_returnState HAL_ADCS_get_estimation(adcs_estimate *data) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_estimation(data);
    #endif
}

ADCS_returnState HAL_ADCS_get_ASGP4(bool *complete, uint8_t *err, adcs_asgp4 *asgp4) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_ASGP4(complete, err, asgp4);
    #endif
}

ADCS_returnState HAL_ADCS_get_raw_sensor(adcs_raw_sensor *measurements) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_raw_sensor(measurements);
    #endif
}

ADCS_returnState HAL_ADCS_get_raw_GPS(adcs_raw_gps *measurements) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_raw_GPS(measurements);
    #endif
}

ADCS_returnState HAL_ADCS_get_star_tracker(adcs_star_track *measurements) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_star_tracker(measurements);
    #endif
}

ADCS_returnState HAL_ADCS_get_MTM2_measurements(xyz16 *Mag) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_MTM2_measurements(Mag);
    #endif
}

ADCS_returnState HAL_ADCS_get_power_temp(adcs_pwr_temp *measurements) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_power_temp(measurements);
    #endif
}

ADCS_returnState HAL_ADCS_set_power_control(uint8_t *control) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_power_control(control);
    #endif
}

ADCS_returnState HAL_ADCS_get_power_control(uint8_t *control) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_power_control(control);
    #endif
}

ADCS_returnState HAL_ADCS_set_attitude_angle(xyz att_angle) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_attitude_angle(att_angle);
    #endif
}

ADCS_returnState HAL_ADCS_get_attitude_angle(xyz *att_angle) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_attitude_angle(att_angle);
    #endif
}

ADCS_returnState HAL_ADCS_set_track_controller(xyz target) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_track_controller(target);
    #endif
}

ADCS_returnState HAL_ADCS_get_track_controller(xyz *target) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_track_controller(target);
    #endif
}

ADCS_returnState HAL_ADCS_set_log_config(uint8_t *flags_arr[10], uint16_t period, uint8_t dest, uint8_t log) {
    return ADCS_set_log_config(flags_arr, period, dest, log);
}

ADCS_returnState HAL_ADCS_get_log_config(uint8_t *flags_arr[10], uint16_t *period, uint8_t *dest, uint8_t log) {
    return ADCS_get_log_config(flags_arr, &period, &dest, log);
}

ADCS_returnState HAL_ADCS_set_inertial_ref(xyz iner_ref) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_inertial_ref(iner_ref);
    #endif
}

ADCS_returnState HAL_ADCS_get_inertial_ref(xyz *iner_ref) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_inertial_ref(iner_ref);
    #endif
}

ADCS_returnState HAL_ADCS_set_sgp4_orbit_params(adcs_sgp4 params) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_sgp4_orbit_params(params);
    #endif
}

ADCS_returnState HAL_ADCS_get_sgp4_orbit_params(adcs_sgp4 *params) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_sgp4_orbit_params(params);
    #endif
}

ADCS_returnState HAL_ADCS_set_system_config(adcs_sysConfig config) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_system_config(config);
    #endif
}

ADCS_returnState HAL_ADCS_get_system_config(adcs_sysConfig *config) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_system_config(config);
    #endif
}

ADCS_returnState HAL_ADCS_set_MTQ_config(xyzu8 params) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_MTQ_config(params);
    #endif
}

ADCS_returnState HAL_ADCS_set_RW_config(uint8_t *RW) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_RW_config(RW);
    #endif
}

ADCS_returnState HAL_ADCS_set_rate_gyro(rate_gyro_config params) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_rate_gyro(params);
    #endif
}

ADCS_returnState HAL_ADCS_set_css_config(css_config config) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_css_config(config);
    #endif
}

ADCS_returnState HAL_ADCS_set_star_track_config(cubestar_config config) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_star_track_config(config);
    #endif
}

ADCS_returnState HAL_ADCS_set_cubesense_config(cubesense_config params) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_cubesense_config(params);
    #endif
}

ADCS_returnState HAL_ADCS_set_mtm_config(mtm_config params, uint8_t mtm) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_mtm_config(params, mtm);
    #endif
}

ADCS_returnState HAL_ADCS_set_detumble_config(detumble_config config) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_detumble_config(config);
    #endif
}

ADCS_returnState HAL_ADCS_set_ywheel_config(ywheel_ctrl_config params) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_ywheel_config(params);
    #endif
}

ADCS_returnState HAL_ADCS_set_rwheel_config(rwheel_ctrl_config params) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_rwheel_config(params);
    #endif
}

ADCS_returnState HAL_ADCS_set_tracking_config(track_ctrl_config params) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_tracking_config(params);
    #endif
}

ADCS_returnState HAL_ADCS_set_MoI_mat(moment_inertia_config cell) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_MoI_mat(cell);
    #endif
}

ADCS_returnState HAL_ADCS_set_estimation_config(estimation_config config) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_estimation_config(config);
    #endif
}

ADCS_returnState HAL_ADCS_set_usercoded_setting(usercoded_setting setting) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_usercoded_setting(setting);
    #endif
}

ADCS_returnState HAL_ADCS_set_asgp4_setting(aspg4_setting setting) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_set_asgp4_setting(setting);
    #endif
}

ADCS_returnState HAL_ADCS_get_full_config(adcs_config *config) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        return ADCS_get_full_config(config);
    #endif
}

ADCS_returnState HAL_ADCS_getHK(ADCS_HouseKeeping *adcs_hk) {
    #ifdef ADCS_IS_STUBBED
        return IS_STUBBED_A;
    #else
        ADCS_returnState temp;
        ADCS_returnState return_state = 0;
        adcs_state data;
        adcs_measures mes;
        adcs_pwr_temp pwr;
        xyz pos;

        if (temp = HAL_ADCS_get_current_state(&data) != 0) {
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


        if (temp = HAL_ADCS_get_measurements(&mes) != 0) {
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


        if (temp = HAL_ADCS_get_power_temp(&pwr) != 0) {
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

        // if (temp = HAL_ADCS_get_sat_pos_LLH(&adcs_hk->Sat_Position_LLH) != 0) return_state = temp;
        if (temp = HAL_ADCS_get_sat_pos_LLH(&pos) != 0) {
            return_state = temp;
        } else {
            adcs_hk->Sat_Position_LLH_X = pos.x;
            adcs_hk->Sat_Position_LLH_Y = pos.y;
            adcs_hk->Sat_Position_LLH_Z = pos.z;
        }

        if (temp = HAL_ADCS_get_comms_stat(&adcs_hk->Comm_Status) != 0)
            return_state = temp;

        return return_state;
    #endif
    }

    if (temp = HAL_ADCS_get_measurements(&mes) != 0) {
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

    if (temp = HAL_ADCS_get_power_temp(&pwr) != 0) {
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

    // if (temp = HAL_ADCS_get_sat_pos_LLH(&adcs_hk->Sat_Position_LLH) != 0) return_state = temp;
    if (temp = HAL_ADCS_get_sat_pos_LLH(&pos) != 0) {
        return_state = temp;
    } else {
        adcs_hk->Sat_Position_LLH_X = pos.x;
        adcs_hk->Sat_Position_LLH_Y = pos.y;
        adcs_hk->Sat_Position_LLH_Z = pos.z;
    }

    if (temp = HAL_ADCS_get_comms_stat(&adcs_hk->Comm_Status) != 0)
        return_state = temp;

    return return_state;
}
