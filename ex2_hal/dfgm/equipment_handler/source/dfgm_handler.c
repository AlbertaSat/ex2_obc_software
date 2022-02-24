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
#include <string.h>
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

#include <string.h>

// For debugging...?
#include "printf.h"

#define scilinREG PRINTF_SCI

// Macros
#ifndef DFGM_RX_PRIO
#define DFGM_RX_PRIO (tskIDLE_PRIORITY + 1) //configMAX_PRIORITIES - 1
#endif

#ifndef DFGM_SCI
#define DFGM_SCI scilinREG
#endif

// Static values (dfgm_buffer, dfgmQueue, etc.)
static uint8_t dfgmBuffer;
static xQueueHandle dfgmQueue;
static SemaphoreHandle_t tx_semphr;
static int dfgmRuntime;
static dfgm_housekeeping dfgmHKBuffer = {0};
static int collectingHK;

// HKScales and HKOffsets array
const float HKScales[] = {HK0Scale, HK1Scale, HK2Scale, HK3Scale,
                          HK4Scale, HK5Scale, HK6Scale, HK7Scale,
                          HK8Scale, HK9Scale, HK10Scale, HK11Scale};

const float HKOffsets[] = {HK0Offset, HK1Offset, HK2Offset, HK3Offset,
                           HK4Offset, HK5Offset, HK6Offset, HK7Offset,
                           HK8Offset, HK9Offset, HK10Offset, HK11Offset};

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


struct SECOND secBuffer[2]; // SECOND buffer
struct SECOND *sptr[2]; // SECOND pointer

// Converter functions
void dfgm_convert_mag(dfgm_packet_t *const data) {

    // convert part of raw data to magnetic field data
    int i;
    for (i = 0; i < 100; i++) {
        short xdac = (data->tup[i].X) >> 16;
        short xadc = ((data->tup[i].X) % (1 << 16));
        short ydac = (data->tup[i].Y) >> 16;
        short yadc = ((data->tup[i].Y) % (1 << 16));
        short zdac = (data->tup[i].Z) >> 16;
        short zadc = ((data->tup[i].Z) % (1 << 16));
        float X = (XDACScale * (float)xdac + XADCScale * (float)xadc + XOffset);
        float Y = (YDACScale * (float)ydac + YADCScale * (float)yadc + YOffset);
        float Z = (ZDACScale * (float)zdac + ZADCScale * (float)zadc + ZOffset);
        data->tup[i].X = (*(uint32_t *)&X);
        data->tup[i].Y = (*(uint32_t *)&Y);
        data->tup[i].Z = (*(uint32_t *)&Z);
    }
}

void dfgm_convert_HK(dfgm_packet_t *const data) {
    // convert part of raw data into house keeping data
    for (int i = 0; i < 12; i++) {
        float HK_value = ((float)(data->hk[i]) * HKScales[i] + HKOffsets[i]);
        data->hk[i] = (uint16_t)HK_value;
    };
}

void update_HK(dfgm_data_t const *data) {
    dfgmHKBuffer.timestamp = data->time;
    dfgmHKBuffer.coreVoltage = (float) ((data->pkt).hk[0]); // Leave voltage values in mV or V?
    dfgmHKBuffer.sensorTemp = (float) ((data->pkt).hk[1]);
    dfgmHKBuffer.refTemp = (float) ((data->pkt).hk[2]);
    dfgmHKBuffer.boardTemp = (float) ((data->pkt).hk[3]);
    dfgmHKBuffer.posRailVoltage = (float) ((data->pkt).hk[4]);
    dfgmHKBuffer.inputVoltage = (float) ((data->pkt).hk[5]);
    dfgmHKBuffer.refVoltage = (float) ((data->pkt).hk[6]);
    dfgmHKBuffer.inputCurrent = (float) ((data->pkt).hk[7]);
    dfgmHKBuffer.reserved1 = (float) ((data->pkt).hk[8]);
    dfgmHKBuffer.reserved2 = (float) ((data->pkt).hk[9]);
    dfgmHKBuffer.reserved3 = (float) ((data->pkt).hk[10]);
    dfgmHKBuffer.reserved4 = (float) ((data->pkt).hk[11]);
}

