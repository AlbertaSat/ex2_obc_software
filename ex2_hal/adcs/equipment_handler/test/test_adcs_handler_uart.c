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

// Make sure to define "USE_I2C" in adcs_handler.c to pass tests

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "adcs_handler.h"
#include "adcs_io.h"
#include "mock_uart_i2c.h"
#include "unity.h"

uint8_t TC_ID_array[] = {1,   4,   5,   6,   33,  108, 112,
                         113, 114, 115, 116, 117, 118, 119};

void setUp(void) {}

void tearDown(void) {}

void test_ADCS_common_commands(void) {
  // Same for all uart TC replies
  uint8_t reply[6];
  reply[0] = ADCS_ESC_CHAR;
  reply[1] = ADCS_SOM;
  reply[2] = LOAD_FILE_DOWNLOAD_BLOCK_ID;
  reply[3] = ADCS_OK;
  reply[4] = ADCS_ESC_CHAR;
  reply[5] = ADCS_EOM;

  // load file
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 6);
  TEST_ASSERT_EQUAL_INT(ADCS_OK,
                        ADCS_load_file_download_block(2, 17, 6324, 145));

  // file upload
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 6);
  TEST_ASSERT_EQUAL_INT(ADCS_OK,
                        ADCS_file_upload_packet(145, "This is test file"));
}

void test_ADCS_BootLoader_telecommands(void) {
  // Same for all uart TC replies
  uint8_t reply[6];
  reply[0] = ADCS_ESC_CHAR;
  reply[1] = ADCS_SOM;
  reply[2] = CLEAR_ERR_FLAGS_ID;
  reply[3] = ADCS_OK;
  reply[4] = ADCS_ESC_CHAR;
  reply[5] = ADCS_EOM;

  // Doesn't matter what we put in frame!
  uint8_t *frame = NULL;
  frame = realloc(frame, sizeof(*frame) * 5);
  frame[0] = ADCS_ESC_CHAR;
  frame[1] = ADCS_SOM;
  frame[2] = CLEAR_ERR_FLAGS_ID;
  frame[3] = ADCS_ESC_CHAR;
  frame[4] = ADCS_EOM;
  uart_send_Expect(frame, 5);
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 6);
  TEST_ASSERT_EQUAL_INT(ADCS_OK, ADCS_clear_err_flags());

  // Set boot index
  frame = realloc(frame, sizeof(*frame) * 6);
  frame[0] = ADCS_ESC_CHAR;
  frame[1] = ADCS_SOM;
  frame[2] = SET_BOOT_INDEX_ID;
  frame[3] = 1;
  frame[4] = ADCS_ESC_CHAR;
  frame[5] = ADCS_EOM;
  uart_send_Expect(frame, 6);
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 6);
  TEST_ASSERT_EQUAL_INT(ADCS_OK,
                        ADCS_set_boot_index(1));  // only argument 1 passes

  free(frame);
}

