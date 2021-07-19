#include "adcs.h"

ADCS_returnState HAL_ADCS_reset() {
    return ADCS_reset();
}

ADCS_returnState HAL_ADCS_reset_log_pointer() {
    return ADCS_reset_log_pointer();
}

ADCS_returnState HAL_ADCS_advance_log_pointer() {
    return ADCS_advance_log_pointer();
}

ADCS_returnState HAL_ADCS_reset_boot_registers() {
    return ADCS_reset_boot_registers();
}

ADCS_returnState HAL_ADCS_format_sd_card() {
    return ADCS_format_sd_card();
}

ADCS_returnState HAL_ADCS_erase_file(uint8_t file_type, uint8_t file_counter, bool erase_all) {
    return ADCS_erase_file(file_type, file_counter, erase_all);
}

ADCS_returnState HAL_ADCS_load_file_download_block(uint8_t file_type, uint8_t counter, uint32_t offset, uint16_t block_length) {
    return ADCS_load_file_download_block(file_type, counter, offset, block_length);
}

ADCS_returnState HAL_ADCS_advance_file_list_read_pointer() {
    return ADCS_advance_file_list_read_pointer();
}

ADCS_returnState HAL_ADCS_initiate_file_upload(uint8_t file_dest, uint8_t block_size) {
    return ADCS_initiate_file_upload(file_dest, block_size);
}

ADCS_returnState HAL_ADCS_file_upload_packet(uint16_t packet_number, char* file_bytes) {
    return ADCS_file_upload_packet(packet_number, &file_bytes);
}

ADCS_returnState HAL_ADCS_finalize_upload_block(uint8_t file_dest, uint32_t offset, uint16_t block_length) {
    return ADCS_finalize_upload_block(file_dest, offset, block_length);
}

ADCS_returnState HAL_ADCS_reset_upload_block() {
    return ADCS_reset_upload_block();
}

ADCS_returnState HAL_ADCS_reset_file_list_read_pointer() {
    return ADCS_reset_file_list_read_pointer();
}

ADCS_returnState HAL_ADCS_initiate_download_burst(uint8_t msg_length, bool ignore_hole_map) {
    return ADCS_initiate_download_burst(msg_length, ignore_hole_map);
}

ADCS_returnState HAL_ADCS_get_node_identification(
    uint8_t* node_type, uint8_t* interface_ver, uint8_t* major_firm_ver,
    uint8_t* minor_firm_ver, uint16_t* runtime_s, uint16_t* runtime_ms) {
        return ADCS_get_node_identification(&node_type, &interface_ver, &major_firm_ver,
        &minor_firm_ver, &runtime_s, &runtime_ms);
}

ADCS_returnState HAL_ADCS_get_boot_program_stat(uint8_t* mcu_reset_cause, uint8_t* boot_cause, uint16_t* boot_count, uint8_t* boot_idx) {
    return ADCS_get_boot_program_stat(&mcu_reset_cause, &boot_cause, &boot_count, &boot_idx);
}

ADCS_returnState HAL_ADCS_get_boot_index(uint8_t* program_idx, uint8_t* boot_stat) {
    return ADCS_get_boot_index(&program_idx, &boot_stat);
}

ADCS_returnState HAL_ADCS_get_last_logged_event(uint32_t* time, uint8_t* event_id, uint8_t* event_param) {
    return ADCS_get_last_logged_event(&time, &event_id, &event_param);
}

ADCS_returnState HAL_ADCS_get_SD_format_progress(bool* format_busy, bool* erase_all_busy) {
    return ADCS_get_SD_format_progress(&format_busy, &erase_all_busy);
}

ADCS_returnState HAL_ADCS_get_TC_ack(uint8_t* last_tc_id, bool* tc_processed, ADCS_returnState* tc_err_stat, uint8_t* tc_err_idx) {
    return ADCS_get_TC_ack9(&last_tc_id, &tc_processed, &tc_err_stat, &tc_err_idx);
}

ADCS_returnState HAL_ADCS_get_file_download_buffer(uint16_t* packet_count, uint8_t* file[20]) {
    return ADCS_get_file_download_buffer(&packet_count, &file);
}

