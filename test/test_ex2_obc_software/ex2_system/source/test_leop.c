/*
 * leop.c
 *
 *  Created on: Oct, 2021
 *      Author: Grace
 */

#include "test_leop.h"

Describe(leop);
BeforeEach(leop){};
AfterEach(leop){};

#define snprintf_ snprintf
#define printf_ printf
#define vsnprintf_ vsnprintf

bool leop_eeprom() { return (bool)mock(); }
bool eeprom_get_leop_status() { return (bool)mock(); }
bool eeprom_set_leop_status() { return (bool)mock(); }
bool switchstatus(Deployable_t sw) { return (bool)mock(sw); }
// bool hard_switch_status() { return (bool)mock(); }

bool deploy(Deployable_t deployable) { return (bool)mock(deployable); }
int activate(Deployable_t knife) { return (int)mock(knife); }

/*
bool switchstatus(Deployable_t sw) {
    switch(sw){
        case Port:
            return (bool)mock(1);
        case UHF_P:
            return (bool)mock(1);
        case UHF_Z:
            return (bool)mock(1);
        case Payload:
            return (bool)mock(1);
        case UHF_S:
            return (bool)mock(1);
        case UHF_N:
            return (bool)mock(1);
        case Starboard:
            return (bool)mock(1);
        case DFGM:
            return (bool)mock(1);
        default:
            return 1;//check this
    }
}


//This should return 1, assuming switches are not connected to ground when undepressed
bool deploy(Deployable_t deployable) {
    activate(deployable);
    return switchstatus(deployable);
}
*/

/* Leop test code */
/*------------------------------------------------------------------------------------*/

/* expect() function is a test for whether the function is called, and behaves the way we want it to,
hence we need to write mock functions for expect()*/

/* when testing functions from other ".c" files, the function can be called directly, hence expect()
does not need to be used here */

Ensure(leop, execute_leop_returns_true_when_eeprom_true) {
    always_expect(eeprom_get_leop_status, will_return(is_true));
    bool open = execute_leop();
    assert_that(open, is_true);
}

Ensure(leop, execute_leop_returns_true_when_switchstatus_true) {
    always_expect(eeprom_get_leop_status, will_return(is_false));
    always_expect(switchstatus, will_return(is_true));
    bool open = execute_leop();
    assert_that(open, is_true);
}

Ensure(leop, execute_leop_returns_false_when_switchstatus_false) {
    expect(eeprom_get_leop_status, will_return(false));
    always_expect(switchstatus, will_return(false));
    always_expect(activate, will_return(is_equal_to(0)));
    always_expect(xTaskGetSchedulerState, will_return(0));
    always_expect(vTaskDelay);
    // always_expect(eeprom_set_leop_status, will_return(is_true));
    bool open = execute_leop();
    assert_that(open, is_false);
}

Ensure(leop, hard_switch_status_is_checked_3times) {
    expect(eeprom_get_leop_status, will_return(false));
    expect(switchstatus, will_return(false));
    expect(activate, will_return(is_equal_to(0)));
    expect(switchstatus, will_return(false));
    expect(activate, will_return(is_equal_to(0)));
    expect(switchstatus, will_return(false));
    expect(activate, will_return(is_equal_to(0)));
    expect(switchstatus, will_return(false));
    always_expect(xTaskGetSchedulerState, will_return(0));
    always_expect(vTaskDelay);
    // always_expect(eeprom_set_leop_status, will_return(is_true));
    bool open = execute_leop();
    assert_that(open, is_false);
}

TestSuite *leop_test_code() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, leop, execute_leop_returns_true_when_eeprom_true);
    add_test_with_context(suite, leop, execute_leop_returns_true_when_switchstatus_true);
    add_test_with_context(suite, leop, execute_leop_returns_false_when_switchstatus_false);
    add_test_with_context(suite, leop, hard_switch_status_is_checked_3times);

    return suite;
}

int test_leop() {
    TestSuite *suite = create_test_suite();
    add_suite(suite, leop_test_code());
    return run_test_suite(suite, create_text_reporter());
}
