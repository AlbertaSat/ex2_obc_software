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
/**
 * @file adcs_handler.h
 * @author Andrew Rooney, Vasu Gupta, Arash Yazdani, Thomas Ganley, Nick Sorensen, Pundeep Hundal
 * @date 2020-08-09
 */

#ifndef ADCS_HANDLER_H
#define ADCS_HANDLER_H

#include <stdbool.h>
#include <stdint.h>

#include "adcs_types.h"
#include "adcs_io.h"
#include "redposix.h"
#include "FreeRTOS.h"

// Structs
typedef struct __attribute__((packed)) {
    float x;
    float y;
    float z;
} xyz;

typedef struct __attribute__((packed)) {
    int16_t latitude;
    int16_t longitude;
    uint16_t altitude;
} LLH;

typedef struct __attribute__((packed)) {
    int16_t x;
    int16_t y;
    int16_t z;
} xyz16;

typedef struct __attribute__((packed)){
    uint8_t x;
    uint8_t y;
    uint8_t z;
} xyzu8;

typedef struct __attribute__((packed)){
    int32_t x;
    int32_t y;
    int32_t z;
} xyz32;

typedef struct {
    int32_t pos;
    int16_t vel;
} ecef;

typedef struct {
    uint16_t x_angle;
    uint16_t y_angle;
    uint8_t busy;
    uint8_t result;
} cam_sim_sensor;

typedef struct {
    uint32_t unix_t;
    uint16_t css_raw[10];
    cam_sim_sensor cam1;
    cam_sim_sensor cam2;
    xyz16 MTM;
    xyz32 rate;
    xyz16 wheel_speed;
    xyz16 star1_cam;
    xyz16 star1_inertial;
    xyz16 star2_cam;
    xyz16 star2_inertial;
    xyz16 star3_cam;
    xyz16 star3_inertial;
    uint8_t gps_sol_stat;
    uint16_t gps_week;
    uint32_t gps_time; // ms
    ecef x;
    ecef y;
    ecef z;
    xyz pos_std_dev;
    xyzu8 vel_std_dev;
} sim_sensor_data;

typedef struct __attribute__((packed)) {
    uint8_t att_estimate_mode;
    uint8_t att_ctrl_mode;
    uint8_t run_mode;
    uint8_t ASGP4_mode;
    uint8_t flags_arr[52];
    uint8_t MTM_sample_mode;
    xyz est_angle; // est: estimated
    xyz16 est_quaternion;
    xyz est_angular_rate;
    xyz ECI_pos;
    xyz ECI_vel;
    xyz longlatalt;
    xyz16 ecef_pos;
} adcs_state;

typedef struct __attribute__((packed)) {
    xyz magnetic_field;
    xyz coarse_sun;
    xyz sun;
    xyz nadir;
    xyz angular_rate;
    xyz wheel_speed;
    xyz star1b;
    xyz star1o;
    xyz star2b;
    xyz star2o;
    xyz star3b;
    xyz star3o;
} adcs_measures;

typedef struct {
    xyz magnetorquer;
    xyz wheel_speed;
} adcs_actuator;

typedef struct {
    xyz igrf_magnetic_field;
    xyz sun;
    xyz gyro_bias;
    xyz innovation;
    xyz quaternion_err;
    xyz quaternion_covar;
    xyz angular_rate_covar;
} adcs_estimate;

typedef struct {
    float epoch;
    float inclination;
    float RAAN;
    float ECC;
    float AOP;
    float MA;
    float MM;
    float Bstar;
} adcs_asgp4;

typedef struct {
    int16_t centroid_x;
    int16_t centroid_y;
    uint8_t capture_stat;
    uint8_t detect_result;
} cam_sensor;

typedef struct __attribute__((packed)){
    cam_sensor cam1;
    cam_sensor cam2;
    uint8_t css[10];
    xyz16 MTM;
    xyz16 rate;
} adcs_raw_sensor;

typedef struct {
    uint8_t sol_stat;
    uint8_t tracked_sats;
    uint8_t usedInSol_sats;
    uint8_t xyz_lof_count;
    uint8_t range_log_count;
    uint8_t response_msg;
    uint16_t reference_week;
    uint32_t time; // ms
    ecef x;
    ecef y;
    ecef z;
    xyz pos_std_dev;
    xyzu8 vel_std_dev;
} adcs_raw_gps;

