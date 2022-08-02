/*
 * test_system_tasks.c
 *
 *  Created on: Jul, 2022
 *      Author: Grace
 */

#include <config.h>
#include "test_system_tasks.h"

Describe(system_tasks);
BeforeEach(system_tasks){};
AfterEach(system_tasks){};

#define snprintf_ snprintf
#define printf_ printf
#define vsnprintf_ vsnprintf

SAT_returnState start_system_tasks(void) { return (SAT_returnState)mock(); }

/* Leop test code */
/*------------------------------------------------------------------------------------*/

/* expect() function is a test for whether the function is called, and behaves the way we want it to,
hence we need to write mock functions for expect()*/

/* when testing functions from other ".c" files, the function can be called directly, hence expect()
does not need to be used here */

Ensure(system_tasks, start_system_tasks_return_ok) {
    expect(start_system_tasks, will_return(SATR_OK));
    SAT_returnState state = start_system_tasks();
    assert_that(state, is_equal_to(SATR_OK));
}

TestSuite *system_tasks_test_code() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, system_tasks, start_system_tasks_return_ok);

    return suite;
}

int test_system_tasks() {
    TestSuite *suite = create_test_suite();
    add_suite(suite, system_tasks_test_code());
    return run_test_suite(suite, create_text_reporter());
}