void test_ADCS_BootLoader_telemetries(void) {
  // BootLoader state
  uint8_t request[5];
  request[0] = ADCS_ESC_CHAR;
  request[1] = ADCS_SOM;
  request[2] = GET_BOOTLOADER_STATE_ID;
  request[3] = ADCS_ESC_CHAR;
  request[4] = ADCS_EOM;
  uint8_t *reply = NULL;

  uint16_t uptime = 7785;
  uint32_t flags = 0x0127;  // hex presentation to see it better
  reply = realloc(reply, sizeof(*reply) * 11);
  reply[0] = ADCS_ESC_CHAR;
  reply[1] = ADCS_SOM;
  reply[2] = GET_BOOTLOADER_STATE_ID;
  memcpy(&reply[3], &uptime, 2);
  memcpy(&reply[5], &flags, 4);
  reply[9] = ADCS_ESC_CHAR;
  reply[10] = ADCS_EOM;

  uint16_t uptime_f;
  uint8_t flags_arr[11];
  uart_send_Expect(request, 5);
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 11);
  ADCS_get_bootloader_state(&uptime_f, flags_arr);
  TEST_ASSERT_EQUAL_UINT16(uptime_f, uptime);
  uint8_t flags_f[11] = {1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0};  // 7 2 1
  TEST_ASSERT_EQUAL_UINT8_ARRAY(flags_arr, flags_f, 11);

  // Program info
  uint8_t index = 5;
  uint8_t busy = 1;
  uint32_t file_size = 14569;
  uint16_t crc16_checksum = 3172;
  reply = realloc(reply, sizeof(*reply) * 13);
  reply[0] = ADCS_ESC_CHAR;
  reply[1] = ADCS_SOM;
  reply[2] = GET_PROGRAM_INFO_ID;
  reply[3] = index;
  reply[4] = busy;
  memcpy(&reply[5], &file_size, 4);
  memcpy(&reply[9], &crc16_checksum, 2);
  reply[11] = ADCS_ESC_CHAR;
  reply[12] = ADCS_EOM;
  uint8_t index_f;
  bool busy_f;
  uint32_t file_size_f;
  uint16_t crc16_checksum_f;
  request[2] = GET_PROGRAM_INFO_ID;
  uart_send_Expect(request, 5);
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 13);
  ADCS_get_program_info(&index_f, &busy_f, &file_size_f, &crc16_checksum_f);
  TEST_ASSERT_EQUAL_UINT8(index, index_f);
  TEST_ASSERT_EQUAL_INT(1, busy_f);
  TEST_ASSERT_EQUAL_UINT32(file_size, file_size_f);
  TEST_ASSERT_EQUAL_UINT16(crc16_checksum, crc16_checksum_f);

  free(reply);
}

void test_ADCS_ACP_telecommands(void) {
  // Same for all uart TC replies
  uint8_t reply[6];
  reply[0] = ADCS_ESC_CHAR;
  reply[1] = ADCS_SOM;
  reply[2] = CLEAR_LATCHED_ERRS_ID;
  reply[3] = ADCS_OK;
  reply[4] = ADCS_ESC_CHAR;
  reply[5] = ADCS_EOM;

  // Clear latched errors
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 6);
  TEST_ASSERT_EQUAL_INT(ADCS_OK, ADCS_clear_latched_errs(1, 1));

  // Set attitude control
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 6);
  TEST_ASSERT_EQUAL_INT(ADCS_OK, ADCS_set_attitude_ctrl_mode(14, 6589));

  // set magnetorquer output
  xyz16 mtq_out;
  mtq_out.x = 630;
  mtq_out.y = 786;
  mtq_out.z = 912;
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 6);
  TEST_ASSERT_EQUAL_INT(ADCS_OK, ADCS_set_magnetorquer_output(mtq_out));

  // Set simulated sensor data
  sim_sensor_data data;
  data.css_raw[5] = 124;
  data.pos_std_dev.y = 1.1;
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 6);
  TEST_ASSERT_EQUAL_INT(ADCS_OK, ADCS_trigger_adcs_loop_sim(data));
}

