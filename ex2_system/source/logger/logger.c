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
 * @author Robert Taylor, Dustin Wagner
 * @date 2021-03-06
 */
#include "logger/logger.h"

#include "os_queue.h"
#include "printf.h"
#include <FreeRTOS.h>
#include <HL_hal_stdtypes.h>
#include <assert.h>
#include <redposix.h>
#include <stdbool.h>
#include <string.h>
#include "HL_reg_sci.h"

//#define LOGGER_SWAP_PERIOD_MS 10000

#define DEFAULT_INPUT_QUEUE_LEN 10
#define TASK_NAME_SIZE configMAX_TASK_NAME_LEN + 3
#define LEVEL_LEN 3 // room for log level
#define INPUT_QUEUE_ITEM_SIZE PRINT_BUF_LEN + TASK_NAME_SIZE

static bool fs_init; // true if filesystem initialized

static xQueueHandle input_queue = NULL;
static TaskHandle_t my_handle;

char logger_file[] = "VOL0:/syslog.log";
char old_logger_file[] = "VOL0:/syslog.log.old";
uint32_t logger_file_handle = 0;

// uint32_t next_swap = LOGGER_SWAP_PERIOD_MS;

uint32_t next_swap = 10000; // 10kb seems reasonable..
uint32_t current_size = 0;
const char logger_config[] = "VOL0:/syslog.config";
static bool config_loaded = false;

/**
 * @brief
 *      Check if file with given name exists
 * @param filename
 *      const char * to name of file to check
 * @return bool
 */
static bool exists(const char *filename) {
    int32_t file;
    file = red_open(filename, RED_O_CREAT | RED_O_EXCL | RED_O_RDWR); // attempt to create file
    if (red_errno == RED_EEXIST) {                                    // does file already exist?
        return true;
    }
    red_close(file);      // didn't exist. was created. now close it
    red_unlink(filename); // delete file. file creation would be a side affect
    return false;
}

/**
 * @brief
 *      The maximum file size is a global variable that is stored, and used
 *      when the system reboots to avoid having the value reset. This function
 *      stores the size that the old and new logger file should be.
 *
 * @return int8_t
 *      1 signifies an error. 0 signifies success
 */
int8_t store_logger_file_size(void) {
    int32_t fout = red_open(logger_config, RED_O_CREAT | RED_O_RDWR); // open or create file to write binary
    if (fout == -1) {
        return 1;
    }
    red_write(fout, &next_swap, sizeof(next_swap));
    red_close(fout);
    return 0;
}

/**
 * @brief
 *      The maximum file size is a global variable that is stored, and used
 *      when the system reboots to avoid having the value reset. This function
 *      loads the size that the old and new logger file should be.
 * @return int8_t
 *      1 signifies an error. 0 signifies success.
 */
int8_t load_logger_file_size(void) {
    if (exists(logger_config) == false) {
        return 1;
    }
    int32_t fin = red_open(logger_config, RED_O_CREAT | RED_O_RDWR); // open or create file to write binary
    if (fin == -1) {
        return 1;
    }
    red_read(fin, &next_swap, sizeof(next_swap));
    red_close(fin);
    return 0;
}

/**
 * @brief
 *      This is a setter to be used by other services to modify the maximum size
 *      of the logger files.
 * @param file_size
 *      uint32_t the desired number of bytes to set the file lengths to
 * @return int8_t
 *      error code. 0 means success
 */
int8_t set_logger_file_size(uint32_t file_size) {
    next_swap = file_size;
    store_logger_file_size();
    return 0;
}

/**
 * @brief
 *      This is a getter to be used by other services to get the maximum size
 *      of each of the files used in the logger
 * @param file_size
 *      uint32_t pointer that will hold the value of the file size
 * @return int8_t
 *      error code. 0 means success
 *
 */
int8_t get_logger_file_size(uint32_t *file_size) {
    load_logger_file_size();
    *file_size = next_swap;
    return 0;
}

/**
 * @brief
 *      This function is used to get the name of the primary logger file. this
 *      is returned as a pointer to a global variable of the name.
 * @return char*
 *      pointer to the char array holding the filename
 */
char *get_logger_file() { return logger_file; }

/**
 * @brief
 *      This function is used to get the name of the primary logger file. this
 *      is returned as a pointer to a global variable of the name.
 * @return char*
 *      pointer to the char array holding the filename
 */
char *get_logger_old_file() { return old_logger_file; }

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
    char output_string[STRING_MAX_LEN];

    uint32_t uptime = (uint32_t)(xTaskGetTickCount() / configTICK_RATE_HZ);

    snprintf(output_string, STRING_MAX_LEN, "%010d,%s\r\n", uptime, str);
    size_t string_length = strlen(output_string);
    current_size += string_length;

    if (current_size > next_swap) {
        stop_logger_fs(); // reset the logger file
        init_logger_fs();
        current_size = string_length;
    }

    if (fs_init) {
        red_write(logger_file_handle, output_string, string_length);
        red_transact("VOL0:");
    }

#if defined(PRINTF_SCI)
    printf("%s", output_string);
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
void sys_log(SysLog_Level level, const char *format, ...) {
    const char *main_name = "MAIN";
    const char *task_name;
    const char abbreviations[] = {'P', 'A', 'C', 'E', 'W', 'N', 'I', 'D'};

    if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED) {
        return;
    }

    if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
        task_name = main_name;
    } else {
        task_name = pcTaskGetName(NULL);
    }

    if (level > DEBUG)
        level = DEBUG;

    char buffer[PRINT_BUF_LEN + TASK_NAME_SIZE + LEVEL_LEN];

    va_list arg;
    va_start(arg, format);
    char *msg = buffer + TASK_NAME_SIZE + LEVEL_LEN;
    vsnprintf(msg, PRINT_BUF_LEN, format, arg);
    va_end(arg);
    snprintf(buffer, TASK_NAME_SIZE + LEVEL_LEN + PRINT_BUF_LEN, "%c,%.*s,%s", abbreviations[(int)level],
             TASK_NAME_SIZE, task_name, msg);

    int string_len = strlen(buffer);
    if (buffer[string_len - 1] == '\n') {
        buffer[string_len - 1] = '\0';
        if (buffer[string_len - 2] == '\r') {
            buffer[string_len - 2] = '\0';
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

    REDSTAT file_stats;

    if (config_loaded == false) {
        load_logger_file_size();
        config_loaded = true;
    }

    int32_t fd = red_open(logger_file, RED_O_RDWR | RED_O_APPEND);
    if (fd < 0) {
        fd = red_open(logger_file, RED_O_CREAT | RED_O_RDWR);
        if (fd < 0) {
            return false;
        }
    } else if (fd > 0) {
        int32_t stat_worked = red_fstat(fd, &file_stats);
        if (stat_worked == 0 && file_stats.st_size >= next_swap) {
            red_close(fd);
            red_rename(logger_file, old_logger_file);
            fd = red_open(logger_file, RED_O_CREAT | RED_O_RDWR);
            if (fd < 0) {
                return false;
            }
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

    for (;;) {
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
 * @returns status
 *   error report of task creation
 */
SAT_returnState start_logger_daemon() {
    if (xTaskCreate((TaskFunction_t)logger_daemon, "logger", LOGGER_DM_SIZE, NULL, LOGGER_TASK_PRIO, NULL) !=
        pdPASS) {
        ex2_log("FAILED TO CREATE TASK logger\n");
        return SATR_ERROR;
    }
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
