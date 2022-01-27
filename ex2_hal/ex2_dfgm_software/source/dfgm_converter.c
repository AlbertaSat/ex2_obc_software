#include "dfgm_converter.h"
#include "HL_sci.h"
//#include "hl_sys_common.h"
#include <FreeRTOS.h>
#include <os_queue.h>
#include <stdlib.h>
#include <os_task.h>
#include "system.h"
#include "redposix.h"
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

#include "dfgm_filter.h"

#define BUFFER_SIZE 1248
#define QUEUE_DEPTH 32

#ifndef DFGM_RX_PRIO
#define DFGM_RX_PRIO configMAX_PRIORITIES - 1
#endif

#ifndef DFGM_SCI
#define DFGM_SCI scilinREG
#endif

#ifndef PRINTF_SCI
#define PRINTF_SCI scilinREG
#endif

const float HKScales[] = {HK0Scale, HK1Scale, HK2Scale, HK3Scale,
                          HK4Scale, HK5Scale, HK6Scale, HK7Scale,
                          HK8Scale, HK9Scale, HK10Scale, HK11Scale};

const float HKOffsets[] = {HK0Offset, HK1Offset, HK2Offset, HK3Offset,
                           HK4Offset, HK5Offset, HK6Offset, HK7Offset,
                           HK8Offset, HK9Offset, HK10Offset, HK11Offset};

/**
 * files:
 *  last_file.txt
 *  dfgm_<unix_timestamp>.dat
 *  data_lookup.dat
 */

#define SAMPLES_PER_FILE 10000 // this will get around 62 files per week of runtime
#define RED_TRANSACT_INTERVAL 100

static uint8_t dfgmBuffer;
static xQueueHandle dfgmQueue;
static SemaphoreHandle_t tx_semphr;

typedef struct __attribute__((packed)) {
    time_t time;
    dfgm_packet_t pkt;
} dfgm_data_t;

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
        data->tup[i].X = (uint32_t )X;
        data->tup[i].Y = (uint32_t )Y;
        data->tup[i].Z = (uint32_t )Z;
    }
}

void dfgm_convert_HK(dfgm_packet_t *const data) {
    // convert part of raw data into house keeping data
    for (int i = 0; i < 12; i++) {
        float HK_value = ((float)(data->hk[i]) * HKScales[i] + HKOffsets[i]);
        data->hk[i] = (uint16_t)HK_value;
    };
}

void save_packet(dfgm_packet_t *data, char *filename) {
    int32_t iErr;
    const char *pszVolume0 = gaRedVolConf[1].pszPathPrefix;

    // initialize reliance edge
    iErr = red_init();
    if (iErr == -1) {
        fprintf(stderr, "Unexpected error %d from red_init()\r\n", (int)red_errno);
        exit(red_errno);
    }

    // format file system volume    // does not need to be done every time
//    iErr = red_format(pszVolume0);
//    if (iErr == -1) {
//        fprintf(stderr, "Unexpected error %d from red_format()\r\n", (int)red_errno);
//        exit(red_errno);
//    }

    // mount volume
    iErr = red_mount(pszVolume0);
    if (iErr == -1) {
        fprintf(stderr, "Unexpected error %d from red_mount()\r\n", (int)red_errno);
        exit(red_errno);
    }

    // open or create file
    int32_t dataFile;
    dataFile = red_open(filename, RED_O_WRONLY | RED_O_CREAT | RED_O_APPEND);
    if (iErr == -1) {
        fprintf(stderr, "Unexpected error %d from red_open()\r\n", (int)red_errno);
        exit(red_errno);
    }

    // Save samples line by line
    char dataSample[100];
    for(int i = 0; i < 100; i++) {
        // Build string to save
        memset(dataSample, 0, sizeof(dataSample));

        // build string for only magnetic field data
        sprintf(dataSample, " %d %d %d\n",
                data->tup[i].X, data->tup[i].Y, data->tup[i].Z);

        // Save string to file
        iErr = red_write(dataFile, dataSample, strlen(dataSample)); // throws error 9 - RED_EBADF (bad file number)
        if (iErr == -1) {
            fprintf(stderr, "Unexpected error %d from red_write()\r\n", (int)red_errno);
            exit(red_errno);
        }
    }

    // close file
    iErr = red_close(dataFile);
    if (iErr == -1) {
        fprintf(stderr, "Unexpected error %d from red_close()\r\n", (int)red_errno);
        exit(red_errno);
    }
}

