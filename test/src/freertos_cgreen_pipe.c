#include "cgreen/internal/cgreen_pipe.h"

#include "utils.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <queue.h>


int cgreen_pipe_open(int pipes[2])
{
    int pipe_open_result;
    int pipe_nonblock_result;

    int queueLength = 50;
    int itemSize = 512;

    QueueHandle_t read_write_queue;

    read_write_queue = xQueueCreate(
        queueLength,
        itemSize
    );

    pipes[0] = (int) read_write_queue;
    pipes[1] = (int) read_write_queue;

    return 0;
}

void cgreen_pipe_close(int p)
{
  vQueueDelete((void *) p);
}

ssize_t cgreen_pipe_read(int p, void *buf, size_t count)
{
    int x = xQueueReceive((void *)p, buf, 0);

    return x;
}

ssize_t cgreen_pipe_write(int p, const void *buf, size_t count)
{
    int pipe_write_result = xQueueSendToBack((void *)p, buf, 0);
    return pipe_write_result;
}
