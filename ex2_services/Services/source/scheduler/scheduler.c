/*
 * scheduler.c
 *
 *  Created on: Nov. 22, 2021
 *      Author: Grace Yi
 */

#include "scheduler/scheduler.h"

char *fileName1 = "VOL0:/gs_cmds.TMP";

static uint32_t svc_wdt_counter = 0;

static uint32_t get_svc_wdt_counter() { return svc_wdt_counter; }

int delay_aborted = 0;
int delete_task = 0;
static TaskHandle_t SchedulerHandler = 0;
static SemaphoreHandle_t scheduleSemaphore = NULL;

/**
 * @brief
 *      Private. Collect scheduled commands from the groundstation
 * @param gs_cmds
 *      pointer to the struct of all the gs commands
 * @return SAT_returnState
 *      SATR_OK or SATR_ERROR
 */
// SAT_returnState scheduler_service_app(char *gs_cmds) {
SAT_returnState scheduler_service_app(csp_packet_t *gs_cmds, SemaphoreHandle_t scheduleSemaphore) {
    uint8_t ser_subtype = (uint8_t)gs_cmds->data[SUBSERVICE_BYTE];
    int8_t status, number_of_cmds;

    if (scheduleSemaphore == NULL) {
        sys_log(ERROR, "schedulerSemaphore is NULL, therefore file system cannot be accessed safely");
        // try to re-create mutex
        scheduleSemaphore = xSemaphoreCreateMutex();
        status = MUTEX_ERROR;
        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
        return SATR_ERROR;
    }

    switch (ser_subtype) {
    case SET_SCHEDULE: {
        // allocating buffer for MAX_NUM_CMDS numbers of incoming commands
        scheduled_commands_t *cmds = (scheduled_commands_t *)calloc(MAX_NUM_CMDS, sizeof(scheduled_commands_t));
        //------------------------------------TODO: test code below, to be deleted-----------------------------------//
        //        scheduled_commands_t *cmds = NULL;
        //------------------------------------TODO: test code above, to be deleted-----------------------------------//
        if (MAX_NUM_CMDS > 0 && cmds == NULL) {
            sys_log(ERROR, "calloc for cmds failed in SET_SCHEDULE, out of memory");
            status = CALLOC_ERROR;
            memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
            return SATR_ERROR;
        }
        // parse the commands
        number_of_cmds = prv_set_scheduler(&(gs_cmds->data[SUBSERVICE_BYTE + 1]), cmds);
        if (number_of_cmds < 0) {
            sys_log(ERROR, "prv_set_scheduler failed, unable to parse commands");
            status = SSCANF_ERROR;
            memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
            return SATR_ERROR;
        }
        // calculate frequency of cmds. Non-repetitive commands have a frequency of 0
        scheduled_commands_unix_t *sorted_cmds =
            (scheduled_commands_unix_t *)calloc(number_of_cmds, sizeof(scheduled_commands_unix_t));
        if (number_of_cmds > 0 && sorted_cmds == NULL) {
            sys_log(ERROR, "calloc for sorted_cmds failed in SET_SCHEDULE, out of memory");
            free(cmds);
            status = CALLOC_ERROR;
            memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
            return SATR_ERROR;
        }
        int calc_cmd_status = calc_cmd_frequency(cmds, number_of_cmds, sorted_cmds);
        if (calc_cmd_status != SATR_OK) {
            sys_log(ERROR, "calc_cmd_ferquency failed in SET_SCHEDULE with error %d", calc_cmd_status);
            free(cmds);
            free(sorted_cmds);
            status = calc_cmd_status;
            memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
            return SATR_ERROR;
        }
            if (xSemaphoreTake(scheduleSemaphore, (TickType_t)EX2_SEMAPHORE_WAIT) == pdTRUE) {
                // open file that stores the cmds in the SD card
                int32_t fout = red_open(fileName1, RED_O_RDWR);
                // if file does not exist, create a scheduler
                if (fout < 0) {
                    fout = red_open(fileName1, RED_O_CREAT | RED_O_RDWR); // open or create file to write binary
                    if (fout < 0) {
                        sys_log(NOTICE, "unexpected error %d from red_open() in SET_SCHEDULE for file: '%s'", (int)red_errno, fileName1);
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        status = (int)red_errno;
                        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                        return SATR_ERROR;
                    }
                    // sort the commands
                    SAT_returnState sort_status = sort_cmds(sorted_cmds, number_of_cmds);
                    if (sort_status != SATR_OK) {
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        free(cmds);
                        free(sort_cmds);
                        return sort_status;
                    }
                    red_lseek(fout, 0, RED_SEEK_SET);
                    // write cmds to file
                    int32_t f_write = red_write(fout, sorted_cmds, number_of_cmds * sizeof(scheduled_commands_unix_t));
                    if (f_write < 0) {
                        sys_log(ERROR, "unexpected error %d from red_write() in SET_SCHEDULE for file: '%s'", (int)red_errno, fileName1);
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        free(cmds);
                        free(sorted_cmds);
                        status = (int)red_errno;
                        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                        return SATR_ERROR;
                    }
                    // return number of commands to groundstation
                    gs_cmds->data[OUT_DATA_BYTE] = number_of_cmds;
                    // close file
                    red_close(fout);
                    xSemaphoreGive(scheduleSemaphore);
                    // create the scheduler
                    // TODO: review stack size
                    xTaskCreate(vSchedulerHandler, "scheduler", 1000, scheduleSemaphore, NORMAL_SERVICE_PRIO, &SchedulerHandler);
                    sys_log(INFO, "xTaskHandler has been created: %d", SchedulerHandler);
                }

                // if file already exists, modify the existing scheduler
                else if (fout >= 0) {
                    // TODO: use mutex/semaphores to protect the file while being written
                    // get file size through file stats
                    REDSTAT scheduler_stat;
                    int32_t f_stat = red_fstat(fout, &scheduler_stat);
                    if (f_stat < 0) {
                        sys_log(ERROR, "unexpected error %d from f_stat() in SET_SCHEDULE for file: '%s'", (int)red_errno, fileName1);
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        status = (int)red_errno;
                        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                        return SATR_ERROR;
                    }
                    // get number of existing cmds
                    uint32_t num_existing_cmds = scheduler_stat.st_size / sizeof(scheduled_commands_unix_t);
                    int total_cmds = number_of_cmds + num_existing_cmds;
                    // TODO: use error handling to check calloc was successful
                    scheduled_commands_unix_t *existing_cmds = (scheduled_commands_unix_t *)calloc(num_existing_cmds, sizeof(scheduled_commands_unix_t));
                    if (num_existing_cmds > 0 && sorted_cmds == NULL) {
                        sys_log(ERROR, "calloc for sorted_cmds failed in SET_SCHEDULE, out of memory");
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        free(cmds);
                        free(sorted_cmds);
                        status = CALLOC_ERROR;
                        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                        return SATR_ERROR;
                    }
                    scheduled_commands_unix_t *updated_cmds = (scheduled_commands_unix_t *)calloc(total_cmds, sizeof(scheduled_commands_unix_t));
                    if (total_cmds > 0 && sorted_cmds == NULL) {
                        sys_log(ERROR, "calloc for updated_cmds failed in SET_SCHEDULE, out of memory");
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        free(cmds);
                        free(sorted_cmds);
                        free(existing_cmds);
                        status = CALLOC_ERROR;
                        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                        return SATR_ERROR;
                    }
                    // read file
                    red_lseek(fout, 0, RED_SEEK_SET);
                    int32_t f_read = red_read(fout, existing_cmds, (uint32_t)scheduler_stat.st_size);
                    if (f_read < 0) {
                        sys_log(ERROR, "unexpected error %d from red_read() in SET_SCHEDULE for file '%s", (int)red_errno, fileName1);
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        free(cmds);
                        free(sorted_cmds);
                        free(existing_cmds);
                        free(updated_cmds);
                        status = (int)red_errno;
                        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                        return SATR_ERROR;
                    }
                    // combine new commands and old commands into a single struct for sorting
                    memcpy(updated_cmds, sorted_cmds, sizeof(sorted_cmds));
                    memcpy((updated_cmds + number_of_cmds), existing_cmds, sizeof(existing_cmds));
                    SAT_returnState sort_status = sort_cmds(updated_cmds, total_cmds);
                    if (sort_status != SATR_OK) {
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        free(cmds);
                        free(sorted_cmds);
                        free(existing_cmds);
                        free(updated_cmds);
                        return sort_status;
                    }
                    // write new cmds to file
                    red_lseek(fout, 0, RED_SEEK_END);
                    uint32_t needed_size = total_cmds * sizeof(scheduled_commands_unix_t);
                    int32_t f_write = red_write(fout, updated_cmds, needed_size);
                    if (f_write < 0) {
                        sys_log(ERROR, "unexpected error %d from red_write() in SET_SCHEDULE for file '%s", (int)red_errno, fileName1);
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        free(cmds);
                        free(sorted_cmds);
                        free(existing_cmds);
                        free(updated_cmds);
                        status = (int)red_errno;
                        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                        return SATR_ERROR;
                    }
                    // return number of commands to groundstation
                    number_of_cmds = total_cmds;
                    // close file
                    red_close(fout);
                    xSemaphoreGive(scheduleSemaphore);
                    // set Abort delay flag to 1
                    delay_aborted = 1;
                    xTaskAbortDelay(SchedulerHandler);
                    // free calloc
                    free(existing_cmds);
                    free(updated_cmds);
                }
            } 
            else {
                sys_log(ERROR, "cannot obtain schedulerSemaphore, therefore file system cannot be accessed safely");
                free(cmds);
                free(sorted_cmds);
                status = MUTEX_ERROR;
                memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                return SATR_ERROR;
            }

        // free calloc
        free(cmds);
        free(sorted_cmds);

        status = NO_ERROR;
        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&gs_cmds->data[OUT_DATA_BYTE], &number_of_cmds, sizeof(int8_t));
        set_packet_length(gs_cmds, sizeof(int8_t) + 2); // plus one for sub-service
    } 
    break;

    case DELETE_SCHEDULE: {
            if (xSemaphoreTake(scheduleSemaphore, (TickType_t)EX2_SEMAPHORE_WAIT) == pdTRUE) {
                // open file from SD card
                int32_t fout = red_open(fileName1, RED_O_RDWR); // open or create file to write binary
                if (fout < 0) {
                    sys_log(ERROR, "unexpected error %d from red_open() in DELETE_SCHEDULE for file: '%s'", (int)red_errno, fileName1);
                    status = (int)red_errno;
                    memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                    return SATR_ERROR;
                }
                // if file exists, search for the command to be deleted
                else if (fout >= 0) {
                    // allocating buffer for MAX_NUM_CMDS numbers of incoming commands
                    scheduled_commands_t *cmds = (scheduled_commands_t *)calloc(MAX_NUM_CMDS, sizeof(scheduled_commands_t));
                    if (MAX_NUM_CMDS > 0 && cmds == NULL) {
                        sys_log(ERROR, "calloc for cmds failed in DELETE_SCHEDULE, out of memory");
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        status = CALLOC_ERROR;
                        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                        return SATR_ERROR;
                    }
                    // parse the commands to be deleted
                    number_of_cmds = prv_set_scheduler(&(gs_cmds->data[SUBSERVICE_BYTE + 1]), cmds);
                    if (number_of_cmds < 0) {
                        sys_log(ERROR, "prv_set_scheduler failed for DELETE_SCHEDULE, unable to parse commands");
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        free(cmds);
                        status = number_of_cmds;
                        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                        return SATR_ERROR;
                    }
                    // calculate frequency of cmds. Non-repetitive commands have a frequency of 0
                    scheduled_commands_unix_t *sorted_cmds = (scheduled_commands_unix_t *)calloc(number_of_cmds, sizeof(scheduled_commands_unix_t));
                    if (number_of_cmds > 0 && sorted_cmds == NULL) {
                        sys_log(ERROR, "calloc for sorted_cmds failed in DELETE_SCHEDULE, out of memory");
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        free(cmds);
                        status = CALLOC_ERROR;
                        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                        return SATR_ERROR;
                    }
                    int calc_cmd_status = calc_cmd_frequency(cmds, number_of_cmds, sorted_cmds);
                    if (calc_cmd_status != SATR_OK) {
                        sys_log(ERROR, "calc_cmd_ferquency failed in DELETE_SCHEDULE with error %d", calc_cmd_status);
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        free(cmds);
                        free(sorted_cmds);
                        status = calc_cmd_status;
                        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                        return SATR_ERROR;
                    }
                    // sort the cmds to be deleted
                    SAT_returnState sort_status = sort_cmds(sorted_cmds, number_of_cmds);
                    if (sort_status != SATR_OK) {
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        free(cmds);
                        free(sorted_cmds);
                        status = sort_status;
                        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                        return sort_status;
                    }
                    // get existing schedule file size through file stats
                    REDSTAT scheduler_stat;
                    red_lseek(fout, 0, RED_SEEK_SET);
                    int32_t f_stat = red_fstat(fout, &scheduler_stat);
                    if (f_stat < 0) {
                        if (f_stat < 0) {
                            sys_log(ERROR, "unexpected error %d from f_stat() in DELETE_SCHEDULE for file: '%s'", (int)red_errno, fileName1);
                            status = (int)red_errno;
                        }
                        else if (f_stat == 0) {
                            sys_log(ERROR, "no more cmds left, DELETE_SCHEDULE not executed");
                            status = DELETE_ERROR;
                        }
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        free(cmds);
                        free(sorted_cmds);
                        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                        return SATR_ERROR;
                    }
                    // get number of existing cmds
                    uint32_t num_existing_cmds = scheduler_stat.st_size / sizeof(scheduled_commands_unix_t);
                    uint32_t needed_size = (uint32_t)scheduler_stat.st_size;
                    // TODO: use error handling to check calloc was successful
                    scheduled_commands_unix_t *existing_cmds = (scheduled_commands_unix_t *)calloc(num_existing_cmds, sizeof(scheduled_commands_unix_t));
                    if (num_existing_cmds > 0 && existing_cmds == NULL) {
                        sys_log(ERROR, "calloc for existing_cmds failed in DELETE_SCHEDULE, out of memory");
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        free(cmds);
                        free(sorted_cmds);
                        status = CALLOC_ERROR;
                        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                        return SATR_ERROR;
                    }
                    scheduled_commands_unix_t *updated_cmds = (scheduled_commands_unix_t *)calloc(num_existing_cmds, sizeof(scheduled_commands_unix_t));
                    if (num_existing_cmds > 0 && updated_cmds == NULL) {
                        sys_log(ERROR, "calloc for updated_cmds failed in DELETE_SCHEDULE, out of memory");
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        free(cmds);
                        free(sorted_cmds);
                        free(existing_cmds);
                        status = CALLOC_ERROR;
                        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                        return SATR_ERROR;
                    }
                    // read file
                    red_lseek(fout, 0, RED_SEEK_SET);
                    int32_t f_read = red_read(fout, existing_cmds, (uint32_t)scheduler_stat.st_size);
                    if (f_read < 0) {
                        sys_log(ERROR, "unexpected error %d from red_read() in DELETE_SCHEDULE for file: '%s'", (int)red_errno, fileName1);
                        red_close(fout);
                        xSemaphoreGive(scheduleSemaphore);
                        free(cmds);
                        free(sorted_cmds);
                        free(existing_cmds);
                        status = (int)red_errno;
                        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                        return SATR_ERROR;
                    }
                    // search for cmds that need to be deleted and update the schedule
                    int cmd_ctr = 0;
                    for (int i = 0; i < num_existing_cmds; i++) {
                        for (int j = 0; j < number_of_cmds; j++) {
                            if (((existing_cmds + i)->unix_time == (sorted_cmds + j)->unix_time) &&
                                ((existing_cmds + i)->frequency == (sorted_cmds + j)->frequency) &&
                                ((existing_cmds + i)->dst == (sorted_cmds + j)->dst) &&
                                ((existing_cmds + i)->dport == (sorted_cmds + j)->dport) &&
                                ((existing_cmds + i)->data[SUBSERVICE_BYTE] ==
                                 (sorted_cmds + j)->data[SUBSERVICE_BYTE])) {
                                needed_size = needed_size - sizeof(scheduled_commands_unix_t);
                            } else {
                                memcpy(updated_cmds + cmd_ctr, existing_cmds + i, sizeof(scheduled_commands_unix_t));
                                cmd_ctr++;
                            }
                        }
                    }

                    // overwrite the existing cmds with updated cmds
                    red_lseek(fout, 0, RED_SEEK_SET);
                    // TODO: confirm that the file overwrites all old contents, or truncate the file to new length
                    // Write remaining cmds to file
                    if (needed_size > 0) {
                        int32_t f_write = red_write(fout, updated_cmds, needed_size);
                        if (f_write < 0) {
                            sys_log(ERROR,
                                    "unexpected error %d from red_write() in DELETE_SCHEDULE for file: '%s'",
                                    (int)red_errno, fileName1);
                            red_close(fout);
                            xSemaphoreGive(scheduleSemaphore);
                            free(cmds);
                            free(sorted_cmds);
                            free(existing_cmds);
                            free(updated_cmds);
                            status = (int)red_errno;
                            memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                            return SATR_ERROR;
                        }
                        // truncate file to new size
                        int32_t f_truc = red_ftruncate(fout, needed_size);
                        if (f_truc < 0) {
                            sys_log(ERROR, "unexpected error %d from red_ftruncate() in DELETE_SCHEDULE for file: '%s'", (int)red_errno, fileName1);
                            red_close(fout);
                            xSemaphoreGive(scheduleSemaphore);
                            free(cmds);
                            free(sorted_cmds);
                            free(existing_cmds);
                            free(updated_cmds);
                            status = (int)red_errno;
                            memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                            return SATR_ERROR;
                        }
                        // return number of commands to groundstation
                        number_of_cmds = needed_size / sizeof(scheduled_commands_unix_t);
                        // close file
                        int32_t f_close = red_close(fout);
                        if (f_close < 0) {
                            sys_log(NOTICE, "unexpected error %d from red_close() in DELETE_SCHEDULE for file: '%s'", (int)red_errno, fileName1);
                        }
                        // set Abort delay flag to 1
                        delay_aborted = 1;
                    }
                    // If all cmds were deleted, delete the file and worker task
                    else if (needed_size < sizeof(scheduled_commands_unix_t)) {
                        // terminate the worker task inside of the worker task
                        delete_task = 1;
                        // set Abort delay flag to 1
                        delay_aborted = 1;
                        // return number of commands to groundstation
                        number_of_cmds = 0;
                    }

                    // free calloc
                    free(cmds);
                    free(sorted_cmds);
                    free(existing_cmds);
                    free(updated_cmds);

                    xSemaphoreGive(scheduleSemaphore);
                    xTaskAbortDelay(SchedulerHandler);
                }
            }

        status = NO_ERROR;
        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&gs_cmds->data[OUT_DATA_BYTE], &number_of_cmds, sizeof(int8_t));
        set_packet_length(gs_cmds, sizeof(int8_t) + 2); // plus one for sub-service
    } 
    break;

    case REPLACE_SCHEDULE: {
            if (xSemaphoreTake(scheduleSemaphore, (TickType_t)EX2_SEMAPHORE_WAIT) == pdTRUE) {
                // open file from SD card
                //TODO: define fout, f_close etc before the switch cases
                int32_t fout = red_open(fileName1, RED_O_RDONLY); // open file to write binary
                xSemaphoreGive(scheduleSemaphore);
                // if file exists, delete old file
                if (fout >= 0) {
                    // terminate the worker task inside of the worker task
                    // set Abort delay flag to 1
                    red_close(fout);
                    delete_task = 1;
                    delay_aborted = 1;
                    eTaskState taskState = eTaskGetState(SchedulerHandler);
                    sys_log(INFO, "task state is %d", taskState);
                    sys_log(INFO, "xTaskHandler is %d", SchedulerHandler);
                    BaseType_t ret = xTaskAbortDelay(SchedulerHandler);
                }
            }
            if (xSemaphoreTake(scheduleSemaphore, (TickType_t)EX2_SEMAPHORE_WAIT) == pdTRUE) {
                // create file for new schedule
                int32_t fout = red_open(fileName1, RED_O_CREAT | RED_O_RDWR);
                if (fout < 0) {
                    sys_log(ERROR, "unexpected error %d from red_open() in REPLACE_SCHEDULE for file: '%s'", (int)red_errno, fileName1);
                    xSemaphoreGive(scheduleSemaphore);
                    status = (int)red_errno;
                    memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                    return SATR_ERROR;
                }
                // allocating buffer for MAX_NUM_CMDS numbers of incoming commands
                scheduled_commands_t *cmds = (scheduled_commands_t *)calloc(MAX_NUM_CMDS, sizeof(scheduled_commands_t));
                if (MAX_NUM_CMDS > 0 && cmds == NULL) {
                    sys_log(ERROR, "calloc for cmds failed in REPLACE_SCHEDULE, out of memory");
                    red_close(fout);
                    xSemaphoreGive(scheduleSemaphore);
                    status = CALLOC_ERROR;
                    memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                    return SATR_ERROR;
                }
                // parse the commands
                number_of_cmds = prv_set_scheduler(&(gs_cmds->data[SUBSERVICE_BYTE + 1]), cmds);
                if (number_of_cmds < 0) {
                    sys_log(ERROR, "prv_set_scheduler failed for REPLACE_SCHEDULE, unable to parse commands");
                    red_close(fout);
                    xSemaphoreGive(scheduleSemaphore);
                    status = SSCANF_ERROR;
                    memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                    return SATR_ERROR;
                }
                // calculate frequency of cmds. Non-repetitive commands have a frequency of 0
                scheduled_commands_unix_t *sorted_cmds = (scheduled_commands_unix_t *)calloc(number_of_cmds, sizeof(scheduled_commands_unix_t));
                if (number_of_cmds > 0 && sorted_cmds == NULL) {
                    sys_log(ERROR, "calloc for sorted_cmds failed in REPLACE_SCHEDULE, out of memory");
                    red_close(fout);
                    xSemaphoreGive(scheduleSemaphore);
                    free(cmds);
                    status = CALLOC_ERROR;
                    memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                    return SATR_ERROR;
                }
                int calc_cmd_status = calc_cmd_frequency(cmds, number_of_cmds, sorted_cmds);
                if (calc_cmd_status != SATR_OK) {
                    sys_log(ERROR, "calc_cmd_ferquency failed in REPLACE_SCHEDULE with error %d", calc_cmd_status);
                    red_close(fout);
                    xSemaphoreGive(scheduleSemaphore);
                    free(cmds);
                    free(sorted_cmds);
                    status = calc_cmd_status;
                    memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                    return SATR_ERROR;
                }
                // sort the commands
                SAT_returnState sort_status = sort_cmds(sorted_cmds, number_of_cmds);
                if (sort_status != SATR_OK) {
                    red_close(fout);
                    xSemaphoreGive(scheduleSemaphore);
                    free(cmds);
                    free(sorted_cmds);
                    return sort_status;
                }
                // write cmds to file
                red_lseek(fout, 0, RED_SEEK_SET);
                uint32_t needed_size = number_of_cmds * sizeof(scheduled_commands_unix_t);
                int32_t f_write = red_write(fout, sorted_cmds, needed_size);
                if (f_write < 0) {
                    sys_log(ERROR, "unexpected error %d from red_write() in REPLACE_SCHEDULE for file: '%s'", (int)red_errno, fileName1);
                    red_close(fout);
                    xSemaphoreGive(scheduleSemaphore);
                    free(cmds);
                    free(sorted_cmds);
                    status = (int)red_errno;
                    memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                    return SATR_ERROR;
                }
                // truncate file to new size
                red_lseek(fout, 0, RED_SEEK_SET);
                int32_t f_truc = red_ftruncate(fout, needed_size);
                if (f_truc < 0) {
                    sys_log(ERROR, "unexpected error %d from red_ftruncate() in REPLACE_SCHEDULE for file: '%s'", (int)red_errno, fileName1);
                    red_close(fout);
                    xSemaphoreGive(scheduleSemaphore);
                    free(cmds);
                    free(sorted_cmds);
                    status = (int)red_errno;
                    memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                    return SATR_ERROR;
                }
                // close file
                red_close(fout);
                xSemaphoreGive(scheduleSemaphore);
                // create the scheduler worker task
                // TODO: review stack size
                xTaskCreate(vSchedulerHandler, "scheduler", 1000, scheduleSemaphore, NORMAL_SERVICE_PRIO, &SchedulerHandler);
            } 
            else {
                sys_log(ERROR, "cannot obtain schedulerSemaphore, therefore file system cannot be accessed safely");
                status = MUTEX_ERROR;
                memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                return SATR_ERROR;
            }
       
        status = NO_ERROR;
        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&gs_cmds->data[OUT_DATA_BYTE], &number_of_cmds, sizeof(int8_t));
        set_packet_length(gs_cmds, sizeof(int8_t) + 2); // plus one for sub-service
    } 
    break;

    case PING_SCHEDULE: {
        // open file from SD card
        int32_t f_ping = red_open(fileName1, RED_O_RDONLY); // open file to read binary
        // if file exists, delete old file
        if (f_ping >= 0) {
            // get file size through file stats
            REDSTAT scheduler_stat;
            int32_t f_stat = red_fstat(f_ping, &scheduler_stat);
            if (f_stat < 0) {
                sys_log(NOTICE, "unexpected error %d from f_stat() in PING_SCHEDULE for file: '%s'", (int)red_errno, fileName1);
                red_close(f_ping);
                status = (int)red_errno;
                memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
                return SATR_ERROR;
            }
            number_of_cmds = scheduler_stat.st_size / sizeof(scheduled_commands_unix_t);
        } else {
            number_of_cmds = 0;
        }
        red_close(f_ping);
        status = NO_ERROR;
        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&gs_cmds->data[OUT_DATA_BYTE], &number_of_cmds, sizeof(int8_t));
        set_packet_length(gs_cmds, 2 * sizeof(int8_t) + 1); // plus one for sub-service
    } 
    break;

    default:
        sys_log(ERROR, "No such subservice");
        status = SATR_PKT_ILLEGAL_SUBSERVICE;
        number_of_cmds = 0;
        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
        memcpy(&gs_cmds->data[OUT_DATA_BYTE], &status, sizeof(int8_t));
        set_packet_length(gs_cmds, 2 * sizeof(int8_t) + 1); // plus one for sub-service
        return SATR_PKT_ILLEGAL_SUBSERVICE;
    }

    /*consider if struct should hold error codes returned from these functions*/
    return SATR_OK;
}

