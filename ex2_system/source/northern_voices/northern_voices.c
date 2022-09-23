/*
 * northern_voices.c
 *
 *  Created on: Sep. 21, 2022
 *      Author: Robert Taylor
 */

#include "northern_voices/northern_voices.h"

sdr_interface_data_t *interface = 0;
int radio_bitrate = 0;
int audio_bitrate = 1300;
int bytes_per_block = 48;
int target_pause_ms = 1000;

void nv_register_interface(sdr_interface_data_t *ifdata) {
    interface = ifdata;
    radio_bitrate = sdr_uhf_get_baudrate(ifdata);
}

int get_blocks_until_pause() { return 1000; }
