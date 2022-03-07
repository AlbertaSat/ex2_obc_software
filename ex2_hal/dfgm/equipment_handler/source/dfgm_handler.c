/*
 * Copyright (C) 2015  University of Alberta
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
 * @file
 * @author
 * @date
 */


// Headers used in converter.c and filter.c (Reliance Edge, etc.)
#include "dfgm_handler.h"

#include "FreeRTOS.h"
#include "HL_sci.h"
#include <stdio.h>
#include <stdlib.h>
#include "system.h"
#include <stdint.h>

#include <os_queue.h>
#include <os_task.h>
#include <time.h>
#include "os_semphr.h"

#include "stdio.h"
#include <redconf.h>
#include <redfs.h>
#include <redfse.h>
#include <redposix.h>
#include <redtests.h>
#include <redvolume.h>
#include "rtcmk.h"

// For debugging...?
#include "printf.h"

#define scilinREG PRINTF_SCI

// Macros
#ifndef DFGM_RX_PRIO
#define DFGM_RX_PRIO (tskIDLE_PRIORITY + 1) //configMAX_PRIORITIES - 1 originally
#endif

#ifndef DFGM_SCI
#define DFGM_SCI scilinREG
#endif

// Static values (dfgm_buffer, DFGM_queue, etc.)
static uint8_t DFGM_byteBuffer;
static xQueueHandle DFGM_queue;
static SemaphoreHandle_t TX_semaphore;
static dfgm_housekeeping HK_buffer = {0};

static int secondsPassed = 0;
static int DFGM_runtime = 0;
static int collecting_HK = 0;
static int firstPacketFlag = 1;

// HKScales and HKOffsets array
const float HK_scales[] = {HK_SCALE_0, HK_SCALE_1, HK_SCALE_2, HK_SCALE_3,
                          HK_SCALE_4, HK_SCALE_5, HK_SCALE_6, HK_SCALE_7,
                          HK_SCALE_8, HK_SCALE_9, HK_SCALE_10, HK_SCALE_11};

const float HK_offsets[] = {HK_OFFSET_0, HK_OFFSET_1, HK_OFFSET_2, HK_OFFSET_3,
                           HK_OFFSET_4, HK_OFFSET_5, HK_OFFSET_6, HK_OFFSET_7,
                           HK_OFFSET_8, HK_OFFSET_9, HK_OFFSET_10, HK_OFFSET_11};

// Coefficients for 1 Hz filter
double filter[81] = {
    0.014293879,   0.014285543,    0.014260564,    0.014219019,
    0.014161035,   0.014086794,    0.013996516,    0.013890488,
    0.013769029,   0.013632505,    0.013481341,    0.013315983,
    0.013136936,   0.012944728,    0.012739926,    0.012523144,
    0.012295013,   0.012056194,    0.011807376,    0.011549255,
    0.011282578,   0.011008069,    0.010726496,    0.010438624,
    0.010145215,   0.0098470494,   0.0095449078,   0.0092395498,
    0.0089317473,  0.0086222581,   0.0083118245,   0.0080011814,
    0.0076910376,  0.0073820935,   0.0070750111,   0.0067704498,
    0.0064690227,  0.0061713282,   0.0058779319,   0.0055893637,
    0.0053061257,  0.0050286865,   0.0047574770,   0.0044928941,
    0.0042353003,  0.0039850195,   0.0037423387,   0.0035075136,
    0.0032807556,  0.0030622446,   0.0028521275,   0.0026505100,
    0.0024574685,  0.0022730422,   0.0020972437,   0.0019300507,
    0.0017714123,  0.0016212496,   0.0014794567,   0.0013459044,
    0.0012204390,  0.0011028848,   0.00099304689,  0.00089071244,
    0.00079565205, 0.00070762285,  0.00062636817,  0.00055162174,
    0.00048310744, 0.00042054299,  0.00036364044,  0.00031210752,
    0.00026565061, 0.00022397480,  0.00018678687,  0.00015379552,
    0.00012471308, 9.9256833e-005, 7.7149990e-005, 5.8123173e-005,
    4.1914571e-005

};