/*------------------------------Private-------------------------------------*/

/**
 * @brief
 *      Parse and store groundstation commands from the buffer to the array @param cmds
 * @param cmd_buff
 *      pointer to the buffer that stores the groundstation commands
 * @return Result
 *      FAILURE or SUCCESS
 */
int prv_set_scheduler(char *cmd_buff, scheduled_commands_t *cmds) {
    int number_of_cmds = 0;
    // Parse the commands
    // Initialize counters that point to different locations in the string of commands
    int old_str_position = 0;
    int str_position_1 = 0;
    int str_position_2 = 0;

    while (number_of_cmds < MAX_NUM_CMDS) {
        // A carraige followed by a space or nothing indicates there is no more commands
        // TODO: determine if this is the best way to detect the end of the gs cmd script
        if (cmd_buff[old_str_position] == '\0') {
            break;
        } else {
            int buf_scanf;
            char *buf_string = &cmd_buff[old_str_position];
            int f_scanf = sscanf(buf_string, "%d", &buf_scanf);
            if (f_scanf != 1) {
                break;
            }
        }
        // Count the number of spaces before the scheduled time
        while (cmd_buff[str_position_2] == ' ') {
            str_position_2++;
        }
        // Advance the pointers to the first digit of scheduled time
        old_str_position = str_position_2;
        str_position_1 = str_position_2;

        /*-----------------------Fetch time in milliseconds-----------------------*/
        // Count the number of digits
        while (cmd_buff[str_position_1] != ' ') {
            str_position_1++;
        }
        str_position_2 = str_position_1;
        // Count the number of spaces following the digits
        while (cmd_buff[str_position_2] == ' ') {
            str_position_2++;
        }
        // Scan the value
        if (cmd_buff[str_position_1 - 1] == '*') {
            (cmds + number_of_cmds)->milliseconds = ASTERISK;
        } else {
            int buf_scanf;
            char *buf_string = &cmd_buff[old_str_position];
            int f_scanf = sscanf(buf_string, "%d", &buf_scanf);
            if (f_scanf != 1) {
                sys_log(NOTICE, "unable to scan time in milliseconds for command: %d", number_of_cmds + 1);
                // Default ms to 0 if scanf fails, ms provides more accuracy but is not vital to the scheduler
                (cmds + number_of_cmds)->milliseconds = 0;
            } else {
                (cmds + number_of_cmds)->milliseconds = (uint8_t)buf_scanf;
            }
        }

        old_str_position = str_position_2;
        str_position_1 = str_position_2;
        str_position_1++;

        /*-----------------------Fetch time in seconds-----------------------*/
        // Count the number of digits
        while (cmd_buff[str_position_1] != ' ') {
            str_position_1++;
        }
        str_position_2 = str_position_1;
        // Count the number of spaces following the digits
        while (cmd_buff[str_position_2] == ' ') {
            str_position_2++;
        }
        // Scan the value
        if (cmd_buff[str_position_1 - 1] == '*') {
            (cmds + number_of_cmds)->scheduled_time.Second = ASTERISK;
        } else {
            int buf_scanf;
            char *buf_string = &cmd_buff[old_str_position];
            int f_scanf = sscanf(buf_string, "%d", &buf_scanf);
            if (f_scanf != 1) {
                sys_log(ERROR, "unable to scan time in Seconds for command: %d", number_of_cmds + 1);
                return SSCANF_ERROR;
            }
            (cmds + number_of_cmds)->scheduled_time.Second = (uint8_t)buf_scanf;
        }

        old_str_position = str_position_2;
        str_position_1 = str_position_2;
        str_position_1++;

        /*-----------------------Fetch time in minutes-----------------------*/
        // Count the number of digits
        while (cmd_buff[str_position_1] != ' ') {
            str_position_1++;
        }
        str_position_2 = str_position_1;
        // Count the number of spaces following the digits
        while (cmd_buff[str_position_2] == ' ') {
            str_position_2++;
        }
        // Scan the value
        if (cmd_buff[str_position_1 - 1] == '*') {
            (cmds + number_of_cmds)->scheduled_time.Minute = ASTERISK;
        } else {
            int buf_scanf;
            char *buf_string = &cmd_buff[old_str_position];
            int f_scanf = sscanf(buf_string, "%d", &buf_scanf);
            if (f_scanf != 1) {
                sys_log(ERROR, "unable to scan time in Minute for command: %d", number_of_cmds + 1);
                return SSCANF_ERROR;
            }
            (cmds + number_of_cmds)->scheduled_time.Minute = (uint8_t)buf_scanf;
        }

        old_str_position = str_position_2;
        str_position_1 = str_position_2;
        str_position_1++;

        /*-----------------------Fetch time in hour-----------------------*/
        // Count the number of digits
        while (cmd_buff[str_position_1] != ' ') {
            str_position_1++;
        }
        str_position_2 = str_position_1;
        // Count the number of spaces following the digits
        while (cmd_buff[str_position_2] == ' ') {
            str_position_2++;
        }
        // Scan the value
        if (cmd_buff[str_position_1 - 1] == '*') {
            (cmds + number_of_cmds)->scheduled_time.Hour = ASTERISK;
        } else {
            int buf_scanf;
            char *buf_string = &cmd_buff[old_str_position];
            int f_scanf = sscanf(buf_string, "%d", &buf_scanf);
            if (f_scanf != 1) {
                sys_log(ERROR, "unable to scan time in Hour for command: %d", number_of_cmds + 1);
                return SSCANF_ERROR;
            }
            (cmds + number_of_cmds)->scheduled_time.Hour = (uint8_t)buf_scanf;
        }

        old_str_position = str_position_2;
        str_position_1 = str_position_2;
        str_position_1++;

        /*-----------------------Fetch the Wday-----------------------*/
        // Count the number of digits
        while (cmd_buff[str_position_1] != ' ') {
            str_position_1++;
        }
        str_position_2 = str_position_1;
        // Count the number of spaces following the digits
        while (cmd_buff[str_position_2] == ' ') {
            str_position_2++;
        }
        // Scan the value
        if (cmd_buff[str_position_1 - 1] == '*') {
            (cmds + number_of_cmds)->scheduled_time.Wday = ASTERISK;
        } else {
            int buf_scanf;
            char *buf_string = &cmd_buff[old_str_position];
            int f_scanf = sscanf(buf_string, "%d", &buf_scanf);
            if (f_scanf != 1) {
                sys_log(ERROR, "unable to scan time in Wday for command: %d", number_of_cmds + 1);
                return SSCANF_ERROR;
            }
            (cmds + number_of_cmds)->scheduled_time.Wday = (uint8_t)buf_scanf;
        }

        old_str_position = str_position_2;
        str_position_1 = str_position_2;
        str_position_1++;

        /*-----------------------Fetch the Day-----------------------*/
        // Count the number of digits
        while (cmd_buff[str_position_1] != ' ') {
            str_position_1++;
        }
        str_position_2 = str_position_1;
        // Count the number of spaces following the digits
        while (cmd_buff[str_position_2] == ' ') {
            str_position_2++;
        }
        // Scan the value
        if (cmd_buff[str_position_1 - 1] == '*') {
            (cmds + number_of_cmds)->scheduled_time.Day = ASTERISK;
        } else {
            int buf_scanf;
            char *buf_string = &cmd_buff[old_str_position];
            int f_scanf = sscanf(buf_string, "%d", &buf_scanf);
            if (f_scanf != 1) {
                sys_log(ERROR, "unable to scan time in Day for command: %d", number_of_cmds + 1);
                return SSCANF_ERROR;
            }
            (cmds + number_of_cmds)->scheduled_time.Day = (uint8_t)buf_scanf;
        }

        old_str_position = str_position_2;
        str_position_1 = str_position_2;
        str_position_1++;

        /*-----------------------Fetch the month-----------------------*/
        // Count the number of digits
        while (cmd_buff[str_position_1] != ' ') {
            str_position_1++;
        }
        str_position_2 = str_position_1;
        // Count the number of spaces following the digits
        while (cmd_buff[str_position_2] == ' ') {
            str_position_2++;
        }
        // Scan the value
        if (cmd_buff[str_position_1 - 1] == '*') {
            (cmds + number_of_cmds)->scheduled_time.Month = ASTERISK;
        } else {
            int buf_scanf;
            char *buf_string = &cmd_buff[old_str_position];
            int f_scanf = sscanf(buf_string, "%d", &buf_scanf);
            if (f_scanf != 1) {
                sys_log(ERROR, "unable to scan time in Month for command: %d", number_of_cmds + 1);
                return SSCANF_ERROR;
            }
            (cmds + number_of_cmds)->scheduled_time.Month = (uint8_t)buf_scanf;
        }

        old_str_position = str_position_2;
        str_position_1 = str_position_2;
        str_position_1++;

        /*-----------------------Fetch the number of years since 1970-----------------------*/
        // Count the number of digits
        while (cmd_buff[str_position_1] != ' ') {
            str_position_1++;
        }
        str_position_2 = str_position_1;
        // Count the number of spaces following the digits
        while (cmd_buff[str_position_2] == ' ') {
            str_position_2++;
        }
        // Scan the value
        if (cmd_buff[str_position_1 - 1] == '*') {
            (cmds + number_of_cmds)->scheduled_time.Year = ASTERISK;
        } else {
            int buf_scanf;
            char *buf_string = &cmd_buff[old_str_position];
            int f_scanf = sscanf(buf_string, "%d", &buf_scanf);
            if (f_scanf != 1) {
                sys_log(ERROR, "unable to scan time in Year for command: %d", number_of_cmds + 1);
                return SSCANF_ERROR;
            }
            (cmds + number_of_cmds)->scheduled_time.Year = (uint8_t)buf_scanf;
        }

        // advance pointers to the first byte of the next field
        old_str_position = str_position_2;
        str_position_1 = str_position_2;

        /*-----------------------Fetch the CSP dst(1 byte), dport(1byte), and data length(2 bytes)-----------------------*/
        // copy the dst
        uint8_t dst = cmd_buff[str_position_2];
        // advance pointer to dport
        str_position_2++;
        uint8_t dport = cmd_buff[str_position_2];
        // advance pointer to first byte of length
        str_position_2++;
        uint16_t highByte = cmd_buff[str_position_2];
        // advance pointer to second byte of length
        str_position_2++;
        uint16_t lowByte = cmd_buff[str_position_2];
        // combine length bytes
        uint16_t embeddedLength = (highByte << 8) | lowByte;

        // advance pointers to embedded data field
        str_position_2++;
        old_str_position = str_position_2;
        str_position_1 = str_position_2;

        /*-----------------------Fetch command as am embedded CSP packet-----------------------*/
        // get CSP buffer in order to reconstruct the embedded CSP packet
        (cmds + number_of_cmds)->length = embeddedLength;
        (cmds + number_of_cmds)->dst = dst;
        (cmds + number_of_cmds)->dport = dport;
        memcpy(&((cmds + number_of_cmds)->data), &(cmd_buff[str_position_2]), embeddedLength);

        // advance the pointers to read the next line of command
        str_position_2 += embeddedLength;
        old_str_position = str_position_2;
        str_position_1 = str_position_2;

        /*
        if (cmd_buff[str_position_2] == '\n') {
            //TODO: discuss whether this is the best way to log error
            ex2_log("command is empty for command: %d\n", number_of_cmds+1);
        }*/

        number_of_cmds++;
    }

    return number_of_cmds;
}

