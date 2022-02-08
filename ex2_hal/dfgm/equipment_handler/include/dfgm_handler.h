// Copyright stuff


#ifndef DFGM_HANDLER_H
#define DFGM_HANDLER_H

// Headers
#include "system.h"

typedef enum {
    DFGM_SUCCESS = 0,

    DFGM_BAD_READ = 1,
    DFGM_BAD_WRITE = 2,

    IS_STUBBED_DFGM = 0;
} STX_return;

// Macros for conversions from converter and filter header and source files (QUEUE_DEPTH, etc.)

// Structs from converter and filter - dfgm_data_tuple_t, dfgm_packet_t, dfgm_data_t, HK data struct etc.
// SECOND + dfgm_1Hz_file_t combo? should include timestamps, 100Hz data, and filtered data




typedef struct {
    float attribute1;
} dfgm_housekeeping;




STX_return STX_startDFGM(int runtime);
STX_return STX_stopDFGM();
STX_return STX_filterTo10Hz();
STX_return STX_filterTo1Hz();
STX_return STX_getHK(dfgm_housekeeping *hk);
