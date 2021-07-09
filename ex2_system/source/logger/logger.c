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
#include "services.h"

#include <assert.h>
#include <HL_hal_stdtypes.h>
#include <FreeRTOS.h>
#include "os_queue.h"
#include "printf.h"
#include <stdbool.h>
#include <string.h>
#include <redposix.h>
#include <csp/csp.h>
#include "util/service_utilities.h" //for setting csp packet length

#define XSTR_(X) STR_(X)
#define STR_(X) #X



#define DEFAULT_INPUT_QUEUE_LEN 10
#define TASK_NAME_SIZE configMAX_TASK_NAME_LEN + 3
#define INPUT_QUEUE_ITEM_SIZE PRINT_BUF_LEN + TASK_NAME_SIZE

static bool fs_init; // true if filesystem initialized

static xQueueHandle input_queue = NULL;
static TaskHandle_t my_handle;

const char logger_file[] = "VOL0:/syslog.log";
uint32_t logger_file_handle = 0;

uint32_t max_file_size = 4096;

uint32_t max_string_length = 100;
char* log_data[100] = {0};

static void test_logger_daemon(void *pvParameters);

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
 * Initialize filesystem related contructs for logger
 * 
 * @details
 * Opens syslog file and sets fs_init to true on success
 */
bool init_logger_fs() {
    if (fs_init) {
        return true;
    }

    int32_t fd = red_open(logger_file, RED_O_RDWR | RED_O_APPEND);
    if (fd < 0) {
        int errno = red_errno;
        fd = red_open(logger_file, RED_O_CREAT | RED_O_RDWR);
        if (fd < 0) {
            errno = red_errno;
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
 /* @brief
 *      Check if file with given name exists
 * @param filename
 *      const char * to name of file to check
 * @return 0 if exists, 1 otherwise
 */
int file_exists(const char *filename){
    int32_t file;
    if (file = red_open(filename, RED_O_CREAT | RED_O_EXCL | RED_O_RDWR) == -1){ //open file to read binary
        if (red_errno == RED_EEXIST) {
          red_close(file);
          return 0;
        }
        
    }
    red_close(file);
    red_unlink(filename);
    return 1;
}



SAT_returnState logger_service_app(csp_packet_t *packet) {
    uint8_t ser_subtype = (uint8_t)packet->data[SUBSERVICE_BYTE];
    int8_t status;
    int32_t file;
    uint32_t data_size;

    switch (ser_subtype) {
        case SET_FILE_SIZE:
            status = 0;
            memcpy(&packet->data[STATUS_BYTE], &status, sizeof(int8_t));
            set_packet_length(packet, sizeof(int8_t) + 1);  // +1 for subservice
            break;
        case GET_FILE_SIZE:
            break;
        case GET_LIST:
            break;
        case GET_FILE:
            if (file_exists(logger_file) == 0) {
                file = red_open(logger_file, RED_O_RDONLY);
                if(file > -1){
                    data_size = red_read(file, log_data, max_file_size);
                    if (data_size == 0){
                        status = -1;
                        strncpy(log_data, "Log file is empty\n", max_string_length);
                    } else {
                        status = 0;
                    }
                } else {
                    status = -1;
                    strncpy(log_data, "Can't open log file\n", max_string_length);
                }
            } else {
                status = -1;
                strncpy(log_data, "Log file does not exist\n", max_string_length);
            }
            memcpy(&packet->data[STATUS_BYTE], &status, 1);
            memcpy(&packet->data[OUT_DATA_BYTE], log_data, max_string_length);
            set_packet_length(packet, max_string_length + 2);

            break;
        default:
            ex2_log("No such subservice\n");
            return SATR_PKT_ILLEGAL_SUBSERVICE;
    }

    return SATR_OK;
}



SAT_returnState start_logger_service(void);

/**
 * @brief
 *      FreeRTOS logger server task
 * @details
 *      Accepts incoming logger service packets and executes the application
 * @param void* param
 * @return None
 */
void logger_service(void * param) {
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_RDPREQ);
    csp_bind(sock, TC_LOGGER_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN);

    for(;;) {
        csp_conn_t *conn;
        csp_packet_t *packet;
        if ((conn = csp_accept(sock, CSP_MAX_TIMEOUT)) == NULL) {
          /* timeout */
          continue;
        }
        while ((packet = csp_read(conn, 50)) != NULL) {
          if (logger_service_app(packet) != SATR_OK) {
            // something went wrong, this shouldn't happen
            csp_buffer_free(packet);
          } else {
              if (!csp_send(conn, packet, 50)) {
                  csp_buffer_free(packet);
              }
          }
        }
        csp_close(conn); //frees buffers used
    }
}

/**
 * @brief
 *      Start the logger server task
 * @details
 *      Starts the FreeRTOS task responsible for accepting incoming
 *      logger service requests
 * @param None
 * @return SAT_returnState
 *      success report
 */
SAT_returnState start_logger_service(void) {
  if (xTaskCreate((TaskFunction_t)logger_service,
                  "start_logger_service", 1200, NULL, NORMAL_SERVICE_PRIO,
                  NULL) != pdPASS) {
    ex2_log("FAILED TO CREATE TASK start_logger_service\n");
    return SATR_ERROR;
  }
  ex2_log("Service handlers started\n");
  return SATR_OK;
}