ADCS_returnState HAL_ADCS_get_file_download_block_stat(bool* ready, bool* param_err, uint16_t* crc16_checksum, uint16_t* length) {
    return ADCS_get_file_download_block_stat(&ready, &param_err, &crc16_checksum, &length);
}

ADCS_returnState HAL_ADCS_get_file_info(uint8_t* type, bool* updating, uint8_t* counter, uint32_t* size, uint32_t* time, uint16_t* crc16_checksum) {
    return ADCS_get_file_info(&type, &updating, &counter, &size, &time, &crc16_checksum);
}

ADCS_returnState HAL_ADCS_get_init_upload_stat(bool* busy) {
    return ADCS_get_init_upload_stat(&busy);
}

ADCS_returnState HAL_ADCS_get_finalize_upload_stat(bool* busy, bool* err) {
    return ADCS_get_finalize_upload_stat(&busy, &err);
}

ADCS_returnState HAL_ADCS_get_upload_crc16_checksum(uint16_t* checksum) {
    return ADCS_get_upload_crc16_checksum(&checksum);
}

ADCS_returnState HAL_ADCS_get_SRAM_latchup_count(uint16_t* sram1, uint16_t* sram2) {
    return ADCS_get_SRAM_latchup_count(&sram1, &sram2);
}

ADCS_returnState HAL_ADCS_get_EDAC_err_count(uint16_t* single_sram, uint16_t* double_sram, uint16_t* multi_sram) {
    return ADCS_get_EDAC_err_count(&single_sram, &double_sram, &multi_sram);
}

ADCS_returnState HAL_ADCS_get_comms_stat(uint16_t* comm_status) {
    ADCS_returnState return_state;
    uint16_t TC_num = 0;
    uint16_t TM_num = 0;
    uint8_t flags_arr = 0;
    return_state = ADCS_get_comms_stat(&comm_status, &TC_num, &TM_num, &flags_arr);

    return return_state;
}

ADCS_returnState HAL_ADCS_set_cache_en_state(bool en_state) {
    return ADCS_set_cache_en_state(en_state);
}

ADCS_returnState HAL_ADCS_set_sram_scrub_size(uint16_t size) {
    return ADCS_set_sram_scrub_size(size);
}

ADCS_returnState HAL_ADCS_set_UnixTime_save_config(uint8_t when, uint8_t period) {
    return ADCS_set_UnixTime_save_config(when, period);
}

ADCS_returnState HAL_ADCS_set_hole_map(uint8_t* hole_map, uint8_t num) {
    return ADCS_set_hole_map(&hole_map, num);
}

ADCS_returnState HAL_ADCS_set_unix_t(uint32_t unix_t, uint16_t count_ms) {
    return ADCS_set_unix_t(unix_t, count_ms);
}

ADCS_returnState HAL_ADCS_get_cache_en_state(bool* en_state) {
    return ADCS_get_cache_en_state(&en_state);
}

ADCS_returnState HAL_ADCS_get_sram_scrub_size(uint16_t* size) {
    return ADCS_get_sram_scrub_size(&size);
}

ADCS_returnState HAL_ADCS_get_UnixTime_save_config(uint8_t* when, uint8_t* period) {
    return ADCS_get_UnixTime_save_config(&when, &period);
}

ADCS_returnState HAL_ADCS_get_hole_map(uint8_t* hole_map, uint8_t num) {
    return ADCS_get_hole_map(&hole_map, num);
}

ADCS_returnState HAL_ADCS_get_unix_t(uint32_t* unix_t, uint16_t* count_ms) {
    return ADCS_get_unix_t(&unix_t, &count_ms);
}

ADCS_returnState HAL_ADCS_clear_err_flags() {
    return ADCS_clear_err_flags();
}

ADCS_returnState HAL_ADCS_set_boot_index(uint8_t index) {
    return ADCS_set_boot_index(index);
}

ADCS_returnState HAL_ADCS_run_selected_program() {
    return ADCS_run_selected_program();
}

ADCS_returnState HAL_ADCS_read_program_info(uint8_t index) {
    return ADCS_read_program_info(index);
}