typedef struct {
    uint8_t confidence;
    uint16_t magnitude;
    uint16_t catalouge_num;
    int16_t centroid_x;
    int16_t centroid_y;
} star_data;

typedef struct {
    uint8_t detected_stars;
    uint8_t img_noise;
    uint8_t invalid_stars;
    uint8_t identified_stars;
    uint8_t identification_mode; // Table 147
    uint8_t img_dark_val;
    uint8_t flags_arr[8];
    uint16_t sample_T;
    star_data star1;
    star_data star2;
    star_data star3;
    uint16_t capture_t;
    uint16_t detect_t;
    uint16_t identification_t;
    xyz estimated_rate;
    xyz estimated_att;
} adcs_star_track;

typedef struct {
    float cubesense1_3v3_I; // I : current
    float cubesense1_camSram_I;
    float cubesense2_3v3_I;
    float cubesense2_camSram_I;
    float cubecontrol_3v3_I;
    float cubecontrol_5v_I;
    float cubecontrol_vBat_I;
    float wheel1_I;
    float wheel2_I;
    float wheel3_I;
    float cubestar_I;
    float magnetorquer_I;
    float cubestar_temp; // temperature
    float MCU_temp;
    float MTM_temp;
    float MTM2_temp;
    xyz16 rate_sensor_temp;
} adcs_pwr_temp;

typedef struct {
    double inclination;
    double ECC;   // eccentricity [deg]
    double RAAN;  // right-ascension of the ascending node
    double AOP;   // argument of perigee [deg]
    double Bstar; // B-star drag term [deg]
    double MM;    // mean motion [orbit/day]
    double MA;    // mean anomaly [deg]
    double epoch; // [year.day]
} adcs_sgp4;

typedef struct {
    xyz bias_d1;
    xyz bias_d2;
    xyz sens_s1;
    xyz sens_s2;
} mtm_biasSens;

typedef struct {
    uint8_t port; // signal enable GPIO port. Table 206
    uint8_t pin;  // signal enable GPIO port pin. Table 207
} port_pin;

typedef struct {
    uint8_t acp_type;         // Refer to table 202
    uint8_t special_ctrl_sel; // Refer to table 203
    uint8_t CC_sig_ver;       // CC: CubeControl
    uint8_t CC_motor_ver;
    uint8_t CS1_ver; // CS: CubeSense
    uint8_t CS2_ver;
    uint8_t CS1_cam; // Refer to table 204
    uint8_t CS2_cam; // Refer to table 204
    uint8_t cubeStar_ver;
    uint8_t GPS; // Refer to table 205
    bool include_MTM2;
    xyz MTQ_max_dipole;   // MTQ: magnetorquer [A.m^2]
    float MTQ_ontime_res; // [s]
    float MTQ_max_ontime; // [s]
    xyz RW_max_torque;    // [N.m]
    xyz RW_max_moment;    // [Nms]
    xyz RW_inertia;       // [kg.m^2]
    float RW_torque_inc;  // increment [N.m]
    mtm_biasSens MTM1;
    mtm_biasSens MTM2;
    port_pin CC_signal;
    port_pin CC_motor;
    port_pin CC_common;
    port_pin CS1;
    port_pin CS2;
    port_pin cubeStar;
    port_pin CW1; // CW: CubeWheel
    port_pin CW2;
    port_pin CW3;
} adcs_sysConfig;

typedef struct {
    xyzu8 gyro;
    xyz sensor_offset;
    uint8_t rate_sensor_mult;
} rate_gyro_config;

typedef struct {
    uint8_t config[10];
    float rel_scale[10];
    uint8_t threshold;
} css_config;

