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
 * @file dfgm_handler.c
 * @author Daniel Sacro, Robert Taylor, Andrew Rooney
 * @date 2022-02-08
 */

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

#define scilinREG PRINTF_SCI // sciREG1 / UART3

#ifndef DFGM_RX_PRIO
#define DFGM_RX_PRIO (tskIDLE_PRIORITY + 1)
#endif

// DFGM_SCI is usually defined as sciREG4 / UART 1 by default
#ifndef DFGM_SCI
#define DFGM_SCI scilinREG // in case DFGM_SCI is not defined
#endif

static uint8_t DFGM_byteBuffer;
static xQueueHandle DFGM_queue;
static SemaphoreHandle_t TX_semaphore;
static DFGM_Housekeeping HK_buffer = {0};
static int last_hk_rx;

// Flags and counters used by the DFGM Rx Task
static int secondsPassed = 0;
static int DFGM_runtime = 0;
static bool DFGM_running = false;
static int firstPacketFlag = 1;

// Makes HK conversions & calculations easier via looping through each array
const float HK_scales[] = {HK_SCALE_0, HK_SCALE_1, HK_SCALE_2, HK_SCALE_3, HK_SCALE_4,  HK_SCALE_5,
                           HK_SCALE_6, HK_SCALE_7, HK_SCALE_8, HK_SCALE_9, HK_SCALE_10, HK_SCALE_11};

const float HK_offsets[] = {HK_OFFSET_0, HK_OFFSET_1, HK_OFFSET_2, HK_OFFSET_3, HK_OFFSET_4,  HK_OFFSET_5,
                            HK_OFFSET_6, HK_OFFSET_7, HK_OFFSET_8, HK_OFFSET_9, HK_OFFSET_10, HK_OFFSET_11};

// Coefficients for 1 Hz filter
double filter[81] = {
    0.014293879,    0.014285543,    0.014260564,    0.014219019,   0.014161035,   0.014086794,   0.013996516,
    0.013890488,    0.013769029,    0.013632505,    0.013481341,   0.013315983,   0.013136936,   0.012944728,
    0.012739926,    0.012523144,    0.012295013,    0.012056194,   0.011807376,   0.011549255,   0.011282578,
    0.011008069,    0.010726496,    0.010438624,    0.010145215,   0.0098470494,  0.0095449078,  0.0092395498,
    0.0089317473,   0.0086222581,   0.0083118245,   0.0080011814,  0.0076910376,  0.0073820935,  0.0070750111,
    0.0067704498,   0.0064690227,   0.0061713282,   0.0058779319,  0.0055893637,  0.0053061257,  0.0050286865,
    0.0047574770,   0.0044928941,   0.0042353003,   0.0039850195,  0.0037423387,  0.0035075136,  0.0032807556,
    0.0030622446,   0.0028521275,   0.0026505100,   0.0024574685,  0.0022730422,  0.0020972437,  0.0019300507,
    0.0017714123,   0.0016212496,   0.0014794567,   0.0013459044,  0.0012204390,  0.0011028848,  0.00099304689,
    0.00089071244,  0.00079565205,  0.00070762285,  0.00062636817, 0.00055162174, 0.00048310744, 0.00042054299,
    0.00036364044,  0.00031210752,  0.00026565061,  0.00022397480, 0.00018678687, 0.00015379552, 0.00012471308,
    9.9256833e-005, 7.7149990e-005, 5.8123173e-005, 4.1914571e-005

};

// Structs used for filtering
struct dfgm_second secondBuffer[2];
struct dfgm_second *secondPointer[2];

/**
 * @brief
 *      Converts part of the raw DFGM data packet into useful magnetic field data
 * @details
 *      Converts part of the raw DFGM data packet into useful magnetic field data by applying
 *      a specific formula that deciphers bytes into floating point values
 * @param dfgm_packet_t *const data
 *      The DFGM packet that will have part of its raw data converted into useful magnetic field
 *      data
 * @return None
 */
