#include "spi_io.h"
#include "FreeRTOS.h"
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

BYTE SPI_RW (BYTE d) {
    while ((SD_SPI->FLG & 0x0200) == 0); // Wait until TXINTFLG is set for previous transmission
    SD_SPI->DAT1 = d | 0x100D0000;    // transmit register address

    while ((SD_SPI->FLG & 0x0100) == 0); // Wait until RXINTFLG is set when new value is received
    return((unsigned char)SD_SPI->BUF);  // Return received value
}

void SPI_Release(void) {
    WORD idx;
    for (idx = 512; idx && (SPI_RW(0xFF) != 0xFF); idx--)
        ;
}

inline void SPI_CS_Low(uint8_t bVolNum) {
    if (bVolNum == 0) {
    #ifdef IS_ATHENA
        //gioSetBit(gioPORTA, 3, 0); //CS LOW
        gioSetBit(hetPORT2, 6, 0); //CS HIGH
    #else
        gioSetBit(hetPORT1, 12, 0); //CS LOW
        gioSetBit(hetPORT1, 14, 1); //CS HIGH

#endif
    } else if (bVolNum == 1) {

    #ifdef IS_ATHENA
        //gioSetBit(hetPORT2, 6, 0); //CS LOW
        gioSetBit(gioPORTA, 3, 1); //CS HIGH
    #else
        gioSetBit(hetPORT1, 12, 1); //CS LOW
        gioSetBit(hetPORT1, 14, 0); //CS HIGH
    #endif
    }
}

inline void SPI_CS_High(uint8_t bVolNum) {
#ifdef IS_ATHENA
    //gioSetBit(gioPORTA, 3, 1); //CS HIGH
    gioSetBit(hetPORT2, 6, 1); //CS HIGH
#else
    gioSetBit(hetPORT1, 12, 1);     // CS HIGH
    gioSetBit(hetPORT1, 14, 1);     // CS HIGH
#endif
}

inline void SPI_Freq_High(void) {}

inline void SPI_Freq_Low(void) {}

void SPI_Timer_On(WORD ms) {
    //    SIM_SCGC5 |= SIM_SCGC5_LPTMR_MASK;  // Make sure clock is enabled
    //    LPTMR0_CSR = 0;                     // Reset LPTMR settings
    //    LPTMR0_CMR = ms;                    // Set compare value (in ms)
    //    // Use 1kHz LPO with no prescaler
    //    LPTMR0_PSR = LPTMR_PSR_PCS(1) | LPTMR_PSR_PBYP_MASK;
    //    // Start the timer and wait for it to reach the compare value
    //    LPTMR0_CSR = LPTMR_CSR_TEN_MASK;
}

inline BOOL SPI_Timer_Status(void) { return TRUE; }

inline void SPI_Timer_Off(void) {
    //    LPTMR0_CSR = 0;                     // Turn off timer
}
