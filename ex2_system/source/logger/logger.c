/*
 * logger.c
 *
 *  Created on: Jan. 26, 2021
 *      Author: Andrew
 */
#include "logger/logger.h"

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>
#include <HL_hal_stdtypes.h>
#include <FreeRTOS.h>

struct circular_buf_t {
    uint8_t * buffer;
    size_t head;
    size_t tail;
    size_t max; //of the buffer
    bool full;
};

cbuf_handle_t circular_buf_init(uint8_t* buffer, size_t size)
{
    configASSERT(buffer && size);

    cbuf_handle_t cbuf = pvPortMalloc(sizeof(circular_buf_t));
    configASSERT(cbuf);

    cbuf->buffer = buffer;
    cbuf->max = size;
    circular_buf_reset(cbuf);

    configASSERT(circular_buf_empty(cbuf));

    return cbuf;
}

void circular_buf_reset(cbuf_handle_t cbuf)
{
    configASSERT(cbuf);

    cbuf->head = 0;
    cbuf->tail = 0;
    cbuf->full = FALSE;
}

void circular_buf_free(cbuf_handle_t cbuf)
{
    configASSERT(cbuf);
    free(cbuf);
}

bool circular_buf_full(cbuf_handle_t cbuf)
{
    configASSERT(cbuf);

    return cbuf->full;
}

bool circular_buf_empty(cbuf_handle_t cbuf)
{
    configASSERT(cbuf);

    return (!cbuf->full && (cbuf->head == cbuf->tail));
}

size_t circular_buf_capacity(cbuf_handle_t cbuf)
{
    configASSERT(cbuf);

    return cbuf->max;
}

size_t circular_buf_size(cbuf_handle_t cbuf)
{
    configASSERT(cbuf);

    size_t size = cbuf->max;

    if(!cbuf->full)
    {
        if(cbuf->head >= cbuf->tail)
        {
            size = (cbuf->head - cbuf->tail);
        }
        else
        {
            size = (cbuf->max + cbuf->head - cbuf->tail);
        }
    }

    return size;
}

static void advance_pointer(cbuf_handle_t cbuf)
{
    configASSERT(cbuf);

    if(cbuf->full)
    {
        cbuf->tail = (cbuf->tail + 1) % cbuf->max;
    }

    cbuf->head = (cbuf->head + 1) % cbuf->max;
    cbuf->full = (cbuf->head == cbuf->tail);
}

static void retreat_pointer(cbuf_handle_t cbuf)
{
    configASSERT(cbuf);

    cbuf->full = FALSE;
    cbuf->tail = (cbuf->tail + 1) % cbuf->max;
}

void circular_buf_put(cbuf_handle_t cbuf, uint8_t data)
{
    configASSERT(cbuf && cbuf->buffer);

    cbuf->buffer[cbuf->head] = data;

    advance_pointer(cbuf);
}

int circular_buf_get(cbuf_handle_t cbuf, uint8_t * data)
{
    configASSERT(cbuf && data && cbuf->buffer);

    int r = -1;

    if(!circular_buf_empty(cbuf))
    {
        *data = cbuf->buffer[cbuf->tail];
        retreat_pointer(cbuf);

        r = 0;
    }

    return r;
}



