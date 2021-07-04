/* Copyright (C) 2015  Brendan Bruner
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
 *
 * bbruner@ualberta.ca
 *
 * This test suite was inspired from the test suite at:
 * http://www.jera.com/techinfo/jtns/jtn002.html - January 2015
 */

#ifndef UNIT_H_
#define UNIT_H_

#include <stdio.h>

#define UNIT_PRINT( msg, ... )  fprintf(stderr, msg, ##__VA_ARGS__ )

extern volatile unsigned int unit_asserts_passed;
extern volatile unsigned int delta_unit_asserts_passed;
extern volatile unsigned int unit_asserts_failed;
extern volatile unsigned int delta_unit_asserts_failed;
extern volatile unsigned int unit_tests_run;
extern volatile unsigned int unit_tests_aborted;

#define FAIL_TEST       0
#define PASS_TEST       1

#define TEST(name)        static void test_##name (void)
#define TEST_SETUP( )     static void _unit_test_setup( )
#define TEST_TEARDOWN( )    static void _unit_test_teardown( )

#define TEST_SUITE(suite)     void all_tests_##suite (void)

#define EXIT_TEST()         ABORT_TEST("line %d", __LINE__)
#define ABORT_TEST(message,...)   do {                    \
                    UNIT_PRINT( "\t\tABORTED: " );      \
                    UNIT_PRINT( message, ##__VA_ARGS__ ); \
                    UNIT_PRINT( "\n" );           \
                    ++unit_tests_aborted;         \
                    return;                 \
                  } while( 0 )

#define TEST_ASSERT(message, test,...) do {                    \
                  if( !(test) ) {               \
                    UNIT_PRINT( "\t\tFAILURE: " );        \
                    UNIT_PRINT( message, ##__VA_ARGS__ );   \
                    UNIT_PRINT( "\n" );             \
                    fflush( stdout );           \
                    ++unit_asserts_failed;          \
                    }                   \
                  else {                    \
                    ++unit_asserts_passed;          \
                  }                     \
                  } while(0)

#define ADD_TEST(name)      do {                    \
                UNIT_PRINT( "\tTest: %s...\n", #name );     \
                _unit_test_setup( );            \
                ++unit_tests_run;             \
                test_##name ();               \
                _unit_test_teardown( );           \
                } while(0)

#define RUN_TEST_SUITE(suite) do {                    \
                UNIT_PRINT("Running test suite: %s...\n", #suite);  \
                delta_unit_asserts_passed = unit_asserts_passed;  \
                delta_unit_asserts_failed = unit_asserts_failed;  \
                all_tests_##suite ();             \
                UNIT_PRINT("Asserts passed: %d\nAsserts failed: %d\n\n",  \
                    unit_asserts_passed - delta_unit_asserts_passed,  \
                    unit_asserts_failed - delta_unit_asserts_failed); \
                } while(0)

#define INIT_TESTING()      do {                    \
                  unit_asserts_passed = 0;        \
                  delta_unit_asserts_passed = 0;      \
                  unit_asserts_failed = 0;        \
                  delta_unit_asserts_failed = 0;      \
                  unit_tests_aborted = 0;         \
                  unit_tests_run = 0;           \
                  UNIT_PRINT("\n");           \
                } while( 0 )

#define PRINT_DIAG()      do {                    \
                  UNIT_PRINT("DIAGNOSTICS...\n");       \
                  UNIT_PRINT( "\tAsserts passed:\t%d\n" \
                      "\tAsserts failed:\t%d\n" \
                      "\tAsserts made:\t%d\n" \
                      "\tTest aborted:\t%d\n"\
                      "\tTests run:\t%d\n", \
                      unit_asserts_passed,      \
                      unit_asserts_failed,      \
                      unit_asserts_passed + unit_asserts_failed, \
                      unit_tests_aborted,\
                      unit_tests_run );       \
                } while( 0 )


#endif /* UNIT_H_ */
