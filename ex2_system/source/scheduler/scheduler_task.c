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

char *fileName2 = "VOL0:/cmd_history.TMP";
char *fileName3 = "VOL0:/hist_count.TMP";

extern int delay_aborted;

/**
 * Command scheduler
 *
 * @param pvParameters
 *    scheduleSemaphore, which is the mutex used for protecting the file system from being accessed by multiple
 * threads
 */
void vSchedulerHandler(SemaphoreHandle_t scheduleSemaphore) {
    TickType_t xLastWakeTime;
    int32_t fout, f_stat, f_read, f_write, f_close;
    REDSTAT scheduler_stat;

    // open file from SD card
    if (xSemaphoreTake(scheduleSemaphore, (TickType_t)EX2_SEMAPHORE_WAIT) == pdTRUE) {
        fout = red_open(fileName1, RED_O_CREAT | RED_O_RDWR);
        if (fout < 0) {
            sys_log(ERROR, "vSchedulerHandler failed on error %d from red_open() for file: '%s'", (int)red_errno,
                    fileName1);
            xSemaphoreGive(scheduleSemaphore);
            vTaskDelete(0);
        }
        // get file size through file stats
        f_stat = red_fstat(fout, &scheduler_stat);
        if (f_stat < 0) {
            sys_log(ERROR, "vSchedulerHandler failed on error %d from red_fstat() for file: '%s'", (int)red_errno,
                    fileName1);
            red_close(fout);
            xSemaphoreGive(scheduleSemaphore);
            vTaskDelete(0);
        }
        // close file from SD card
        f_close = red_close(fout);
        if (f_close < 0) {
            sys_log(NOTICE, "error %d from red_close() in vSchedulerHandler for file: '%s'", (int)red_errno,
                    fileName1);
        }
        xSemaphoreGive(scheduleSemaphore);
    }
    // while the scheduler has cmds scheduled
    while (scheduler_stat.st_size >= sizeof(scheduled_commands_unix_t)) {
        // initialize buffer to read the commands
        uint32_t number_of_cmds = scheduler_stat.st_size / sizeof(scheduled_commands_unix_t);
        scheduled_commands_unix_t *cmds =
            (scheduled_commands_unix_t *)pvPortMalloc(number_of_cmds * sizeof(scheduled_commands_unix_t));
        memset(cmds, 0, number_of_cmds * sizeof(scheduled_commands_unix_t));
        if (number_of_cmds > 0 && cmds == NULL) {
            sys_log(ERROR, "pvPortMalloc for cmds failed in vSchedulerHandler, out of memory");
            xSemaphoreGive(scheduleSemaphore);
            vTaskDelete(0);
        }
        // open file from SD card
        if (xSemaphoreTake(scheduleSemaphore, (TickType_t)EX2_SEMAPHORE_WAIT) == pdTRUE) {
            fout = red_open(fileName1, RED_O_RDWR); // open or create file to write binary
            if (fout < 0) {
                sys_log(ERROR, "vSchedulerHandler failed on error %d from red_open() for file: '%s'",
                        (int)red_errno, fileName1);
                vPortFree(cmds);
                xSemaphoreGive(scheduleSemaphore);
                vTaskDelete(0);
            }
            // read file
            red_lseek(fout, 0, RED_SEEK_SET);
            f_read = red_read(fout, cmds, number_of_cmds * sizeof(scheduled_commands_unix_t));
            if (f_read < 0) {
                sys_log(ERROR, "vSchedulerHandler failed on error %d from red_read() for file: '%s'",
                        (int)red_errno, fileName1);
                red_close(fout);
                vPortFree(cmds);
                xSemaphoreGive(scheduleSemaphore);
                vTaskDelete(0);
            }
            // close file from SD card
            f_close = red_close(fout);
            if (f_close < 0) {
                sys_log(NOTICE, "error %d from red_close() in vSchedulerHandler for file: '%s'", (int)red_errno,
                        fileName1);
            }
            xSemaphoreGive(scheduleSemaphore);
        }
        // construct the embedded CSP packet
        csp_packet_t *packet = csp_buffer_get((size_t)(cmds->length + 2)); // +2 for subservice and error
        packet->id.dst = cmds->dst;
        packet->id.dport = cmds->dport;
        packet->length = cmds->length;
        memcpy(&(packet->data), &(cmds->data), cmds->length);
        // keep a log of executed cmds. TODO: use pvPortMalloc for this
        char *args = (char *)pvPortMalloc(cmds->length + 1);
        if (args == NULL) {
            sys_log(NOTICE, "pvPortMalloc for args failed in vSchedulerHandler, out of memory");
        } else {
            memcpy(args, &(cmds->data[IN_DATA_BYTE]), cmds->length - 1);
            args[cmds->length] = '\0';
        }
        // char args_buffer[MAX_CMD_LENGTH];
        // int buff_size = MAX_CMD_LENGTH;
        // int args_return = snprintf(args_buffer, buff_size, "cmd argument is %d");
        // get current unix time
        time_t current_time;
        if (RTCMK_GetUnix(&current_time) < 0) {
            sys_log(ERROR, "unable to get current time");
            vPortFree(cmds);
            csp_buffer_free(packet);
            vTaskDelete(0);
        }
        // get milliseconds from the RTC (only include this if ms accuracy is required)
        int RTC_ms = RTCMK_GetMs();
        // calculate delay until the next command
        int delay_time = cmds->unix_time - current_time; // in seconds
        if (delay_time < 0) {
            sys_log(NOTICE, "scheduled time is invalid or in the past");
        }
        TickType_t delay_ticks =
            pdMS_TO_TICKS(1000 * delay_time) - RTC_ms + cmds->milliseconds; // in # of ticks, ie. milliseconds

        // get the freeRTOS time
        xLastWakeTime = xTaskGetTickCount();

        /*-------------------------------wait until it's time to execute the
         * command--------------------------------*/
        if (delay_time >= 0) {
            vTaskDelayUntil(&xLastWakeTime, delay_ticks);
        }
        sys_log(INFO, "vTaskDelayUntil executed\n");

        // if the delay was aborted due to updated schedule
        while (delay_aborted == 1) {
            sys_log(INFO, "vTaskDelayUntil was aborted");
            // set Abort delay flag to 0
            delay_aborted = 0;
            vPortFree(cmds);
            if (args != NULL) {
                vPortFree(args);
            }
            // if delete_task flag is 1, free all pvPortMalloc and gracefully self-destruct
            if (delete_task == 1) {
                delete_task = 0;
                sys_log(INFO, "vSchedulerHandler self-destructed");
                int32_t f_delete = red_unlink(fileName1);
                csp_buffer_free(packet);
                vTaskDelete(0);
            }
            // open file from SD card
            if (xSemaphoreTake(scheduleSemaphore, (TickType_t)EX2_SEMAPHORE_WAIT) == pdTRUE) {
                fout = red_open(fileName1, RED_O_RDWR); // open or create file to write binary
                if (fout < 0) {
                    sys_log(ERROR, "vSchedulerHandler failed on error %d from red_open() for file: '%s'",
                            (int)red_errno, fileName1);
                    xSemaphoreGive(scheduleSemaphore);
                    vTaskDelete(0);
                }
                // get file size through file stats
                f_stat = red_fstat(fout, &scheduler_stat);
                if (f_stat < 0) {
                    sys_log(ERROR, "vSchedulerHandler failed on error %d from red_fstat() for file: '%s'",
                            (int)red_errno, fileName1);
                    red_close(fout);
                    xSemaphoreGive(scheduleSemaphore);
                    vTaskDelete(0);
                }
                // initialize buffer to read the commands
                uint32_t updated_num_cmds = scheduler_stat.st_size / sizeof(scheduled_commands_unix_t);
                scheduled_commands_unix_t *cmds = (scheduled_commands_unix_t *)pvPortMalloc(
                    updated_num_cmds * sizeof(scheduled_commands_unix_t));
                memset(cmds, 0, updated_num_cmds * sizeof(scheduled_commands_unix_t));
                if (updated_num_cmds > 0 && cmds == NULL) {
                    sys_log(ERROR, "pvPortMalloc for cmds failed in vSchedulerHandler, out of memory");
                    red_close(fout);
                    xSemaphoreGive(scheduleSemaphore);
                    vTaskDelete(0);
                }
                // read file
                red_lseek(fout, 0, RED_SEEK_SET);
                f_read = red_read(fout, cmds, updated_num_cmds * sizeof(scheduled_commands_unix_t));
                if (f_read < 0) {
                    sys_log(ERROR, "vSchedulerHandler failed on error %d from red_read() for file: '%s'",
                            (int)red_errno, fileName1);
                    red_close(fout);
                    xSemaphoreGive(scheduleSemaphore);
                    vTaskDelete(0);
                }
                // close file from SD card
                f_close = red_close(fout);
                if (f_close < 0) {
                    sys_log(NOTICE, "error %d from red_close() in vSchedulerHandler for file: '%s'",
                            (int)red_errno, fileName1);
                }
                xSemaphoreGive(scheduleSemaphore);
            }
            // re-construct the embedded CSP packet
            csp_buffer_free(packet);
            packet = csp_buffer_get((size_t)(cmds->length + 2)); // +2 for subservice and error
            packet->id.dst = cmds->dst;
            packet->id.dport = cmds->dport;
            packet->length = cmds->length;
            memcpy(&(packet->data), &(cmds->data), cmds->length);
            // keep a log of executed cmds. TODO: use pvPortMalloc for this
            char *args = (char *)pvPortMalloc(cmds->length + 1);
            if (args == NULL) {
                sys_log(NOTICE, "pvPortMalloc for args failed in vSchedulerHandler, out of memory");
            } else {
                memcpy(args, &(cmds->data[IN_DATA_BYTE]), cmds->length - 1);
                args[cmds->length] = '\0';
            }
            // get current unix time
            RTCMK_GetUnix(&current_time);
            // calculate delay until the next command
            delay_time = current_time - cmds->unix_time; // in seconds
            if (delay_time < 0) {
                sys_log(NOTICE, "scheduled time is invalid or in the past");
            }
            delay_ticks = pdMS_TO_TICKS(1000 * delay_time); // in # of ticks
            // get the freeRTOS time
            xLastWakeTime = xTaskGetTickCount();

            if (delay_time > 0) {
                vTaskDelayUntil(&xLastWakeTime, delay_ticks);
            }
        }

        /*------------------------------------execute the scheduled cmds with
         * CSP------------------------------------*/

        // if cmds->unix_time is 0, it means a problem occurred with the RTC and the unix time is not valid, skip
        // to the next command
        if (cmds->unix_time > 0) {
            csp_conn_t *connect;

            connect = csp_connect(CSP_PRIO_NORM, packet->id.dst, packet->id.dport, CSP_MAX_TIMEOUT, CSP_SO_NONE);

            int send_embeddedPacket = csp_send(connect, packet, CSP_MAX_TIMEOUT);
            // TODO: find out if csp_send frees the packet, is so, the free packet above is not neededs
            if (send_embeddedPacket != 1) {
                sys_log(NOTICE, "failed to send embedded packet");
                csp_buffer_free(packet);
            }
            int close_connection = csp_close(connect);

            /*-------------------------------TODO: test code below, delete after
             * testing-----------------------------------*/
            RTCMK_GetUnix(&current_time);
            /*-------------------------------TODO: test code above, delete after
             * testing-----------------------------------*/

            /*------------------------------------keep a log of executed
             * commands------------------------------------*/
            // store only the useful information into history in ex2_log, discard things like CSP id and padding
            // the max length of each log entry is defined with STRING_MAX_LEN in logger.h
            if (args != NULL) {
                sys_log(INFO,
                        "cmd executed: unix_time %d, milliseconds %d, frequency %d , dst %d, dport %d, subservice "
                        "%d, data '%s'",
                        cmds->unix_time, cmds->milliseconds, cmds->frequency, cmds->dst, cmds->dport, args);
                vPortFree(args);
            }
        } else {
            sys_log(NOTICE, "cmd not executed due to invalid time, skipping to the next cmd");
        }

        /*---------------------------------prepare the scheduler for the next
         * command--------------------------------*/
        // open file from SD card
        if (xSemaphoreTake(scheduleSemaphore, (TickType_t)EX2_SEMAPHORE_WAIT) == pdTRUE) {
            fout = red_open(fileName1, RED_O_RDWR); // open or create file to write binary
            if (fout < 0) {
                sys_log(ERROR, "vSchedulerHandler failed on error %d from red_open() for file: '%s'",
                        (int)red_errno, fileName1);
                xSemaphoreGive(scheduleSemaphore);
                vPortFree(cmds);
                vTaskDelete(0);
            }

            // if the command is repetitive, add its next execution to the scheduler
            if ((cmds->frequency) != 0 && (cmds->unix_time) > 0) {
                cmds->unix_time = cmds->unix_time + cmds->frequency;
                // re-sort the updated scheduler
                if (sort_cmds(cmds, number_of_cmds) != SATR_OK) {
                    sys_log(ERROR, "vSchedulerHandler sorting failed for file: '%s'", fileName1);
                    red_close(fout);
                    xSemaphoreGive(scheduleSemaphore);
                    vPortFree(cmds);
                    vTaskDelete(0);
                }
                // reset the file offset to the start of file
                red_lseek(fout, 0, RED_SEEK_SET);
                // update the file
                f_write = red_write(fout, cmds, (uint32_t)scheduler_stat.st_size);
                if (f_write < 0) {
                    sys_log(ERROR, "failed to write to file: '%s' in vSchedulerHandler for file: '%s'",
                            (int)red_errno, fileName1);
                    red_close(fout);
                    xSemaphoreGive(scheduleSemaphore);
                    vPortFree(cmds);
                    vTaskDelete(0);
                }
                // close file
                f_close = red_close(fout);
                if (f_close < 0) {
                    sys_log(NOTICE, "error %d from red_close() in vSchedulerHandler for file: '%s'",
                            (int)red_errno, fileName1);
                }
            }

            // if the command is non-repetitive or has an invalid unix time, delete the current command, and
            // overwrite file with remaining commands
            else {
                red_errno = 0;
                uint32_t needed_size = (uint32_t)scheduler_stat.st_size - sizeof(scheduled_commands_unix_t);
                if (needed_size > 0) {
                    // reset the file offset to start of file
                    red_lseek(fout, 0, RED_SEEK_SET);
                    // TODO: confirm that the file overwrites all old contents, or truncate the file to new length
                    int32_t f_write = red_write(fout, cmds + 1, needed_size);
                    if (f_write < 0) {
                        sys_log(ERROR, "vSchedulerHandler failed on error %d from red_write() for file: '%s'",
                                (int)red_errno, fileName1);
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        vPortFree(cmds);
                        vTaskDelete(0);
                    }
                    // truncate file to new size
                    int32_t f_truc = red_ftruncate(fout, needed_size);
                    if (f_truc < 0) {
                        sys_log(NOTICE, "error %d from red_ftruncate() in vSchedulerHandler for file: '%s'",
                                (int)red_errno, fileName1);
                    }
                    // update the file size
                    red_lseek(fout, 0, RED_SEEK_SET);
                    f_stat = red_fstat(fout, &scheduler_stat);
                    if (f_stat < 0) {
                        sys_log(ERROR, "vSchedulerHandler failed on error %d from red_fstat() for file: '%s'",
                                (int)red_errno, fileName1);
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        vPortFree(cmds);
                        vTaskDelete(0);
                    }
                    // close file
                    f_close = red_close(fout);
                    if (f_close < 0) {
                        sys_log(ERROR, "vSchedulerHandler failed on error %d from red_close() for file: '%s'",
                                (int)red_errno, fileName1);
                        xSemaphoreGive(scheduleSemaphore);
                        vPortFree(cmds);
                        vTaskDelete(0);
                    }
                }

                // if this is the last command, delete the file
                else if (needed_size <= 0) {
                    // update the file size
                    scheduler_stat.st_size = needed_size;
                    // close file
                    f_close = red_close(fout);
                    if (f_close < 0) {
                        sys_log(NOTICE, "error %d from red_close() in vSchedulerHandler for file: '%s'",
                                (int)red_errno, fileName1);
                    }
                    // delete file once all cmds have been executed
                    int32_t f_delete = red_unlink(fileName1);
                    if (f_delete < 0) {
                        sys_log(ERROR, "vSchedulerHandler failed on error %d from red_unlink() for file: '%s'",
                                (int)red_errno, fileName1);
                        xSemaphoreGive(scheduleSemaphore);
                        vPortFree(cmds);
                        vTaskDelete(0);
                    }
                }
            }
            xSemaphoreGive(scheduleSemaphore);
        }
        // free up the stack
        vPortFree(cmds);
    }
    // self destruct when there are no more commands left
    vTaskDelete(0);
}