ADCS_returnState HAL_ADCS_copy_program_internal_flash(uint8_t index, uint8_t overwrite_flag) {
    return ADCS_copy_program_internal_flash(index, overwrite_flag);
}

ADCS_returnState HAL_ADCS_get_bootloader_state(uint16_t* uptime, uint8_t* flags_arr) {
    return ADCS_get_bootloader_state(&uptime, &flags_arr);
}

ADCS_returnState HAL_ADCS_get_program_info(uint8_t* index, bool* busy,
                                       uint32_t* file_size,
                                       uint16_t* crc16_checksum) {
                                           return ADCS_get_program_info(&index, &busy, &file_size, &crc16_checksum);
                                       }

ADCS_returnState HAL_ADCS_copy_internal_flash_progress(bool* busy, bool* err) {
    return HAL_ADCS_copy_internal_flash_progress(&busy, &err);
}

ADCS_returnState HAL_ADCS_deploy_magnetometer_boom(uint8_t actuation_timeout) {
    return ADCS_deploy_magnetometer_boom(actuation_timeout);
}

ADCS_returnState HAL_ADCS_set_enabled_state(uint8_t state) {
    return ADCS_set_enabled_state(state);
}

ADCS_returnState HAL_ADCS_clear_latched_errs(bool adcs_flag, bool hk_flag) {
    return ADCS_clear_latched_errs(adcs_flag, hk_flag);
}

ADCS_returnState HAL_ADCS_set_attitude_ctrl_mode(uint8_t ctrl_mode, uint16_t timeout) {
    return ADCS_set_attitude_ctrl_mode(ctrl_mode, timeout);
}

ADCS_returnState HAL_ADCS_set_attitude_estimate_mode(uint8_t mode) {
    return ADCS_set_attitude_estimate_mode(mode);
}

ADCS_returnState HAL_ADCS_trigger_adcs_loop() {
    return ADCS_trigger_adcs_loop();
}

ADCS_returnState HAL_ADCS_trigger_adcs_loop_sim(sim_sensor_data sim_data) {
    return ADCS_trigger_adcs_loop_sim(sim_data);
}

ADCS_returnState HAL_ADCS_set_ASGP4_rune_mode(uint8_t mode) {
    return ADCS_set_ASGP4_rune_mode(mode);
}

ADCS_returnState HAL_ADCS_trigger_ASGP4() {
    return ADCS_trigger_ASGP4();
}

ADCS_returnState HAL_ADCS_set_MTM_op_mode(uint8_t mode) {
    return ADCS_set_MTM_op_mode(mode);
}

ADCS_returnState HAL_ADCS_cnv2jpg(uint8_t source, uint8_t QF, uint8_t white_balance) {
    return ADCS_cnv2jpg(source, QF, white_balance);
}

ADCS_returnState HAL_ADCS_save_img(uint8_t camera, uint8_t img_size) {
    return ADCS_save_img(camera, img_size);
}

ADCS_returnState HAL_ADCS_set_magnetorquer_output(xyz16 duty_cycle) {
    return ADCS_set_magnetorquer_output(duty_cycle);
}

ADCS_returnState HAL_ADCS_set_wheel_speed(xyz16 speed) {
    return ADCS_set_wheel_speed(speed);
}

ADCS_returnState HAL_ADCS_save_config() {
    return ADCS_save_config();
}

ADCS_returnState HAL_ADCS_save_orbit_params() {
    return ADCS_save_orbit_params();
}

ADCS_returnState HAL_ADCS_get_current_state(adcs_state* data) {
    return ADCS_get_current_state(&data);
}

ADCS_returnState HAL_ADCS_get_jpg_cnv_progress(uint8_t* percentage, uint8_t* result, uint8_t* file_counter) {
    return ADCS_get_jpg_cnv_progress(&percentage, &result, &file_counter);
}

ADCS_returnState HAL_ADCS_get_cubeACP_state(uint8_t* flags_arr) {
    return ADCS_get_cubeACP_state(&flags_arr);
}

ADCS_returnState HAL_ADCS_get_execution_times(uint16_t* adcs_update,
                                          uint16_t* sensor_comms,
                                          uint16_t* sgp4_propag,
                                          uint16_t* igrf_model) {
                                              return ADCS_get_execution_times(&adcs_update, &sensor_comms, &sgp4_propag, &igrf_model);
                                          }

