/*
 * housekeeping_mocks.h
 *
 *  Created on: Sep. 7, 2022
 *      Author: robert
 */

#ifndef EX2_SYSTEM_INCLUDE_HOUSEKEEPING_HOUSEKEEPING_MOCKS_H_
#define EX2_SYSTEM_INCLUDE_HOUSEKEEPING_HOUSEKEEPING_MOCKS_H_

#include "adcs.h"
#include "housekeeping_athena.h"
#include "eps.h"
#include "uhf.h"
#include "hal_sband.h"
#include "hyperion.h"
#include "housekeeping_charon.h"
#include "dfgm.h"
#include "dfgm_handler.h"
#include "northern_spirit_handler.h"
#include "iris.h"

uint32_t mock_time();

void mock_adcs(ADCS_HouseKeeping *hk);

void mock_athena(athena_housekeeping *hk);

void mock_eps_instantaneous(eps_instantaneous_telemetry_t *hk);

void mock_eps_startup(eps_startup_telemetry_t *hk);

void mock_uhf(UHF_housekeeping *hk);

void mock_sband(Sband_Housekeeping *hk);

void mock_hyperion(Hyperion_HouseKeeping *hk);

void mock_charon(charon_housekeeping *hk);

void mock_dfgm(DFGM_Housekeeping *hk);

void mock_ns(ns_telemetry *hk);

void mock_iris(IRIS_Housekeeping *hk);
#endif /* EX2_SYSTEM_INCLUDE_HOUSEKEEPING_HOUSEKEEPING_MOCKS_H_ */