/**
 * @brief
 *      For non-repetitive commands: turn scheduled time into unix time, and set frequency of command to 0
 *      For repetitive commands: calculate the unix time of the first execution, and the frequency each command
 * will be executed at
 * @param cmds
 *      pointer to the structure that stores the parsed groundstation commands
 * @param number_of_cmds
 *      number of commands in @param cmds
 * @param sorted_cmds
 *      pointer to the structure that stores the groundstation commands after frequency has been calculated
 * @return int
 *      SATR_OK or other error codes
 */
int calc_cmd_frequency(scheduled_commands_t *cmds, int number_of_cmds, scheduled_commands_unix_t *sorted_cmds) {
    /*--------------------------------Initialize structures to store sorted commands--------------------------------*/
    // TODO: Confirm that the entire struct has been initialized with zeros
    scheduled_commands_unix_t *non_reoccurring_cmds =
        (scheduled_commands_unix_t *)calloc(number_of_cmds, sizeof(scheduled_commands_unix_t));
    if (number_of_cmds > 0 && non_reoccurring_cmds == NULL) {
        sys_log(ERROR, "calloc for non_reoccurring_cmds failed, out of memory");
        return CALLOC_ERROR;
    }
    scheduled_commands_t *reoccurring_cmds =
        (scheduled_commands_t *)calloc(number_of_cmds, sizeof(scheduled_commands_t));
    if (number_of_cmds > 0 && reoccurring_cmds == NULL) {
        sys_log(ERROR, "calloc for reoccurring_cmds failed, out of memory");
        free(non_reoccurring_cmds);
        return CALLOC_ERROR;
    }
    num_of_cmds.non_rep_cmds = 0;
    num_of_cmds.rep_cmds = 0;

    int unix_time_buff;
    int j_rep = 0;
    int j_non_rep = 0;

    for (int j = 0; j < number_of_cmds; j++) {
        // Separate the non-repetitve and repetitve commands, the sum of time fields should not exceed the value of
        // ASTERISK (255) if non-repetitive
        if ((cmds + j)->scheduled_time.Second + (cmds + j)->scheduled_time.Minute + (cmds + j)->scheduled_time.Hour + (cmds + j)->scheduled_time.Month <
            ASTERISK) {

            // Store the non-repetitve commands into the new struct non_reoccurring_cmds
            if (j_non_rep < number_of_cmds) {
                // Convert the time into unix time for sorting convenience
                unix_time_buff = makeTime((cmds + j)->scheduled_time);
                (non_reoccurring_cmds + j_non_rep)->unix_time = unix_time_buff;
                // Copy the address of the embedded CSP packet to the non_reoccurring_cmds list
                (non_reoccurring_cmds + j_non_rep)->milliseconds = (cmds + j)->milliseconds;
                (non_reoccurring_cmds + j_non_rep)->frequency = 0; // set frequency to zero for non-repetitive cmds
                (non_reoccurring_cmds + j_non_rep)->length = (cmds + j)->length;
                (non_reoccurring_cmds + j_non_rep)->dst = (cmds + j)->dst;
                (non_reoccurring_cmds + j_non_rep)->dport = (cmds + j)->dport;
                memcpy(&((non_reoccurring_cmds + j_non_rep)->data), &((cmds + j)->data), (cmds + j)->length);
                j_non_rep++;
            }
        }

        else if (j_rep < number_of_cmds) {
            // Store the repetitve commands into the new struct reoccurring_cmds
            memcpy(reoccurring_cmds + j_rep, cmds + j, sizeof(scheduled_commands_t));
            j_rep++;
        }
    }

    // update the number of commands
    num_of_cmds.non_rep_cmds = j_non_rep;
    num_of_cmds.rep_cmds = j_rep;

    /*--------------------------------calculate the frequency of repeated cmds--------------------------------*/
    static tmElements_t time_buff;
    // TODO: check that all callocs have been freed
    scheduled_commands_unix_t *repeated_cmds_buff =
        (scheduled_commands_unix_t *)calloc(j_rep, sizeof(scheduled_commands_unix_t));
    if (j_rep > 0 && repeated_cmds_buff == NULL) {
        sys_log(ERROR, "calloc for repeated_cmds_buff failed, out of memory");
        free(non_reoccurring_cmds);
        free(reoccurring_cmds);
        return CALLOC_ERROR;
    }
    // Obtain the soonest time that the command will be executed, and calculate the frequency it needs to be
    // executed at
    for (int j = 0; j < j_rep; j++) {
        time_buff.Wday = (reoccurring_cmds + j)->scheduled_time.Wday;
        time_buff.Month = (reoccurring_cmds + j)->scheduled_time.Month;
        (repeated_cmds_buff + j)->milliseconds = (reoccurring_cmds + j)->milliseconds;
        (repeated_cmds_buff + j)->length = (reoccurring_cmds + j)->length;
        (repeated_cmds_buff + j)->dst = (reoccurring_cmds + j)->dst;
        (repeated_cmds_buff + j)->dport = (reoccurring_cmds + j)->dport;
        memcpy(&((repeated_cmds_buff + j)->data), &((reoccurring_cmds + j)->data), (reoccurring_cmds + j)->length);
        int rtc_read_hr, rtc_read_min, rtc_read_sec;
        rtc_read_hr = rtc_read_min = rtc_read_sec = 0;
        // If command repeats every second
        if ((reoccurring_cmds + j)->scheduled_time.Hour == ASTERISK &&
            (reoccurring_cmds + j)->scheduled_time.Minute == ASTERISK &&
            (reoccurring_cmds + j)->scheduled_time.Second == ASTERISK) {
            // TODO: consider edge cases where the hour increases as soon as this function is executed - complete
            rtc_read_hr = RTCMK_ReadHours(RTCMK_ADDR, &time_buff.Hour);
            rtc_read_min = RTCMK_ReadMinutes(RTCMK_ADDR, &time_buff.Minute);
            rtc_read_sec = RTCMK_ReadSeconds(RTCMK_ADDR, &time_buff.Second);
            // convert the first execution time into unix time. Add 60 seconds to allow processing time
            if (rtc_read_hr < 0 || rtc_read_min < 0 || rtc_read_sec < 0) {
                (repeated_cmds_buff + j)->unix_time = 0;
                sys_log(NOTICE, "RTCMK_READ failed, execution unix time is set to 0");
            } else {
                (repeated_cmds_buff + j)->unix_time = makeTime(time_buff) + 60;
            }
            (repeated_cmds_buff + j)->frequency = 1; // 1 second
            continue;
        }
        // If command repeats every minute
        if (((reoccurring_cmds + j)->scheduled_time.Hour == ASTERISK &&
            (reoccurring_cmds + j)->scheduled_time.Minute == ASTERISK) || 
            (reoccurring_cmds + j)->scheduled_time.Minute == ASTERISK) {
            if ((reoccurring_cmds + j)->scheduled_time.Hour != ASTERISK) {
                (reoccurring_cmds + j)->scheduled_time.Hour = ASTERISK;
            }
            // TODO: consider edge cases where the hour increases as soon as this function is executed - complete
            rtc_read_hr = RTCMK_ReadHours(RTCMK_ADDR, &time_buff.Hour);
            rtc_read_min = RTCMK_ReadMinutes(RTCMK_ADDR, &time_buff.Minute);
            time_buff.Second = (reoccurring_cmds + j)->scheduled_time.Second;
            // convert the first execution time into unix time. Add 60 seconds to allow processing time
            if (rtc_read_hr < 0 || rtc_read_min < 0) {
                (repeated_cmds_buff + j)->unix_time = 0;
                sys_log(NOTICE, "RTCMK_READ failed, execution unix time is set to 0");
            } else {
                (repeated_cmds_buff + j)->unix_time = makeTime(time_buff) + 60;
            }
            (repeated_cmds_buff + j)->frequency = 60; // 1 min
            continue;
        }
        // If command repeats every hour
        if ((reoccurring_cmds + j)->scheduled_time.Hour == ASTERISK) {
            // TODO: consider edge cases where the hour increases as soon as this function is executed - complete
            rtc_read_hr = RTCMK_ReadHours(RTCMK_ADDR, &time_buff.Hour);
            time_buff.Minute = (reoccurring_cmds + j)->scheduled_time.Minute;
            time_buff.Second = (reoccurring_cmds + j)->scheduled_time.Second;
            // convert the first execution time into unix time. If the hour is almost over, increase the hour by
            // one
            if (rtc_read_hr < 0) {
                (repeated_cmds_buff + j)->unix_time = 0;
                sys_log(NOTICE, "RTCMK_READ failed, execution unix time is set to 0");
            } else {
                time_t scheduled_time = makeTime(time_buff);
                time_t current_time;
                if (scheduled_time - RTCMK_GetUnix(&current_time) < 60) {
                    (repeated_cmds_buff + j)->unix_time = scheduled_time + 3600;
                } else {
                    (repeated_cmds_buff + j)->unix_time = scheduled_time;
                }
            }
            (repeated_cmds_buff + j)->frequency = 3600; // 1 hr
            continue;
        }
    }

    /*--------------------------------Combine non-repetitive and repetitive commands into a single struct--------------------------------*/
    memcpy(sorted_cmds, repeated_cmds_buff, sizeof(scheduled_commands_unix_t) * j_rep);
    memcpy((sorted_cmds + j_rep), non_reoccurring_cmds, sizeof(scheduled_commands_unix_t) * j_non_rep);

    // free calloc
    free(non_reoccurring_cmds);
    free(reoccurring_cmds);
    free(repeated_cmds_buff);

    return SATR_OK;
}

