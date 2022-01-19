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

    // convert raw data to magnetic field data
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
        data->tup[i].X = *(uint32_t *)&X;
        data->tup[i].Y = *(uint32_t *)&Y;
        data->tup[i].Z = *(uint32_t *)&Z;
    }
}

void dfgm_convert_HK(dfgm_packet_t *const data) {
    for (int i = 0; i < 12; i++) {
        float HK_value = ((float)(data->hk[i]) * HKScales[i] + HKOffsets[i]);
        data->hk[i] = *(uint16_t *)&HK_value;
    };
}

void send_packet(dfgm_packet_t *packet) {
    sciSend(DFGM_SCI, sizeof(dfgm_packet_t), packet);
}

void dfgm_rx_task(void *pvParameters) {
    static dfgm_data_t dat = {0};
    int received = 0;
    for (;;) {
        memset(&dat, 0, sizeof(dfgm_data_t));
        // receive packet from queue
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

        send_packet(&(dat.pkt));
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