// Coefficients for 10 Hz filter


struct dfgm_second secondBuffer[2]; // dfgm_second buffer
struct dfgm_second *secondPointer[2]; // dfgm_second pointer

// Converter functions
void DFGM_convertRawMagData(dfgm_packet_t *const data) {

    // convert part of raw data to magnetic field data
    int i;
    for (i = 0; i < 100; i++) {
        short x_DAC = (data->tuple[i].x) >> 16;
        short x_ADC = ((data->tuple[i].x) % (1 << 16));
        short y_DAC = (data->tuple[i].y) >> 16;
        short y_ADC = ((data->tuple[i].y) % (1 << 16));
        short z_DAC = (data->tuple[i].z) >> 16;
        short z_ADC = ((data->tuple[i].z) % (1 << 16));
        float x = (X_DAC_SCALE * (float)x_DAC + X_ADC_SCALE * (float)x_ADC + X_OFFSET);
        float y = (Y_DAC_SCALE * (float)y_DAC + Y_ADC_SCALE * (float)y_ADC + Y_OFFSET);
        float z = (Z_DAC_SCALE * (float)z_DAC + Z_ADC_SCALE * (float)z_ADC + Z_OFFSET);
        data->tuple[i].x = (*(uint32_t *)&x);
        data->tuple[i].y = (*(uint32_t *)&y);
        data->tuple[i].z = (*(uint32_t *)&z);
    }
}

void DFGM_convertRaw_HK_data(dfgm_packet_t *const data) {
    // convert part of raw data into house keeping data
    for (int i = 0; i < 12; i++) {
        float HK_value = ((float)(data->HK[i]) * HK_scales[i] + HK_offsets[i]);
        data->HK[i] = (uint16_t)HK_value;
    };
}

void update_HK(dfgm_data_t const *data) {
    HK_buffer.time = data->time;
    HK_buffer.coreVoltage = (float) ((data->packet).HK[0]); // Leave voltage values in mV or V?
    HK_buffer.sensorTemp = (float) ((data->packet).HK[1]);
    HK_buffer.refTemp = (float) ((data->packet).HK[2]);
    HK_buffer.boardTemp = (float) ((data->packet).HK[3]);
    HK_buffer.posRailVoltage = (float) ((data->packet).HK[4]);
    HK_buffer.inputVoltage = (float) ((data->packet).HK[5]);
    HK_buffer.refVoltage = (float) ((data->packet).HK[6]);
    HK_buffer.inputCurrent = (float) ((data->packet).HK[7]);
    HK_buffer.reserved1 = (float) ((data->packet).HK[8]);
    HK_buffer.reserved2 = (float) ((data->packet).HK[9]);
    HK_buffer.reserved3 = (float) ((data->packet).HK[10]);
    HK_buffer.reserved4 = (float) ((data->packet).HK[11]);
}

// File system functions
void savePacket(dfgm_data_t *data, char *fileName) {
    int32_t iErr;

    // open or create file
    int32_t dataFile;
    dataFile = red_open(fileName, RED_O_WRONLY | RED_O_CREAT | RED_O_APPEND);
    if (dataFile == -1) {
        exit(red_errno);
    }

    // Save only mag field data sample by sample w/ timestamps
    dfgm_data_sample_t dataSample = {0};
    for (int i = 0; i < 100; i++) {
        memset(&dataSample, 0, sizeof(dfgm_data_sample_t));
        dataSample.time = data->time;
        dataSample.x = *(float *)&(data->packet).tuple[i].x;
        dataSample.y = *(float *)&(data->packet).tuple[i].y;
        dataSample.z = *(float *)&(data->packet).tuple[i].z;

        iErr = red_write(dataFile, &dataSample, sizeof(dfgm_data_sample_t));
        if (iErr == -1) {
            exit(red_errno);
        }
    }

    // close file
    iErr = red_close(dataFile);
    if (iErr == -1) {
        exit(red_errno);
    }
}