static void DFGM_convertRawMagData(dfgm_packet_t *const data) {
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

        // Because a float is 4 bytes big, its bytes can be stored into a uint32_t data type
        data->tuple[i].x = (*(uint32_t *)&x);
        data->tuple[i].y = (*(uint32_t *)&y);
        data->tuple[i].z = (*(uint32_t *)&z);
    }
}

/**
 * @brief
 *      Converts part of the raw DFGM data packet into useful housekeeping data
 * @details
 *      Converts part of the raw DFGM data packet into useful housekeeping data by applying
 *      a specific formula
 * @param dfgm_packet_t *const data
 *      The DFGM packet that will have part of its raw data converted into useful housekeeping
 *      data
 * @return None
 */
static void DFGM_convertRaw_HK_data(dfgm_packet_t *const data) {
    for (int i = 0; i < 12; i++) {
        float HK_value = ((float)(data->HK[i]) * HK_scales[i] + HK_offsets[i]);
        data->HK[i] = (uint16_t)HK_value;
    };
}

/**
 * @brief
 *      Updates the DFGM housekeeping data buffer
 * @details
 *      Copies the housekeeping data from the most recent DFGM data packet into a housekeeping
 *      buffer
 * @param dfgm_data_t *const data
 *      The DFGM packet that will have its housekeeping data saved into the housekeeping buffer
 * @return None
 */
void update_HK(dfgm_data_t const *data) {
    last_hk_rx = data->time;
    HK_buffer.coreVoltage = (data->packet).HK[0];
    HK_buffer.sensorTemp = (data->packet).HK[1];
    HK_buffer.refTemp = (data->packet).HK[2];
    HK_buffer.boardTemp = (data->packet).HK[3];
    HK_buffer.posRailVoltage = (data->packet).HK[4];
    HK_buffer.inputVoltage = (data->packet).HK[5];
    HK_buffer.refVoltage = (data->packet).HK[6];
    HK_buffer.inputCurrent = (data->packet).HK[7];
    HK_buffer.reserved1 = (data->packet).HK[8];
    HK_buffer.reserved2 = (data->packet).HK[9];
    HK_buffer.reserved3 = (data->packet).HK[10];
    HK_buffer.reserved4 = (data->packet).HK[11];
}

/**
 * @brief
 *      Saves a data packet's samples into a file along with the packet's time stamp
 * @details
 *      Converts a packet's data samples from uint32_t into floats, and then saves those values into a file
 *      along with the packet's time stamp
 * @param dfgm_data_t *data
 *      A DFGM data struct containing both the packet data and time stamp needed to save the samples
 * @param int fileFD
 *      The file handle to write to
 * @return None
 */
static void savePacket(dfgm_data_t *data, int fileFd) {
    if (fileFd <= 0) {
        return;
    }

    int32_t iErr;

    // Save only the magnetic field data from the packet sample by sample with time stamps
    dfgm_data_sample_t dataSample = {0};
    for (int i = 0; i < 100; i++) {
        memset(&dataSample, 0, sizeof(dfgm_data_sample_t));
        dataSample.time = data->time;
        dataSample.x = *(float *)&(data->packet).tuple[i].x;
        dataSample.y = *(float *)&(data->packet).tuple[i].y;
        dataSample.z = *(float *)&(data->packet).tuple[i].z;

        iErr = red_write(fileFd, &dataSample, sizeof(dfgm_data_sample_t));
        if (iErr == -1) {
            return;
            ;
        }
    }
}

/**
 * @brief
 *      Filters and downsamples 100 Hz magnetic field data into 1 Hz data
 * @details
 *      Applies the coefficients of a 100 to 1 Hz filter to 2 consecutive seconds of 100 Hz data to create
 *      a single second of 1 Hz data
 * @param None
 * @return None
 */