typedef struct {
    xyz mounting_angle; // alpha, beta, gamma
    uint16_t exposure_t;
    uint16_t analog_gain;
    uint8_t detect_th;
    uint8_t star_th;
    uint8_t max_star_matched;
    uint16_t detect_timeout_t;
    uint8_t max_pixel;
    uint8_t min_pixel;
    uint8_t err_margin; // percent
    uint16_t delay_t;
    float centroid_x;
    float centroid_y;
    float focal_len; // [mm]
    float radical_distor_ceof1;
    float radical_distor_ceof2;
    float tangent_distor_ceof1;
    float tangent_distor_ceof2;
    uint8_t window_wid;
    uint8_t track_margin; // percent
    uint8_t valid_margin; // percent
    bool module_en;
    bool loc_predict_en;
    uint8_t search_wid; //* might need to switch to float
} cubestar_config;

typedef struct {
    uint16_t min;
    uint16_t max;
} minmax;

typedef struct {
    minmax x;
    minmax y;
} area;

typedef struct {
    area area1;
    area area2;
    area area3;
    area area4;
    area area5;
} cam_area;

typedef struct {
    xyz mounting_angle; // alpha, beta, gamma
    uint8_t detect_th;
    bool auto_adjust;
    uint16_t exposure_t;
    float boresight_x; // [pixels] //* coef
    float boresight_y;
} camsensor_config;

typedef struct __attribute__((packed)){
    camsensor_config cam1_sense;
    camsensor_config cam2_sense;
    uint8_t nadir_max_deviate;
    uint8_t nadir_max_bad_edge;
    uint8_t nadir_max_radius;
    uint8_t nadir_min_radius;
    cam_area cam1_area;
    cam_area cam2_area;
} cubesense_config;

typedef struct {
    xyz mounting_angle;
    xyz channel_offset;       // 1, 2, 3
    float sensitivity_mat[9]; // Not the same order as manual!
} mtm_config;

typedef struct {
    float spin_gain;
    float damping_gain;
    float spin_rate;
    float fast_bDot;
} detumble_config;

typedef struct {
    float control_gain;
    float damping_gain;
    float proportional_gain;
    float derivative_gain;
    float reference; // [Nms]
} ywheel_ctrl_config;

typedef struct {
    float proportional_gain;
    float derivative_gain;
    float bias_moment;
    uint8_t sun_point_facet; // Refer to table 180
    bool auto_transit;
} rwheel_ctrl_config;

typedef struct {
    float proportional_gain;
    float derivative_gain;
    float integral_gain;
    uint8_t target_facet; // Refer to table 180
} track_ctrl_config;

typedef struct {
    xyz diag;
    xyz nondiag; // Ixy Ixz Iyz
} moment_inertia_config;

typedef struct {
    float MTM_rate_nosie;
    float EKF_noise;
    float CSS_noise;
    float suns_sensor_noise;
    float nadir_sensor_noise;
    float MTM_noise;
    float star_track_noise;
    uint8_t select_arr[8];
    uint8_t MTM_mode;
    uint8_t MTM_select;
    uint8_t cam_sample_period;
} estimation_config;

typedef struct {
    uint8_t controller[48];
    uint8_t estimator[48];
} usercoded_setting;

typedef struct {
    float inclination;
    float RAAN;
    float ECC;
    float AoP;
    float time;
    float pos;
    float xp;
    float yp;
} filter_coef;

typedef struct {
    float inclination;
    float RAAN;
    float ECC;
    float AoP;
    float time;
    float pos;
    float max_pos_err;
    uint8_t asgp4_filter; // Refer to table 193
    float xp;
    float yp;
    uint8_t gps_rollover;
    float pos_sd;
    float vel_sd;
    uint8_t min_sat;
    float time_gain;
    float max_lag;
    uint16_t min_samples;
} aspg4_setting;

typedef struct {
    xyzu8 MTQ;
    uint8_t RW[4];
    rate_gyro_config rate_gyro;
    css_config css;
    cubesense_config cubesense;
    mtm_config MTM1;
    mtm_config MTM2;
    cubestar_config star_tracker;
    detumble_config detumble;
    ywheel_ctrl_config ywheel;
    rwheel_ctrl_config rwheel;
    track_ctrl_config tracking;
    moment_inertia_config MoI;
    estimation_config estimation;
    aspg4_setting aspg4;
    usercoded_setting usercoded;
} adcs_config;