void test_ADCS_ACP_telemetries(void) {
  uint8_t *reply = NULL;

  // attitude ctrl
  uint8_t estimate = 4;
  uint8_t ctrl = 4;
  uint8_t run = 1;
  uint8_t aspg4 = 3;
  uint32_t flags1 = 0x051FC327;
  uint32_t flags2 = 0x15E71;  //  : MTM_sample_mode = 3
  reply = realloc(reply, sizeof(*reply) * 59);
  reply[0] = ADCS_ESC_CHAR;
  reply[1] = ADCS_SOM;
  reply[2] = ADCS_STATE;
  reply[3] = estimate | (ctrl << 4);
  reply[4] = run | (aspg4 << 2) | 0xA0;  // 1 0 1 0 flags
  memcpy(&reply[5], &flags1, 4);
  memcpy(&reply[9], &flags2, 4);
  int16_t anglex = 45;
  int16_t anglez = -9998;
  memcpy(&reply[15], &anglex, 2);
  memcpy(&reply[19], &anglez, 2);
  int16_t qy = -8745;
  memcpy(&reply[23], &qy, 2);
  int16_t latitude = 1001;
  int16_t longitude = -478;
  uint16_t altitude = 45000;  // make sure the 8th bit is 1
  memcpy(&reply[45], &latitude, 2);
  memcpy(&reply[47], &longitude, 2);
  memcpy(&reply[49], &altitude, 2);
  reply[57] = ADCS_ESC_CHAR;
  reply[58] = ADCS_EOM;
  adcs_state data;
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 59);
  ADCS_get_current_state(&data);
  TEST_ASSERT_EQUAL_UINT8(data.att_estimate_mode, estimate);
  TEST_ASSERT_EQUAL_UINT8(data.att_ctrl_mode, ctrl);
  TEST_ASSERT_EQUAL_UINT8(data.run_mode, run);
  TEST_ASSERT_EQUAL_UINT8(data.ASGP4_mode, aspg4);
  uint8_t flags_f[52] = {0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0,
                         1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
                         1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 0};
  // A723CF150	0x16E7(2LSB)1
  TEST_ASSERT_EQUAL_UINT8_ARRAY(data.flags_arr, flags_f, 36);
  TEST_ASSERT_EQUAL_UINT8(data.MTM_sample_mode, 3);
  TEST_ASSERT_FLOAT_WITHIN(0.001, data.est_angle.x, anglex * 0.01);
  TEST_ASSERT_FLOAT_WITHIN(0.001, data.est_angle.z, anglez * 0.01);
  TEST_ASSERT_EQUAL_UINT16(data.est_quaternion.y, qy);
  TEST_ASSERT_FLOAT_WITHIN(0.001, data.longlatalt.x, latitude * 0.01);
  TEST_ASSERT_FLOAT_WITHIN(0.001, data.longlatalt.y, longitude * 0.01);
  TEST_ASSERT_FLOAT_WITHIN(0.001, data.longlatalt.z, altitude * 0.01);

  // measurements
  adcs_measures measurements;
  int16_t x = 2;
  int16_t y = 2;
  int16_t z = 2;
  reply = realloc(reply, sizeof(*reply) * 77);
  reply[0] = ADCS_ESC_CHAR;
  reply[1] = ADCS_SOM;
  reply[2] = ADCS_MEASUREMENTS_ID;
  memcpy(&reply[3], &x, 2);
  memcpy(&reply[5], &y, 2);
  memcpy(&reply[7], &z, 2);
  reply[75] = ADCS_ESC_CHAR;
  reply[76] = ADCS_EOM;
  adcs_measures measurements_f;
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 77);
  ADCS_get_measurements(&measurements_f);
  TEST_ASSERT_FLOAT_WITHIN(0.001, measurements_f.magnetic_field.z, z * 0.01);

  // ASGP4
  uint8_t err = 3;
  uint8_t err_f;
  bool complete = 1;
  bool complete_f;
  float epoch = 8.1;
  adcs_asgp4 asgp4_f;
  reply = realloc(reply, sizeof(*reply) * 38);
  reply[0] = ADCS_ESC_CHAR;
  reply[1] = ADCS_SOM;
  reply[2] = ASGP4_TLEs_ID;
  reply[3] = err << 1 | complete;
  memcpy(&reply[4], &epoch, 4);
  reply[36] = ADCS_ESC_CHAR;
  reply[37] = ADCS_EOM;
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 38);
  ADCS_get_ASGP4(&complete_f, &err_f, &asgp4_f);
  TEST_ASSERT_EQUAL_UINT8(err, err_f);
  TEST_ASSERT_EQUAL_INT(complete, complete_f);
  TEST_ASSERT_FLOAT_WITHIN(0.001, asgp4_f.epoch, epoch);

  // raw measurement
  adcs_raw_sensor raw_measurements;
  reply = realloc(reply, sizeof(*reply) * 39);
  reply[0] = ADCS_ESC_CHAR;
  reply[1] = ADCS_SOM;
  reply[2] = RAW_SENSOR_MEASUREMENTS_ID;
  uint8_t cam1detect = 87;
  reply[8] = cam1detect;  // cam1detect
  int16_t cam2x = 874;
  memcpy(&reply[9], &cam2x, 2);
  reply[15] = 2;
  reply[16] = 3;
  reply[17] = 4;
  int16_t raw_x = -2;
  int16_t raw_y = -3;
  int16_t raw_z = -4;
  memcpy(&reply[25], &raw_x, 2);
  memcpy(&reply[27], &raw_y, 2);
  memcpy(&reply[29], &raw_z, 2);
  reply[37] = ADCS_ESC_CHAR;
  reply[38] = ADCS_EOM;
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 39);
  ADCS_get_raw_sensor(&raw_measurements);
  TEST_ASSERT_EQUAL_UINT8(raw_measurements.cam1.detect_result, cam1detect);
  TEST_ASSERT_EQUAL_INT16(raw_measurements.cam2.centroid_x, cam2x);
  TEST_ASSERT_EQUAL_UINT8(raw_measurements.css[1], 3);
  TEST_ASSERT_EQUAL_INT16(raw_measurements.MTM.z, -4);

  // raw GPS
  adcs_raw_gps gps_measurements;
  reply = realloc(reply, sizeof(*reply) * 41);
  reply[0] = ADCS_ESC_CHAR;
  reply[1] = ADCS_SOM;
  reply[2] = RAW_GPS_MEASUREMENTS_ID;
  uint8_t sol_stat = 1;
  reply[3] = sol_stat;
  uint16_t week = 546;
  memcpy(&reply[9], &week, 2);
  uint32_t time = 547862;
  memcpy(&reply[11], &time, 4);
  ecef ecef_y;
  ecef_y.pos = 600325;
  ecef_y.vel = 174;
  memcpy(&reply[21], &ecef_y.pos, 4);
  memcpy(&reply[25], &ecef_y.vel, 2);
  uint8_t posstddevz = 2;
  reply[35] = posstddevz;
  uint8_t velstddevx = 3;
  reply[36] = velstddevx;
  reply[39] = ADCS_ESC_CHAR;
  reply[40] = ADCS_EOM;

  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 41);
  ADCS_get_raw_GPS(&gps_measurements);
  TEST_ASSERT_EQUAL_UINT8(gps_measurements.sol_stat, sol_stat);
  TEST_ASSERT_EQUAL_UINT16(gps_measurements.reference_week, week);
  TEST_ASSERT_EQUAL_UINT32(gps_measurements.time, time);
  TEST_ASSERT_EQUAL_UINT32(gps_measurements.y.pos, ecef_y.pos);
  TEST_ASSERT_EQUAL_UINT16(gps_measurements.y.vel, ecef_y.vel);
  TEST_ASSERT_FLOAT_WITHIN(0.001, gps_measurements.pos_std_dev.z,
                           0.1 * posstddevz);
  TEST_ASSERT_EQUAL_UINT8(gps_measurements.vel_std_dev.x, velstddevx);

  // star tracker
  adcs_star_track star_tracker;
  reply = realloc(reply, sizeof(*reply) * 59);
  reply[0] = ADCS_ESC_CHAR;
  reply[1] = ADCS_SOM;
  reply[2] = RAW_STAR_TRACKER_ID;
  uint8_t flags = 0x5B;
  reply[9] = flags;
  uint8_t flags_star[11] = {1, 1, 0, 1, 1, 0, 1, 0};  // B 5
  star_data star_y;
  star_y.confidence = 54;
  star_y.magnitude = 36;
  star_y.catalouge_num = 17;
  star_y.centroid_x = -98;
  star_y.centroid_y = -2300;
  reply[13] = star_y.confidence;
  memcpy(&reply[17], &star_y.magnitude, 2);
  memcpy(&reply[27], &star_y.catalouge_num, 2);
  memcpy(&reply[29], &star_y.centroid_x, 2);
  memcpy(&reply[31], &star_y.centroid_y, 2);
  uint16_t capture = 11456;
  memcpy(&reply[39], &capture, 2);
  int16_t att_z = -587;
  memcpy(&reply[55], &att_z, 2);
  reply[57] = ADCS_ESC_CHAR;
  reply[58] = ADCS_EOM;

  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 59);
  ADCS_get_star_tracker(&star_tracker);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(star_tracker.flags_arr, flags_star, 8);
  TEST_ASSERT_EQUAL_UINT8(star_tracker.star2.confidence, star_y.confidence);
  TEST_ASSERT_EQUAL_UINT16(star_tracker.star2.magnitude, star_y.magnitude);
  TEST_ASSERT_EQUAL_UINT16(star_tracker.star2.catalouge_num,
                           star_y.catalouge_num);
  TEST_ASSERT_EQUAL_INT16(star_tracker.star2.centroid_x, star_y.centroid_x);
  TEST_ASSERT_EQUAL_INT16(star_tracker.star2.centroid_y, star_y.centroid_y);
  TEST_ASSERT_EQUAL_UINT16(star_tracker.capture_t, capture);
  TEST_ASSERT_FLOAT_WITHIN(0.0001, star_tracker.estimated_att.z,
                           0.0001 * att_z);

  // power_temp
  adcs_pwr_temp pwr_temp;
  reply = realloc(reply, sizeof(*reply) * 43);
  reply[0] = ADCS_ESC_CHAR;
  reply[1] = ADCS_SOM;
  reply[2] = POWER_TEMP_ID;
  uint16_t current = 513;
  memcpy(&reply[11], &current, 2);
  int16_t temp = -37;
  memcpy(&reply[29], &temp, 2);
  reply[41] = ADCS_ESC_CHAR;
  reply[42] = ADCS_EOM;
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 43);
  ADCS_get_power_temp(&pwr_temp);

  TEST_ASSERT_FLOAT_WITHIN(0.000000001, pwr_temp.cubecontrol_3v3_I,
                           0.48828125 * current);
  TEST_ASSERT_FLOAT_WITHIN(0.0001, pwr_temp.MTM_temp, 0.1 * temp);

  free(reply);
}