ADCS_returnState HAL_ADCS_get_ACP_loop_stat(uint16_t* time, uint8_t* execution_point) {
    return ADCS_get_ACP_loop_stat(&time, &execution_point);
}

ADCS_returnState HAL_ADCS_get_sat_pos_LLH(xyz* target) {
    return ADCS_get_sat_pos_LLH(&target);
}

ADCS_returnState HAL_ADCS_get_img_save_progress(uint8_t* percentage, uint8_t* status) {
    return ADCS_get_img_save_progress(&percentage, &status);
}

ADCS_returnState HAL_ADCS_get_measurements(adcs_measures* measurements) {
    return ADCS_get_measurements(&measurements);
}

ADCS_returnState HAL_ADCS_get_actuator(adcs_actuator* commands) {
    return ADCS_get_actuator(&commands);
}

ADCS_returnState HAL_ADCS_get_estimation(adcs_estimate* data) {
    return ADCS_get_estimation(&data);
}

ADCS_returnState HAL_ADCS_get_ASGP4(bool* complete, uint8_t* err, adcs_asgp4* asgp4) {
    return ADCS_get_ASGP4(&complete, &err, &asgp4);
}

ADCS_returnState HAL_ADCS_get_raw_sensor(adcs_raw_sensor* measurements) {
    return ADCS_get_raw_sensor(&measurements);
}

ADCS_returnState HAL_ADCS_get_raw_GPS(adcs_raw_gps* measurements) {
    return ADCS_get_raw_GPS(&measurements);
}

ADCS_returnState HAL_ADCS_get_star_tracker(adcs_star_track* measurements) {
    return ADCS_get_star_tracker(&measurements);
}

ADCS_returnState HAL_ADCS_get_MTM2_measurements(xyz16* Mag) {
    return ADCS_get_MTM2_measurements(&Mag);
}

ADCS_returnState HAL_ADCS_get_power_temp(adcs_pwr_temp* measurements) {
    return ADCS_get_power_temp(&measurements);
}

ADCS_returnState HAL_ADCS_set_power_control(uint8_t* control) {
    return ADCS_set_power_control(&control);
}

ADCS_returnState HAL_ADCS_get_power_control(uint8_t* control) {
    return ADCS_get_power_control(&control);
}

ADCS_returnState HAL_ADCS_set_attitude_angle(xyz att_angle) {
    return ADCS_set_attitude_angle(att_angle);
}

ADCS_returnState HAL_ADCS_get_attitude_angle(xyz* att_angle) {
    return ADCS_get_attitude_angle(&att_angle);
}

ADCS_returnState HAL_ADCS_set_track_controller(xyz target) {
    return ADCS_set_track_controller(target);
}

ADCS_returnState HAL_ADCS_get_track_controller(xyz* target) {
    return ADCS_get_track_controller(&target);
}

ADCS_returnState HAL_ADCS_set_log_config(uint8_t* flags_arr, uint16_t period, uint8_t dest, uint8_t log) {
    return ADCS_set_log_config(&flags_arr, period, dest, log);
}

ADCS_returnState HAL_ADCS_get_log_config(uint8_t* flags_arr, uint16_t* period, uint8_t* dest, uint8_t log) {
    return ADCS_get_log_config(&flags_arr, &period, &dest, log);
}

ADCS_returnState HAL_ADCS_set_inertial_ref(xyz iner_ref) {
    return ADCS_set_inertial_ref(iner_ref);
}

ADCS_returnState HAL_ADCS_get_inertial_ref(xyz* iner_ref) {
    return ADCS_get_inertial_ref(&iner_ref);
}

ADCS_returnState HAL_ADCS_set_sgp4_orbit_params(adcs_sgp4 params) {
    return ADCS_set_sgp4_orbit_params(params);
}

ADCS_returnState HAL_ADCS_get_sgp4_orbit_params(adcs_sgp4* params) {
    return ADCS_get_sgp4_orbit_params(&params);
}

ADCS_returnState HAL_ADCS_set_system_config(adcs_sysConfig config) {
    return ADCS_set_system_config(config);
}

