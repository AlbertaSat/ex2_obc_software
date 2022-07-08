// spi.h
// Author: Thomas Ganley
// May 14, 2020

#ifndef spi_H
#define spi_H

// Mocked spi functions

#define SPI_SBAND_BUS_REG spiREG3
#define SPI_SBAND_DEF_FMT 0

#include "HL_spi.h"
#include "system.h"

void spi_writeData(void);

void SPIMasterTx(spiBASE_t *regset, uint16_t *data, uint32_t length, uint8_t format);

void SPISbandTx(uint8_t *data, uint32_t length);

#endif /* spi_H */