void test_uint82int16(void) {
  // test uint82int16
  int16_t b16 = -189;
  uint8_t b12[2];
  b12[1] = b16 >> 8;
  b12[0] = b16 & 0xFF;
  //  memcpy(&b12[0], &b16, 2);
  int16_t b = uint82int16(b12[0], b12[1]);
  TEST_ASSERT_FLOAT_WITHIN(0.1, b16, b);

  // test get_xyz
  xyz meas;
  meas.x = -18.9;
  xyz meas_f;
  get_xyz(&meas_f, b12, 0.1);
  TEST_ASSERT_FLOAT_WITHIN(0.1, meas.x, meas_f.x);

  // test uint82int32
  int32_t b32 = -18976;
  uint8_t arr[4];
  arr[3] = b32 >> 24;
  arr[2] = (b32 >> 16) & 0xFF;
  arr[1] = (b32 >> 8) & 0xFF;
  arr[0] = b32 & 0xFF;
  int32_t b_f = uint82int32(arr);
  TEST_ASSERT_EQUAL_INT32(b32, b_f);
}

void test_get_3x3(void) {
  int16_t mat[9] = {-132, 35, 471, 203, 14, 87, -148, -1, 1784};
  uint8_t frame[18];
  memcpy(&frame[0], &mat[0], 18);
  adcs_config config;
  get_3x3(&config.MTM1.sensitivity_mat[0], &frame[0], 1);
  TEST_ASSERT_FLOAT_WITHIN(0.1, config.MTM1.sensitivity_mat[0], mat[0]);
  TEST_ASSERT_FLOAT_WITHIN(0.1, config.MTM1.sensitivity_mat[3], mat[5]);
}

