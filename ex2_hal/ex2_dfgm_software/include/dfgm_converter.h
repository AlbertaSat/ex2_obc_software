#ifndef DFGM_CONVERTER_H
#define DFGM_CONVERTER_H

#include <stdint.h>
#include "HL_sci.h"
/**
 * File name convention
 * ccc-xxxxxxxxxx
 */

// mag field constants
#define XDACScale 1.97477
#define XADCScale 0.0057
#define XOffset 0
#define YDACScale 1.97477
#define YADCScale 0.0057
#define YOffset 0
#define ZDACScale 1.97477
#define ZADCScale 0.0057
#define ZOffset 0

// HK constants
#define HK0Scale (2.5*1000000.0/(4096.0*5000.0)) // for sensor temp
#define HK0Offset -273.15
#define HK1Scale 0.0313 // for board temp
#define HK1Offset -20.51
#define HK2Scale 0.0313 // for reference temp
#define HK2Offset -20.51
#define HK3Scale (10.1*2.5/4096.0) // for input voltage
#define HK3Offset 0
#define HK4Scale 0.107 // for input current
#define HK4Offset 0
#define HK5Scale (2.5/4096.0) //for core voltage
#define HK5Offset 0
#define HK6Scale (3.0*2.5/4096.0) // for positive rail voltage
#define HK6Offset 0
#define HK7Scale (2.5/4096.0) // for reference voltage
#define HK7Offset 0
#define HK8Scale 1 // for regPSUOFfset
#define HK8Offset 0
#define HK9Scale 1 // for regPSUDeadSpace
#define HK9Offset 0
#define HK10Scale 1 // for controlChannel
#define HK10Offset 0
#define HK11Scale 1 // for serialRxData
#define HK11Offset 0

typedef struct __attribute__((packed)) {
    uint32_t X; // [xdac, xadc]
    uint32_t Y; // [ydac, yadc]
    uint32_t Z; // [zdac, zadc]
} dfgm_data_tuple_t;

typedef struct __attribute__((__packed__)) {
    uint8  dle;
    uint8  stx;
    uint8  pid;
    uint8  packet_type;
    uint16 packet_length;
    uint16 fs;
    uint32 pps_offset;
    uint16 hk[12];
    dfgm_data_tuple_t tup[100];
    uint16 board_id;
    uint16 sensor_id;
    uint8  reservedA;
    uint8  reservedB;
    uint8  reservedC;
    uint8  reservedD;
    uint8  reservedE;
    uint8 etx;
    uint16 crc;
} dfgm_packet_t;

/**
 * @brief convert part of raw DFGM data to magnetic field data
 * 
 * @param data DFGM packet to process
 */
void dfgm_convert_mag(dfgm_packet_t * const data);

/**
 * @brief convert part of raw DFGM data to house keeping data
 *
 * @param data DFGM packet to process
 */
void dfgm_convert_HK(dfgm_packet_t * const data);

/**
 * @brief save data packet onto OBC
 *
 * @param data DFGM packet to save
 */
void save_packet(dfgm_packet_t *data, char *filename);

/**
 * @brief read dfgm data file
 *
 * @param name of file to save data to
 */
void print_file(char* filename); // temp function for binary testing

/**
 * @brief read dfgm data packet
 *
 * @param packet data to be printed
 */
void print_packet(dfgm_packet_t *data);

/**
 * @brief Send DFGM data to terminal via serial port
 * 
 * @param packet Data to be displayed
 */
void send_packet(dfgm_packet_t *packet);

/**
 * @brief Initialize the DFGM interface
 */
void dfgm_init();

/**
 * @brief DFGM interrupt handling hook
 * 
 * @param sci 
 * @param flags 
 */
//static void dfgm_sciNotification(sciBASE_t *sci, unsigned flags);

#endif /* DFGM_CONVERTER_H_ */
