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
 * @file test_sTransmitter.c
 * @author Thomas Ganley
 * @date 2020-05-13
 */

#include "unity.h"
#include "sTransmitter.h"
#include "mock_i2c.h"
#include "mock_spi.h"
#include <stdint.h>

void test_setControl_getControl(void)
{
	STX_setControl(S_PA_ENABLE, S_DATA_MODE);

	uint8_t pa = 0, mode = 0;
	STX_getControl(&pa, &mode);

	TEST_ASSERT_EQUAL_UINT8(S_DATA_MODE, mode);
	TEST_ASSERT_EQUAL_UINT8(S_PA_ENABLE, pa);
	resetTest();
}

void test_setEncoder_getEncoder(void)
{
	STX_setControl(S_PA_DISABLE, S_CONF_MODE); // Must be in Configuration Mode
	STX_setEncoder(S_SCRAMBLER_ENABLE, S_FILTER_DISABLE, S_MOD_OQPSK, S_RATE_FULL);

	uint8_t scrambler = 0, filter = 0, mod = 0, rate = 0;
       	STX_getEncoder(&scrambler, &filter, &mod, &rate);

	TEST_ASSERT_EQUAL_UINT8(S_SCRAMBLER_ENABLE, scrambler);
	TEST_ASSERT_EQUAL_UINT8(S_FILTER_DISABLE, filter);
	TEST_ASSERT_EQUAL_UINT8(S_MOD_OQPSK, mod);
	TEST_ASSERT_EQUAL_UINT8(S_RATE_FULL, rate);
}

void test_setPAPower26_getPAPower26(void)
{
	uint8_t new_paPower = 26;
	STX_setPaPower(new_paPower);

	uint8_t power = 0;
	STX_getPaPower(&power);

	TEST_ASSERT_EQUAL_UINT8(new_paPower, power);
}

void test_setFrequency_getFrequency(void)
{
	float new_frequency = 2225.5f;
	STX_setFrequency(new_frequency);

	float frequency = 0;
	STX_getFrequency(&frequency);

	TEST_ASSERT_FLOAT_WITHIN(0.1, new_frequency, frequency);
}

void test_resetFPGA(void)
{
	STX_softResetFPGA();
	uint8_t reg = 1;
	read_reg(0x05, &reg);

	TEST_ASSERT_EQUAL_UINT8(0, reg);
}

void test_getFirmwareVersion(void)
{
	float version = 0;
	STX_getFirmwareV(&version);

	TEST_ASSERT_EQUAL_FLOAT(1.14, version);
}

void test_getStatus(void)
{
	uint8_t pwrgd = 0, txl = 0;

	STX_getStatus(&pwrgd, &txl);

	TEST_ASSERT_EQUAL_UINT8(S_PAPWR_GOOD, pwrgd);
	TEST_ASSERT_EQUAL_UINT8(S_FREQ_LOCK, txl);
}

void test_putAmountBytesInBuffer(void)
{
	int amount = 10000;
	add_vBuffer(amount);
	uint16_t count = 0;
	STX_getBuffer(S_BUFFER_COUNT, &count);
	TEST_ASSERT_EQUAL_UINT16(amount, count);
}
void test_sendAmountBytesInBuffer(void)
{
	int amount = 10000;
	STX_setControl(S_PA_ENABLE, S_DATA_MODE);
	transmit_vBuffer(amount);
	uint16_t count = 0;
	STX_getBuffer(S_BUFFER_COUNT, &count);
	TEST_ASSERT_EQUAL_UINT16(0, count);
}

void test_get_TR(void)
{
        empty_vBuffer();

        int transmit = 0;

        add_vBuffer(1);
        STX_getTR(&transmit);
        TEST_ASSERT_EQUAL_INT(1, transmit);

        add_vBuffer(2560);
        STX_getTR(&transmit);
        TEST_ASSERT_EQUAL_INT(0,transmit);

        empty_vBuffer();
}

void test_bufferOverrun(void)
{
	empty_vBuffer();
	int amount = 20481;
	add_vBuffer(amount);
	uint16_t overrun = 0;
	STX_getBuffer(S_BUFFER_OVERRUN, &overrun);

	TEST_ASSERT_EQUAL_UINT16(1, overrun);
}
void test_bufferUnderrun()
{
	empty_vBuffer();
	int amount = 1;
	transmit_vBuffer(amount);
	uint16_t underrun = 0;
	STX_getBuffer(S_BUFFER_UNDERRUN, &underrun);

	TEST_ASSERT_EQUAL_UINT16(1, underrun);
}

void test_housekeeping()
{
  sBand_housekeeping hkStruct;
  if (STX_getHK(&hkStruct) == FUNC_PASS) {
    TEST_ASSERT_FLOAT_WITHIN(0.05, 2.34, hkStruct.outputPower);
    TEST_ASSERT_FLOAT_WITHIN(0.05, 100.6, hkStruct.paTemp);
    TEST_ASSERT_FLOAT_WITHIN(0.05, 50, hkStruct.topTemp);
    TEST_ASSERT_FLOAT_WITHIN(0.05, -0.25, hkStruct.bottomTemp);
    TEST_ASSERT_FLOAT_WITHIN(0.005, 0.08224, hkStruct.batCurrent);
    TEST_ASSERT_FLOAT_WITHIN(0.005, 16.448, hkStruct.batVoltage);
    TEST_ASSERT_FLOAT_WITHIN(0.005, -0.6528, hkStruct.paCurrent);
    TEST_ASSERT_FLOAT_WITHIN(0.005, 0.036, hkStruct.paVoltage);
  } else {
    TEST_FAIL();
  }
}