void test_ADCS_Common_configMsgs(void) {
  uint8_t reply[6];
  reply[0] = ADCS_ESC_CHAR;
  reply[1] = ADCS_SOM;
  reply[2] = SET_HOLE_MAP_ID + 4;
  reply[3] = ADCS_OK;
  reply[4] = ADCS_ESC_CHAR;
  reply[5] = ADCS_EOM;

  // Set Hole map
  uint8_t hole_map[16];
  hole_map[8] = 17;
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 6);
  TEST_ASSERT_EQUAL_UINT(ADCS_OK, ADCS_set_hole_map(hole_map, 4));
}

void test_ADCS_ACP_set_configMsgs(void) {
  uint8_t reply[6];
  reply[0] = ADCS_ESC_CHAR;
  reply[1] = ADCS_SOM;
  reply[2] = SET_POWER_CONTROL_ID;
  reply[3] = ADCS_OK;
  reply[4] = ADCS_ESC_CHAR;
  reply[5] = ADCS_EOM;

  // Set power control
  uint8_t control[10] = {0};
  control[0] = 1;
  control[1] = 2;
  control[3] = 2;
  // 10001001 = 137
  control[8] = 1;
  control[9] = 2;
  // 1001 = 9
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 6);
  TEST_ASSERT_EQUAL_INT(ADCS_OK, ADCS_set_power_control(control));

  // Set att angle
  xyz angle;
  angle.x = 42.2;
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 6);
  TEST_ASSERT_EQUAL_INT(ADCS_OK, ADCS_set_attitude_angle(angle));

  // Set log
  uint8_t flags[80] = {0};
  flags[57] = 1;
  flags[59] = 1;
  uint16_t period = 2560;
  uint8_t dest = 1;
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 6);
  TEST_ASSERT_EQUAL_INT(ADCS_OK, ADCS_set_log_config(flags, period, dest, 1));

  // Set SGP4
  adcs_sgp4 sgp4;
  sgp4.ECC = 689520.126;
  sgp4.epoch = -7125630.6987;
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 6);
  TEST_ASSERT_EQUAL_INT(ADCS_OK, ADCS_set_sgp4_orbit_params(sgp4));

  // Set system config
  adcs_sysConfig sys_config;
  sys_config.include_MTM2 = 1;
  sys_config.CC_signal.pin = 7;
  sys_config.CC_signal.port = 3;
  sys_config.GPS = 2;
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 6);
  TEST_ASSERT_EQUAL_INT(ADCS_OK, ADCS_set_system_config(sys_config));

  // Set MTM
  mtm_config params;
  params.sensitivity_mat[0] = 14.3;
  params.sensitivity_mat[1] = 27.03;
  params.sensitivity_mat[4] = 6.6;
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 6);
  TEST_ASSERT_EQUAL_INT(ADCS_OK, ADCS_set_mtm_config(params, 1));

  // Set estimation
  estimation_config estimate;
  uint8_t arr[7] = {1, 0, 0, 1, 1, 0, 1};
  memcpy(&estimate.select_arr, &arr, 7);
  estimate.MTM_mode = 2;
  estimate.MTM_select = 3;
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 6);
  TEST_ASSERT_EQUAL_INT(ADCS_OK, ADCS_set_estimation_config(estimate));
}

