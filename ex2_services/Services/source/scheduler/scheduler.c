/*
 * scheduler.c
 *
 *  Created on: Nov. 22, 2021
 *      Author: Grace Yi
 */

#include "scheduler/scheduler.h"

char* fileName1 = "VOL0:/gs_cmds.TMP";

static uint32_t svc_wdt_counter = 0;

static uint32_t get_svc_wdt_counter() { return svc_wdt_counter; }

int delay_aborted = 0;
static TaskHandle_t SchedulerHandler;

/**
 * @brief
 *      Private. Collect scheduled commands from the groundstation
 * @param gs_cmds
 *      pointer to the struct of all the gs commands
 * @return SAT_returnState
 *      SATR_OK or SATR_ERROR
 */
//SAT_returnState scheduler_service_app(char *gs_cmds) {
SAT_returnState scheduler_service_app(csp_packet_t *gs_cmds) {
    uint8_t ser_subtype = (uint8_t)gs_cmds->data[SUBSERVICE_BYTE];
    //uint8_t ser_subtype = (uint8_t)gs_cmds[0];
    int8_t status;

    switch(ser_subtype) {
    case SET_SCHEDULE:
        {
        // allocating buffer for MAX_NUM_CMDS numbers of incoming commands
        scheduled_commands_t *cmds = (scheduled_commands_t*)calloc(MAX_NUM_CMDS, sizeof(scheduled_commands_t));
        if (MAX_NUM_CMDS > 0 && cmds == NULL) {
            ex2_log("calloc for cmds failed, out of memory");
            return SATR_ERROR;
        }
        // parse the commands
        int number_of_cmds = prv_set_scheduler(&(gs_cmds->data[SUBSERVICE_BYTE+1]), cmds);
        // calculate frequency of cmds. Non-repetitive commands have a frequency of 0
        scheduled_commands_unix_t *sorted_cmds = (scheduled_commands_unix_t*)calloc(number_of_cmds, sizeof(scheduled_commands_unix_t));
        if (number_of_cmds > 0 && sorted_cmds == NULL) {
            ex2_log("calloc for sorted_cmds failed, out of memory");
            free(cmds);
            return SATR_ERROR;
        }
        calc_cmd_frequency(cmds, number_of_cmds, sorted_cmds);
        // open file that stores the cmds in the SD card
        int32_t fout = red_open(fileName1, RED_O_RDONLY | RED_O_RDWR);

        // if file does not exist, create a scheduler
        if (fout == -1) {
            fout = red_open(fileName1, RED_O_CREAT | RED_O_RDWR); // open or create file to write binary
            // sort the commands
            sort_cmds(sorted_cmds, number_of_cmds);
            // write cmds to file
            int32_t f_write = write_cmds_to_file(fout, sorted_cmds, number_of_cmds, fileName1);
            if (f_write < 0) {
                return SATR_ERROR;
            }
            // close file
            red_close(fout);
            // create the scheduler
            //TODO: review stack size
            xTaskCreate(vSchedulerHandler, "scheduler", 1000, NULL, NORMAL_SERVICE_PRIO, &SchedulerHandler);
        }

        // if file already exists, modify the existing scheduler
        else if (fout != -1) {
            // TODO: use mutex/semaphores to protect the file while being written
            // get file size through file stats
            REDSTAT scheduler_stat;
            int32_t f_stat = red_fstat(fout, &scheduler_stat);
            if (f_stat == -1) {
                printf("Unexpected error %d from f_stat()\r\n", (int)red_errno);
                ex2_log("Failed to read file stats from: '%s'\n", fileName1);
                red_close(fout);
                return SATR_ERROR;
            }
            // get number of existing cmds
            uint32_t num_existing_cmds = scheduler_stat.st_size / sizeof(scheduled_commands_unix_t);
            int total_cmds = number_of_cmds + num_existing_cmds;
            // TODO: use error handling to check calloc was successful
            scheduled_commands_unix_t *existing_cmds = (scheduled_commands_unix_t*)calloc(num_existing_cmds, sizeof(scheduled_commands_unix_t));
            if (num_existing_cmds > 0 && sorted_cmds == NULL) {
                ex2_log("calloc for sorted_cmds failed, out of memory");
                free(cmds);
                free(sorted_cmds);
                return SATR_ERROR;
            }
            scheduled_commands_unix_t *updated_cmds = (scheduled_commands_unix_t*)calloc(total_cmds, sizeof(scheduled_commands_unix_t));
            if (total_cmds > 0 && sorted_cmds == NULL) {
                ex2_log("calloc for sorted_cmds failed, out of memory");
                free(cmds);
                free(sorted_cmds);
                free(existing_cmds);
                return SATR_ERROR;
            }
            // read file
            int32_t f_read = red_read(fout, &existing_cmds, (uint32_t)scheduler_stat.st_size);
            if (f_read == -1) {
                printf("Unexpected error %d from red_read()\r\n", (int)red_errno);
                ex2_log("Failed to read file: '%s'\n", fileName1);
                red_close(fout);
                return SATR_ERROR;
            }
            // combine new commands and old commands into a single struct for sorting
            memcpy(&updated_cmds,&sorted_cmds,sizeof(sorted_cmds));
            memcpy((updated_cmds+number_of_cmds),&existing_cmds,sizeof(existing_cmds));
            sort_cmds(updated_cmds, total_cmds);
            // write new cmds to file
            write_cmds_to_file(fout, updated_cmds, total_cmds, fileName1);
            // close file
            red_close(fout);
            // set Abort delay flag to 1
            if (xTaskAbortDelay(SchedulerHandler) == pdPASS) {
                delay_aborted = 1;
            }
            // free calloc
            free(existing_cmds);
            free(updated_cmds);
        }
    
        // free calloc
        free(cmds);
        free(sorted_cmds);

        status = 0;
        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(gs_cmds, sizeof(int8_t) + 1); // plus one for sub-service
        }
        break;
    
    case GET_SCHEDULE:
        {
        //TODO: develop a way to translate schedule back 
        int32_t fout = red_open(fileName1, RED_O_RDWR); // open or create file to write binary
        if (fout < 0) {
            printf("Unexpected error %d from red_open()\r\n", (int)red_errno);
            ex2_log("Failed to open or create file to reset: '%s'\n", fileName1);
            return SATR_ERROR;
        }

        // if file exists, reset the existing scheduler
        else if (fout >= 0) {
            // get file size through file stats
            REDSTAT scheduler_stat;
            int32_t f_stat = red_fstat(fout, &scheduler_stat);
            if (f_stat < 0) {
                printf("Unexpected error %d from f_stat()\r\n", (int)red_errno);
                ex2_log("Failed to read file stats from: '%s'\r\n", fileName1);
                red_close(fout);
                return SATR_ERROR;
            }
            // allocate buffer to store schedule
            int num_existing_cmds = scheduler_stat.st_size / sizeof(scheduled_commands_unix_t);
            scheduled_commands_unix_t *cmds = (scheduled_commands_unix_t*)calloc(num_existing_cmds, sizeof(scheduled_commands_unix_t));
            if (num_existing_cmds > 0 && cmds == NULL) {
                ex2_log("calloc for cmds failed, out of memory");
                return SATR_ERROR;
            }
            red_lseek(fout, 0, RED_SEEK_SET);
            // read schedule from file
            int32_t f_read = red_read(fout, cmds, (uint32_t)scheduler_stat.st_size);
            if (f_read < 0) {
                printf("Unexpected error %d from red_read()\r\n", (int)red_errno);
                ex2_log("Failed to read file: '%s'\r\n", fileName1);
                red_close(fout);
                return SATR_ERROR;
            }
            // extract dst, dport, length, and data from each embedded CSP packet
            get_schedule_t *get_schedule = (get_schedule_t*)calloc(num_existing_cmds, sizeof(get_schedule_t));
            if (num_existing_cmds > 0 && get_schedule == NULL) {
                ex2_log("calloc for get_schedule failed, out of memory");
                free(cmds);
                return SATR_ERROR;
            }
            for (int i = 0; i < num_existing_cmds; i++) {
                (get_schedule+i)->unix_time = (cmds+i)->unix_time;
                (get_schedule+i)->frequency = (cmds+i)->frequency;
                (get_schedule+i)->dst = (cmds+i)->dst;
                (get_schedule+i)->dport = (cmds+i)->dport;
                (get_schedule+i)->length = (cmds+i)->length;
                memcpy(&((cmds+i)->data), &((get_schedule+i)->data), ((get_schedule+i)->length));
            }
        }

        status = 0;
        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(gs_cmds, sizeof(int8_t) + 1); // plus one for sub-service
        }
        break;

    case DELETE_SCHEDULE:
        {
        // open file from SD card
        int32_t fout = red_open(fileName1, RED_O_RDWR); // open or create file to write binary
        if (fout < 0) {
            printf("Unexpected error %d from red_open()\r\n", (int)red_errno);
            ex2_log("Failed to open or create file to reset: '%s'\n", fileName1);
            return SATR_ERROR;
        }

        // if file exists, reset the existing scheduler
        else if (fout >= 0) {
            // allocating buffer for MAX_NUM_CMDS numbers of incoming commands
            scheduled_commands_t *cmds = (scheduled_commands_t*)calloc(MAX_NUM_CMDS, sizeof(scheduled_commands_t));
            if (MAX_NUM_CMDS > 0 && cmds == NULL) {
                ex2_log("calloc for cmds failed, out of memory");
                return SATR_ERROR;
            }
            // parse the commands to be deleted
            int number_of_cmds = prv_set_scheduler(&(gs_cmds->data[SUBSERVICE_BYTE+1]), cmds);
            // calculate frequency of cmds. Non-repetitive commands have a frequency of 0
            scheduled_commands_unix_t *sorted_cmds = (scheduled_commands_unix_t*)calloc(number_of_cmds, sizeof(scheduled_commands_unix_t));
            if (number_of_cmds > 0 && sorted_cmds == NULL) {
                ex2_log("calloc for sorted_cmds failed, out of memory");
                free(cmds);
                return SATR_ERROR;
            }
            calc_cmd_frequency(cmds, number_of_cmds, sorted_cmds);
            // sort the cmds to be deleted
            sort_cmds(sorted_cmds, number_of_cmds);
            // get existing schedule file size through file stats
            REDSTAT scheduler_stat;
            int32_t f_stat = red_fstat(fout, &scheduler_stat);
            if (f_stat == -1) {
                printf("Unexpected error %d from f_stat()\r\n", (int)red_errno);
                ex2_log("Failed to read file stats from: '%s'\n", fileName1);
                red_close(fout);
                return SATR_ERROR;
            }
            // get number of existing cmds
            uint32_t num_existing_cmds = scheduler_stat.st_size / sizeof(scheduled_commands_unix_t);
            uint32_t needed_size = (uint32_t)scheduler_stat.st_size;
            // TODO: use error handling to check calloc was successful
            scheduled_commands_unix_t *existing_cmds = (scheduled_commands_unix_t*)calloc(num_existing_cmds, sizeof(scheduled_commands_unix_t));
            if (num_existing_cmds > 0 && existing_cmds == NULL) {
                ex2_log("calloc for existing_cmds failed, out of memory");
                free(cmds);
                free(sorted_cmds);
                return SATR_ERROR;
            }
            scheduled_commands_unix_t *updated_cmds = (scheduled_commands_unix_t*)calloc(num_existing_cmds, sizeof(scheduled_commands_unix_t));
            if (num_existing_cmds > 0 && updated_cmds == NULL) {
                ex2_log("calloc for updated_cmds failed, out of memory");
                free(cmds);
                free(sorted_cmds);
                free(existing_cmds);
                return SATR_ERROR;
            }
            // read file
            int32_t f_read = red_read(fout, &existing_cmds, (uint32_t)scheduler_stat.st_size);
            if (f_read == -1) {
                printf("Unexpected error %d from red_read()\r\n", (int)red_errno);
                ex2_log("Failed to read file: '%s'\n", fileName1);
                red_close(fout);
                return SATR_ERROR;
            }
            // search for cmds that need to be deleted and update the schedule
            for (int i = 0; i < number_of_cmds; i++) {
                for (int j = 0; j < num_existing_cmds; j++) {
                    int cmd_ctr = 0;
                    if (((existing_cmds+j)->unix_time == (sorted_cmds+i)->unix_time) && 
                        ((existing_cmds+j)->frequency == (sorted_cmds+i)->frequency) &&
                        ((existing_cmds+j)->dst == (sorted_cmds+i)->dst) &&
                        ((existing_cmds+j)->dport == (sorted_cmds+i)->dport) &&
                        ((existing_cmds+j)->data[SUBSERVICE_BYTE] == (sorted_cmds+i)->data[SUBSERVICE_BYTE])) {
                        needed_size = needed_size - sizeof(scheduled_commands_unix_t);
                    }
                    else {
                        memcpy(updated_cmds+cmd_ctr, existing_cmds+j, sizeof(scheduled_commands_unix_t));
                        cmd_ctr++;
                    }
                }
            }
            // overwrite the existing cmds with updated cmds
            // reset the file offset to start of file
            red_lseek(fout, 0, RED_SEEK_SET);
            //TODO: confirm that the file overwrites all old contents, or truncate the file to new length
            int32_t f_write = red_write(fout, updated_cmds, needed_size);
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
            // close file
            red_close(fout);
            // free calloc
            free(existing_cmds);
            free(updated_cmds);
            // set Abort delay flag to 1
            if (xTaskAbortDelay(SchedulerHandler) == pdPASS) {
                delay_aborted = 1;
            }

            // free calloc
            free(cmds);
            free(sorted_cmds);
        }

        }
        break;

    case REPLACE_SCHEDULE:
        {
        vTaskDelete(SchedulerHandler);
        // open file from SD card
        int32_t fout = red_open(fileName1, RED_O_RDONLY); // open or create file to write binary
        // if file exists, delete old file
        if (fout >= 0) {
            red_close(fout);
            // delete file once all cmds have been executed
            int32_t f_delete = red_unlink(fileName1);
            if (f_delete < 0) {
                printf("Unexpected error %d from f_delete()\r\n", (int)red_errno);
                ex2_log("Failed to close file: '%s'\r\n", fileName1);
                return SATR_ERROR;
            }
        }

        // create file for new schedule
        fout = red_open(fileName1, RED_O_CREAT | RED_O_RDWR);
        // allocating buffer for MAX_NUM_CMDS numbers of incoming commands
        scheduled_commands_t *cmds = (scheduled_commands_t*)calloc(MAX_NUM_CMDS, sizeof(scheduled_commands_t));
        if (MAX_NUM_CMDS > 0 && cmds == NULL) {
            ex2_log("calloc for cmds failed, out of memory");
            return SATR_ERROR;
        }
        // parse the commands
        int number_of_cmds = prv_set_scheduler(&(gs_cmds->data[SUBSERVICE_BYTE+1]), cmds);
        // calculate frequency of cmds. Non-repetitive commands have a frequency of 0
        scheduled_commands_unix_t *sorted_cmds = (scheduled_commands_unix_t*)calloc(number_of_cmds, sizeof(scheduled_commands_unix_t));
        if (number_of_cmds > 0 && sorted_cmds == NULL) {
            ex2_log("calloc for sorted_cmds failed, out of memory");
            free(cmds);
            return SATR_ERROR;
        }
        calc_cmd_frequency(cmds, number_of_cmds, sorted_cmds);
        // sort the commands
        sort_cmds(sorted_cmds, number_of_cmds);
        // write cmds to file
        int32_t f_write = write_cmds_to_file(fout, sorted_cmds, number_of_cmds, fileName1);
        if (f_write < 0) {
            return SATR_ERROR;
        }

        // close file
        red_close(fout);
        // create the scheduler
        //TODO: review stack size
        xTaskCreate(vSchedulerHandler, "scheduler", 1000, NULL, NORMAL_SERVICE_PRIO, &SchedulerHandler);

        // free calloc
        free(cmds);
        free(sorted_cmds);

        status = 0;
        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(gs_cmds, sizeof(int8_t) + 1); // plus one for sub-service
        }
        break;

    case PING_SCHEDULE:
        {
        //this code is for testing purposes
        status = 0;
        memcpy(&gs_cmds->data[STATUS_BYTE], &status, sizeof(int8_t));
        set_packet_length(gs_cmds, sizeof(int8_t) + 1); // plus one for sub-service
        }
        break;

    default:
        ex2_log("No such subservice\n");
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
        }
        else {
            int buf_scanf;
            char *buf_string = &cmd_buff[old_str_position];
            int f_scanf = sscanf(buf_string,"%d",&buf_scanf);
            if (f_scanf != 1) {
                break;
            }
        }
        //Count the number of spaces before the scheduled time
        while (cmd_buff[str_position_2] == ' ') {
            str_position_2++;
        }
        //Advance the pointers to the first digit of scheduled time
        old_str_position = str_position_2;
        str_position_1 = str_position_2;
        
        /*-----------------------Fetch time in seconds-----------------------*/
        //Count the number of digits
        while (cmd_buff[str_position_1] != ' ') {
            str_position_1++;
        }
        str_position_2 = str_position_1;
        //Count the number of spaces following the digits
        while (cmd_buff[str_position_2] == ' ') {
            str_position_2++;
        }
        //Scan the value
        if (cmd_buff[str_position_1 - 1] == '*') {
            (cmds + number_of_cmds)->scheduled_time.Second = ASTERISK;
        }
        else {
            int buf_scanf;
            char *buf_string = &cmd_buff[old_str_position];
            int f_scanf = sscanf(buf_string,"%d",&buf_scanf);
            if (f_scanf != 1) {
                ex2_log("Error: unable to scan time in seconds for command: %d\n", number_of_cmds+1);
                return -1;
            }
            (cmds + number_of_cmds)->scheduled_time.Second = (uint8_t)buf_scanf;
        }
        
        old_str_position = str_position_2;
        str_position_1 = str_position_2;
        str_position_1++;

        /*-----------------------Fetch time in minutes-----------------------*/
        //Count the number of digits
        while (cmd_buff[str_position_1] != ' ') {
            str_position_1++;
        }
        str_position_2 = str_position_1;
        //Count the number of spaces following the digits
        while (cmd_buff[str_position_2] == ' ') {
            str_position_2++;
        }
        //Scan the value
        if (cmd_buff[str_position_1 - 1] == '*') {
            (cmds + number_of_cmds)->scheduled_time.Minute = ASTERISK;
        }
        else {
            int buf_scanf;
            char *buf_string = &cmd_buff[old_str_position];
            int f_scanf = sscanf(buf_string,"%d",&buf_scanf);
            if (f_scanf != 1) {
                ex2_log("Error: unable to scan time in Minute for command: %d\n", number_of_cmds+1);
                return -1;
            }
            (cmds + number_of_cmds)->scheduled_time.Minute = (uint8_t)buf_scanf;
        }

        old_str_position = str_position_2;
        str_position_1 = str_position_2;
        str_position_1++;

        /*-----------------------Fetch time in hour-----------------------*/
        //Count the number of digits
        while (cmd_buff[str_position_1] != ' ') {
            str_position_1++;
        }
        str_position_2 = str_position_1;
        //Count the number of spaces following the digits
        while (cmd_buff[str_position_2] == ' ') {
            str_position_2++;
        }
        //Scan the value
        if (cmd_buff[str_position_1 - 1] == '*') {
            (cmds + number_of_cmds)->scheduled_time.Hour = ASTERISK;
        }
        else {
            int buf_scanf;
            char *buf_string = &cmd_buff[old_str_position];
            int f_scanf = sscanf(buf_string,"%d",&buf_scanf);
            if (f_scanf != 1) {
                ex2_log("Error: unable to scan time in Hour for command: %d\n", number_of_cmds+1);
                return -1;
            }
            (cmds + number_of_cmds)->scheduled_time.Hour = (uint8_t)buf_scanf;
        }

        old_str_position = str_position_2;
        str_position_1 = str_position_2;
        str_position_1++;

        /*-----------------------Fetch the Wday-----------------------*/
        //Count the number of digits
        while (cmd_buff[str_position_1] != ' ') {
            str_position_1++;
        }
        str_position_2 = str_position_1;
        //Count the number of spaces following the digits
        while (cmd_buff[str_position_2] == ' ') {
            str_position_2++;
        }
        //Scan the value
        if (cmd_buff[str_position_1 - 1] == '*') {
            (cmds + number_of_cmds)->scheduled_time.Wday = ASTERISK;
        }
        else {
            int buf_scanf;
            char *buf_string = &cmd_buff[old_str_position];
            int f_scanf = sscanf(buf_string,"%d",&buf_scanf);
            if (f_scanf != 1) {
                ex2_log("Error: unable to scan time in Wday for command: %d\n", number_of_cmds+1);
                return -1;
            }
            (cmds + number_of_cmds)->scheduled_time.Wday = (uint8_t)buf_scanf;
        }

        old_str_position = str_position_2;
        str_position_1 = str_position_2;
        str_position_1++;

        /*-----------------------Fetch the Day-----------------------*/
        //Count the number of digits
        while (cmd_buff[str_position_1] != ' ') {
            str_position_1++;
        }
        str_position_2 = str_position_1;
        //Count the number of spaces following the digits
        while (cmd_buff[str_position_2] == ' ') {
            str_position_2++;
        }
        //Scan the value
        if (cmd_buff[str_position_1 - 1] == '*') {
            (cmds + number_of_cmds)->scheduled_time.Day = ASTERISK;
        }
        else {
            int buf_scanf;
            char *buf_string = &cmd_buff[old_str_position];
            int f_scanf = sscanf(buf_string,"%d",&buf_scanf);
            if (f_scanf != 1) {
                ex2_log("Error: unable to scan time in Day for command: %d\n", number_of_cmds+1);
                return -1;
            }
            (cmds + number_of_cmds)->scheduled_time.Day = (uint8_t)buf_scanf;
        }

        old_str_position = str_position_2;
        str_position_1 = str_position_2;
        str_position_1++;

        /*-----------------------Fetch the month-----------------------*/
        //Count the number of digits
        while (cmd_buff[str_position_1] != ' ') {
            str_position_1++;
        }
        str_position_2 = str_position_1;
        //Count the number of spaces following the digits
        while (cmd_buff[str_position_2] == ' ') {
            str_position_2++;
        }
        //Scan the value
        if (cmd_buff[str_position_1 - 1] == '*') {
            (cmds + number_of_cmds)->scheduled_time.Month = ASTERISK;
        }
        else {
            int buf_scanf;
            char *buf_string = &cmd_buff[old_str_position];
            int f_scanf = sscanf(buf_string,"%d",&buf_scanf);
            if (f_scanf != 1) {
                ex2_log("Error: unable to scan time in Month for command: %d\n", number_of_cmds+1);
                return -1;
            }
            (cmds + number_of_cmds)->scheduled_time.Month = (uint8_t)buf_scanf;
        }

        old_str_position = str_position_2;
        str_position_1 = str_position_2;
        str_position_1++;

        /*-----------------------Fetch the number of years since 1970-----------------------*/
        //Count the number of digits
        while (cmd_buff[str_position_1] != ' ') {
            str_position_1++;
        }
        str_position_2 = str_position_1;
        //Count the number of spaces following the digits
        while (cmd_buff[str_position_2] == ' ') {
            str_position_2++;
        }
        //Scan the value
        if (cmd_buff[str_position_1 - 1] == '*') {
            (cmds + number_of_cmds)->scheduled_time.Year = ASTERISK;
        }
        else {
            int buf_scanf;
            char *buf_string = &cmd_buff[old_str_position];
            int f_scanf = sscanf(buf_string,"%d",&buf_scanf);
            if (f_scanf != 1) {
                ex2_log("Error: unable to scan time in Year for command: %d\n", number_of_cmds+1);
                return -1;
            }
            (cmds + number_of_cmds)->scheduled_time.Year = (uint8_t)buf_scanf;
        }

        //advance pointers to the first byte of the next field
        old_str_position = str_position_2;
        str_position_1 = str_position_2;

        /*-----------------------Fetch the CSP dst(1 byte), dport(1byte), and data length(2 bytes)-----------------------*/
        //copy the dst
        uint8_t dst = cmd_buff[str_position_2];
        //advance pointer to dport
        str_position_2++;
        uint8_t dport = cmd_buff[str_position_2];
        //advance pointer to first byte of length
        str_position_2++;
        uint16_t highByte = cmd_buff[str_position_2];
        //advance pointer to second byte of length
        str_position_2++;
        uint16_t lowByte = cmd_buff[str_position_2];
        //combine length bytes
        uint16_t embeddedLength = (highByte << 8) | lowByte;

        //advance pointers to embedded data field
        str_position_2++;
        old_str_position = str_position_2;
        str_position_1 = str_position_2;

        /*-----------------------Fetch command as am embedded CSP packet-----------------------*/
        //get CSP buffer to reconstruct the embedded CSP packet
        (cmds + number_of_cmds)->length =  embeddedLength; 
        (cmds + number_of_cmds)->dst = dst;
        (cmds + number_of_cmds)->dport = dport;
        //(cmds + number_of_cmds)->subservice = cmd_buff[str_position_2];
        memcpy(&((cmds + number_of_cmds)->data), &(cmd_buff[str_position_2]), embeddedLength);
        //memcpy(&(packet->data), &embeddedSubservice, embeddedLength);

        //advance the pointers to read the next line of command
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
 *      For repetitive commands: calculate the unix time of the first execution, and the frequency each command will be executed at
 * @param cmds
 *      pointer to the structure that stores the parsed groundstation commands
 * @param number_of_cmds
 *      number of commands in @param cmds
 * @param sorted_cmds
 *      pointer to the structure that stores the groundstation commands after frequency has been calculated
 * @return SAT_returnState
 *      SATR_OK or SATR_ERROR
 */
SAT_returnState calc_cmd_frequency(scheduled_commands_t *cmds, int number_of_cmds, scheduled_commands_unix_t *sorted_cmds) {
    /*--------------------------------Initialize structures to store sorted commands--------------------------------*/
    //TODO: Confirm that the entire struct has been initialized with zeros
    scheduled_commands_unix_t *non_reoccurring_cmds = (scheduled_commands_unix_t*)calloc(number_of_cmds, sizeof(scheduled_commands_unix_t));
    if (number_of_cmds > 0 && non_reoccurring_cmds == NULL) {
        ex2_log("calloc for non_reoccurring_cmds failed, out of memory");
        return SATR_ERROR;
    }
    scheduled_commands_t *reoccurring_cmds = (scheduled_commands_t*)calloc(number_of_cmds, sizeof(scheduled_commands_t));
    if (number_of_cmds > 0 && reoccurring_cmds == NULL) {
        ex2_log("calloc for reoccurring_cmds failed, out of memory");
        free(non_reoccurring_cmds);
        return SATR_ERROR;
    }
    num_of_cmds.non_rep_cmds = 0;
    num_of_cmds.rep_cmds = 0;

    int unix_time_buff;
    int j_rep = 0;
    int j_non_rep = 0;

    for (int j = 0; j < number_of_cmds; j++) {
        // Separate the non-repetitve and repetitve commands, the sum of time fields should not exceed the value of ASTERISK (255) if non-repetitive
        if ((cmds+j)->scheduled_time.Second + (cmds+j)->scheduled_time.Minute + (cmds+j)->scheduled_time.Hour + (cmds+j)->scheduled_time.Month < ASTERISK) {

            // Store the non-repetitve commands into the new struct non_reoccurring_cmds
            if (j_non_rep < number_of_cmds) {
                // Convert the time into unix time for sorting convenience
                unix_time_buff = makeTime((cmds+j)->scheduled_time);
                (non_reoccurring_cmds+j_non_rep)->unix_time = unix_time_buff;
                if( unix_time_buff == -1 ) {
                    // TODO: create error handling routine to handle this error during cmd sorting and execution
                    ex2_log("Error: unable to make time using makeTime\n");
                    // TODO: delete this cmd if makeTime fails
                }

                // Copy the address of the embedded CSP packet to the non_reoccurring_cmds list
                (non_reoccurring_cmds+j_non_rep)->frequency = 0; //set frequency to zero for non-repetitive cmds
                (non_reoccurring_cmds+j_non_rep)->length = (cmds+j)->length;
                (non_reoccurring_cmds+j_non_rep)->dst = (cmds+j)->dst;
                (non_reoccurring_cmds+j_non_rep)->dport = (cmds+j)->dport;
                memcpy(&((non_reoccurring_cmds+j_non_rep)->data), &((cmds+j)->data), (cmds+j)->length);
                j_non_rep++;
            }
        }
        
        else if (j_rep < number_of_cmds) {
            // Store the repetitve commands into the new struct reoccurring_cmds
            memcpy(reoccurring_cmds+j_rep, cmds+j, sizeof(scheduled_commands_t));
            j_rep++;
        }
    }

    // update the number of commands
    num_of_cmds.non_rep_cmds = j_non_rep;
    num_of_cmds.rep_cmds = j_rep;

    /*--------------------------------calculate the frequency of repeated cmds--------------------------------*/
    static tmElements_t time_buff;
    //TODO: check that all callocs have been freed
    scheduled_commands_unix_t *repeated_cmds_buff = (scheduled_commands_unix_t*)calloc(j_rep, sizeof(scheduled_commands_unix_t));
    if (j_rep > 0 && repeated_cmds_buff == NULL) {
        ex2_log("calloc for repeated_cmds_buff failed, out of memory");
        free(non_reoccurring_cmds);
        free(reoccurring_cmds);
        return SATR_ERROR;
    }
    // Obtain the soonest time that the command will be executed, and calculate the frequency it needs to be executed at
    for (int j=0; j < j_rep; j++) {
        time_buff.Wday = (reoccurring_cmds+j)->scheduled_time.Wday;
        time_buff.Month = (reoccurring_cmds+j)->scheduled_time.Month;
        (repeated_cmds_buff+j)->length = (reoccurring_cmds+j)->length;
        (repeated_cmds_buff+j)->dst = (reoccurring_cmds+j)->dst;
        (repeated_cmds_buff+j)->dport = (reoccurring_cmds+j)->dport;
        memcpy(&((repeated_cmds_buff+j)->data), &((reoccurring_cmds+j)->data), (reoccurring_cmds+j)->length);
        // If command repeats every second
        if ((reoccurring_cmds+j)->scheduled_time.Hour == ASTERISK && (reoccurring_cmds+j)->scheduled_time.Minute == ASTERISK && (reoccurring_cmds+j)->scheduled_time.Second == ASTERISK) {
            //TODO: consider edge cases where the hour increases as soon as this function is executed - complete
            RTCMK_ReadHours(RTCMK_ADDR, &time_buff.Hour);
            RTCMK_ReadMinutes(RTCMK_ADDR, &time_buff.Minute);
            RTCMK_ReadSeconds(RTCMK_ADDR, &time_buff.Second);
            //convert the first execution time into unix time. Add 60 seconds to allow processing time
            (repeated_cmds_buff+j)->unix_time = makeTime(time_buff) + 60;
            (repeated_cmds_buff+j)->frequency = 1; //1 second
            continue;
        }
        // If command repeats every minute
        if ((reoccurring_cmds+j)->scheduled_time.Hour == ASTERISK && (reoccurring_cmds+j)->scheduled_time.Minute == ASTERISK) {
            //TODO: consider edge cases where the hour increases as soon as this function is executed - complete
            RTCMK_ReadHours(RTCMK_ADDR, &time_buff.Hour);
            RTCMK_ReadMinutes(RTCMK_ADDR, &time_buff.Minute);
            time_buff.Second = (reoccurring_cmds+j)->scheduled_time.Second;
            //convert the first execution time into unix time. Add 60 seconds to allow processing time
            (repeated_cmds_buff+j)->unix_time = makeTime(time_buff) + 60;
            (repeated_cmds_buff+j)->frequency = 60; //1 min
            continue;
        }
        // If command repeats every hour
        if ((reoccurring_cmds+j)->scheduled_time.Hour == ASTERISK) {
            //TODO: consider edge cases where the hour increases as soon as this function is executed - complete
            RTCMK_ReadHours(RTCMK_ADDR, &time_buff.Hour);
            time_buff.Minute = (reoccurring_cmds+j)->scheduled_time.Minute;
            time_buff.Second = (reoccurring_cmds+j)->scheduled_time.Second;
            //convert the first execution time into unix time. If the hour is almost over, increase the hour by one
            time_t current_time;
            time_t scheduled_time = makeTime(time_buff);
            if (scheduled_time - RTCMK_GetUnix(&current_time) < 60) {
                (repeated_cmds_buff+j)->unix_time = scheduled_time + 3600;
            }
            else {
                (repeated_cmds_buff+j)->unix_time = scheduled_time;
            }
            (repeated_cmds_buff+j)->frequency = 3600; //1 hr
            continue;
        }
    }

    /*--------------------------------Combine non-repetitive and repetitive commands into a single struct--------------------------------*/
    memcpy(sorted_cmds, repeated_cmds_buff, sizeof(scheduled_commands_unix_t)*j_rep);
    memcpy((sorted_cmds+j_rep), non_reoccurring_cmds, sizeof(scheduled_commands_unix_t)*j_non_rep);

    // free calloc
    free(non_reoccurring_cmds);
    free(reoccurring_cmds);
    free(repeated_cmds_buff);
    
    //prv_give_lock(cmds);
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
    scheduled_commands_unix_t sorting_buff;
    int ptr1, ptr2, min_ptr;
    for (ptr1 = 0; ptr1 < number_of_cmds; ptr1++) {
        for (ptr2 = ptr1+1; ptr2 < number_of_cmds; ptr2++) {
            //find minimum unix time
            if ((sorted_cmds+ptr1)->unix_time < (sorted_cmds+ptr2)->unix_time) {
                min_ptr = ptr1;
            }
            else {
                min_ptr = ptr2;
            }
        }
        //swap the minimum with the current
        if ((sorted_cmds+ptr1)->unix_time != (sorted_cmds+min_ptr)->unix_time) {
            memcpy(&sorting_buff, sorted_cmds+ptr1, sizeof(scheduled_commands_unix_t));
            memcpy((sorted_cmds+ptr1), (sorted_cmds+min_ptr), sizeof(scheduled_commands_unix_t));
            memcpy(sorted_cmds+min_ptr, &sorting_buff, sizeof(scheduled_commands_unix_t));
        }
    }
    return SATR_OK;
}


//TODO: consider writing a function to retrieve the list of scheduled commands
/*static scheduled_commands_t *prv_get_cmds_scheduler() {
    //if (!prvEps.eps_lock) {
        //prvEps.eps_lock = xSemaphoreCreateMutex();
    //}
    //configASSERT(prvEps.eps_lock);
    return &prvGS_cmds;
}
*/

/*--------------------------------Private----------------------------------------*/

/**
 * @brief
 *      Write groundstation commands to the given file location
 * @details
 *      Writes scheduled groundstation commands to the SD card
 *      Order of writes must match the appropriate read function
 * @param fileiFildes
 *      The file descriptor from which to read.
 * @param scheduled_cmds
 *      Struct containing groundstation commands to be executed at a given time
 * @param number_of_cmds
 *      Number of scheduled commands that the file contains
 * @param filename
 *      Name of the file the function is writing to
 * @return Result
 *      FAILURE or SUCCESS
 */
Result write_cmds_to_file(int32_t fileiFildes, scheduled_commands_unix_t *scheduled_cmds, int number_of_cmds, char *filename) {

    //TODO: good practice to check for <0 instead of == -1
    if (fileiFildes < 0) {
        printf("Unexpected error %d from red_open()\r\n", (int)red_errno);
        ex2_log("Failed to open or create file to write: '%s'\r\n", filename);
        return FAILURE;
    }
    uint32_t needed_size = number_of_cmds * sizeof(scheduled_commands_unix_t);

    red_errno = 0;
    /*The order of writes and subsequent reads must match*/
    int32_t f_write = red_write(fileiFildes, scheduled_cmds, needed_size);

    if (red_errno != 0) {
        ex2_log("Failed to write to file: '%s'\r\n", filename);
        return f_write;
    }

    return f_write;
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

        if (xTaskCreate((TaskFunction_t)scheduler_service, "scheduler_service", SCHEDULER_SIZE, NULL, NORMAL_SERVICE_PRIO,
                        &svc_tsk) != pdPASS) {
            ex2_log("FAILED TO CREATE TASK scheduler_service\n");
            return SATR_ERROR;
        }
        ex2_register(svc_tsk, svc_funcs);
        ex2_log("Scheduler service started\n");
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
SAT_returnState scheduler_service(void) {
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_NONE);
    csp_bind(sock, TC_SCHEDULER_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN);   // TODO: SERVICE_BACKLOG_LEN constant TBD
    //svc_wdt_counter++;

    /*The code below is for testing only, comment out from final code*/
    //=====================================================================
//    char *schedTime = "* * 1 14 2 2 52";
//    //char *test_command = "050 1 2 3 24 2 52   obc.time_management.get_time()\n 12 * * 14 2 2 52 obc.time_management.get_time()\n ";
//
//    csp_packet_t *test_cmd, *embedded_packet;
//    embedded_packet->id.dst = 1;
//    embedded_packet->id.dport = 15;
//    embedded_packet->length = 1;
//    embedded_packet->data[0] = 1;
//    test_cmd->length = 15 + sizeof(csp_packet_t);
//    test_cmd->data[0] = 0;
//    memcpy(test_cmd->data[1], schedTime, 15);
//    memcpy(test_cmd->data[16], embedded_packet, sizeof(csp_packet_t));
//
//    //memcpy(test_cmd->data, test_command, sizeof(test_command));
//    //char *test_cmd = "050 1 2 3 24 2 52   obc.time_management.get_time()\n 12 * * 14 2 2 52 obc.time_management.get_time()\n ";
//    //char *test_cmd = "test string\n ";
////    uint8_t test_subservice = 11;
////    uint32_t test_cmd_int = 1646289251;
////    csp_packet_t *packet = csp_buffer_get(5);
////    //unsigned int count = 0;
////    //snprintf((char *) packet->data, csp_buffer_data_size(), "Hello World (%u)", ++count);
////    memcpy(&packet->data[SUBSERVICE_BYTE],&test_subservice, sizeof(test_subservice));
////    memcpy(packet->data+1, &test_cmd_int, sizeof(test_cmd_int));
//    //packet->length = (strlen((char *) packet->data) + 1); /* include the 0 termination */
//    packet->length = (5); /* include the 0 termination */
//    ex2_log("timer started");
//    vTaskDelay(5000);
//    ex2_log("timer called again");
//    vTaskDelay(3000);
//    ex2_log("timer called again");
//
//    csp_conn_t *connect;
//    connect = csp_connect(CSP_PRIO_NORM,  1,  8, CSP_MAX_TIMEOUT, CSP_SO_NONE);
//    int send_packet_test = csp_send(connect, packet, CSP_MAX_TIMEOUT);
//    int read_packet_test = csp_read(connect, CSP_MAX_TIMEOUT);
//    int check_status = packet->data[STATUS_BYTE];
//    int close_connection_test = csp_close(connect);
////
//    //int csp_test = csp_sendto(CSP_PRIO_NORM, packet->id.dst, packet->id.dport, 0, CSP_O_NONE, packet->data, CSP_MAX_TIMEOUT);
//    //int csp_test = csp_sendto(CSP_PRIO_NORM, 1, 8, 255, CSP_O_RDP, packet->data32, CSP_MAX_TIMEOUT);
//    //CSP_PRIO_NORM, address, dport, 0, CSP_O_NONE, packet, CSP_SEND_TIMEOUT
//    //int rtc_test = RTCMK_SetUnix(1646289251); //march 02
//    //time_t rtc_unix_1, rtc_unix_2;
//    //int rtc_unix_test = RTCMK_GetUnix(&rtc_unix_1);
//    //rtc_test = RTCMK_SetUnix(1646375651); //march 03
    //rtc_unix_test = RTCMK_GetUnix(&rtc_unix_2);
    //--------------------------------- file system test ------------------------------------//
//    char* fileName2 = "VOL0:/test.TMP";
//    //char fileName2[] = "VOL0:/test.TMP";
//    char *file_output;
////    int file_output_int;
//    // open file from SD card
//    int32_t fout = red_open(fileName2, RED_O_CREAT | RED_O_RDWR);
//    if (fout == -1) {
//        printf("Unexpected error %d from red_open()\r\n", (int)red_errno);
//        ex2_log("Failed to open or create file to write: '%s'\n", fileName2);
//        red_errno = 0;
//        vTaskDelete(0);
//        return SATR_ERROR;
//    }
//    int needed_size = 13;
//    int32_t f_write = red_write(fout, test_cmd, needed_size + 1);
//    if (red_errno != 0) {
//        ex2_log("Failed to write to file: '%s'\n", fileName2);
//        red_errno = 0;
//        red_close(fout);
//        return SATR_ERROR;
//    }
//    red_lseek(fout, 0, 0);
//    // read file
//    int32_t f_read = red_read(fout, file_output, needed_size + 1);
//    if (f_read < 0) {
//        printf("Unexpected error %d from red_read()\r\n", (int)red_errno);
//        ex2_log("Failed to read file: '%s'\n", fileName2);
//        red_errno = 0;
//        red_close(fout);
//        return SATR_ERROR;
//    }
//    //close file
//    int32_t f_close = red_close(fout);
//    if (f_close < 0) {
//        printf("Unexpected error %d from red_close()\r\n", (int)red_errno);
//        ex2_log("Failed to open or create file to write: '%s'\n", fileName2);
//        red_errno = 0;
//        vTaskDelete(0);
//        return SATR_ERROR;
//    }
//    // delete file once all cmds have been executed
//    int32_t f_delete = red_unlink(fileName2);
//    if (f_delete < 0) {
//        printf("Unexpected error %d from f_delete()\r\n", (int)red_errno);
//        ex2_log("Failed to close file: '%s'\n", fileName2);
//        //TODO: ensure all red_errno are reinitialized (check housekeeping as well for consistency)
//        red_errno = 0;
//        return SATR_ERROR;
//    }
    
//    scheduler_service_app(test_cmd);

    //char *test_cmd = "50 1 2 3 24 2 52       obc.time_management.get_time()\n 12 * 13 14 2 2 52 obc.time_management.get_time()\n ";
//    int test_scanf = 0;
//    char *test_string = &test_cmd[2];
//    int f_scanf = sscanf(test_string,"%d", &test_scanf);
//    // allocating buffer for MAX_NUM_CMDS numbers of incoming commands
//    scheduled_commands_t *cmds = (scheduled_commands_t*)calloc(MAX_NUM_CMDS, sizeof(scheduled_commands_t));
//    // parse the commands
//    int number_of_cmds = prv_set_scheduler(test_cmd, cmds);
//    // calculate frequency of cmds. Non-repetitive commands have a frequency of 0
//    scheduled_commands_unix_t *sorted_cmds = (scheduled_commands_unix_t*)calloc(number_of_cmds, sizeof(scheduled_commands_unix_t));
//    calc_cmd_frequency(cmds, number_of_cmds, sorted_cmds);
//    free(cmds);
//    free(sorted_cmds);
    //=====================================================================
    /*TODO: delete code above after testing is complete*/

    for (;;) {
        csp_conn_t *conn;
        csp_packet_t *packet;

        if ((conn = csp_accept(sock, CSP_MAX_TIMEOUT)) == NULL) {
            /* timeout */
            //ex2_log("woke up");
            continue;
        }
        //TODO: is a watchdog needed?
        //svc_wdt_counter++;

        while ((packet = csp_read(conn, 50)) != NULL) {
            ex2_log("received packet");
            if (scheduler_service_app(packet) != SATR_OK) {
                //TODO: define max # of commands that can be scheduled per CSP packet, incorporate this limit into the gs ops manual
                ex2_log("Error responding to packet");
                csp_buffer_free(packet);
            }
            else {
                if (!csp_send(conn, packet, 50)) {
                    csp_buffer_free(packet);
                }
            }
        }
        csp_close(conn); // frees buffers used
    }
}

