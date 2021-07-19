/*
 * test_system_state.c
 *
 *  Created on: Jun. 28, 2021
 *      Author: Andrew
 */

#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "os_task.h"
#include "os_queue.h"
#include <redposix.h>
#include "logger/logger.h"
#include <stdio.h>



Describe(logger);
BeforeEach(logger) {};
AfterEach(logger) {};

int32_t red_open( const char *pszPath, uint32_t ulOpenMode) {
    return mock(pszPath, ulOpenMode);
}

int32_t red_write(
    int32_t     iFildes,
    const void *pBuffer,
    uint32_t    ulLength) {
        return mock(iFildes, pBuffer, ulLength);
    }

int32_t red_close(
    int32_t     iFildes) {
        return mock(iFildes);
    }

TickType_t xTaskGetTickCount() {
    return mock();
}

BaseType_t xTaskGetSchedulerState() {
    return mock();
}

char *pcTaskGetName(TaskHandle_t task) {
    return (char *)mock(task);
}

BaseType_t MPU_xQueueGenericReceive(QueueHandle_t	xQueue, void *pvBuffer, TickType_t xTicksToWait, const BaseType_t xJustPeek ) {
    return mock(xQueue, pvBuffer, xTicksToWait, xJustPeek);
}

BaseType_t xQueueGenericSend( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait, const BaseType_t xCopyPosition ) {
    return mock(xQueue, pvItemToQueue, xTicksToWait, xCopyPosition);
}

REDSTATUS *red_errnoptr() {
    return (REDSTATUS *)mock();
}

void vTaskDelay( const TickType_t xTicksToDelay ) {
    mock(xTicksToDelay);
}

BaseType_t xTaskCreate(	TaskFunction_t pxTaskCode,
							const char * const pcName,
							const uint16_t usStackDepth,
							void * const pvParameters,
							UBaseType_t uxPriority,
							TaskHandle_t * const pxCreatedTask ) { /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
    return mock(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask);
}
void vTaskDelete( TaskHandle_t xTaskToDelete ) {
    mock(xTaskToDelete);
}

void vQueueDelete(QueueHandle_t queue) {
    mock(queue);
}

QueueHandle_t xQueueGenericCreate( const UBaseType_t uxQueueLength, const UBaseType_t uxItemSize, const uint8_t ucQueueType ) {
    return (QueueHandle_t)mock(uxQueueLength, uxItemSize, ucQueueType);
}

#define snprintf_ snprintf
#define printf_ printf
#define vsnprintf_ vsnprintf

#include "../ex2_system/source/logger/logger.c"

/* Logger FS tests */
/*------------------------------------------------------------------------------------*/

Ensure(logger, notices_file_does_not_exist) {
    expect(red_open, will_return(-1));
    expect(red_open, will_return(1));
    bool open = init_logger_fs();
    assert_that(open, is_true);
}

Ensure(logger, notices_file_exists) {
    expect(red_open, will_return(1));
    never_expect(red_open);
    bool open = init_logger_fs();
    assert_that(open, is_true);
}

Ensure(logger, sets_internals_when_file_does_not_exist) {
    expect(red_open, will_return(-1));
    expect(red_open, will_return(1));
    bool open = init_logger_fs();
    assert_that(open, is_true);
    assert_that(logger_file_handle, is_equal_to(1));
    assert_that(fs_init, is_true);
}

Ensure(logger, sets_internals_when_file_exists) {
    expect(red_open, will_return(1));
    never_expect(red_open);
    bool open = init_logger_fs();
    assert_that(open, is_true);
    assert_that(logger_file_handle, is_equal_to(1));
    assert_that(fs_init, is_true);
}

Ensure(logger, init_fs_returns_false_on_failure) {
    expect(red_open, will_return(-1));
    expect(red_open, will_return(-1));
    bool fd = init_logger_fs();
    assert_that(fd, is_false);
}

Ensure(logger, stop_fs_returns_when_not_init) {
    never_expect(red_close);
    stop_logger_fs();
}

Ensure(logger, stop_fs_closes_file_when_init) {
    fs_init = true;
    logger_file_handle = 1452;
    expect(red_close);
    stop_logger_fs();
    assert_that(logger_file_handle, is_equal_to(0));
    assert_that(fs_init, is_false);
}

TestSuite *logger_fs_tests() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, logger, notices_file_does_not_exist);
    add_test_with_context(suite, logger, notices_file_exists);
    add_test_with_context(suite, logger, init_fs_returns_false_on_failure);
    add_test_with_context(suite, logger, stop_fs_returns_when_not_init);
    add_test_with_context(suite, logger, sets_internals_when_file_does_not_exist);
    add_test_with_context(suite, logger, sets_internals_when_file_exists);
    add_test_with_context(suite, logger, stop_fs_closes_file_when_init);

    return suite;
}

/* Logger Input Tests */
/*------------------------------------------------------------------------------------*/

Ensure(logger, ex2_log_returns_when_scheduler_suspended) {
    expect(xTaskGetSchedulerState, will_return(taskSCHEDULER_SUSPENDED));
    ex2_log("unimportant");
}

Ensure(logger, ex2_log_prints_main_when_scheduler_not_running) {
    input_queue = (QueueHandle_t)1;
    expect(xTaskGetSchedulerState, will_return(taskSCHEDULER_NOT_STARTED), times(2));

}

TestSuite *logger_input_tests() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, logger, ex2_log_returns_when_scheduler_suspended);

    return suite;
}

int test_logger() {
  TestSuite *suite = create_test_suite();
  add_suite(suite, logger_fs_tests());
  add_suite(suite, logger_input_tests());
  return run_test_suite(suite, create_text_reporter());
}
