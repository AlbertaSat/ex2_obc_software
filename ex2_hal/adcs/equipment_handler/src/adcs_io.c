/*
 * Copyright (C) 2020  University of Alberta
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
/**
 * @file adcs_io.c
 * @author Andrew Rooney, Arash Yazdani, Vasu Gupta
 * @date 2020-08-09
 */

#include "adcs_io.h"
#include "adcs_types.h"

#define MOCKED

#if defined(MOCKED)
#include "mock_uart_i2c.h"
#else
#include "uart_i2c.h"
#endif

#include <string.h>
#include <stdbool.h>


/**
 * @brief
 * 		Send telecommand via UART protocol
 * @param command
 * 		Telecommand frame 
 * @param length
 * 		Length of the data (in bytes)
 * 
 */
ADCS_returnState send_uart_telecommand(uint8_t* command, uint32_t length) {
  uint8_t frame[length + 4];
  frame[0] = ADCS_ESC_CHAR;
  frame[1] = ADCS_SOM;
  memcpy(&frame[2], &command, length);
  frame[length + 2] = ADCS_ESC_CHAR;
  frame[length + 2] = ADCS_EOM;
  uart_send(frame, length + 4);
  uint8_t reply[6];
  uart_receive(reply, 6);
  ADCS_returnState TC_err_flag = reply[3];
  return TC_err_flag;
}

/**
 * @brief
 * 		Send telecommand via I2C protocol
 * @param command
 * 		Telecommand frame 
 * @param length
 * 		Length of the data (in bytes)
 * 
 */
ADCS_returnState send_i2c_telecommand(uint8_t *command, uint32_t length)
{
  // Send telecommand
  i2c_send(command, length);

  // Poll TC Acknowledge Telemetry Format until the Processed flag equals 1.
  bool processed = false;
  uint8_t tc_ack[4];
  while (!processed)
  {
    request_i2c_telemetry(LAST_TC_ACK_ID, tc_ack, 4);
    processed = tc_ack[1] & 1;
  }

  // Confirm telecommand validity by checking the TC Error flag of the last read TC Acknowledge Telemetry Format.
  request_i2c_telemetry(LAST_TC_ACK_ID, tc_ack, 4);
  ADCS_returnState TC_err_flag = tc_ack[2];

  return TC_err_flag;
}

/**
 * @brief
 * 		Request and receive telemetry via UART protocol
 * @param TM_ID
 * 		Telemetry ID byte
 * @param telemetry
 *    Received telemetry data
 * @param length
 * 		Length of the data (in bytes)
 * 
 */
ADCS_returnState request_uart_telemetry(uint8_t TM_ID, uint8_t* telemetry,
                                        uint32_t length) {
  uint8_t frame[5];
  frame[0] = ADCS_ESC_CHAR;
  frame[1] = ADCS_SOM;
  frame[2] = TM_ID;
  frame[3] = ADCS_ESC_CHAR;
  frame[4] = ADCS_EOM;
  uart_send(frame, 5);
  uint8_t reply[length + 5];
  uart_receive(reply, length + 5);
  for (int i = 0; i < length; i++) {
    *(telemetry + i) = reply[3 + i];
  }
  return ADCS_OK;
}

/**
 * @brief
 * 		Request and receive telemetry via I2C protocol
 * @param TM_ID
 * 		Telemetry ID byte
 * @param telemetry
 *    Received telemetry data
 * @param length
 * 		Length of the data (in bytes)
 * 
 */
ADCS_returnState request_i2c_telemetry(uint8_t TM_ID, uint8_t *telemetry,
                                       uint32_t length)
{
  i2c_receive(telemetry, TM_ID, length);

  // Read error flag from Communication Status telemetry frame
  // to determine if an incorrect number of bytes are read. 
  // Should this check be done here?
  // (Refer to CubeADCS Firmware Manual section 5.3.1)

  // uint8_t err_reply[6];
  // i2c_receive(err_reply, COMMS_STAT_ID, 6);
  // uint8_t TL_err_flag = (err_reply[4] >> 3) & 1;

  return ADCS_OK;
}
