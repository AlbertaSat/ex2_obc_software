#include <FreeRTOS.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "os_task.h"
#include "services.h"
#include <csp/csp.h>
#include <csp/csp_endian.h>
#include <redposix.h> //include for file system
#include "scheduler/scheduler.h"
#include "scheduler/scheduler_task.h"
#include "logger.h"
/*
 * scheduler.c
 *
 *  Created on: Nov. 22, 2021
 *      Author: Grace Yi
 */

#include "scheduler/scheduler.h"

const char *ScheduleFile = "VOL0:/gs_cmds.TMP";
SemaphoreHandle_t SchedLock = NULL;

static int parse_packet(csp_packet_t *pkt, ScheduledCmd_t **cmds);

/**
 * @brief
 *      Private. Collect scheduled commands from the groundstation
 * @param packet
 *      pointer to the ground station packet containing a raw schedule
 * @return int
 *      SCHED_ERR_OK
 *      SCHED_ERR_IO - I/O error, red_errno in OUT_DATA_BYTE
 */
static int scheduler_service_app(csp_packet_t *packet) {
    int rc = SATR_OK;
    uint8_t service_subtype = packet->data[SUBSERVICE_BYTE];

    switch (service_subtype) {
    case SET_SCHEDULE: {
        ScheduledCmd_t *cmds[MAX_NUM_CMDS] = {0};
        if ((rc = parse_packet(packet, cmds)) != SCHED_ERR_OK) {
            packet->data[OUT_DATA_BYTE] = 0;
            packet->length = 3;
            return rc;
        }

        int num_cmds = 0;
        while(cmds[num_cmds]) num_cmds++;

        sys_log(DEBUG, "Set Schedule: received %d tasks", num_cmds);

        // Commands are sorted in order of increasing time to execution
        sort_cmds(cmds, num_cmds);

        // We're done with the input packet, so we can initialize the output
        packet->data[OUT_DATA_BYTE] = num_cmds;
        packet->length = 2 * sizeof(int8_t) + 1;

        if (xSemaphoreTake(SchedLock, (TickType_t) SCHED_SEM_WAIT) == pdTRUE) {
            int fd = red_open(ScheduleFile, RED_O_CREAT | RED_O_RDWR);
            if (fd < 0) {
                packet->data[OUT_DATA_BYTE] = red_errno;
                sys_log(WARN, "red_open error: %d", (int)red_errno);
                rc = SCHED_ERR_IO;
            }
            else {
                for (int i=0; i<num_cmds; i++) {
                    if (red_write(fd, cmds[i], sizeof(ScheduledCmd_t)) < 0) {
                        packet->data[OUT_DATA_BYTE] = red_errno;
                        sys_log(WARN, "red_write error: %d", (int)red_errno);
                        rc = SCHED_ERR_IO;
                    }
                    vPortFree(cmds[i]);
                }
                red_close(fd);
            }
            xSemaphoreGive(SchedLock);

            /* The SchedulerNotificationQueue exists to allow the scheduler
             * service to notify the scheduler daemon that something has changed.
             * The message contents don't matter, the act of queuing the message
             * is what wakes up the other side.
             */
            int ctx = 1;
            if (xQueueSendToBack(SchedulerNotificationQueue, &ctx, 0) != pdTRUE)
                sys_log(NOTICE, "xQueueSend failed");
        }
        else {
            sys_log(WARN, "semaphore error %s", ScheduleFile);
            rc = SCHED_ERR_LOCK;
        }
    } break;

    case DELETE_SCHEDULE: {
        sys_log(DEBUG, "Delete Schedule request received");
        packet->length = 2; // Normally only have sub-service and status

        if (xSemaphoreTake(SchedLock, (TickType_t) SCHED_SEM_WAIT) == pdTRUE) {
            int32_t err = red_unlink(ScheduleFile);
            if (err < 0) {
                // Put the Reliance Edge error code after the status byte
                sys_log(NOTICE, "red_unlink error: %d", red_errno);
                packet->data[OUT_DATA_BYTE] = red_errno;
                packet->length = 3;
                rc = SCHED_ERR_IO;
            }
            xSemaphoreGive(SchedLock);

            // Queue a message to the scheduler daemon to wake it up.
            int ctx = 2;
            if (xQueueSendToBack(SchedulerNotificationQueue, &ctx, 0) != pdTRUE)
                sys_log(NOTICE, "xQueueSend failed");
        }
    } break;

    case GET_SCHEDULE: {
        sys_log(DEBUG, "Get Schedule request received");
        // Open schedule file. Note that it's OK if the file doesn't exist.
        if (xSemaphoreTake(SchedLock, (TickType_t) SCHED_SEM_WAIT) == pdTRUE) {
            int32_t fd = red_open(ScheduleFile, RED_O_RDONLY);
            if (fd >= 0) {
                // get file size through file stats
                REDSTAT fstat;
                int err = red_fstat(fd, &fstat);
                if (err < 0) {
                    sys_log(NOTICE, "red_fstat error: %d", red_errno);
                    packet->data[OUT_DATA_BYTE] = red_errno;
                    rc = SCHED_ERR_IO;
                    packet->length = 3;
                }
                else {
                    int num_cmds = fstat.st_size / sizeof(ScheduledCmd_t);
                    packet->data[OUT_DATA_BYTE] = num_cmds;
                    int out_index = OUT_DATA_BYTE + 1;

                    ScheduledCmd_t cmd;
                    for (int i=0; i<num_cmds; i++) {
                        if (red_read(fd, &cmd, sizeof(cmd)) < 0) {
                            packet->data[OUT_DATA_BYTE] = red_errno;
                            sys_log(WARN, "red_open error: %d", (int)red_errno);
                            rc = SCHED_ERR_IO;
                            break;
                        }

                        memcpy(&packet->data[out_index], &cmd.next, sizeof(cmd.next));
                        out_index += sizeof(cmd.next);
                        memcpy(&packet->data[out_index], &cmd.period, sizeof(cmd.period));
                        out_index += sizeof(cmd.period);
                        packet->data[out_index++] = cmd.dst;
                        packet->data[out_index++] = cmd.dport;
                    }
                    packet->length = out_index;
                }
                red_close(fd);
            } // red_open fc >= 0
            else {
                if (red_errno == RED_ENOENT) {
                    packet->data[OUT_DATA_BYTE] = 0;
                }
                else {
                    sys_log(NOTICE, "red_open error: %d", (int)red_errno);
                    rc = SCHED_ERR_IO;
                    packet->data[OUT_DATA_BYTE] = red_errno;
                }
                packet->length = 3;
            }
            xSemaphoreGive(SchedLock);
        }
        else {
            sys_log(WARN, "semaphore error %s", ScheduleFile);
            rc = SCHED_ERR_LOCK;
            packet->length = 2;
        }
    } break;

    default:
        sys_log(ERROR, "No such subservice: %d", service_subtype);
        packet->length = 2;
        rc = SATR_PKT_ILLEGAL_SUBSERVICE;
    }

    return rc;
}

