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
 * @file communication_service.c
 * @author Robert Taylor
 * @date 2021-03-06
 */
#include "logger/logger.h"

#include <assert.h>
#include <HL_hal_stdtypes.h>
#include <FreeRTOS.h>
#include "os_queue.h"
#include "printf.h"
#include <stdbool.h>
#include <string.h>
#include <redposix.h>

#define XSTR_(X) STR_(X)
#define STR_(X) #X

#define LOGGER_SWAP_PERIOD_MS 10000

#define DEFAULT_INPUT_QUEUE_LEN 10
#define TASK_NAME_SIZE configMAX_TASK_NAME_LEN + 3
#define INPUT_QUEUE_ITEM_SIZE PRINT_BUF_LEN + TASK_NAME_SIZE

static bool fs_init; // true if filesystem initialized

static xQueueHandle input_queue = NULL;
static TaskHandle_t my_handle;

const char logger_file[] = "VOL0:/syslog.log";
const char old_logger_file[] = "VOL0:/syslog.log.old";
uint32_t logger_file_handle = 0;

uint32_t next_swap = LOGGER_SWAP_PERIOD_MS;

static void test_logger_daemon(void *pvParameters);

/**
 * @brief
 * Puts a string on the output
 *
 * @details
 * If filesystem is initialized it will write to the syslog.
 * If the MCU it is being run on is a flatsat it will not print on the uart
 * Prepends the uptime in seconds
 *
 * @param String to print
 * @return None
 */
static void do_output(const char *str) {
    char output_string[STRING_MAX_LEN] = {0};

    uint32_t uptime = (uint32_t)(xTaskGetTickCount()/configTICK_RATE_HZ);

    if (uptime > next_swap) {
        stop_logger_fs(); // reset the logger file
        init_logger_fs();
        next_swap += LOGGER_SWAP_PERIOD_MS; // if this overflows that is okay. The tick count also overflows;
    }

    snprintf(output_string, STRING_MAX_LEN, "[%010d]%s\r\n", uptime, str);

    if (fs_init) {
        red_write(logger_file_handle, output_string, strlen(output_string));
    }

#ifndef IS_FLATSAT
    printf(output_string);
#endif
}

/**
 * @brief
 * Logs a string
 * 
 * @details
 * Will either log a string to the filesystem or the UART
 * Will log to filesystem if it can
 * Will lot of UART if IS_FLATSAT is not defined
 * Prepends calling task name and timestamp
 */
void ex2_log(const char *format, ...) {
    const char main_name[] = "MAIN";
    char *task_name;

    if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED) {
        return;
    }

    if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
        task_name = main_name;
    } else {
        task_name = pcTaskGetName(NULL);
    }

    char buffer[PRINT_BUF_LEN + TASK_NAME_SIZE] = {0};

    va_list arg;
    va_start(arg, format);
    vsnprintf(buffer + TASK_NAME_SIZE, PRINT_BUF_LEN, format, arg);
    va_end(arg);
    snprintf(buffer, PRINT_BUF_LEN + TASK_NAME_SIZE, "[%." XSTR_(configMAX_TASK_NAME_LEN) "s]%s", task_name, buffer + TASK_NAME_SIZE);

    int string_len = strlen(buffer);
    if (buffer[string_len - 1] == '\n') {
        buffer[string_len - 1] = '\0';
        if (buffer[string_len - 2] == '\r') {
            buffer[string_len -2] = '\0';
        }
    }

    if (input_queue == NULL) {
        do_output(buffer);
        return;
    } else {
        xQueueSend(input_queue, buffer, 0); // will not wait for queue to have space
    }

    return;
}

/**
 * @brief
 * Initialize filesystem related contructs for logger
 * 
 * @details
 * Opens syslog file and sets fs_init to true on success
 */
bool init_logger_fs() {
    if (fs_init) {
        return true;
    }

    int32_t fd = red_open(logger_file, RED_O_RDWR);
    if (fd < 0) {

        fd = red_open(logger_file, RED_O_CREAT | RED_O_RDWR);
        if (fd < 0) {
            return false;
        }
    } else if (fd > 0) {
        red_close(fd);
        red_rename(logger_file, old_logger_file);
        fd = red_open(logger_file, RED_O_CREAT | RED_O_RDWR);
        if (fd < 0) {
            return false;
        }
    }
    fs_init = true;
    logger_file_handle = fd;
    return true;
}

/**
 * @brief
 * Shut down filesystem related constructs for logger
 * 
 * @details
 * Closes syslog file and sets fs_init to false
 */
void stop_logger_fs() {
    if (!fs_init) {
        return;
    }
    red_close(logger_file_handle);
    logger_file_handle = 0;
    fs_init = false;
}

/**
 * @brief
 * Logger Daemon. Handle string data sent to the log system. Send with ex2_log
 *
 * @param pvParameters
 *    task parameters (not used)
 */
static void logger_daemon(void *pvParameters) {
    char buffer[PRINT_BUF_LEN] = {0};

    if (!init_logger_fs()) {
        // On the flatsat this will be unreported
        ex2_log("Failed to initialize logger file");
    }

    init_logger_queue();

    for ( ;; ) {
        xQueueReceive(input_queue, buffer, portMAX_DELAY);
        if (!fs_init) {
            init_logger_fs(); // just keep trying
        }
        do_output(buffer);
        memset(buffer, 0, PRINT_BUF_LEN);
    }
}

// Logs a couple strings every second
void test_logger_daemon(void *pvParameters) {
    vTaskDelay(10000);
    for (;;) {
        ex2_log("TEST TASK!");
        ex2_log("Test %d TASK", 5);
        ex2_log("Test Task\r\n");
        vTaskDelay(1000);
    }
}

/**
 * @brief
 * Start the logger daemon
 * 
 * @param handle
 *    Pointer as a return for the task handle
 *
 * @returns status
 *   error report of task creation
 */
SAT_returnState start_logger_daemon(TaskHandle_t *handle) {
    if (xTaskCreate((TaskFunction_t)logger_daemon,
                  "logger", 1000, 0, LOGGER_TASK_PRIO,
                  handle) != pdPASS) {
        ex2_log("FAILED TO CREATE TASK logger\n");
        return SATR_ERROR;
    }
    my_handle = *handle;
    ex2_log("Logger Service Started");
    return SATR_OK;
}

/**
 * Kill the logger daemon gracefully
 */
void kill_logger_daemon() {
    vTaskDelete(my_handle);
    stop_logger_fs();
    vQueueDelete(input_queue);
}

/**
 * Initialize the queue if it has not been
 */
void init_logger_queue() {
    if (input_queue == NULL) {
        input_queue = xQueueCreate(DEFAULT_INPUT_QUEUE_LEN, INPUT_QUEUE_ITEM_SIZE);
    }
}