ADCS_returnState HAL_ADCS_get_system_config(adcs_sysConfig* config) {
    return ADCS_get_system_config(&config);
}

ADCS_returnState HAL_ADCS_set_MTQ_config(xyzu8 params) {
    return ADCS_set_MTQ_config(params);
}

ADCS_returnState HAL_ADCS_set_RW_config(uint8_t* RW) {
    return ADCS_set_RW_config(&RW);
}

ADCS_returnState HAL_ADCS_set_rate_gyro(rate_gyro_config params) {
    return ADCS_set_rate_gyro(params);
}

ADCS_returnState HAL_ADCS_set_css_config(css_config config) {
    return ADCS_set_css_config(config);
}

ADCS_returnState HAL_ADCS_set_star_track_config(cubestar_config config) {
    return ADCS_set_star_track_config(config);
}

ADCS_returnState HAL_ADCS_set_cubesense_config(cubesense_config params) {
    return ADCS_set_cubesense_config(params);
}

ADCS_returnState HAL_ADCS_set_mtm_config(mtm_config params, uint8_t mtm) {
    return ADCS_set_mtm_config(params, mtm);
}

ADCS_returnState HAL_ADCS_set_detumble_config(detumble_config config) {
    return ADCS_set_detumble_config(config);
}

ADCS_returnState HAL_ADCS_set_ywheel_config(ywheel_ctrl_config params) {
    return ADCS_set_ywheel_config(params);
}

ADCS_returnState HAL_ADCS_set_rwheel_config(rwheel_ctrl_config params) {
    return ADCS_set_rwheel_config(params);
}

ADCS_returnState HAL_ADCS_set_tracking_config(track_ctrl_config params) {
    return ADCS_set_tracking_config(params);
}

ADCS_returnState HAL_ADCS_set_MoI_mat(moment_inertia_config cell) {
    return ADCS_set_MoI_mat(cell);
}

ADCS_returnState HAL_ADCS_set_estimation_config(estimation_config config) {
    return ADCS_set_estimation_config(config);
}

ADCS_returnState HAL_ADCS_set_usercoded_setting(usercoded_setting setting) {
    return ADCS_set_usercoded_setting(setting);
}

ADCS_returnState HAL_ADCS_set_asgp4_setting(aspg4_setting setting) {
    return ADCS_set_asgp4_setting(setting);
}

ADCS_returnState HAL_ADCS_get_full_config(adcs_config* config) {
    return ADCS_get_full_config(&config);
}

ADCS_returnState HAL_ADCS_getHK(ADCS_HouseKeeping* adcs_hk) {
    ADCS_returnState temp;
    ADCS_returnState return_state = 0;
    adcs_state data;
    adcs_measures mes;
    adcs_pwr_temp pwr;

    if (temp = HAL_ADCS_get_current_state(&data) != 0) {
        return_state = temp;
    } else {
        adcs_hk->Estimated_Angular_Rate = data.est_angular_rate;
        adcs_hk->Estimated_Angular_Angle = data.est_angular_rate;
        adcs_hk->Sat_Position_ECI = data.ECI_pos;
        adcs_hk->Sat_Velocity_ECI = data.ECI_vel;
        adcs_hk->ECEF_Position = data.ecef_pos;
    }
    

    if (temp = HAL_ADCS_get_measurements(&mes) != 0) {
        return_state = temp;
    } else {
        adcs_hk->Coarse_Sun_Vector = mes.coarse_sun;
        adcs_hk->Fine_Sun_Vector = mes.sun;
        adcs_hk->Nadir_Vector = mes.nadir;
        adcs_hk->Wheel_Speed = mes.wheel_speed;
        adcs_hk->Mag_Field_Vector = mes.magnetic_field;
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
        adcs_hk->Rate_Sensor_Temp = pwr.rate_sensor_temp;
    }

    if (temp = HAL_ADCS_get_sat_pos_LLH(&adcs_hk->Sat_Position_LLH) != 0) return_state = temp;
    if (temp = HAL_ADCS_get_comms_stat(&adcs_hk->Comm_Status) != 0) return_state = temp;
    
    return return_state;
}