void test_ADCS_ACP_get_configMsgs(void) {
  uint8_t *reply = NULL;

  // Get power control
  reply = realloc(reply, sizeof(*reply) * 8);
  reply[0] = ADCS_ESC_CHAR;
  reply[1] = ADCS_SOM;
  reply[2] = GET_POWER_CONTROL_ID;
  reply[3] = 137;
  reply[4] = 72;
  reply[5] = 9;
  reply[6] = ADCS_ESC_CHAR;
  reply[7] = ADCS_EOM;

  uint8_t control[10];
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 8);
  ADCS_get_power_control(control);
  uint8_t control_f[10] = {1, 2, 0, 2, 0, 2,
                           0, 1, 1, 2};  // 137(10001001) 72(01001000) 9(1001)
  TEST_ASSERT_EQUAL_UINT8_ARRAY(control, control_f, 10);

  // Get log
  uint8_t log = 2;
  uint16_t period = 2413;
  uint8_t dest = 1;
  uint8_t flags[10] = {0};
  reply = realloc(reply, sizeof(*reply) * 18);
  reply[0] = ADCS_ESC_CHAR;
  reply[1] = ADCS_SOM;
  reply[2] = GET_SD_LOG1_CONFIG_ID + log - 1;
  memcpy(&reply[3], &flags[0], 10);
  reply[4] = 65;  // 01000001
  memcpy(&reply[13], &period, 2);
  reply[15] = dest;
  reply[16] = ADCS_ESC_CHAR;
  reply[17] = ADCS_EOM;
  uint16_t period_f;
  uint8_t dest_f;
  uint8_t flags_arr[80] = {0};
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 18);
  ADCS_get_log_config(flags_arr, &period_f, &dest_f, log);
  TEST_ASSERT_EQUAL_UINT8(dest, dest_f);
  TEST_ASSERT_EQUAL_UINT16(period, period_f);
  TEST_ASSERT_EQUAL_UINT8(flags_arr[8] + flags_arr[14], 2);

  // Get SGP4
  double MA = -8956.214;
  uint8_t zeros[64] = {0};
  reply = realloc(reply, sizeof(*reply) * 69);
  reply[0] = ADCS_ESC_CHAR;
  reply[1] = ADCS_SOM;
  reply[2] = GET_SGP4_ORBIT_PARAMS_ID;
  memcpy(&reply[3], &zeros, 64);
  memcpy(&reply[51], &MA, 8);
  reply[67] = ADCS_ESC_CHAR;
  reply[68] = ADCS_EOM;
  adcs_sgp4 sgp4;
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 69);
  ADCS_get_sgp4_orbit_params(&sgp4);
  TEST_ASSERT_FLOAT_WITHIN(0.001, sgp4.MA, MA);

  // Get full config
  reply = realloc(reply, sizeof(*reply) * 509);
  reply[0] = ADCS_ESC_CHAR;
  reply[1] = ADCS_SOM;
  reply[2] = GET_FULL_CONFIG_ID;
  reply[3] = 87;
  reply[8] = 4;
  int16_t yrate_off = -9874;
  memcpy(&reply[15], &yrate_off, 2);
  reply[30] = 75;
  reply[48] = 1;
  uint16_t area1_max = 1207;
  memcpy(&reply[75], &area1_max, 2);
  uint16_t s22 = 154;
  memcpy(&reply[167], &s22, 2);
  reply[264] = 3;
  uint16_t spin_rate = 1027;
  memcpy(&reply[274], &spin_rate, 2);
  reply[312] = 134;
  reply[507] = ADCS_ESC_CHAR;
  reply[508] = ADCS_EOM;

  adcs_config config;
  uart_send_Ignore();
  uart_receive_ExpectAnyArgs();
  uart_receive_ReturnArrayThruPtr_data(reply, 509);
  ADCS_get_full_config(&config);
  TEST_ASSERT_EQUAL_UINT8(config.MTQ.x, 87);
  TEST_ASSERT_EQUAL_UINT8(config.RW[2], 4);
  TEST_ASSERT_FLOAT_WITHIN(0.001, config.rate_gyro.sensor_offset.y,
                           yrate_off * 0.001);
  TEST_ASSERT_FLOAT_WITHIN(0.001, config.css.rel_scale[0], 0.75);
  TEST_ASSERT_EQUAL_INT(config.cubesense.cam1_sense.auto_adjust, 1);
  TEST_ASSERT_EQUAL_UINT16(config.cubesense.cam1_area.area1.x.max, 1207);
  TEST_ASSERT_FLOAT_WITHIN(0.001, config.MTM1.sensitivity_mat[4], s22 * 0.001);
  TEST_ASSERT_EQUAL_INT(config.star_tracker.module_en, 1);
  TEST_ASSERT_EQUAL_INT(config.star_tracker.loc_predict_en, 1);
  TEST_ASSERT_FLOAT_WITHIN(0.001, config.detumble.spin_rate, spin_rate * 0.001);
  TEST_ASSERT_EQUAL_INT(config.rwheel.sun_point_facet, 6);
  TEST_ASSERT_EQUAL_INT(config.rwheel.auto_transit, 1);

  free(reply);
  // The rest are straightforward.
}
