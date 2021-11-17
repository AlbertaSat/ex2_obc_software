/*
 * leop.c
 *
 *  Created on: Oct, 2021
 *      Author: Grace
 */

#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

#include "FreeRTOS.h"
#include "leop.h"
#include "os_queue.h"
#include "os_task.h"
#include <redposix.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <system.h>

#include "HL_gio.h"
#include "HL_het.h"
//#include "deployablescontrol.h"
#include "test_leop.h"
//#include "leop_eeprom.h"

#include "../source/leop.c"

Describe(leop);
BeforeEach(leop){};
AfterEach(leop){};

#define snprintf_ snprintf
#define printf_ printf
#define vsnprintf_ vsnprintf

bool leop_eeprom() { return mock(); }
bool eeprom_get_leop_status() { return mock(); }
void eeprom_set_leop_status() { return mock(); }
bool switchstatus(Deployable_t sw) { return is_true; }
//bool hard_switch_status() { return is_true; }
//bool hard_switch_status() { return mock(); }

bool deploy(Deployable_t deployable) { return is_true; }

void activate(Deployable_t knife) {mock();}

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

Ensure(leop, ex2_log_returns_test_when_true) {
    expect(eeprom_get_leop_status, will_return(is_false));
    always_expect(switchstatus(Deployable_t deployable), will_return(is_true));
    //expect(eeprom_set_leop_status, will_return(mock()));
    bool open = leop_init();
    assert_that(open, is_false);
}

Ensure(leop, hard_switch_status_is_checked_3times) {
    expect(eeprom_get_leop_status, will_return(is_false));
    always_expect(switchstatus(Deployable_t deployable), will_return(is_false));
    //expect(switchstatus(Deployable_t deployable), will_return(is_false));
    //expect(switchstatus(Deployable_t deployable), will_return(is_true));
    /*int i;
    for (i = 1; i < 3; i++) {
        expect(switchstatus(Deployable_t deployable), will_return(is_false));
    }
    expect(switchstatus(Deployable_t deployable), will_return(is_true)); */

    //expect(hard_switch_status(), will_return(is_true));
    bool open = leop_init();
    assert_that(open, is_false);
}

Ensure(leop, returns_false_when_not_executed) {
    expect(eeprom_get_leop_status, will_return(is_true));
    bool open = leop_init();
    assert_that(open, is_true);
}

TestSuite *leop_test_code() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, leop, ex2_log_returns_test_when_true);
    add_test_with_context(suite, leop, hard_switch_status_is_checked_3times);
    add_test_with_context(suite, leop, returns_false_when_not_executed);

    return suite;
}

int test_leop() {
    TestSuite *suite = create_test_suite();
    add_suite(suite, leop_test_code());
    return run_test_suite(suite, create_text_reporter());
}