static void applyFilter(void) {
    double xFiltered, yFiltered, zFiltered;
    int i, negsamp, possamp;

    // "DC" component centered on the 0 time sample
    xFiltered = secondPointer[1]->x[0] * filter[0];
    yFiltered = secondPointer[1]->y[0] * filter[0];
    zFiltered = secondPointer[1]->z[0] * filter[0];

    // Sample indices
    negsamp = 99;
    possamp = 1;

    // Apply filter to data
    for (i = 1; i < 81; i++) {
        xFiltered += (secondPointer[0]->x[negsamp] + secondPointer[1]->x[possamp]) * filter[i];
        yFiltered += (secondPointer[0]->y[negsamp] + secondPointer[1]->y[possamp]) * filter[i];
        zFiltered += (secondPointer[0]->z[negsamp] + secondPointer[1]->z[possamp]) * filter[i];
        negsamp -= 1;
        possamp += 1;
    }

    // Copy filtered data
    secondPointer[1]->xFiltered = xFiltered;
    secondPointer[1]->yFiltered = yFiltered;
    secondPointer[1]->zFiltered = zFiltered;
}

/**
 * @brief
 *      Shifts one second pointer (1) to another second pointer (0)
 * @details
 *      Sets secondPointer[0] to reference the variable/value that secondPointer[1] was originally
 *      pointing to. This allows secondPointer[1] to point to a new variable containing a different
 *      value
 * @param None
 * @return None
 */
static void shiftSecondPointer(void) { secondPointer[0] = secondPointer[1]; }

/**
 * @brief
 *      Saves the 1 Hz data from a second struct into a file
 * @details
 *      Stores the 1 Hz data sample from the second struct into a dfgm sample struct, then saves
 *      that struct directly into the file byte by byte with a time stamp
 * @param struct dfgm_second *second
 *      The second struct that contains the data you want to save
 * @param int fileFD
 *      The file handle to write to
 * @return None
 */
static void saveSecond(struct dfgm_second *second, int fileFD) {
    int32_t iErr;

    // Open or create file
    if (fileFD <= 0) {
        return;
    }

    // Store second data into a sample struct
    dfgm_data_sample_t dataSample = {0};
    memset(&dataSample, 0, sizeof(dfgm_data_sample_t));
    dataSample.time = second->time;

    dataSample.x = (float)second->xFiltered;
    dataSample.y = (float)second->yFiltered;
    dataSample.z = (float)second->zFiltered;

    // Save sample
    iErr = red_write(fileFD, &dataSample, sizeof(dfgm_data_sample_t));
    if (iErr == -1) {
        return;
    }
}

/**
 * @brief
 *      FreeRTOS DFGM data collection & processing task
 * @details
 *      Constantly receives and reads DFGM data from a queue, but only processes and
 *      saves that data for a set amount of time when a runtime is given to it
 * @param void * pvParameters
 * @return None
 */