/**
 * @brief
 *      Sort groundstation commands from the lowest unix time to highest unix time
 * @param sorted_cmds
 *      pointer to the structure that stores the groundstation commands
 * @param number_of_cmds
 *      number of commands in the struct @param sorted_cmds
 * @return SAT_returnState
 *      SATR_OK or SATR_ERROR
 */
SAT_returnState sort_cmds(scheduled_commands_unix_t *sorted_cmds, int number_of_cmds) {
    /*--------------------------------Sort the list using selection sort--------------------------------*/
    if (number_of_cmds > 1) {
        scheduled_commands_unix_t sorting_buff;
        int ptr1, ptr2, min_ptr;
        for (ptr1 = 0; ptr1 < number_of_cmds; ptr1++) {
            for (ptr2 = ptr1 + 1; ptr2 < number_of_cmds; ptr2++) {
                // find minimum unix time
                if ((sorted_cmds + ptr1)->unix_time < (sorted_cmds + ptr2)->unix_time) {
                    min_ptr = ptr1;
                } else {
                    min_ptr = ptr2;
                }
            }
            // swap the minimum with the current
            if ((sorted_cmds + ptr1)->unix_time != (sorted_cmds + min_ptr)->unix_time) {
                memcpy(&sorting_buff, sorted_cmds + ptr1, sizeof(scheduled_commands_unix_t));
                memcpy((sorted_cmds + ptr1), (sorted_cmds + min_ptr), sizeof(scheduled_commands_unix_t));
                memcpy(sorted_cmds + min_ptr, &sorting_buff, sizeof(scheduled_commands_unix_t));
            }
        }
    }

    return SATR_OK;
}

