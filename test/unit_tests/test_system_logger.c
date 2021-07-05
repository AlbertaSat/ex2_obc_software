/*
 * test_system_logger.c
 *
 *  Created on: Jun. 28, 2021
 *      Author: Robert Taylor
 */

#include <unit.h>
#include "logger/logger.h"
#include "HL_sys_core.h"
#include "HL_spi.h"
#include <stdbool.h>
#include "redposix.h"

TEST_SETUP() {
}

TEST_TEARDOWN() {
}

TEST(filesystem_will_init) {
    TEST_ASSERT("Logger fs starts not initialized", is_logger_fs_init() == false);

    TEST_ASSERT("Logger reports fs initialized", init_logger_fs() == true);

    TEST_ASSERT("Logger fs status getter reports initialized", is_logger_fs_init() == true);

    stop_logger_fs();
    TEST_ASSERT("Logger fs will shutdown", is_logger_fs_init() == false);
}

TEST(logger_will_handle_file_does_not_exist) {
// if the syslog file doesn't exist the logger will create it
    int32_t fd = red_open(logger_file, RED_O_RDWR);
    if (fd >= 0) {
        red_close(fd);
        red_errno = 0;
        red_unlink(logger_file);
        if (red_errno == RED_EBUSY) {
            ABORT_TEST("Could not delete %s", logger_file);
        }
    }
    init_logger_fs();
    fd = red_open(logger_file, RED_O_RDWR);
    TEST_ASSERT("Logger will create syslog if it does not exist", fd >= 0);
}

TEST(logger_will_handle_file_exists) {
    // if the syslog file already exists the logger will simply append to it
    int32_t fd = red_open(logger_file, RED_O_RDWR);
    if (fd >= 0) {
        red_close(fd);
        red_errno = 0;
        red_unlink(logger_file);
        if (red_errno == RED_EBUSY) {
            ABORT_TEST("Could not delete %s", logger_file);
        }
    } else {
        fd = red_open(logger_file, RED_O_CREAT | RED_O_RDWR);
        if (fd < 0) {
            ABORT_TEST("Could not create %d", logger_file);
        }
        red_write(fd, "TEST_WRITE\n", 11);
        red_close(fd);
    }
    init_logger_fs();
    stop_logger_fs();
    fd = red_open(logger_file, RED_O_RDWR);
    if (fd < 0) {
        TEST_ASSERT("Logger broke the file", false);
        ABORT_TEST("Logger broke the file");
    } else {
        char *test_buf[12] = {0};
        red_read(fd, test_buf, 11);
        TEST_ASSERT("Logger did not delete contents of log file", strncmp("TEST_WRITE\n", test_buf, 12) == 0);
    }
}



TEST_SUITE(system_logger) {
  ADD_TEST(filesystem_will_init);
  ADD_TEST(logger_will_handle_file_does_not_exist);
  ADD_TEST(logger_will_handle_file_exists);
}