void dfgm_rx_task(void *pvParameters) {
    static dfgm_data_t data = {0};
    int received = 0;
    int32_t iErr = 0;

    // Initialize variables for filtering/downsampling
    secondPointer[0] = &secondBuffer[0];
    secondPointer[1] = &secondBuffer[1];
    float tempX;
    float tempY;
    float tempZ;

    // Set initial conditions for Rx Task
    secondsPassed = 0;
    DFGM_runtime = 0;
    firstPacketFlag = 1;

    // Trigger dfgm_sciNotification
    sciReceive(DFGM_SCI, 1, &DFGM_byteBuffer);

    bool dfgm_directory_initialized = false;

    while (!dfgm_directory_initialized) {
        // Enter DFGM directory
        iErr = red_chdir("VOL0:/dfgm");
        if (iErr == -1) {
            if ((red_errno == RED_ENOENT) || (red_errno == RED_ENOTDIR)) {
                // Directory does not exist. Create it
                iErr = red_mkdir("VOL0:/dfgm");
                if (iErr == -1) {
                    sys_log(ERROR, "Problem %d creating the DFGM directory", red_errno);
                    vTaskDelay(10 * ONE_SECOND);
                    continue;
                }
            }
            iErr = red_chdir("VOL0:/dfgm");
            if (iErr == -1) {
                sys_log(ERROR, "Problem %d changing into the DFGM directory", red_errno);
                vTaskDelay(10 * ONE_SECOND);
                continue;
            }
        }
        sys_log(INFO, "Successfully entered DFGM directory");
        dfgm_directory_initialized = true;
    }

    int HZ_100_fd = 0;
    int HZ_1_fd = 0;
    int HZ_raw_fd = 0;

    for (;;) {
        received = 0;

        // Always receive packets from queue
        memset(&data, 0, sizeof(dfgm_data_t));
        while (received < sizeof(dfgm_packet_t)) {
            uint8_t *pkt = (uint8_t *)&(data.packet);
            if (xQueueReceive(DFGM_queue, &(pkt[received]), pdMS_TO_TICKS(100)) != pdTRUE) {
                // No packet yet, or we started mid-way through a packet. Restart.
                received = 0;
            } else {
                received++;
            }
        }

        // Get time
        data.time = RTCMK_Unix_Now();

        // Always save HK if DFGM is on
        DFGM_convertRaw_HK_data(&(data.packet));
        update_HK(&data);

        // If a runtime is specified, process data
        if (secondsPassed >= DFGM_runtime) {
            DFGM_running = false;
        }

        if (DFGM_running == false) {
            DFGM_runtime = 0;
            secondsPassed = 0;
            firstPacketFlag = 1;

            if (HZ_100_fd > 0) {
                red_close(HZ_100_fd);
                HZ_100_fd = 0;
            }
            if (HZ_1_fd > 0) {
                red_close(HZ_1_fd);
                HZ_1_fd = 0;
            }
            if (HZ_raw_fd > 0) {
                red_close(HZ_100_fd);
                HZ_raw_fd = 0;
            }

        } else {
            if (firstPacketFlag) {
                if (HZ_100_fd > 0) {
                    red_close(HZ_100_fd);
                    HZ_100_fd = 0;
                }
                if (HZ_1_fd > 0) {
                    red_close(HZ_1_fd);
                    HZ_1_fd = 0;
                }
                if (HZ_raw_fd > 0) {
                    red_close(HZ_raw_fd);
                    HZ_raw_fd = 0;
                }

                char DFGM_raw_file_name[DFGM_FILE_NAME_MAX_SIZE] = {0};
                char DFGM_100Hz_file_name[DFGM_FILE_NAME_MAX_SIZE] = {0};
                char DFGM_1Hz_file_name[DFGM_FILE_NAME_MAX_SIZE] = {0};

                snprintf(DFGM_raw_file_name, DFGM_FILE_NAME_MAX_SIZE, "%u_%s", (unsigned int)data.time,
                         "rawDFGM.hex");
                snprintf(DFGM_100Hz_file_name, DFGM_FILE_NAME_MAX_SIZE, "%u_%s", (unsigned int)data.time,
                         "100HzDFGM.hex");
                snprintf(DFGM_1Hz_file_name, DFGM_FILE_NAME_MAX_SIZE, "%u_%s", (unsigned int)data.time,
                         "1HzDFGM.hex");

                HZ_raw_fd = red_open(DFGM_raw_file_name, RED_O_WRONLY | RED_O_CREAT | RED_O_APPEND);
                HZ_100_fd = red_open(DFGM_100Hz_file_name, RED_O_WRONLY | RED_O_CREAT | RED_O_APPEND);
                HZ_1_fd = red_open(DFGM_1Hz_file_name, RED_O_WRONLY | RED_O_CREAT | RED_O_APPEND);
            }

            // Save raw (unconverted) 100Hz data from DFGM
            savePacket(&data, HZ_raw_fd);
            DFGM_convertRawMagData(&(data.packet));

            // Save 100Hz data to DFGM
            savePacket(&data, HZ_100_fd);

            secondsPassed += 1;

            // Only try to filter/downsample data when there will be 2 or more packets
            if (DFGM_runtime > 1) {
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

                // Filter 100 Hz data into 1 Hz
                if (firstPacketFlag) {
                    // Ensure there are at least 2 packets in the buffer before filtering
                    firstPacketFlag = 0;
                    shiftSecondPointer();
                } else {
                    applyFilter();
                    // Save 1Hz (filtered) data from DFGM
                    saveSecond(secondPointer[1], HZ_1_fd);
                    shiftSecondPointer();
                }
            }
        }
    }
}