// Filter functions
void applyFilter(void) {
  /*  The 41 point filter is applied to every other data point */
  double xFiltered, yFiltered, zFiltered;
  int i, negsamp, possamp;

  /*  "DC" component centred on the 0 time sample */
  xFiltered = secondPointer[1]->x[0] * filter[0];
  yFiltered = secondPointer[1]->y[0] * filter[0];
  zFiltered = secondPointer[1]->z[0] * filter[0];

  negsamp = 99; // sample indices
  possamp = 1;

  // Apply filter to data
  for (i = 1; i < 81; i++) {
    // Apply filter to data
    xFiltered += (secondPointer[0]->x[negsamp] + secondPointer[1]->x[possamp]) * filter[i];
    yFiltered += (secondPointer[0]->y[negsamp] + secondPointer[1]->y[possamp]) * filter[i];
    zFiltered += (secondPointer[0]->z[negsamp] + secondPointer[1]->z[possamp]) * filter[i];
    negsamp -= 1;
    possamp += 1;
  }

  // Save filtered data to struct
  secondPointer[1]->xFiltered = xFiltered;
  secondPointer[1]->yFiltered = yFiltered;
  secondPointer[1]->zFiltered = zFiltered;
}

void shiftSecondPointer(void) {
    secondPointer[0] = secondPointer[1];
}

void saveSecond(struct dfgm_second *second, char * fileName) {
    int32_t iErr;

    int32_t dataFile;
    dataFile = red_open(fileName, RED_O_WRONLY | RED_O_CREAT | RED_O_APPEND);
    if (dataFile == -1) {
        exit(red_errno);
    }

    dfgm_data_sample_t dataSample = {0};
    memset(&dataSample, 0, sizeof(dfgm_data_sample_t));
    dataSample.time = second->time;

    dataSample.x = (float) second->xFiltered;
    dataSample.y = (float) second->yFiltered;
    dataSample.z = (float) second->zFiltered;

    iErr = red_write(dataFile, &dataSample, sizeof(dfgm_data_sample_t));
    if (iErr == -1) {
        exit(red_errno);
    }

    iErr = red_close(dataFile);
    if (iErr == -1) {
        exit(red_errno);
    }
}

// FreeRTOS
void dfgm_rx_task(void *pvParameters) {
    static dfgm_data_t data = {0};
    int received = 0;
    int32_t iErr = 0;

    // initialize reliance edge
    iErr = red_init();
    if (iErr == -1) {
        exit(red_errno);
    }

    // Initialize variables for filtering
    secondPointer[0] = &secondBuffer[0];
    secondPointer[1] = &secondBuffer[1];
    float tempX;
    float tempY;
    float tempZ;

    // Set initial conditions
    secondsPassed = 0;
    DFGM_runtime = 0;
    collecting_HK = 0;
    firstPacketFlag = 1;

    sciReceive(DFGM_SCI, 1, &DFGM_byteBuffer);
    for (;;) {
        // Always receive packets from queue
        memset(&data, 0, sizeof(dfgm_data_t));
        while (received < sizeof(dfgm_packet_t)) {
            uint8_t *pkt = (uint8_t *)&(data.packet);
            xQueueReceive(DFGM_queue, &(pkt[received]), portMAX_DELAY);
            received++;
        }
        received = 0;

        // If runtime specified, process data
        if (secondsPassed < DFGM_runtime) {

            // Get time
            RTCMK_GetUnix(&(data.time));

            // Don't save or convert raw mag field data if receiving packet for HK
            if(!collecting_HK) {
                savePacket(&data, "raw_DFGM_data");
                DFGM_convertRawMagData(&(data.packet));
            }

            DFGM_convertRaw_HK_data(&(data.packet));
            update_HK(&data);

            // Don't save if receiving packet for HK
            if (!collecting_HK) {
                savePacket(&data, "high_rate_DFGM_data");
            }

            secondsPassed += 1;

            if (!collecting_HK && DFGM_runtime > 1) {
                // Convert packet into second struct
                secondPointer[1]->time = data.time;
                for (int sample = 0; sample < 100; sample++) {
                    tempX = *(float *)&(data.packet).tuple[sample].x;
                    tempY = *(float *)&(data.packet).tuple[sample].y;
                    tempZ = *(float *)&(data.packet).tuple[sample].z;
                    secondPointer[1]->x[sample] = tempX;
                    secondPointer[1]->y[sample] = tempY;
                    secondPointer[1]->z[sample] = tempZ;
                }

                // Filter 100 Hz packets into 1 Hz
                if (firstPacketFlag) {
                    // Ensure at least 2 packets in the buffer before filtering
                    firstPacketFlag = 0;
                    shiftSecondPointer();
                } else {
                    applyFilter();
                    saveSecond(secondPointer[1], "survey_rate_DFGM_data");
                    shiftSecondPointer();
                }

                // TODO Filter 100 Hz packets into 10 Hz
            }

            // Before the task stops processing data...
            if (secondsPassed >= DFGM_runtime) {
                // Reset the task to its original state
                secondsPassed = 0;
                DFGM_runtime = 0;
                collecting_HK = 0;
                firstPacketFlag = 1;
            }
        }
    }
}