typedef struct {
    uint8_t type;
    uint8_t counter;
    bool updating;
    uint32_t size;
    uint32_t time;
    uint16_t crc16_checksum;
} adcs_file_info;

typedef struct __attribute__((packed)) {
    uint8_t type;
    uint8_t counter;
    uint32_t size;
    char file_name[REDCONF_NAME_MAX];
} adcs_file_download_id;

// General functions
int16_t uint82int16(uint8_t b1, uint8_t b2);
int32_t uint82int32(uint8_t *address);
uint16_t uint82uint16(uint8_t b1, uint8_t b2);
void get_xyz(xyz *measurement, uint8_t *address, float coef);
void get_xyz16(xyz16 *measurement, uint8_t *address);
void get_3x3(float *matrix, uint8_t *address, float coef);

// send_telecommand
ADCS_returnState adcs_telecommand(uint8_t *command, uint32_t length);
ADCS_returnState adcs_telemetry(uint8_t TM_ID, uint8_t *reply, uint32_t length);

// File management TC/TM sequences
ADCS_returnState ADCS_init_file_download_mutex(void);
ADCS_returnState ADCS_get_file_list(void);
ADCS_returnState ADCS_download_file(uint8_t type, uint8_t counter, uint32_t size, char *save_as);
void ADCS_download_file_task(void *pvParameters);

// Common Telecommands
ADCS_returnState ADCS_reset(void);
ADCS_returnState ADCS_reset_log_pointer(void);
ADCS_returnState ADCS_advance_log_pointer(void);
ADCS_returnState ADCS_reset_boot_registers(void);
ADCS_returnState ADCS_format_sd_card(void);
ADCS_returnState ADCS_erase_file(uint8_t file_type, uint8_t file_counter, bool erase_all);
ADCS_returnState ADCS_load_file_download_block(uint8_t file_type, uint8_t counter, uint32_t offset,
                                               uint16_t block_length);
ADCS_returnState ADCS_advance_file_list_read_pointer(void);
ADCS_returnState ADCS_initiate_file_upload(uint8_t file_dest, uint8_t block_size);
ADCS_returnState ADCS_file_upload_packet(uint16_t packet_number, char *file_bytes);
ADCS_returnState ADCS_finalize_upload_block(uint8_t file_dest, uint32_t offset, uint16_t block_length);
ADCS_returnState ADCS_reset_upload_block(void);
ADCS_returnState ADCS_reset_file_list_read_pointer(void);
ADCS_returnState ADCS_initiate_download_burst(uint8_t msg_length, bool ignore_hole_map);

// Common Telemetry
ADCS_returnState ADCS_get_node_identification(uint8_t *node_type, uint8_t *interface_ver, uint8_t *major_firm_ver,
                                              uint8_t *minor_firm_ver, uint16_t *runtime_s, uint16_t *runtime_ms);
ADCS_returnState ADCS_get_boot_program_stat(uint8_t *mcu_reset_cause, uint8_t *boot_cause, uint16_t *boot_count,
                                            uint8_t *boot_idx, uint8_t *major_firm_ver, uint8_t *minor_firm_ver);
ADCS_returnState ADCS_get_boot_index(uint8_t *program_idx, uint8_t *boot_stat);
ADCS_returnState ADCS_get_last_logged_event(uint32_t *time, uint8_t *event_id, uint8_t *event_param);
ADCS_returnState ADCS_get_SD_format_progress(bool *format_busy, bool *erase_all_busy);
ADCS_returnState ADCS_get_TC_ack(uint8_t *last_tc_id, bool *tc_processed, ADCS_returnState *tc_err_stat,
                                 uint8_t *tc_err_idx);
ADCS_returnState ADCS_get_file_download_buffer(uint16_t *packet_count, uint8_t file[20]);
ADCS_returnState ADCS_get_file_download_block_stat(bool *ready, bool *param_err, uint16_t *crc16_checksum,
                                                   uint16_t *length);
