// Copyright stuff


// + Other headers used in converter.c and filter.c (Reliance Edge, etc.)
#include "dfgm_handler.h"

#include "FreeRTOS.h"
#include "HL_sci.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system.h"

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

#include <string.h>

// Macros
#ifndef DFGM_RX_PRIO
#define DFGM_RX_PRIO configMAX_PRIORITIES - 1
#endif

#ifndef DFGM_SCI
#define DFGM_SCI scilinREG
#endif

// Static values (dfgm_buffer, dfgmQueue, etc.)
static uint8_t dfgmBuffer;
static xQueueHandle dfgmQueue;
static SemaphoreHandle_t tx_semphr;
static int runtime; // Could be used as a task interrupt
static dfgm_housekeeping dfgmHKBuffer;

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


// SECOND buffer and SECOND sptr (second pointer)
// SECOND is a struct
struct SECOND secBuffer[2];
struct SECOND *sPtr[2];

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

// File system functions
void save_packet(dfgm_packet_t *data, char *filename) {
    int32_t iErr;

    // open or create file
    int32_t dataFile;
    dataFile = red_open(filename, RED_O_WRONLY | RED_O_CREAT | RED_O_APPEND);
    if (dataFile == -1) {
        printf("Unexpected error %d from red_open() in save_packet()\r\n", (int)red_errno);
        exit(red_errno);
    }

    // Save samples line by line
    char dataSample[50];
    for(int i = 0; i < 100; i++) {
        // Build string to save
        memset(dataSample, 0, sizeof(dataSample));

        // build string for only magnetic field data
        // Note that the first char should be a space (needed for parsing successive samples)
        sprintf(dataSample, "%d %d %d\n",
                data->tup[i].X, data->tup[i].Y, data->tup[i].Z);

        // Save string to file
        iErr = red_write(dataFile, dataSample, strlen(dataSample));
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

    // open or create file
    int32_t dataFile;
    dataFile = red_open(filename, RED_O_WRONLY | RED_O_CREAT | RED_O_APPEND);
    if (dataFile == -1) {
        printf("Unexpected error %d from red_open() in save_second()\r\n", (int)red_errno);
        exit(red_errno);
    }

    // build string to save using data in second
    char dataSample[50];
    sprintf(dataSample, "%d %d %d\n", second->Xfilt, second->Yfilt, second->Zfilt);

    // save 1 Hz sample
    iErr = red_write(dataFile, dataSample, strlen(dataSample));
    if (iErr == -1) {
        printf("Unexpected error %d from red_write() in save_second()\r\n", (int)red_errno);
        exit(red_errno);
    }

    // close file
    iErr = red_close(dataFile);
    if (iErr == -1) {
        printf("Unexpected error %d from red_close() in save_second()\r\n", (int)red_errno);
        exit(red_errno);
    }
}

void convert_100Hz_to_1Hz(char * filename100Hz, char * filename1Hz) {
    /*------------------- Initialization ------------------*/
    // Assumes file system is already initialized, formatted, and mounted
    int32_t iErr;

    // open file
    int32_t dataFile100Hz;
    dataFile100Hz = red_open(filename100Hz, RED_O_RDONLY);
    if (dataFile100Hz == -1) {
        printf("Unexpected error %d from red_open() in filter\r\n", (int)red_errno);
        exit(red_errno);
    }

    // initialize variables for string building
    char line[50] = {0};
    char c;
    int i = 0;

    // a SECOND struct contains both the 100 X-Y-Z samples and the 1 filtered X-Y-Z sample
    // sptr probably stands for "second pointer"
    sptr[0] = &buffer[0];
    sptr[1] = &buffer[1];

    // There must be 2 packets of data before filtering can start
    int firstPacketFlag = 1;

    /*------------------- Read packets line by line (sample by sample) ------------------*/

    // Read the first character
    int bytes_read = red_read(dataFile100Hz, &c, 1);
    if (bytes_read == -1) {
        printf("Unexpected error %d from red_read() in filter\r\n", (int)red_errno);
        exit(red_errno);
    } else if (bytes_read == 0) {
        printf("%s is empty!\n", filename100Hz);
    }

    // Read file line by line
    while (bytes_read != 0) {
        // Assumes there are 100 samples per packet in the file
        int sample = 0;
        while (sample < 100) {
            // Reads one sample containing 3 numbers for X, Y, Z mag. coords
            if (c != '\n') {
                line[i] = c;
                i += 1;
                bytes_read = red_read(dataFile100Hz, &c, 1);
            } else {
                line[i] = '\0';

                // Parse, convert, and store coords
                char * token;
                token = strtok(line, " ");
                sptr[1]->X[sample] = strtod(token, NULL);

                token = strtok(NULL, " ");
                sptr[1]->Y[sample] = strtod(token, NULL);

                token = strtok(NULL, " ");
                sptr[1]->Z[sample] = strtod(token, NULL);

                // Move onto next sample
                memset(line, 0, sizeof(line));
                i = 0;
                sample += 1;
                bytes_read = red_read(dataFile100Hz, &c, 1);
            }
        }

        /*---------------------- Apply filter and save filtered sample to a file ----------------------*/
        if (firstPacketFlag) {
            // Ensures that there are at least 2 packets in the buffer before filtering
            shift_sptr();
            firstPacketFlag = 0;
        } else {
            apply_filter();
            save_second(sptr[1], filename1Hz);
            shift_sptr();
        }
    }

    iErr = red_close(dataFile100Hz);
    if (iErr == -1) {
        printf("Unexpected error %d from red_close() in filter\r\n", (int)red_errno);
        exit(red_errno);
    }
}

// DFGM RX Task, dfgm_init(), dfgm_sciNotification()
void dfgm_rx_task(void *pvParameters) {
    static dfgm_data_t dat = {0};
    int received = 0;
    int secondsPassed;
    int32_t iErr = 0;

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
    iErr = red_mount(pszVolume0);
    if (iErr == -1) {
        printf("Unexpected error %d from red_mount()\r\n", (int)red_errno);
        exit(red_errno);
    }

    clear_file("high_rate_DFGM_data");
    clear_file("survey_rate_DFGM_data");
    clear_file("high_rate_DFGM_data.txt");
    clear_file("survey_rate_DFGM_data.txt");

    for (;;) {
        // Wait until a valid runtime is given
        secondsPassed = 0;
        while(secondsPassed >= runtime) {
            ;
        }

        // Run the dfgm "task" for x seconds
        // If runtime is ever set to some value that causes the while condition to become false, "task" stops
        // This could work for the HAL_stop command
        while(secondsPassed < runtime) {
            // receive packet from queue
            memset(&dat, 0, sizeof(dfgm_data_t));
            while (received < sizeof(dfgm_packet_t)) {
                uint8_t *pkt = (uint8_t *)&(dat.pkt);
                xQueueReceive(dfgmQueue, &(pkt[received]), portMAX_DELAY);
                received++;
            }
            received = 0;

            // Save raw data to somehere?

            // converts part of raw data to magnetic field data
            dfgm_convert_mag(&(dat.pkt));
            //RTCMK_GetUnix(&(dat.time));

            // converts part of raw data to house keeping data
            dfgm_convert_HK(&(dat.pkt));

            // save data
            save_packet(&(dat.pkt), "high_rate_DFGM_data.txt");

            secondsPassed += 1;
        }
    }
}

void dfgm_init() {
    TaskHandle_t dfgm_rx_handle;
    dfgmQueue = xQueueCreate(QUEUE_DEPTH*10, sizeof(uint8_t));
    tx_semphr = xSemaphoreCreateBinary();
    xTaskCreate(dfgm_rx_task, "DFGM RX", 20000, NULL, DFGM_RX_PRIO,
                &dfgm_rx_handle);
    sciReceive(DFGM_SCI, 1, &dfgmBuffer);
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

//Actual function implementations (uses any combination of the code above)
//STX_return STX_startDFGM(int runtime);
//STX_return STX_stopDFGM();
//STX_return STX_filterTo10Hz();
//STX_return STX_filterTo1Hz();
//STX_return STX_getHK(dfgm_housekeeping *hk);
