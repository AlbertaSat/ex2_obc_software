/*
 * config.h
 *
 *  Created on: Jul. 2, 2022
 *      Author: Robert
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define IS_ATHENA 0
#define IS_ATHENA_V2 0

#define IS_SATELLITE 0
#if IS_SATELLITE == 1
#define IS_FLATSAT 0
#define IS_EXALTA2 1
#define IS_YUKONSAT 0
#define IS_AURORASAT 0
#endif

#define EXECUTE_LEOP 0

#define BOOTLOADER_PRESENT 0
#define GOLDEN_IMAGE 1
#define WORKING_IMAGE 0

#define HAS_SD_CARD 0
#if HAS_SD_CARD == 1
#define SD_CARD_REFORMAT 0
#endif

#define ATHENA_IS_STUBBED 1
#define UHF_IS_STUBBED 1
#define ADCS_IS_STUBBED 1
#define SBAND_IS_STUBBED 1
#define EPS_IS_STUBBED 1
#define HYPERION_IS_STUBBED 1
#define CHARON_IS_STUBBED 1
#define DFGM_IS_STUBBED 1
#define WATCHDOG_IS_STUBBED 1
#define IRIS_IS_STUBBED 1
#define NS_IS_STUBBED 1

#define HYPERION_PANEL_3U 0
#define HYPERION_PANEL_2U 0
#define HYPERION_PANEL_2U_LIMITED 0

#define IS_SN0072_EPS 0
#define IS_B16_GPS 0 // Ex2 flight model gps only

#define UHF_USE_I2C_CMDS 0

#define SBAND_COMMERCIAL_FREQUENCY 1

#define USE_RADIO_ERROR_CORRECTION 1

/* Only defined for final flight build of software */
#define FLIGHT_CONFIGURATION 0

#define CSP_FREERTOS 1
#define CSP_USE_SDR 1
#define CSP_USE_KISS 0

#define CSP_ADDRESS 1
#define CSP_HOSTNAME "Debug"
#define EPS_ADDRESS 4

/* Define SDR_NO_CSP==0 to use CSP for SDR */
#define SDR_NO_CSP 0
#define OS_FREERTOS

#define FLATSAT_TEST 0

#endif /* MAIN_CONFIG_H_ */
