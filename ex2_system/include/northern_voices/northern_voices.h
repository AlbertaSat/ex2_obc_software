/*
 * northern_voices.h
 *
 *  Created on: Sep. 21, 2022
 *      Author: Robert Taylor
 */

#ifndef EX2_SYSTEM_INCLUDE_NORTHERN_VOICES_NORTHERN_VOICES_H_
#define EX2_SYSTEM_INCLUDE_NORTHERN_VOICES_NORTHERN_VOICES_H_

#include <stdint.h>
#include <stdbool.h>
#include "system.h"

/*
 * Northern voices packet
 * Voice data packets are encoded in CSP packets
 * CSP packet addr is GS_CSP_ADDR, usually 16
 * CSP packet dport is TC_NV_SERVICE, usually 24
 * CSP packet length field is the size of the data + header
 * CSP packet data field contains nv_data_packet_header_t immediately followed by data
 * Data is length of data_size field in nv_data_packet_header_t
 * End of the CSP packet has the CSP crc32 trailer
 */

typedef struct __attribute__((packed)) {
    uint16_t data_size;
    uint16_t packetno;
} nv_data_packet_header_t;

bool start_nv_transmit(uint16_t repeats, char *filename);
bool stop_nv_transmit();
SAT_returnState start_nv_daemon();

#endif /* EX2_SYSTEM_INCLUDE_NORTHERN_VOICES_NORTHERN_VOICES_H_ */
