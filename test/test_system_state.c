/*
 * test_system_state.c
 *
 *  Created on: Jun. 28, 2021
 *      Author: Andrew
 */

#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

#include <FreeRTOS.h>
#include <task.h>

#include "system.h"
#include "eps.h"
// #include <system_state/state_task.c>
#include <system_state/state_task.h>
#include <eps.h>

Describe(system_state);
BeforeEach(system_state) {};
AfterEach(system_state) {};

Ensure(system_state, properly_converts_citical_state) {
  sat_state_e sat_state = eps2sat_mode_cnv(critical);
  assert_that(sat_state, is_equal_to(critical_state));
}

Ensure(system_state, properly_converts_safe_state) {
  sat_state_e sat_state = eps2sat_mode_cnv(safe);
  assert_that(sat_state,  is_equal_to(safe_state));
}

Ensure(system_state, properly_converts_normal_state) {
  sat_state_e sat_state = eps2sat_mode_cnv(normal);
  assert_that(sat_state,  is_equal_to(operational_state));
}

Ensure(system_state, properly_converts_full_state) {
  sat_state_e sat_state = eps2sat_mode_cnv(full);
  assert_that(sat_state,  is_equal_to(operational_state));
}

TestSuite *state_conversion_tests() {
  TestSuite *suite = create_test_suite();
  add_test_with_context(suite, system_state, properly_converts_citical_state);
  add_test_with_context(suite, system_state, properly_converts_safe_state);
  add_test_with_context(suite, system_state, properly_converts_normal_state);
  add_test_with_context(suite, system_state, properly_converts_full_state);

  return suite;
}

/* -- */

Ensure(system_state, starts_and_stops) {
  SAT_returnState sat_state = start_state_daemon();
  assert_that(sat_state, is_equal_to(SATR_OK));
  stop_state_daemon();
}

#define get_eps_batt_mode mock_get_eps_batt_mode;
eps_mode_e mock_get_eps_batt_mode() {
  return (eps_mode_e) mock();
}

#define eps_get_pwr_chnl mock_eps_get_pwr_chnl
int8_t mock_eps_get_pwr_chnl(uint8_t pwr_chnl_port) {
  return mock(pwr_chnl_port);
}

#define eps_set_pwr_chnl mock_eps_set_pwr_chnl
void mock_eps_set_pwr_chnl(uint8_t pwr_chnl_port, bool bit) {
  mock(pwr_chnl_port, bit);
}

Ensure(system_state, sets_critical_state) {
  expect(get_eps_batt_mode, will_return(critical));

  expect(eps_get_pwr_chnl, will_return(1));
  expect(eps_set_pwr_chnl, when(pwr_chnl_port, is_equal_to(STX_PWR_CHNL)), when(bit, is_false));
  expect(eps_get_pwr_chnl, will_return(0));

  expect(eps_get_pwr_chnl, will_return(1));
  expect(eps_set_pwr_chnl, when(pwr_chnl_port, is_equal_to(IRIS_PWR_CHNL)), when(bit, is_false));
  expect(eps_get_pwr_chnl, will_return(0));

  expect(eps_get_pwr_chnl, will_return(1));


  //TODO: crash is here
  expect(eps_set_pwr_chnl, when(pwr_chnl_port, is_equal_to(DFGM_PWR_CHNL)), when(bit, is_false));
  expect(eps_get_pwr_chnl, will_return(0));

  expect(eps_get_pwr_chnl, will_return(1));
  expect(eps_set_pwr_chnl, when(pwr_chnl_port, is_equal_to(ADCS_PWR_CHNL)), when(bit, is_false));
  always_expect(eps_get_pwr_chnl, will_return(0));
  expect(eps_get_pwr_chnl, will_return(0));
  SAT_returnState sat_state = start_state_daemon();
  taskYIELD();
  stop_state_daemon();
}

TestSuite *state_task_tests() {
  TestSuite *suite = create_test_suite();
//  add_test_with_context(suite, system_state, starts_and_stops);
  add_test_with_context(suite, system_state, sets_critical_state);
  return suite;
}


int test_system_state() {
  TestSuite *suite = create_test_suite();
  add_suite(suite, state_conversion_tests());
  add_suite(suite, state_task_tests());
  run_test_suite(suite, create_text_reporter());
  return 0;
}
 
