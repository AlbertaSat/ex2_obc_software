/*
 * test_sdr.h
 *
 * Created on: Oct 18, 2021
 * Author: rcunrau
 */

#ifndef EX2_SYSTEM_TEST_SDR_H_
#define EX2_SYSTEM_TEST_SDR_H_

#include "sdr_driver.h"

void start_test_sdr(sdr_uhf_conf_t *sdr_conf);

void sdr_uhf_receive(void *conf, uint8_t *data, size_t len);

#endif /* EX2_SYSTEM_TEST_SDR_H_ */
