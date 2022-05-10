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
 * @file scheduler_task.c
 * @author Grace Yi
 * @date Jan. 26, 2022
 */

#include "scheduler/scheduler_task.h"

char* fileName2 = "VOL0:/cmd_history.TMP";
char* fileName3 = "VOL0:/hist_count.TMP";

extern int delay_aborted;

/**
 * Command scheduler
 *
 * @param pvParameters
 *    task parameters (not used)
 * @return SAT_returnState
 *      SATR_OK or SATR_ERROR
 */
SAT_returnState vSchedulerHandler (void *pvParameters) {
    TickType_t xLastWakeTime;
    uint8_t hist_file_position = 0;

    // open file from SD card
    int32_t fout = red_open(fileName1, RED_O_CREAT | RED_O_RDWR);
    if (fout < 0) {
        printf("Unexpected error %d from red_open()\r\n", (int)red_errno);
        ex2_log("Failed to open or create file to write: '%s'\r\n", fileName1);
        vTaskDelete(0);
        return SATR_ERROR;
    }
    // get file size through file stats
    REDSTAT scheduler_stat;
    int32_t f_stat = red_fstat(fout, &scheduler_stat);
    if (f_stat < 0) {
        printf("Unexpected error %d from f_stat()\r\n", (int)red_errno);
        ex2_log("Failed to read file stats from: '%s'\r\n", fileName1);
        red_close(fout);
        return SATR_ERROR;
    }
    // close file from SD card
    int32_t f_close = red_close(fout); 
    if (f_close < 0) {
        printf("Unexpected error %d from red_close()\r\n", (int)red_errno);
        ex2_log("Failed to close file: '%s'\n", fileName1);
        return SATR_ERROR;
    }
    // while the scheduler has cmds scheduled
    while (scheduler_stat.st_size > 0) {
        // initialize buffer to read the commands
        uint32_t number_of_cmds = scheduler_stat.st_size / sizeof(scheduled_commands_unix_t);
        // calloc initializes each block with a default value of 0
        scheduled_commands_unix_t* cmds = (scheduled_commands_unix_t*)calloc(number_of_cmds,sizeof(scheduled_commands_unix_t));
        if (number_of_cmds > 0 && cmds == NULL) {
            ex2_log("calloc failed, out of memory");
            vTaskDelete(0);
            return SATR_ERROR;
        }

        // open file from SD card
        fout = red_open(fileName1, RED_O_RDWR); // open or create file to write binary
        if (fout < 0) {
            printf("Unexpected error %d from red_open()\r\n", (int)red_errno);
            ex2_log("Failed to open or create file to write: '%s'\n", fileName1);
            vTaskDelete(0);
            return SATR_ERROR;
        }
        // read file
        int64_t newFilePosition = red_lseek(fout, 0, RED_SEEK_SET);
        int32_t f_read = red_read(fout, cmds, (uint32_t)scheduler_stat.st_size);
        if (f_read < 0) {
            printf("Unexpected error %d from red_read()\r\n", (int)red_errno);
            ex2_log("Failed to read file: '%s'\r\n", fileName1);
            red_close(fout);
            return SATR_ERROR;
        }
        // close file from SD card
        f_close = red_close(fout);
        if (f_close < 0) {
            printf("Unexpected error %d from red_close()\r\n", (int)red_errno);
            ex2_log("Failed to close file: '%s'\r\n", fileName1);
            return SATR_ERROR;
        }
        // construct the embedded CSP packet
        csp_packet_t *packet = csp_buffer_get((size_t)(cmds->length + 2));  // +2 for subservice and error
        packet->id.dst = cmds->dst;
        packet->id.dport = cmds->dport;
        packet->length = cmds->length;
        memcpy(&(packet->data), &(cmds->data), cmds->length);
        // get current unix time
        time_t current_time;
        RTCMK_GetUnix(&current_time);
        // calculate delay until the next command
        int delay_time = (cmds)->unix_time - current_time; //in seconds
        TickType_t delay_ticks = pdMS_TO_TICKS(1000 * delay_time); //in # of ticks

        // get the freeRTOS time
        xLastWakeTime = xTaskGetTickCount();

        /*-------------------------------wait until it's time to execute the command--------------------------------*/
        if (delay_time >= 0) {
            vTaskDelayUntil( &xLastWakeTime, delay_ticks );
        }

        // if the delay was aborted due to updated schedule
        while (delay_aborted == 1) {
            // set Abort delay flag to 0
            delay_aborted = 0;
            // free cmds buffer since a new one need to be allocated
            free(cmds);
            // open file from SD card
            fout = red_open(fileName1, RED_O_RDWR); // open or create file to write binary
            if (fout < 0) {
                printf("Unexpected error %d from red_open()\r\n", (int)red_errno);
                ex2_log("Failed to open or create file to write: '%s'\r\n", fileName1);
                return SATR_ERROR;
            }
            // get file size through file stats
            f_stat = red_fstat(fout, &scheduler_stat);
            if (f_stat < 0) {
                printf("Unexpected error %d from f_stat()\r\n", (int)red_errno);
                ex2_log("Failed to read file stats from: '%s'\r\n", fileName1);
                red_close(fout);
                return SATR_ERROR;
            }
            // initialize buffer to read the commands
            uint32_t updated_num_cmds = scheduler_stat.st_size / sizeof(scheduled_commands_unix_t);
            // calloc initializes each block with a default value of 0
            scheduled_commands_unix_t* cmds = (scheduled_commands_unix_t*)calloc(updated_num_cmds,sizeof(scheduled_commands_unix_t));
            if (updated_num_cmds > 0 && cmds == NULL) {
                ex2_log("calloc for cmds failed, out of memory");
                vTaskDelete(0);
                return SATR_ERROR;
            }
            // read file
            red_lseek(fout, 0, RED_SEEK_SET);
            f_read = red_read(fout, &cmds, (uint32_t)scheduler_stat.st_size);
            if (f_read < 0) {
                printf("Unexpected error %d from red_read()\r\n", (int)red_errno);
                ex2_log("Failed to read file: '%s'\n", fileName1);
                red_close(fout);
                return SATR_ERROR;
            }
            // re-construct the embedded CSP packet
            csp_buffer_free(packet);
            packet = csp_buffer_get((size_t)(cmds->length + 2));  // +2 for subservice and error
            packet->id.dst = cmds->dst;
            packet->id.dport = cmds->dport;
            packet->length = cmds->length;
            memcpy(&(packet->data), &(cmds->data), cmds->length);
            // get current unix time
            RTCMK_GetUnix(&current_time);
            // calculate delay until the next command
            delay_time = current_time - (cmds)->unix_time; //in seconds
            delay_ticks = pdMS_TO_TICKS(1000 * delay_time); //in # of ticks
            // get the freeRTOS time
            xLastWakeTime = xTaskGetTickCount();
            // close file from SD card
            f_close = red_close(fout); 
            if (f_close < 0) {
                printf("Unexpected error %d from red_close()\r\n", (int)red_errno);
                ex2_log("Failed to close file: '%s'\r\n", fileName1);
                return SATR_ERROR;
            }
            vTaskDelayUntil( &xLastWakeTime, delay_ticks );
        }

        /*------------------------------------keep a history of executed commands------------------------------------*/
        // store only the useful information into history, discard things like CSP id and padding

        /*------------------------------------execute the scheduled cmds with CSP------------------------------------*/

        csp_conn_t *connect;

        connect = csp_connect(CSP_PRIO_NORM, packet->id.dst, packet->id.dport, CSP_MAX_TIMEOUT, CSP_SO_NONE);

        int send_packet_test = csp_send(connect, packet, CSP_MAX_TIMEOUT);
        // TODO: find out if csp_send frees the packet, is so, the free packet above is not neededs
        if (send_packet_test != 1) {
            csp_buffer_free(packet);
        }
        int close_connection_test = csp_close(connect);

        // TODO: keep a log of the executed cmd, the max length of each log entry is defined with STRING_MAX_LEN in logger.h

        /*-------------------------------TODO: test code below, delete after testing-----------------------------------*/
        RTCMK_GetUnix(&current_time);
        /*-------------------------------TODO: test code above, delete after testing-----------------------------------*/

        /*------------------------------------update history with executed command status------------------------------------*/
        // TODO: update history in ex2_log

//        // open file that stores the cmd history in the SD card
//        int32_t hout = red_open(fileName2, RED_O_RDONLY | RED_O_RDWR);
//        // if file does not exist, create one
//        if (hout < 0) {
//            hout = red_open(fileName2, RED_O_CREAT | RED_O_RDWR); // open or create file to write binary
//
//            // write the exectued command to cmd history
//            uint32_t needed_size = sizeof(schedule_history_t);
//            red_errno = 0;
//            int32_t h_write = red_write(hout, &schedule_history, needed_size);
//            if (red_errno != 0) {
//                ex2_log("Failed to write to file: '%s'\r\n", fileName2);
//            }
//
//            // keep track of the # of commands
//            int32_t cout = red_open(fileName3, RED_O_CREAT | RED_O_RDWR);
//            hist_file_position++;
//            red_errno = 0;
//            int32_t c_write = red_write(cout, &hist_file_position, sizeof(hist_file_position));
//            if (red_errno != 0) {
//                ex2_log("Failed to write to file: '%s'\r\n", fileName3);
//            }
//
//            // close file
//            red_close(hout);
//            red_close(cout);
//        }
//
//        // if file already exists, add to the existing history
//        else if (hout >= 0 ) {
//            // open and read the history counter
//            int32_t cout = red_open(fileName3, RED_O_RDONLY | RED_O_RDWR);
//            if (cout >= 0) {
//                // read file
//                red_lseek(cout, 0, 0);
//                int32_t c_read = red_read(cout, &hist_file_position, sizeof(hist_file_position));
//                if (c_read < 0) {
//                    printf("Unexpected error %d from red_read()\r\n", (int)red_errno);
//                    ex2_log("Failed to read file: '%s'\r\n", fileName3);
//                }
//            }
//            else if (cout < 0) {
//                ex2_log("Failed to open file: '%s'\n", fileName3);
//            }
//
//            // set the file offset using the history counter
//            red_lseek(hout, hist_file_position, 0);
//
//            // write the executed command to cmd history
//            uint32_t needed_size = sizeof(schedule_history_t) + schedule_history.length;
//            red_errno = 0;
//            red_write(hout, &schedule_history, needed_size);
//            if (red_errno != 0) {
//                ex2_log("Failed to write to file: '%s'\r\n", fileName2);
//            }
//
//            // update the history counter, reset to 1 if 100 commands have been exceeded
//            hist_file_position++;
//            if (hist_file_position > SCHEDULE_HISTORY_SIZE) {
//                hist_file_position = 1;
//            }
//            red_lseek(cout, 0, 0);
//            red_write(cout, &hist_file_position, sizeof(hist_file_position));
//            if (red_errno != 0) {
//                ex2_log("Failed to write to file: '%s'\r\n", fileName3);
//            }
//
//            // close file
//            red_close(hout);
//            red_close(cout);
//        }

        /*---------------------------------prepare the scheduler for the next command--------------------------------*/
        // open file from SD card
        fout = red_open(fileName1, RED_O_RDWR); // open or create file to write binary
        if (fout < 0) {
            printf("Unexpected error %d from red_open()\r\n", (int)red_errno);
            ex2_log("Failed to open or create file to write: '%s'\r\n", fileName1);
            return SATR_ERROR;
        }

        // if the command is repetitive, add its next execution to the scheduler
        if ((cmds->frequency) != 0) {
            cmds->unix_time = cmds->unix_time + cmds->frequency;
            // re-sort the updated scheduler
            sort_cmds(cmds, number_of_cmds);
            // reset the file offset to the start of file
            red_lseek(fout, 0, 0);
            // update the file
            red_write(fout, &cmds, (uint32_t)scheduler_stat.st_size);
            if (red_errno != 0) {
                ex2_log("Failed to write to file: '%s'\r\n", fileName1);
                red_close(fout);
                return SATR_ERROR;
            }
            // close file
            f_close = red_close(fout); 
            if (f_close < 0) {
                printf("Unexpected error %d from red_close()\r\n", (int)red_errno);
                ex2_log("Failed to close file: '%s'\r\n", fileName1);
                return SATR_ERROR;
            }
        }

        // if the command is non-repetitive, delete the command that has been executed, and overwrite file with remaining commands
        if ((cmds->frequency) == 0) {
            red_errno = 0;
            uint32_t needed_size = (uint32_t)scheduler_stat.st_size - sizeof(scheduled_commands_unix_t);
            if (needed_size > 0) {
                // reset the file offset to start of file
                red_lseek(fout, 0, RED_SEEK_SET);
                //TODO: confirm that the file overwrites all old contents, or truncate the file to new length
                int32_t f_write = red_write(fout, cmds+1, needed_size);
                if (red_errno != 0) {
                    ex2_log("Failed to write to file: '%s'\r\n", fileName1);
                    red_close(fout);
                    return SATR_ERROR;
                }
                // truncate file to new size
                int32_t f_truc = red_ftruncate(fout, needed_size);
                if (red_errno != 0) {
                    ex2_log("Failed to truncate file: '%s'\r\n", fileName1);
                    red_close(fout);
                    return SATR_ERROR;
                }
                // update the file size
                f_stat = red_fstat(fout, &scheduler_stat);
                if (f_stat < 0) {
                    printf("Unexpected error %d from f_stat()\r\n", (int)red_errno);
                    ex2_log("Failed to read file stats from: '%s'\r\n", fileName1);
                    red_close(fout);
                    return SATR_ERROR;
                }
                // close file
                f_close = red_close(fout); 
                if (f_close < 0) {
                    printf("Unexpected error %d from red_close()\r\n", (int)red_errno);
                    ex2_log("Failed to close file: '%s'\r\n", fileName1);
                    return SATR_ERROR;
                }
            }

            // if this is the last command, delete the file
            else if (needed_size == 0) {
                // update the file size
                scheduler_stat.st_size = needed_size;
                // close file
                f_close = red_close(fout); 
                if (f_close < 0) {
                    printf("Unexpected error %d from red_close()\r\n", (int)red_errno);
                    ex2_log("Failed to close file: '%s'\r\n", fileName1);
                    return SATR_ERROR;
                }
                // delete file once all cmds have been executed
                int32_t f_delete = red_unlink(fileName1);
                if (f_delete < 0) {
                    printf("Unexpected error %d from f_delete()\r\n", (int)red_errno);
                    ex2_log("Failed to close file: '%s'\r\n", fileName1);
                    return SATR_ERROR;
                }
            }
        }
        // free up the stack
        free(cmds);
    }
    // self destruct when there are no more commands left
    vTaskDelete(0);
    return SATR_OK;
}