// File system functions
void save_packet(dfgm_data_t *data, char *filename) {
    int32_t iErr;

    // open or create file
    int32_t dataFile;
    dataFile = red_open(filename, RED_O_WRONLY | RED_O_CREAT | RED_O_APPEND);
    if (dataFile == -1) {
        printf("Unexpected error %d from red_open() in save_packet()\r\n", (int)red_errno);
        exit(red_errno);
    }

    // Save only mag field data sample by sample w/ timestamps
    dfgm_data_sample_t dataSample = {0};
    for (int i = 0; i < 100; i++) {
        memset(&dataSample, 0, sizeof(dfgm_data_sample_t));
        dataSample.time = data->time;
        dataSample.X = (data->pkt).tup[i].X;
        dataSample.Y = (data->pkt).tup[i].Y;
        dataSample.Z = (data->pkt).tup[i].Z;

        iErr = red_write(dataFile, &dataSample, sizeof(dfgm_data_sample_t));
        if (iErr == -1) {
            printf("Unexpected error %d from red_write() in save_packet()\r\n", (int)red_errno);
            exit(red_errno);
        }
    }

    // close file
    iErr = red_close(dataFile);
    if (iErr == -1) {
        printf("Unexpected error %d from red_close() in save_packet()\r\n", (int)red_errno);
        exit(red_errno);
    }
}

// Might need modification
void clear_file(char* filename) {
    int32_t iErr;
    int32_t dataFile;

    // Clear file if it exists, otherwise do nothing
    dataFile = red_open(filename, RED_O_RDONLY);
    if (dataFile != -1) {
        // File opens if it exists
        iErr = red_close(dataFile);
        if (iErr == -1) {
            printf("Unexpected error %d from red_close() in clear_file()\r\n", (int)red_errno);
            exit(red_errno);
        }

        // Delete file
        iErr = red_unlink(filename);
        if (iErr == -1) {
            printf("Unexpected error %d from red_unlink() in clear_file()\r\n", (int)red_errno);
            exit(red_errno);
        }
    }
}

// Filter functions
void apply_filter(void) {
  /*  The 41 point filter is applied to every other data point */
  double Xfilt, Yfilt, Zfilt;
  int i, negsamp, possamp, negsec, possec;

  /*  "DC" component centred on the 0 time sample */
  Xfilt = sptr[1]->X[0] * filter[0];
  Yfilt = sptr[1]->Y[0] * filter[0];
  Zfilt = sptr[1]->Z[0] * filter[0];

  negsamp = 99; // sample indices
  possamp = 1;

  negsec = 5; // second indices
  possec = 6;

  // Apply filter to data
  for (i = 1; i < 81; i++) {
    // Apply filter to data
    Xfilt += (sptr[0]->X[negsamp] + sptr[1]->X[possamp]) * filter[i];
    Yfilt += (sptr[0]->Y[negsamp] + sptr[1]->Y[possamp]) * filter[i];
    Zfilt += (sptr[0]->Z[negsamp] + sptr[1]->Z[possamp]) * filter[i];
    negsamp -= 1;
    possamp += 1;
  }

  // Save filtered data to struct
  sptr[1]->Xfilt = Xfilt;
  sptr[1]->Yfilt = Yfilt;
  sptr[1]->Zfilt = Zfilt;
}

void shift_sptr(void) {
  sptr[0] = sptr[1];
}

void save_second(struct SECOND *second, char * filename) {
    int32_t iErr;

    int32_t dataFile;
    dataFile = red_open(filename, RED_O_WRONLY | RED_O_CREAT | RED_O_APPEND);
    if (dataFile == -1) {
        printf("Unexpected error %d from red_open() in save_second()\r\n", (int)red_errno);
        exit(red_errno);
    }

    dfgm_data_sample_t dataSample = {0};
    memset(&dataSample, 0, sizeof(dfgm_data_sample_t));
    dataSample.time = second->time;

    // Preserve float characteristics
    float X = second->Xfilt;
    float Y = second->Yfilt;
    float Z = second->Zfilt;
    dataSample.X = (*(uint32_t *)&X);
    dataSample.Y = (*(uint32_t *)&Y);
    dataSample.Z = (*(uint32_t *)&Z);

    iErr = red_write(dataFile, &dataSample, sizeof(dfgm_data_sample_t));
    if (iErr == -1) {
        printf("Unexpected error %d from red_write() in save_second()\r\n", (int)red_errno);
        exit(red_errno);
    }

    iErr = red_close(dataFile);
    if (iErr == -1) {
        printf("Unexpected error %d from red_close() in save_second()\r\n", (int)red_errno);
        exit(red_errno);
    }
}

