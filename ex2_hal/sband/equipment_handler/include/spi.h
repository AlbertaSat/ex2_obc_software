// spi.h
// Author: Thomas Ganley
// May 14, 2020

#ifndef spi_H
#define spi_H

// Mocked spi functions

#define SPI_SBAND_BUS_REG spiREG3
#define SPI_SBAND_DEF_FMT 1 // Defines SPI speed (~8.3 MHz)

#include "HL_spi.h"

void spi_writeData(void);

void SPIMasterTx(spiBASE_t *regset, uint16_t *data, uint8_t format);

void SPISbandTx(uint16_t *data);

#endif /* spi_H */