ADCS_returnState ADCS_get_file_info(adcs_file_info *info);
ADCS_returnState ADCS_get_init_upload_stat(bool *busy);
ADCS_returnState ADCS_get_finalize_upload_stat(bool *busy, bool *err);
ADCS_returnState ADCS_get_upload_crc16_checksum(uint16_t *checksum);
ADCS_returnState ADCS_get_SRAM_latchup_count(uint16_t *sram1, uint16_t *sram2);
ADCS_returnState ADCS_get_EDAC_err_count(uint16_t *single_sram, uint16_t *double_sram, uint16_t *multi_sram);
ADCS_returnState ADCS_get_comms_stat(uint16_t *TC_num, uint16_t *TM_num, uint8_t *flags_arr);

// Common Config Msgs
ADCS_returnState ADCS_set_cache_en_state(bool en_state);
ADCS_returnState ADCS_set_sram_scrub_size(uint16_t size);
ADCS_returnState ADCS_set_UnixTime_save_config(uint8_t when, uint8_t period);
ADCS_returnState ADCS_set_hole_map(uint8_t *hole_map, uint8_t num);
ADCS_returnState ADCS_set_unix_t(uint32_t unix_t, uint16_t count_ms);
ADCS_returnState ADCS_get_cache_en_state(bool *en_state);
ADCS_returnState ADCS_get_sram_scrub_size(uint16_t *size);
ADCS_returnState ADCS_get_UnixTime_save_config(uint8_t *when, uint8_t *period);
ADCS_returnState ADCS_get_hole_map(uint8_t *hole_map, uint8_t num);
ADCS_returnState ADCS_get_unix_t(uint32_t *unix_t, uint16_t *count_ms);

// BootLoader Telecommands
ADCS_returnState ADCS_clear_err_flags(void);
ADCS_returnState ADCS_set_boot_index(uint8_t index);
ADCS_returnState ADCS_run_selected_program(void);
ADCS_returnState ADCS_read_program_info(uint8_t index);
ADCS_returnState ADCS_copy_program_internal_flash(uint8_t index, uint8_t overwrite_flag);

// BootLoader Telemetries
ADCS_returnState ADCS_get_bootloader_state(uint16_t *uptime, uint8_t *flags_arr);
ADCS_returnState ADCS_get_program_info(uint8_t *index, bool *busy, uint32_t *file_size, uint16_t *crc16_checksum);
ADCS_returnState ADCS_copy_internal_flash_progress(bool *busy, bool *err);

// ACP Telecommands
ADCS_returnState ADCS_deploy_magnetometer_boom(uint8_t actuation_timeout);
ADCS_returnState ADCS_set_enabled_state(uint8_t state);
ADCS_returnState ADCS_clear_latched_errs(bool adcs_flag, bool hk_flag);
ADCS_returnState ADCS_set_attitude_ctrl_mode(uint8_t ctrl_mode, uint16_t timeout);
ADCS_returnState ADCS_set_attitude_estimate_mode(uint8_t mode);
ADCS_returnState ADCS_trigger_adcs_loop(void);
ADCS_returnState ADCS_trigger_adcs_loop_sim(sim_sensor_data sim_data);
ADCS_returnState ADCS_set_ASGP4_rune_mode(uint8_t mode);
ADCS_returnState ADCS_trigger_ASGP4(void);
ADCS_returnState ADCS_set_MTM_op_mode(uint8_t mode);
ADCS_returnState ADCS_cnv2jpg(uint8_t source, uint8_t QF, uint8_t white_balance);
ADCS_returnState ADCS_save_img(uint8_t camera, uint8_t img_size);
ADCS_returnState ADCS_set_magnetorquer_output(xyz16 duty_cycle);
ADCS_returnState ADCS_set_wheel_speed(xyz16 speed);
ADCS_returnState ADCS_save_config(void);
ADCS_returnState ADCS_save_orbit_params(void);

// ACP Telemetry
ADCS_returnState ADCS_get_current_state(adcs_state *data);

ADCS_returnState ADCS_get_jpg_cnv_progress(uint8_t *percentage, uint8_t *result, uint8_t *file_counter);
ADCS_returnState ADCS_get_cubeACP_state(uint8_t *flags_arr);
ADCS_returnState ADCS_get_sat_pos_LLH(LLH *target);
ADCS_returnState ADCS_get_execution_times(uint16_t *adcs_update, uint16_t *sensor_comms, uint16_t *sgp4_propag,
                                          uint16_t *igrf_model);