/*//TODO: is a watchdog needed?
static uint32_t svc_wdt_counter = 0;
static uint32_t get_svc_wdt_counter() { return svc_wdt_counter; }
SAT_returnState start_gs_cmds_scheduler_service(void);
*/

/**
 * @brief
 *      Start scheduler service
 * @details
 *      Starts the FreeRTOS task responsible for accepting incoming
 *      schedule packets
 * @param None
 * @return SAT_returnState
 *      success report
 */
SAT_returnState start_scheduler_service(void) {
    TaskHandle_t svc_tsk;
    taskFunctions svc_funcs = {0};
    svc_funcs.getCounterFunction = get_svc_wdt_counter;
    // create mutex to protect file system
    scheduleSemaphore = xSemaphoreCreateMutex();

    if (xTaskCreate((TaskFunction_t)scheduler_service, "scheduler_service", SCHEDULER_SIZE, scheduleSemaphore,
                    NORMAL_SERVICE_PRIO, &svc_tsk) != pdPASS) {
        sys_log(ERROR, "FAILED TO CREATE TASK scheduler_service\n");
        return SATR_ERROR;
    }
    ex2_register(svc_tsk, svc_funcs);
    sys_log(NOTICE, "Scheduler service started\n");
    return SATR_OK;
}

/**
 * @brief
 *      FreeRTOS gs scheduler server task
 * @details
 *      Accepts incoming gs scheduler packets and executes the application
 * @param void* param
 * @return SAT_returnState
 */
SAT_returnState scheduler_service(SemaphoreHandle_t scheduleSemaphore) {
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_NONE);
    csp_bind(sock, TC_SCHEDULER_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN); // TODO: SERVICE_BACKLOG_LEN constant TBD
    // svc_wdt_counter++;

    for (;;) {
        csp_conn_t *conn;
        csp_packet_t *packet;

        if ((conn = csp_accept(sock, CSP_MAX_TIMEOUT)) == NULL) {
            /* timeout */
            // ex2_log("woke up");
            continue;
        }
        // TODO: is a watchdog needed?
        // svc_wdt_counter++;

        while ((packet = csp_read(conn, 50)) != NULL) {
            if (scheduler_service_app(packet, scheduleSemaphore) != SATR_OK) {
                int number_of_cmds = -1;
                memcpy(&packet->data[OUT_DATA_BYTE], &number_of_cmds, sizeof(int8_t));
                set_packet_length(packet, 2 * sizeof(int8_t) + 1); // plus one for sub-service
            }
            if (!csp_send(conn, packet, 50)) {
                csp_buffer_free(packet);
            }
        }
        csp_close(conn); // frees buffers used
    }
}
