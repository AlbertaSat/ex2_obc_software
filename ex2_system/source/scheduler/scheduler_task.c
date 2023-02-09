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

#include <FreeRTOS.h>
#include "rtcmk.h"
#include "services.h"
#include <csp/csp.h>
#include <redposix.h>
#include "logger/logger.h"
#include "scheduler/scheduler_task.h"
#include "scheduler/scheduler.h"

/* The queue is used by the scheduler service to notify this task that something
 * in ScheduleFile has changed.
 */
QueueHandle_t SchedulerNotificationQueue = 0;

// Check the scheduler file every 10 minutes by default
#define SCHED_TIMEOUT_MS 10*60*1000

static ScheduledCmd_t* read_cmd(int32_t fd) {
    /* Read a command from the open schedule file. Note that memory for the
     * command is allocated here and must be freed by the caller.
     *
     * Note: since we're running as a daemon, there is no one to bail us out
     * when things go wrong. We therefore soldier along, trying to make whatever
     * progress we can.
     */
    size_t cmd_len = sizeof(ScheduledCmd_t);
    ScheduledCmd_t *cmd = (ScheduledCmd_t*) pvPortMalloc(cmd_len);
    if (!cmd) {
        sys_log(NOTICE, "Out of memory?");
        return NULL;
    }

    int32_t cnt = red_read(fd, cmd, cmd_len);
    if (cnt == cmd_len)
        return cmd;

    if (cnt < 0) {
        sys_log(WARN, "red_read error: %d", (int)red_errno);
    }
    vPortFree(cmd);
    return NULL;
}

TickType_t schedule_cmd(int32_t fd, ScheduledCmd_t *cmd) {
    /* Schedule and (possibly) dispatch a command. If the command doesn't need
     * run in the next 2 seconds we return the time to wait until it's ready.
     * If we are within 2 seconds we get everything ready and dispatch the
     * command below. Once a command has been dispatched we either reschedule it
     * (if it's periodic) or discard it. We then use the open file handle to
     * read the next command and determine how long to wait before it's ready.
     */
    TickType_t timeout = pdMS_TO_TICKS(SCHED_TIMEOUT_MS);
    time_t current_time = RTCMK_Unix_Now();
    TickType_t start = xTaskGetTickCount();

    /* Caveat: determining time intervals with time_t and TickType_t is a pain
     * because they're unsigned. That means (smaller - bigger = huge) :-(
     * For this reason we're either casting to int32_t (dangerous, int64_t would
     * be better by I don't know if the machine chokes) or we're constantly
     * comparing to make sure we subtract the smaller from the larger.
     */
    int32_t wait_msec, wait = 0;
    bool missed = false;
    if (cmd->next > current_time) {
        wait = cmd->next - current_time;
        if (wait > 2) {
            //sys_log(DEBUG, "next task in %d seconds", wait);
            /* Note that we want to wake up a little bit before the command is
             * scheduled so we can get ready to dispatch it exactly on time.
             */
            return pdMS_TO_TICKS((wait - 2)*1000);
        }
    }
    else {
        missed = true;
        sys_log(NOTICE, "missed deadline by %d seconds", current_time-cmd->next);
    }

    /* We are close enough: get ready to execute the command. We have to
     * prepare the message and connect to the service. Then we can sleep the
     * last few milliseconds and send the message.
     */
    csp_packet_t *pkt = csp_buffer_get(cmd->len);
    pkt->id.dst = cmd->dst;
    pkt->id.dport = cmd->dport;
    pkt->length = cmd->len;
    memcpy(pkt->data, cmd->cmd, cmd->len);

    // ex2_log("%ld: send to  dst %x prt %x", current_time, cmd->dst, cmd->dport);
    csp_conn_t *conn = csp_connect(CSP_PRIO_NORM, cmd->dst, cmd->dport, 0, CSP_SO_HMACREQ);

    if (!missed) {
        /* We are all set to send. Figure out the exact delay, including
         * milliseconds. Funny enough, the RTC doesn't have millisecond
         * resolution, so the wait is ultimately specified in ticks!
         */
        TickType_t delay = 0;
        wait_msec = wait*1000 + cmd->msecs;
        int32_t msec = RTCMK_GetMs();
        if (wait_msec > msec)
            delay = pdMS_TO_TICKS(wait_msec - msec);

        /* Down to the brass tacks: now-start is the number of ticks since we
         * got the unix time (above).
         */
        TickType_t now = xTaskGetTickCount();
        if (delay > (now - start)) {
            vTaskDelay(delay - (now - start));
        }
    }

    if (csp_send(conn, pkt, 0) != 1) {
        sys_log(WARN, "csp_end to <%d,%d> failed", cmd->dst, cmd->dport);
        csp_buffer_free(pkt);
    }
    else {
        /* Get the response from the scheduled task. This is required not so
         * much because we care about the status, but because we need a context
         * switch allow the message to be delivered and for the task to run.
         */
        if (!(pkt = csp_read(conn, 5000))) {
            sys_log(NOTICE, "No response from <%d,%d>", cmd->dst, cmd->dport);
        }
        else {
            // Could check the return code
            csp_buffer_free(pkt);
        }
    }
    csp_close(conn);

    sys_log(INFO, "Dipatched cmd to <%d,%d> (delta %ld)", cmd->dst, cmd->dport,
            (int32_t) RTCMK_Unix_Now() - (int32_t)cmd->next);

    /* Now that the task is executed it is either removed from the scehdule
     * file or rescheduled if it is periodic. The easiest way to do either is
     * just to read the rest of the file (we just handled the first entry),
     * sort it, and write the entries back out again.
     */
    ScheduledCmd_t *cmds[MAX_NUM_CMDS] = {0};
    int cmd_cnt = 0;
    if (cmd->period == 0) { // non-periodic command
        vPortFree(cmd);
    }
    else {
        cmd->next += cmd->period; // next execution
        if (cmd->last && cmd->next>cmd->last) { // Command has expired
            vPortFree(cmd);
        }
        else {
            cmds[0] = cmd;
            cmd_cnt = 1;
        }
    }

    while(cmd_cnt < MAX_NUM_CMDS) {
        /* Note the file is open for read/write and the file pointer is at the
         * start of the next command (or EOF).
         */
        cmds[cmd_cnt] = read_cmd(fd);
        if (cmds[cmd_cnt]) {
            cmd_cnt++;
        }
        else {
            break;
        }
    }

    /* Set the file size to 0 in preparation for the rewrite. Note the truncate
     * does not change the file pointer, but that doesn't matter if there are
     * no commands left.
     */
    int32_t err = red_ftruncate(fd, 0);
    if (err < 0) {
        sys_log(WARN, "red_ftruncate error: %d", (int)red_errno);
    }

    if (cmd_cnt > 0) {
        if (red_lseek(fd, 0, RED_SEEK_SET)) {
            /* Hmm, slightly awkward if we can't start at the beginning of a
             * zero length file (assuming the truncate command worked)
             */
            sys_log(WARN, "red_lseek error: %d", (int)red_errno);
        }
        sort_cmds(cmds, cmd_cnt);

        /* Calculate the coarse-grain (ReceiveQueue) delay since we now know
         * when the next task is due. The coarse delay is 2 seconds short of
         * the fine delay so we can get everything ready for the task.
         */
        current_time = RTCMK_Unix_Now();
        wait_msec = (current_time - cmds[0]->next)*1000;
        if (wait_msec < 2000) wait_msec = 0;
        timeout = pdMS_TO_TICKS(wait_msec);
        if ((current_time > cmds[0]->next)) {
            /* This check is because we seem to get stuck in the ReceiveQueue
             * (and get further behind) even though the timeout should be 0.
             */
            sys_log(WARN, "already late by %ld: timeout %ld\n",
                    current_time - cmds[0]->next, timeout);
        }
        for (int i=0; i<cmd_cnt; i++) {
            if (red_write(fd, cmds[i], sizeof(ScheduledCmd_t)) < 0) {
                sys_log(WARN, "red_write error: %d", (int)red_errno);
            }
            vPortFree(cmds[i]);
        }
    }
    return timeout;
}