ADCS_returnState ADCS_get_ACP_loop_stat(uint16_t *time, uint8_t *execution_point);
ADCS_returnState ADCS_get_img_save_progress(uint8_t *percentage, uint8_t *status);

ADCS_returnState ADCS_get_measurements(adcs_measures *measurements);
ADCS_returnState ADCS_get_actuator(adcs_actuator *commands);
ADCS_returnState ADCS_get_estimation(adcs_estimate *data);
ADCS_returnState ADCS_get_ASGP4(bool *complete, uint8_t *err, adcs_asgp4 *asgp4);
void get_cam_sensor(cam_sensor *cam, uint8_t *address);
ADCS_returnState ADCS_get_raw_sensor(adcs_raw_sensor *measurements);
void get_ecef(ecef *coordinate, uint8_t *address);
ADCS_returnState ADCS_get_raw_GPS(adcs_raw_gps *measurements);
void get_star_data(star_data *coordinate, uint8_t *address, uint8_t i);
ADCS_returnState ADCS_get_star_tracker(adcs_star_track *measurements);
ADCS_returnState ADCS_get_MTM2_measurements(xyz16 *Mag);
void get_current(float *measurement, uint16_t raw, float coef);
void get_temp(float *measurement, int16_t raw, float coef);
ADCS_returnState ADCS_get_power_temp(adcs_pwr_temp *measurements);

// ACP Config Msgs
ADCS_returnState ADCS_set_power_control(uint8_t *control);
ADCS_returnState ADCS_get_power_control(uint8_t *control);
ADCS_returnState ADCS_set_attitude_angle(xyz att_angle);
ADCS_returnState ADCS_get_attitude_angle(xyz *att_angle);
ADCS_returnState ADCS_set_track_controller(xyz target);
ADCS_returnState ADCS_get_track_controller(xyz *target);
ADCS_returnState ADCS_set_log_config(uint8_t *flags_arr, uint16_t period, uint8_t dest, uint8_t log);
ADCS_returnState ADCS_get_log_config(uint8_t *flags_arr, uint16_t *period, uint8_t *dest, uint8_t log);
ADCS_returnState ADCS_set_inertial_ref(xyz iner_ref);
ADCS_returnState ADCS_get_inertial_ref(xyz *iner_ref);

ADCS_returnState ADCS_set_sgp4_orbit_params(adcs_sgp4 params);
ADCS_returnState ADCS_get_sgp4_orbit_params(adcs_sgp4 *params);
ADCS_returnState ADCS_set_system_config(adcs_sysConfig config);
ADCS_returnState ADCS_get_system_config(adcs_sysConfig *config);
ADCS_returnState ADCS_set_MTQ_config(xyzu8 params);
ADCS_returnState ADCS_set_RW_config(uint8_t *RW);
ADCS_returnState ADCS_set_rate_gyro(rate_gyro_config params);
ADCS_returnState ADCS_set_css_config(css_config config);
ADCS_returnState ADCS_set_star_track_config(cubestar_config config);
ADCS_returnState ADCS_get_cubesense_config(cubesense_config *config);
ADCS_returnState ADCS_set_cubesense_config(cubesense_config params);
ADCS_returnState ADCS_set_mtm_config(mtm_config params, uint8_t mtm);
ADCS_returnState ADCS_set_detumble_config(detumble_config config);
ADCS_returnState ADCS_set_ywheel_config(ywheel_ctrl_config params);
ADCS_returnState ADCS_set_rwheel_config(rwheel_ctrl_config params);
ADCS_returnState ADCS_set_tracking_config(track_ctrl_config params);
ADCS_returnState ADCS_set_MoI_mat(moment_inertia_config cell);
ADCS_returnState ADCS_set_estimation_config(estimation_config config);
ADCS_returnState ADCS_set_usercoded_setting(usercoded_setting setting);
ADCS_returnState ADCS_set_asgp4_setting(aspg4_setting setting);
ADCS_returnState ADCS_get_full_config(adcs_config *config);

#endif /* ADCS_HANDLER_H */