void convert_100Hz_to_1Hz(char *filename100Hz, char *filename1Hz) {
    /*------------------- Initialization ------------------*/
    int32_t iErr;

    // Assume file system is already initialized, formatted, and mounted
    int32_t dataFile100Hz;
    dataFile100Hz = red_open(filename100Hz, RED_O_RDONLY);
    if (dataFile100Hz == -1) {
        printf("Unexpected error %d from red_open() in filter\r\n", (int)red_errno);
        exit(red_errno);
    }

    sptr[0] = &secBuffer[0];
    sptr[1] = &secBuffer[1];

    // There must be 2 packets of data before filtering can start
    int firstPacketFlag = 1;

    /*------------------- Read packets sample by sample ------------------*/
    dfgm_data_sample_t dataSample = {0};
    int EOF_reached = 0;
    int bytes_read = 0;

    // Read file sample by sample until EOF is reached
    while (1) {
        // Assume there are 100 samples per packet in the file
        for (int sample = 0; sample < 100; sample++) {
            memset(&dataSample, 0, sizeof(dfgm_data_sample_t));
            bytes_read = red_read(dataFile100Hz, &dataSample, sizeof(dfgm_data_sample_t));
            if (bytes_read == -1) {
                printf("Unexpected error %d from red_read() in filter\r\n", (int) red_errno);
            } else if (bytes_read == 0){
                EOF_reached = 1;
                break;
            }

            sptr[1]->time = dataSample.time;
            sptr[1]->X[sample] = dataSample.X;
            sptr[1]->Y[sample] = dataSample.Y;
            sptr[1]->Z[sample] = dataSample.Z;
        }

        /*---------------------- Apply filter and save filtered sample to a file ----------------------*/
        time_t pktTimeDiff = sptr[1]->time - sptr[0]->time;

        if (EOF_reached) {
            break;
        } else if (firstPacketFlag) {
            // Ensure there are at least 2 packets in the buffer before filtering
            firstPacketFlag = 0;
            shift_sptr();
        } else if (!firstPacketFlag && pktTimeDiff != 1) {
            // Consecutive packets must belong to the same data set
            shift_sptr();
        } else {
            apply_filter();
            save_second(sptr[1], filename1Hz);
            shift_sptr();
        }
    }

    iErr = red_close(dataFile100Hz);
    if (iErr == -1) {
        printf("Unexpected error %d from red_close in filter\r\n", (int) red_errno);
    }
}

void update_1HzFile(void) {
    char filename1Hz[] = "survey_rate_DFGM_data";
    char filename100Hz[] = "high_rate_DFGM_data";

    clear_file(filename1Hz);
    convert_100Hz_to_1Hz(filename100Hz, filename1Hz);
}

// FreeRTOS
void dfgm_rx_task(void *pvParameters) {
    static dfgm_data_t dat = {0};
    int received = 0;
    int secondsPassed;
    int32_t iErr = 0;
    int triggerSciNotif;

    // Set up file system before task is actually run
    const char *pszVolume0 = gaRedVolConf[0].pszPathPrefix;

    // initialize reliance edge
    iErr = red_init();
    if (iErr == -1) {
        printf("Unexpected error %d from red_init()\r\n", (int)red_errno);
        exit(red_errno);
    }

    // format file system volume    // does not need to be done every time
//    iErr = red_format(pszVolume0);
//    if (iErr == -1) {
//        printf("Unexpected error %d from red_format()\r\n", (int)red_errno);
//        exit(red_errno);
//    }

    // mount volume
//    iErr = red_mount(pszVolume0);
//    if (iErr == -1) {
//        printf("Unexpected error %d from red_mount()\r\n", (int)red_errno);
//        exit(red_errno);
//    }


    // Set initial conditions
    secondsPassed = 0;
    dfgmRuntime = 0;
    collectingHK = 0;
    triggerSciNotif = 1;

    for (;;) {
        // Wait until a valid runtime is given
        if (secondsPassed < dfgmRuntime) {
            printf("Runtime received!\t");
            // Read first byte from DFGM to trigger interrupt handler
            if (triggerSciNotif) {
                printf("sciTrigger\t");
                sciReceive(DFGM_SCI, 1, &dfgmBuffer);
                triggerSciNotif = 0;
            }


            // Collect data for the specified runtime
            while(secondsPassed < dfgmRuntime) {
                //DFGM_stopDataCollection();
                printf("Seconds passed: %d\t", secondsPassed);
                // Receive packet from queue
                memset(&dat, 0, sizeof(dfgm_data_t));
                while (received < sizeof(dfgm_packet_t)) {
                    uint8_t *pkt = (uint8_t *)&(dat.pkt);
                    xQueueReceive(dfgmQueue, &(pkt[received]), portMAX_DELAY);
                    received++;
                }
                received = 0;

                printf("Packet received!\t");

                // Time stamp
                RTCMK_GetUnix(&(dat.time));

                // Don't save or convert raw mag field data if receiving packet for HK
                if(!collectingHK) {
                    //save_packet(&(dat.pkt), "raw_DFGM_data");
                    dfgm_convert_mag(&(dat.pkt));
                    printf("Mag field data converted!\t");
                }

                dfgm_convert_HK(&(dat.pkt));
                update_HK(&dat);

                printf("HK data converted!\t");

                // Don't save if receiving packet for HK
                if (!collectingHK) {
                    save_packet(&dat, "high_rate_DFGM_data");
                }

                secondsPassed += 1;

                // Check if data gets corrupt (i.e. gets stored into things with an offset of 1 byte)
                for (int i = 0; i < 12; i++) {
                    printf("%d ", dat.pkt.hk[i]);
                }
            }

            // Don't update if receiving packet for HK
            if (!collectingHK) {
                update_1HzFile();
                //update_10HzFile();
            }

            // Reset to original state
            secondsPassed = 0;
            dfgmRuntime = 0;
            collectingHK = 0;

            // For debugging
            printf("Runtime reset. \t");

            clear_file("high_rate_DFGM_data");
            clear_file("survey_rate_DFGM_data");

            printf("Files cleared. \t");
        } else {
            // Nothing, just wait
            printf("Waiting for runtime...\t");
            DFGM_startDataCollection(20);

        }
    }
}