void print_file(char* filename) {
    int32_t iErr;
    const char *pszVolume0 = gaRedVolConf[1].pszPathPrefix;

    // initialize reliance edge
    iErr = red_init();
    if (iErr == -1) {
        fprintf(stderr, "Unexpected error %d from red_init()\r\n", (int)red_errno);
        exit(red_errno);
    }

    // format file system volume    // doesn't need to be done every time
//    iErr = red_format(pszVolume0);
//    if (iErr == -1) {
//        fprintf(stderr, "Unexpected error %d from red_format()\r\n", (int)red_errno);
//        exit(red_errno);
//    }

    // mount volume
    iErr = red_mount(pszVolume0);
    if (iErr == -1) {
        fprintf(stderr, "Unexpected error %d from red_mount()\r\n", (int)red_errno);
        exit(red_errno);
    }

    // open file
    int32_t dataFile;
    dataFile = red_open(filename, RED_O_RDONLY);
    if (iErr == -1) {
        fprintf(stderr, "Unexpected error %d from red_open()\r\n", (int)red_errno);
        exit(red_errno);
    }

    // read & print data
    char data[100000];
    iErr = red_read(dataFile, data, 100000);
    if (iErr == -1) {
        fprintf(stderr, "Unexpected error %d from red_read()\r\n", (int)red_errno);
        exit(red_errno);
    }
    else {
        printf("%s", data);
    }

    // close file
    iErr = red_close(dataFile);
    if (iErr == -1) {
        fprintf(stderr, "Unexpected error %d from red_close()\r\n", (int)red_errno);
        exit(red_errno);
    }

}

void print_packet(dfgm_packet_t * data) {
    char dataSample[100];
    for(int i = 0; i < 100; i++) {
        memset(dataSample, 0, sizeof(dataSample));
        sprintf(dataSample, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                data->tup[i].X, data->tup[i].Y, data->tup[i].Z, data->hk[0], data->hk[1], data->hk[2],
                data->hk[3], data->hk[4], data->hk[5], data->hk[6], data->hk[7], data->hk[8],
                data->hk[9], data->hk[10], data->hk[11]);
        printf("%s", dataSample);
    }
}

void send_packet(dfgm_packet_t *packet) {
    sciSend(DFGM_SCI, sizeof(dfgm_packet_t), packet);
}

void dfgm_rx_task(void *pvParameters) {
    static dfgm_data_t dat = {0};
    int received = 0;
    char input;
    for (;;) {
        /*---------------------------------------------- Test 1A ----------------------------------------------*/

        // Place a breakpoint here
        printf("%s", "Starting test 1A...");

        // receive packet from queue
        memset(&dat, 0, sizeof(dfgm_data_t));
        while (received < sizeof(dfgm_packet_t)) {
            uint8_t *pkt = (uint8_t *)&(dat.pkt);
            xQueueReceive(dfgmQueue, &(pkt[received]), portMAX_DELAY);
            received++;
        }
        received = 0;

        // converts part of raw data to magnetic field data
        dfgm_convert_mag(&(dat.pkt));
        //RTCMK_GetUnix(&(dat.time));

        // converts part of raw data to house keeping data
        dfgm_convert_HK(&(dat.pkt));

        // Print the contents of 1 packet
        print_packet(&(dat.pkt));

        // Place a breakpoint here
        printf("%s", "Test 1A complete.");

        /*---------------------------------------------- Test 1B ----------------------------------------------*/

        printf("%s", "Starting test 1B...");

        int secondsPassed = 0;
        while(secondsPassed < 300) {
            // receive packet from queue
            memset(&dat, 0, sizeof(dfgm_data_t));
            while (received < sizeof(dfgm_packet_t)) {
                uint8_t *pkt = (uint8_t *)&(dat.pkt);
                xQueueReceive(dfgmQueue, &(pkt[received]), portMAX_DELAY);
                received++;
            }
            received = 0;

            // converts part of raw data to magnetic field data
            dfgm_convert_mag(&(dat.pkt));
            //RTCMK_GetUnix(&(dat.time));

            // converts part of raw data to house keeping data
            dfgm_convert_HK(&(dat.pkt));

            // save data
            save_packet(&(dat.pkt), "high_rate_DFGM_data.txt");

            secondsPassed += 1;
        }

        // print 100 Hz data
        print_file("high_rate_DFGM_data.txt");

        // Place  a breakpoint here
        printf("%s", "Test 1B complete.");

        /*---------------------------------------------- Test 1C ----------------------------------------------*/

        printf("%s", "Starting test 1C...");

        printf("%s", "Displaying 10 Hz data: ");
//        convert_100Hz_to_10Hz("high_rate_DFGM_data.txt", "medium_rate_DFGM_data.txt");
//        print_file("medium_rate_DFGM_data.txt");

        printf("%s", "Press enter again to display 1 Hz data");
        scanf("$c", &input);
        printf("%s", "Displaying 1 Hz data: ");

        convert_100Hz_to_1Hz("high_rate_DFGM_data.txt", "survey_rate_DFGM_data.txt");
        print_file("survey_rate_DFGM_data.txt");

        // Place a breakpoint here
        printf("%s", "Test 1C complete.");
    }
}

void dfgm_init() {
    TaskHandle_t dfgm_rx_handle;
    dfgmQueue = xQueueCreate(QUEUE_DEPTH, sizeof(uint8_t));
    tx_semphr = xSemaphoreCreateBinary();
    xTaskCreate(dfgm_rx_task, "DFGM RX", 256, NULL, DFGM_RX_PRIO,
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
