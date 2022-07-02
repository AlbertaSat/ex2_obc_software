#include "spi_io.h"
#include "FreeRTOS.h"
#include "os_task.h"
#include "HL_gio.h"
#include "HL_het.h"
#include "HL_mibspi.h"
#include "HL_spi.h"
#include "os_semphr.h"
#include "system.h"

/******************************************************************************
 Module Public Functions - Low level SPI control functions
******************************************************************************/

void SPI_Init(void) {}

BYTE SPI_RW(BYTE d) {
    TickType_t start = xTaskGetTickCount();

    while ((SD_SPI->FLG & 0x0200) == 0) { // Wait until TXINTFLG is set for previous transmission
        if (xTaskGetTickCount() - start > pdMS_TO_TICKS(1)) { //  Wait 1 ms
            return 0xFF;
        }
    }
    SD_SPI->DAT1 = d | 0x100D0000; // transmit register address

    start = xTaskGetTickCount();
    while ((SD_SPI->FLG & 0x0100) == 0) { // Wait until RXINTFLG is set when new value is received
        if (xTaskGetTickCount() - start > pdMS_TO_TICKS(1)) { //  Wait 1 ms
            return 0xFF;
        }
    }
    return ((unsigned char)SD_SPI->BUF); // Return received value
}

void SPI_Release(void) {
    WORD idx;
    for (idx = 512; idx && (SPI_RW(0xFF) != 0xFF); idx--)
        ;
}

inline void SPI_CS_Low(uint8_t bVolNum) {
    if (bVolNum == 0) {
#if IS_ATHENA == 1
        gioSetBit(gioPORTA, 3, 0); // CS LOW
        gioSetBit(hetPORT2, 6, 1); // CS HIGH
#else
        gioSetBit(hetPORT1, 12, 0); // CS LOW
        gioSetBit(hetPORT1, 14, 1); // CS HIGH

#endif
    } else if (bVolNum == 1) {

#if IS_ATHENA == 1
        gioSetBit(hetPORT2, 6, 0); // CS LOW
        gioSetBit(gioPORTA, 3, 1); // CS HIGH
#else
        gioSetBit(hetPORT1, 12, 1); // CS LOW
        gioSetBit(hetPORT1, 14, 0); // CS HIGH
#endif
    }
}

inline void SPI_CS_High(uint8_t bVolNum) {
#if IS_ATHENA == 1
    gioSetBit(gioPORTA, 3, 1); // CS HIGH
    gioSetBit(hetPORT2, 6, 1); // CS HIGH
#else
    gioSetBit(hetPORT1, 12, 1);     // CS HIGH
    gioSetBit(hetPORT1, 14, 1);     // CS HIGH
#endif
}

inline void SPI_Freq_High(void) {}

inline void SPI_Freq_Low(void) {}

void SPI_Timer_On(WORD ms, SPI_timer_handle_t *timer) { timer->timeout_tick = ms + xTaskGetTickCount(); }

inline BOOL SPI_Timer_Status(SPI_timer_handle_t *timer) { return timer->timeout_tick > xTaskGetTickCount(); }

inline void SPI_Timer_Off(SPI_timer_handle_t *timer) { timer->timeout_tick = 0; }
