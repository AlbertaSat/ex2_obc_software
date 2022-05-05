#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <redconf.h>
#include <redfs.h>
#include <redfse.h>
#include <redposix.h>
#include <redtests.h>
#include <redvolume.h>

#include "adcs_handler.h"
#include "adcs_io.h"
#include "FreeRTOS.h"

// Programs List Enumeration Values
#define BOOTLOADER 0
#define INTERNAL_FLASH_PROGRAM 1

// ADCS Run modes
#define ONEHZ_LOOP_ENABLE 1

// Att. Est. modes
#define MEMS_RATE_SENSING_MODE      1
#define MAG_RATE_FILTER_MODE        2
#define MAG_RATE_FILTER_W_PITCH_EST 3

// Att. Ctrl. modes
#define DETUMBLING_MODE 1
#define Y_THOMSON_MODE 2

// TLM Logging
#define TLM_LOG_SDCARD_0 0
#define TLM_LOG_PERIOD_10s 10
#define TLM_LOG_PERIOD_1s   1
#define TLM_LOG_PERIOD_STOP 0
#define TLM_LOG_1 1
#define TLM_LOG_2 2

void binaryTest(void);

void CubeMag_Common_Test(bool);
void CubeTorquers_Common_Test(void);
void binaryTest_CubeSense1(void);
void ReactionWheels_Common_Test(uint8_t wheel_number);
void binaryTest_CubeACP();

void commissioning_init_angular_rates_est(void);
void commissioning_initial_detumbling(void);
void commissioning_mag_calibration(void);
void commissioning_ang_rate_pitch_angle_est(void);
