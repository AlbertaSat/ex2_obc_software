/*
 * test_system_state.c
 *
 *  Created on: Jun. 28, 2021
 *      Author: Andrew
 */

#include <unit.h>

#include <system_state/state_task.c>
#include <system_state/state_task.h>
#include <eps.h>

// WEAKEN ALL STATIC FUNCTIONS
#pragma WEAK( eps2sat_mode_cnv )
#pragma WEAK( power_switch_sys )
#pragma WEAK( change_systems_status )
#pragma WEAK( power_switch_uhf )
#pragma WEAK( start_state_daemon )

#pragma WEAK( eps_get_pwr_chnl )

typedef int8_t (*eps_get_pwr_chnl_ptr)(uint8_t pwr_chnl_port);

int8_t eps_get_pwr_chnl_return_previous_state(uint8_t pwr_chnl_port);

TEST_SETUP() {
}

TEST_TEARDOWN() {
}

TEST(verify_eps2sat_mode_cnv) {
  sat_state_e sat_state;
  sat_state = eps2sat_mode_cnv(critical);
  TEST_ASSERT("Wrong mode conversion to critical state", sat_state == critical_state);

  sat_state = eps2sat_mode_cnv(safe);
  TEST_ASSERT("Wrong mode conversion to safe state", sat_state == safe_state);

  sat_state = eps2sat_mode_cnv(normal);
  TEST_ASSERT("Wrong mode conversion to safe state", sat_state == operational_state);

  sat_state = eps2sat_mode_cnv(full);
  TEST_ASSERT("Wrong mode conversion to safe state", sat_state == operational_state);
}

TEST_SUITE(system_state) {
  ADD_TEST(verify_eps2sat_mode_cnv);
}