/**
 * Command scheduler_task
 */
void scheduler_task(void *pvParameters) {
    /* Set the initial notification queue timeout to 0 so that we check the
     * schedule file on boot/reboot.
     */
    TickType_t timeout = 0;

    vTaskDelay(pdMS_TO_TICKS(10000)); // let everyone else start

    while(1) {
        int ctx;
        /* Note: we don't care what we dequeue, we only care that FreeRTOS
         * honors our timeouts.
         */
        //ex2_log("top level timeout %d", timeout);
        TickType_t start = xTaskGetTickCount();
        xQueueReceive(SchedulerNotificationQueue, &ctx, timeout);
        TickType_t elapsed = xTaskGetTickCount() - start;

        if (timeout && elapsed > timeout) {
            /* We just measured the timeout using ticks. While it should be
             * OK to sleep less than timeout (because of notifications), we
             * should never sleep more than timeout.
             */
            sys_log(NOTICE, "*** xQueueReceive sleep %ld > %ld", elapsed, timeout);
        }

        timeout = pdMS_TO_TICKS(SCHED_TIMEOUT_MS);

        /* Check ScheduleFile to see if there's any work for us. Note that the
         * lock might not exist if the scheduler hasn't started yet, and the
         * file might not exist (or be empty), depending on the command history.
         */
        if (!SchedLock || xSemaphoreTake(SchedLock, SCHED_SEM_WAIT) == pdTRUE) {
            int32_t fd = red_open(ScheduleFile, RED_O_RDWR);
            if (fd >= 0) {
                /* If there is a command waiting to be scheduled, schedule_cmd
                 * will either execute it or return the timeout that we'll use
                 * to wait for the next command to execute.
                 */
                ScheduledCmd_t *cmd = read_cmd(fd);
                if (cmd) {
                    timeout = schedule_cmd(fd, cmd);
                }
                red_close(fd);
            }
            else if (red_errno != RED_ENOENT) {
                /* It's OK if there is no ScheduleFile (ENOENT), but not so
                 * great if there is some other error.
                 */
                sys_log(NOTICE, "red_open error: %d", (int)red_errno);
            }

            if (SchedLock) xSemaphoreGive(SchedLock);
        }
        else { // xSemaphoreTake != pdTRUE
            sys_log(WARN, "semaphore error %s", ScheduleFile);
        }
    } // while(1)
}

SAT_returnState start_scheduler_task(void) {
    SchedulerNotificationQueue = xQueueCreate(10, sizeof(int));

    if (xTaskCreate(scheduler_task, "sched_task", SCHEDULER_STACK_SIZE, NULL,
                    SCHEDULER_TASK_PRIO, NULL) != pdPASS) {
        sys_log(WARN, "Could not start scheduler_task");
        return SATR_ERROR;
    }
    return SATR_OK;
}