void DFGM_init() {
    TaskHandle_t dfgm_rx_handle;
    DFGM_queue = xQueueCreate(QUEUE_DEPTH, sizeof(uint8_t));
    TX_semaphore = xSemaphoreCreateBinary();
    xTaskCreate(dfgm_rx_task, "DFGM RX", 256, NULL, DFGM_RX_PRIO,
                &dfgm_rx_handle);
    return;
}

void dfgm_sciNotification(sciBASE_t *sci, unsigned flags) {
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    switch (flags) {
    case SCI_RX_INT:
        a = xQueueSendToBackFromISR(DFGM_queue, &DFGM_byteBuffer, &xHigherPriorityTaskWoken);
        sciReceive(sci, 1, &DFGM_byteBuffer);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        break;
    case SCI_TX_INT:
        xSemaphoreGiveFromISR(TX_semaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        break;
    default: break;
    }
}

//  Hardware interface uses these functions to execute subservices
DFGM_return DFGM_startDataCollection(int givenRuntime) {
    DFGM_return status = DFGM_SUCCESS;
    if (DFGM_runtime == 0 && givenRuntime >= MIN_RUNTIME) {
        DFGM_runtime = givenRuntime;
        status = DFGM_SUCCESS;
    } else if (DFGM_runtime != 0) {
        status = DFGM_BUSY;
    } else if (givenRuntime < MIN_RUNTIME) {
        status = DFGM_BAD_PARAM;
    }
    return status;
}

DFGM_return DFGM_stopDataCollection() {
    secondsPassed = 0;
    DFGM_runtime = 0;
    collecting_HK = 0;
    firstPacketFlag = 1;

    // Will always work whether or not the data collection task is running
    return DFGM_SUCCESS;
}

DFGM_return DFGM_get_HK(dfgm_housekeeping *hk) {
    DFGM_return status = DFGM_SUCCESS;
    time_t currentTime;
    RTCMK_GetUnix(&currentTime);
    time_t timeDiff = currentTime - HK_buffer.time;

    // Update HK if buffer has old data
    if (timeDiff > TIME_THRESHOLD) {
        collecting_HK = 1;
        status = DFGM_startDataCollection(1);
        while (collecting_HK) {
            // Wait until Rx Task is done updating the HK data
        }
    }

    hk->time = HK_buffer.time;
    hk->coreVoltage = HK_buffer.coreVoltage;
    hk->sensorTemp = HK_buffer.sensorTemp;
    hk->refTemp = HK_buffer.refTemp;
    hk->boardTemp = HK_buffer.boardTemp;
    hk->posRailVoltage = HK_buffer.posRailVoltage;
    hk->inputVoltage = HK_buffer.inputVoltage;
    hk->refVoltage = HK_buffer.refVoltage;
    hk->inputCurrent = HK_buffer.inputCurrent;
    hk->reserved1 = HK_buffer.reserved1;
    hk->reserved2 = HK_buffer.reserved2;
    hk->reserved3 = HK_buffer.reserved3;
    hk->reserved4 = HK_buffer.reserved4;

    return status;
}