void dfgm_init() {
    TaskHandle_t dfgm_rx_handle;
    dfgmQueue = xQueueCreate(QUEUE_DEPTH*10, sizeof(uint8_t));
    tx_semphr = xSemaphoreCreateBinary();
    xTaskCreate(dfgm_rx_task, "DFGM RX", 256, NULL, DFGM_RX_PRIO,
                &dfgm_rx_handle);
    return;
}

void dfgm_sciNotification(sciBASE_t *sci, unsigned flags) {
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    switch (flags) {
    case SCI_RX_INT:
        xQueueSendToBackFromISR(dfgmQueue, &dfgmBuffer, &xHigherPriorityTaskWoken);
        sciReceive(sci, 1, &dfgmBuffer);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        break;
    case SCI_TX_INT:
        xSemaphoreGiveFromISR(tx_semphr, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        break;
    default: break;
    }
}

//  Hardware interface uses these functions to execute subservices
DFGM_return DFGM_startDataCollection(int givenRuntime) {
    DFGM_return status = DFGM_SUCCESS;
    if (dfgmRuntime == 0 && givenRuntime >= minRuntime) {
        dfgmRuntime = givenRuntime;
        status =  DFGM_SUCCESS;
    } else if (dfgmRuntime != 0) {
        status = DFGM_BUSY;
    } else if (givenRuntime < minRuntime) {
        status = DFGM_BAD_PARAM;
    }
    return status;
}

DFGM_return DFGM_stopDataCollection() {
    dfgmRuntime = 0;

    // Will always work whether or not the data collection task is running
    return DFGM_SUCCESS;
}

DFGM_return DFGM_getHK(dfgm_housekeeping *hk) {
    DFGM_return status = DFGM_SUCCESS;
    time_t currentTime;
    RTCMK_GetUnix(&currentTime);
    time_t timeDiff = currentTime - dfgmHKBuffer.timestamp;

    // Update HK if buffer has old data
    if (timeDiff > timeThreshold) {
        collectingHK = 1;
        status = DFGM_startDataCollection(1);
    }

    hk->timestamp = dfgmHKBuffer.timestamp;
    hk->coreVoltage = dfgmHKBuffer.coreVoltage;
    hk->sensorTemp = dfgmHKBuffer.sensorTemp;
    hk->refTemp = dfgmHKBuffer.refTemp;
    hk->boardTemp = dfgmHKBuffer.boardTemp;
    hk->posRailVoltage = dfgmHKBuffer.posRailVoltage;
    hk->inputVoltage = dfgmHKBuffer.inputVoltage;
    hk->refVoltage = dfgmHKBuffer.refVoltage;
    hk->inputCurrent = dfgmHKBuffer.inputCurrent;
    hk->reserved1 = dfgmHKBuffer.reserved1;
    hk->reserved2 = dfgmHKBuffer.reserved2;
    hk->reserved3 = dfgmHKBuffer.reserved3;
    hk->reserved4 = dfgmHKBuffer.reserved4;

    return status;
}
