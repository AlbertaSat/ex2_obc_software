// spi.h
// Author: Thomas Ganley
// May 14, 2020

#ifndef spi_H
#define spi_H

// Mocked spi functions

#define SPI_SBAND_BUS_REG spiREG3

void  spi_writeData(void);

void SPIMasterTx(spiBASE_t * regset, uint16_t * data);

void SPISbandTx(uint16_t * data);

#endif /* spi_H */
