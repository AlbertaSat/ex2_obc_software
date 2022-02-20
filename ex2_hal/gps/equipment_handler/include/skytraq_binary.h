#ifndef SKYTRAQ_BINARY_H
#define SKYTRAQ_BINARY_H

#include "skytraq_binary_types.h"
#include <stdbool.h>
#include <stdint.h>

#define GPS_UART_TIMEOUT_MS 2000 * portTICK_PERIOD_MS
#define GPS_TX_TIMEOUT_MS 1000 * portTICK_PERIOD_MS

bool skytraq_verify_checksum(uint8_t *message);

uint8_t calc_checksum(uint8_t *message, uint16_t payload_length);
// as per my discussion with Josh, minimal functionality will be implemented here
// And no justification will be added for functionality not implemented. If it is detemrmined
// something is needed, it will be done at that time.

// sending a message to a receiver will cause the caller to block
// until a reply is received.

// utc time does not have to be perfectly accurate
// skytraq pretty much just wants it in the same relative leap second
GPS_RETURNSTATE skytraq_restart_receiver(StartMode start_mode, uint16_t utc_year, uint8_t utc_month,
                                         uint8_t utc_day, uint8_t utc_hour, uint8_t utc_minute, uint8_t utc_second,
                                         int16_t latitude, int16_t longitude, int16_t altitude);

GPS_RETURNSTATE skytraq_query_software_CRC(uint8_t *reply, uint16_t reply_len);

GPS_RETURNSTATE skytraq_query_software_version(uint8_t *reply, uint16_t reply_len);

GPS_RETURNSTATE skytraq_restore_factory_defaults(void);

GPS_RETURNSTATE skytraq_configure_serial_port(skytraq_baud_rate rate, skytraq_update_attributes attribute);

GPS_RETURNSTATE skytraq_configure_nmea_output_rate(uint8_t GGA_interval, uint8_t GSA_interval,
                                                   uint8_t GSV_interval, uint8_t GLL_interval,
                                                   uint8_t RMC_interval, uint8_t VTG_interval,
                                                   uint8_t ZDA_interval, skytraq_update_attributes attribute);

GPS_RETURNSTATE skytraq_configure_power_mode(skytraq_power_mode mode, skytraq_update_attributes attribute);

GPS_RETURNSTATE skytraq_get_gps_time(uint8_t *reply, uint16_t reply_len);

GPS_RETURNSTATE skytraq_send_message_with_reply(uint8_t *payload, uint16_t size, uint8_t *reply,
                                                uint16_t reply_len);

GPS_RETURNSTATE skytraq_send_message(uint8_t *paylod, uint16_t size);

bool skytraq_binary_init();
#endif // SKYTRAQ_BINARY_H
