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

//Basic I2C functionality test

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

void test_ADCS_I2C_send(void) {
    uint8_t tc_ack_frame[4];
    tc_ack_frame[0] = LAST_TC_ACK_ID;
    tc_ack_frame[1] = 1;
    tc_ack_frame[2] = ADCS_OK;
    tc_ack_frame[3] = 0;
    
    i2c_send_Ignore();
    i2c_receive_ExpectAnyArgs();
    i2c_receive_ReturnArrayThruPtr_data(tc_ack_frame, 4);
    i2c_receive_ExpectAnyArgs();
    i2c_receive_ReturnArrayThruPtr_data(tc_ack_frame, 4);
    TEST_ASSERT_EQUAL_INT(ADCS_OK,
                        ADCS_load_file_download_block(2, 17, 6324, 145));

    i2c_send_Ignore();
    i2c_receive_ExpectAnyArgs();
    i2c_receive_ReturnArrayThruPtr_data(tc_ack_frame, 4);
    i2c_receive_ExpectAnyArgs();
    i2c_receive_ReturnArrayThruPtr_data(tc_ack_frame, 4);
    TEST_ASSERT_EQUAL_INT(ADCS_OK,
                        ADCS_file_upload_packet(145, "This is test file"));
}

void test_ADCS_I2C_request(void) {
    uint8_t node_type = 0;
    uint8_t interface_ver = 1;
    uint8_t major_firm_ver = 1;
    uint8_t minor_firm_ver = 1;
    uint16_t runtime_s = 10;
    uint16_t runtime_ms = 10000;

    uint8_t reply[8];
    reply[0] = node_type;
    reply[1] = interface_ver;
    reply[2] = major_firm_ver;
    reply[3] = minor_firm_ver;
    reply[4] = (runtime_s & 0xFF);
    reply[5] = (runtime_s >> 8);
    reply[6] = (runtime_ms & 0xFF);
    reply[7] = (runtime_ms >> 8);

    uint8_t node_type_f, interface_ver_f, major_firm_ver_f, minor_firm_ver_f;
    uint16_t runtime_s_f, runtime_ms_f;
    i2c_receive_ExpectAnyArgs();
    i2c_receive_ReturnArrayThruPtr_data(reply, 8);
    ADCS_get_node_identification(&node_type_f, &interface_ver_f, &major_firm_ver_f, 
                    &minor_firm_ver_f, &runtime_s_f, &runtime_ms_f);
    
    TEST_ASSERT_EQUAL_UINT8(node_type, node_type_f);
    TEST_ASSERT_EQUAL_UINT8(interface_ver, interface_ver_f);
    TEST_ASSERT_EQUAL_UINT8(major_firm_ver, major_firm_ver_f);
    TEST_ASSERT_EQUAL_UINT8(minor_firm_ver, minor_firm_ver_f);
    TEST_ASSERT_EQUAL_UINT16(runtime_s, runtime_s_f);
    TEST_ASSERT_EQUAL_UINT16(runtime_ms, runtime_ms_f);
}
