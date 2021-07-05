/*
 * Copyright (C) 2021  University of Alberta
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
 * @file run_test_suite.c
 * @author
 * @date 2020-06-28
 */
#include "test_suites.h"


void run_test_suite() {

  INIT_TESTING();
//  init_environment();

  RUN_TEST_SUITE(system_state);

  PRINT_DIAG();

  return;
}

// init CSP, FreeRTOS, FS. It is reasonable to test with the whole system running.