/**
 * @brief
 *      Starts the DFGM data collection & processing task
 * @details
 *      Starts the FreeRTOS task responsible for collecting and processing DFGM data packets
 * @param None
 * @return None
 */
void DFGM_init() {
    TaskHandle_t dfgm_rx_handle;
    DFGM_queue = xQueueCreate(DFGM_QUEUE_DEPTH, sizeof(uint8_t));
    TX_semaphore = xSemaphoreCreateBinary();
    xTaskCreate(dfgm_rx_task, "DFGM RX", DFGM_RX_TASK_SIZE, NULL, DFGM_RX_PRIO, &dfgm_rx_handle);

    return;
}

/**
 * @brief
 *      Handles incoming data packets from the DFGM board
 * @details
 *      Receives data packets from the DFGM board and places it inside a queue
 *      byte by byte for the DFGM Rx Task to read
 * @param sciBASE_t *sci
 *      The sciREG to read from
 * @param unsigned flags
 * @return None
 */
void dfgm_sciNotification(sciBASE_t *sci, unsigned flags) {
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    switch (flags) {
    case SCI_RX_INT:
        xQueueSendToBackFromISR(DFGM_queue, &DFGM_byteBuffer, &xHigherPriorityTaskWoken);
        sciReceive(sci, 1, &DFGM_byteBuffer); // Triggers sciNotification again to handle the next byte
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        break;
    case SCI_TX_INT:
        xSemaphoreGiveFromISR(TX_semaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        break;
    default:
        break;
    }
}

/**
 * @brief
 *      Tells the DFGM Rx task to begin processing the data it receives for a
 *      set amount of time
 * @details
 *      Sets the DFGM_runtime to a nonzero value, which let's the DFGM Rx task know
 *      to start processing data for a set amount time
 * @param int givenRuntime
 *      The amount of time that the DFGM Rx task should process data for in seconds
 * @return DFGM_return
 *      Success report
 */
DFGM_return DFGM_startDataCollection(int givenRuntime) {
    if (DFGM_running) {
        // DFGM is already running
        return DFGM_BUSY;
    }

    if (givenRuntime < DFGM_MIN_RUNTIME) {
        return DFGM_BAD_PARAM;
    }

    if (givenRuntime >= DFGM_MIN_RUNTIME) {
        DFGM_runtime = givenRuntime;
        DFGM_running = true;
        return DFGM_SUCCESS;
    }

    return DFGM_BAD_PARAM;
}

/**
 * @brief
 *      Tells the DFGM Rx task to stop processing data
 * @details
 *      Resets all the counters and flags used by the DFGM Rx Task for data collection
 *      and processing
 * @param None
 * @return DFGM_return
 *      Success report
 */
DFGM_return DFGM_stopDataCollection() {
    DFGM_running = false;
    DFGM_runtime = 0;
    secondsPassed = 0;
    firstPacketFlag = 1;
    // Will always work whether or not the data collection task is running
    return DFGM_SUCCESS;
}

/**
 * @brief
 *      Gets the most recent DFGM housekeeping data
 * @details
 *      Gets the most recent DFGM housekeeping data from a buffer. This data is
 *      guaranteed to be at most 3 minutes old
 * @param DFGM_Housekeeping *hk
 *      The DFGM housekeeping struct that will be populated with the most recent
 *      housekeeping data
 * @return DFGM_return
 *      Success report
 */
DFGM_return DFGM_get_HK(DFGM_Housekeeping *hk) {
    DFGM_return status = DFGM_SUCCESS;
    time_t currentTime = RTCMK_Unix_Now();
    time_t timeDiff = currentTime - last_hk_rx;

    // Only update HK buffer if it has old data
    if (timeDiff > DFGM_TIME_THRESHOLD) {
        status = DFGM_HK_FAIL;
    }

    // Only copy buffer contents into the DFGM_Housekeeping struct if HK collection is new
    if (status != DFGM_HK_FAIL) {
        memcpy(hk, &HK_buffer, sizeof(*hk));
    }

    return status;
}
