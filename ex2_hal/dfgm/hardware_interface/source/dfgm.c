// Copyright stuff

#include "dfgm.h"
#include "dfgm_handler.h"

#include <FreeRTOS.h>
#include <csp/csp_endian.h>
#include <os_queue.h>

#include "services.h"

// Static variable for HK?



STX_return HAL_DFGM_run(int runtime) {
    STX_return status;
#ifndef DFGM_IS_STUBBED
    status = STX_startDFGM(runtime);
#else
    status = IS_STUBBED_DFGM;
#endif
    // something
    return status;
}

STX_return HAL_DFGM_stop() {
    STX_return status;
#ifndef DFGM_IS_STUBBED
    status = STX_stopDFGM();
#else
    status = IS_STUBBED_DFGM;
#endif
    // something
    return status;
}

STX_return HAL_DFGM_filterTo10Hz() {
    STX_return status;
#ifndef DFGM_IS_STUBBED
    status = STX_filter10Hz;
#else
    status = IS_STUBBED_DFGM;
#endif
    return status;
}

STX_return HAL_DFGM_filterTo1Hz() {
    STX_return status;
#ifndef DFGM_IS_STUBBED
    status = STX_filter1Hz;
#else
    status = IS_STUBBED_DFGM;
#endif
    return status;
}

STX_return HAL_DFGM_getHK(DFGM_Housekeeping * DFGM_hk) {
    STX_return status;
#ifndef DFGM_IS_STUBBED
    status = STX_getHK(dfgm_housekeeping *hk);
#else
    // Set HK data

    // Return status
    status = IS_STUBBED_DFGM;
#endif
    // Save to HK buffer ?
    return status;
}
