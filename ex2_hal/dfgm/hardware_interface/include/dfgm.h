// Copyright stuff

#ifndef DFGM_H
#define DFGM_H

#include "dfgm_handler.h" // Equipment handler

typedef struct __attribute__((packed)) {
    float attribute1;
} DFGM_Housekeeping;

STX_return HAL_DFGM_run(int runtime);
STX_return HAL_DFGM_stop();
STX_return HAL_DFGM_filterTo10Hz();
STX_return HAL_DFGM_filterTo1Hz();
STX_return HAL_DFGM_getHK(DFGM_Housekeeping * DFGM_hk);

#endif /* DFGM_H */