/*------------------------------Private-------------------------------------*/

/**
 * @brief
 *      Parse and store groundstation commands from the buffer to the array @param cmds
 * @param cmd_buff
 *      pointer to the buffer that stores the groundstation commands
 * @return Result
 *      ERROR < 0 or number of cmds parsed
 */

static int parse_packet(csp_packet_t *pkt, ScheduledCmd_t **cmds) {
    const uint8_t *ptr = &(pkt->data[IN_DATA_BYTE]);
    int cmd_num = 0;
    while ((ptr - pkt->data) < pkt->length && cmd_num < MAX_NUM_CMDS) {
        cmds[cmd_num] = (ScheduledCmd_t*) pvPortMalloc(sizeof(ScheduledCmd_t));
        if (!cmds[cmd_num]) {
            sys_log(NOTICE, "Out of memory?");
            return SCHED_ERR_NO_MEM;
        }

        cmds[cmd_num]->next = csp_ntoh32(*((uint32_t *) ptr));
        ptr += sizeof(uint32_t);
        cmds[cmd_num]->period = csp_ntoh32(*((uint32_t *) ptr));
        ptr += sizeof(uint32_t);
        cmds[cmd_num]->last = csp_ntoh32(*((uint32_t *) ptr));
        ptr += sizeof(uint32_t);
        cmds[cmd_num]->dst = *ptr++;
        cmds[cmd_num]->dport = *ptr++;
        cmds[cmd_num]->len = *((uint16_t *) ptr);
        ptr += sizeof(uint16_t);

        for (int op=0; op<cmds[cmd_num]->len && op<MAX_CMD_LENGTH; op++) {
            cmds[cmd_num]->cmd[op] = *ptr++;
        }

        // Done with this command, get ready for the next one.
        cmd_num++;
    }

    return 0;
}

/**
 * @brief
 *      Sort groundstation commands from the lowest to highest unix time
 * @param cmds
 *      array of pointers to the structures that store the groundstation commands
 * @param num_cmds
 *      number of commands in the array @param sorted_cmds
 */
void sort_cmds(ScheduledCmd_t **cmds, int num_cmds) {
    // Simple selection sort
    for (int curr=0; curr<num_cmds; curr++) {
        int min_index = curr;
        for (int i=curr+1; i<num_cmds; i++) {
            // Find the next smallest value amongst the remaining values
            if (cmds[i]->next < cmds[min_index]->next) {
                min_index = i;
            }
        }
        if (min_index != curr) {
            // Swap the next smallest value into the current index
            ScheduledCmd_t *tmp = cmds[curr];
            cmds[curr] = cmds[min_index];
            cmds[min_index] = tmp;
        }
    }
}

/**
 * @brief
 *      FreeRTOS scheduler server task
 * @details
 *      Accepts incoming scheduler packets and saves them for execution
 * @param void* param
 * @return SAT_returnState
 */
static SAT_returnState scheduler_service(void) {
    csp_socket_t *sock;
    sock = csp_socket(CSP_SO_HMACREQ);
    csp_bind(sock, TC_SCHEDULER_SERVICE);
    csp_listen(sock, SERVICE_BACKLOG_LEN); // TODO: SERVICE_BACKLOG_LEN constant TBD

    for (;;) {
        csp_conn_t *conn;
        csp_packet_t *packet;

        if ((conn = csp_accept(sock, CSP_MAX_TIMEOUT)) == NULL) {
            /* timeout */
            // ex2_log("woke up");
            continue;
        }
        // TODO: is a watchdog needed?

        while ((packet = csp_read(conn, 50)) != NULL) {
            increment_commands_recv();
            int rc = scheduler_service_app(packet);
            packet->data[STATUS_BYTE] = rc;

            if (!csp_send(conn, packet, 50)) {
                // You have to free your own buffer on error :-/
                csp_buffer_free(packet);
            }
        }
        csp_close(conn); // frees buffers used
    }
}

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

    // create mutex to protect file system
    if (!SchedLock)
        SchedLock = xSemaphoreCreateMutex();

    if (xTaskCreate((TaskFunction_t)scheduler_service, "sched_service",
                    SCHEDULER_SVC_SIZE, NULL, NORMAL_SERVICE_PRIO, NULL)
        != pdPASS) {
        sys_log(ERROR, "FAILED TO CREATE TASK scheduler_service\n");
        return SATR_ERROR;
    }
    sys_log(NOTICE, "Scheduler service started\n");
    return SATR_OK;
}
