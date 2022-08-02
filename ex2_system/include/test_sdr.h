/*
 * test_sdr.h
 *
 * Created on: Oct 18, 2021
 * Author: rcunrau
 */

#ifndef EX2_SYSTEM_TEST_SDR_H_
#define EX2_SYSTEM_TEST_SDR_H_

#include "sdr_driver.h"

void start_test_sdr(sdr_interface_data_t *uhf, sdr_interface_data_t *sband);
void start_test_sband(csp_iface_t *sband_iface);

#endif /* EX2_SYSTEM_TEST_SDR_H_ */
