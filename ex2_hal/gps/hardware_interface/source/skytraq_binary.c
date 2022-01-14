#include "skytraq_binary.h"
#include "FreeRTOS.h"
#include "HL_sci.h"
#include "NMEAParser.h"
#include "os_queue.h"
#include "skytraq_binary_types.h"
#include "system.h"
#include <string.h>

// TODO: implement software download
// TODO: determine if CNR mask will need to be configured
// TODO: determine if ephemeris needs to be uploaded manually
// TODO: determine if (and which) almanacs need to be uploaded manually
// TODO: find what the 3 timing modes mean

/* not implemented:
                    configure position pinning parameters
                    configure elevation and cnr mask
                    configure datum arbitrarily (assuming only WGS-84 will be needed in orbit)
                    configure DOP mask
                    configure 1pps cable delay
                    configure 1pps timing
                    set * almanac
                    set * ephemeris
                    set glonass time correction parameters
                    configure SBAS (seems to be mostly useful for planes)'
                    anything related to beidou
*/

enum current_sentence { none, binary, nmea } line_type;


SemaphoreHandle_t tx_semphr;
static SemaphoreHandle_t uart_mutex;

#define BUFSIZE 100
#define ITEM_SIZE BUFSIZE
#define QUEUE_LENGTH 2
QueueHandle_t binary_queue = NULL;

char binary_message_buffer[BUFSIZE];
int bin_buff_loc;

uint8_t byte;

bool sci_busy;

int current_line_type = none;

#define header_size 4
#define footer_size 3

/**
 * @brief initialises important skytraq variables
 *
 * @return true success
 * @return false failure
 */
bool skytraq_binary_init() {
    memset(binary_message_buffer, 0, BUFSIZE);
    bin_buff_loc = 0;
    sci_busy = false;
    binary_queue = xQueueCreate(QUEUE_LENGTH, ITEM_SIZE);
    tx_semphr = xSemaphoreCreateBinary();
    uart_mutex = xSemaphoreCreateMutex();

    if (tx_semphr == NULL) {
        return false;
    }

    if (binary_queue == NULL) {
        return false;
    }

    // initialise sci
    while ((GPS_SCI->FLR & 0x4) == 4)
        ;

    sciEnableNotification(GPS_SCI, SCI_RX_INT);
    sciReceive(GPS_SCI, 1, &byte);

    return true;
}

/**
 * @brief Sends message to skytraq and waits for ACK or NACK
 *
 * @param paylod pointer to data to send to skytraq. Contains only data unique to that message. I.E not the start
 * symbol, end symbol, or checksum
 * @param size size of the message to send, not including start symbol, checksum, or end symbol
 * @return GPS_RETURNSTATE Error explaining why the failure occurred
 */
GPS_RETURNSTATE skytraq_send_message(uint8_t *payload, uint16_t size) {
    if(xSemaphoreTake(uart_mutex, GPS_UART_TIMEOUT_MS) != pdTRUE) {
          return UNKNOWN_ERROR;
    }

    if (sci_busy) {
        xSemaphoreGive(uart_mutex);
        return RESOURCE_BUSY;
    }
    sci_busy = true;
    int total_size = size + header_size + footer_size;
    uint8_t *message = pvPortMalloc(total_size);
    memset(message, 0, total_size);
    message[0] = 0xA0;
    message[1] = 0xA1;
    *(uint16_t *)&(message[2]) = size;
    memcpy(&(message[4]), payload, size);
    message[total_size - 3] = calc_checksum(message, size);
    message[total_size - 1] = 0x0A;
    message[total_size - 2] = 0x0D;

    sciSend(GPS_SCI, total_size, message);
    if (xSemaphoreTake(tx_semphr, GPS_TX_TIMEOUT_MS) != pdTRUE) {
        xSemaphoreGive(uart_mutex);
        return TX_TIMEDOUT;
    }

    vPortFree(message);

    uint8_t sentence[BUFSIZE];

    // Will wait 1 second for a response

    BaseType_t success = xQueueReceive(binary_queue, sentence, GPS_UART_TIMEOUT_MS);
    if (success != pdPASS) {
        sci_busy = false;
        xSemaphoreGive(uart_mutex);
        return RX_TIMEDOUT;
    }

    bool cs_success = skytraq_verify_checksum(sentence);
    cs_success = true;
    if (cs_success) {
        switch (sentence[4]) {
        case 0x83:
            sci_busy = false;
            xSemaphoreGive(uart_mutex);
            return GPS_SUCCESS;
        case 0x84:
            sci_busy = false;
            xSemaphoreGive(uart_mutex);
            return MESSAGE_INVALID;
        }
    } else {
        sci_busy = false;
        xSemaphoreGive(uart_mutex);
        return INVALID_CHECKSUM_RECEIVE;
    }
    // should never reach here... I pray there is a merciful God
    sci_busy = false;
    xSemaphoreGive(uart_mutex);
    return UNKNOWN_ERROR;
}

/**
 * @brief Sends message to skytraq and waits for ACK or NACK, then waits for reply
 *
 * @param paylod pointer to data to send to skytraq. Contains only data unique to that message. I.E not the start
 * symbol, end symbol, or checksum
 * @param size size of the message to send, not including start symbol, checksum, or end symbol
 * @param reply Pointer to location to put the reply. Must be of correct size for the reply expected including all
 * @param reply_len length of reply expected. If the actual reply is larger it will be truncated
 * start/end symbols
 * @return GPS_RETURNSTATE Error explaining why the failure occurred
 */
GPS_RETURNSTATE skytraq_send_message_with_reply(uint8_t *payload, uint16_t size, uint8_t *reply,
                                                uint16_t reply_len) {
    //skytraq_send_message will receive a confirmation packet from the gps
    GPS_RETURNSTATE worked = skytraq_send_message(payload, size);

    if (worked != GPS_SUCCESS) {
        return worked;
    }
    // WARNING: possible race condition where another task could get control
    // of the sci port in between send message returning and this function setting
    // value to busy.
    sci_busy = true;

    uint8_t sentence[BUFSIZE];

    // Wait for 1 second to receive the actual reponse from the GPS
    BaseType_t success = xQueueReceive(binary_queue, sentence, GPS_UART_TIMEOUT_MS);

    if (success != pdPASS) {
        sci_busy = false;
        return RX_TIMEDOUT;
    }
    bool cs_success = skytraq_verify_checksum(sentence);

    if (cs_success) {
        memcpy((char *)reply, (char *)sentence, reply_len);
        sci_busy = false;
        return GPS_SUCCESS;
    } else {
        sci_busy = false;
        return INVALID_CHECKSUM_RECEIVE;
    }
}

// TODO: should I really keep this?
static inline increment_buffer(int *buf) { *buf += 1; }

/**
 * @brief interrupt handler for receiving byte from skytraq
 *
 * Will send data to appropriate queue, be it binary queue for communication messages or NMEA task queue
 *
 */
void get_byte() {
    uint8_t in = byte;

    if (current_line_type == none) {
        switch (in) {
        case '$':
            current_line_type = nmea;
            break;
        case 0xA0:
            current_line_type = binary;
            break;
        };
    }

    binary_message_buffer[bin_buff_loc] = in;
    increment_buffer(&bin_buff_loc);
    if (in == '\n') {
        if (current_line_type == binary) {
            if (binary_queue != NULL)
                xQueueSendToBackFromISR(binary_queue, binary_message_buffer, pdFALSE);
        } else if (current_line_type == nmea) {
            if (NMEA_queue != NULL)
                xQueueSendToBackFromISR(NMEA_queue, binary_message_buffer, pdFALSE);
        }
        bin_buff_loc = 0;
        memset(binary_message_buffer, 0, BUFSIZE);
        current_line_type = none;
    }

}

/**
 * @brief HalCoGen sci notification. Calls get_byte()
 *
 * @param sci sci port the interruput is from
 * @param flags interrupt flags
 */
void gps_sciNotification(sciBASE_t *sci, unsigned flags) {
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

    switch(flags) {
    case SCI_RX_INT:
        get_byte();
        sciReceive(sci, 1, &byte);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        break;

    case SCI_TX_INT:
        xSemaphoreGiveFromISR(tx_semphr, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        break;
    }
}

/**
 * @brief calculates checksum. Must take full message from start symbol to end of payload length
 *
 * @param message message to calculate checksum on
 * @param payload_length length to calculate checksum over
 * @return uint8_t checksum
 */
uint8_t calc_checksum(uint8_t *message, uint16_t payload_length) {
    // skip first 4 bytes of message
    message += 4;
    uint8_t checksum = 0;
    uint16_t i = 0;
    for (i; i < payload_length; i++) {
        checksum ^= *(message + i);
    }
    return checksum;
}
/**
 * @brief Verifies checksum of full binary message
 * message MUST include null terminating byte '\0'
 * @param message message to checksum
 * @return true checksum passed
 * @return false checksum failed
 */
bool skytraq_verify_checksum(uint8_t *message) {
    uint16_t payload_length = (message[2] << 8) | message[3]; // extract 16 bit payload size
    int checksum_location = 4 + payload_length;
    uint8_t expected = message[checksum_location];

    uint8_t cs = calc_checksum(message, payload_length);
    if (cs == expected) {
        return true;
    }
    return false;
}

GPS_RETURNSTATE skytraq_restart_receiver(StartMode start_mode, uint16_t utc_year, uint8_t utc_month,
                                         uint8_t utc_day, uint8_t utc_hour, uint8_t utc_minute, uint8_t utc_second,
                                         int16_t latitude, int16_t longitude, int16_t altitude) {
    uint16_t length = 15;
    uint8_t payload[15];

    payload[0] = SYSTEM_RESTART;
    payload[1] = start_mode;
    *(uint16_t *)&(payload[2]) = utc_year;
    payload[4] = utc_month;
    payload[5] = utc_day;
    payload[6] = utc_hour;
    payload[7] = utc_minute;
    payload[8] = utc_second;
    *(int16_t *)&(payload[9]) = latitude;
    *(int16_t *)&(payload[11]) = longitude;
    *(int16_t *)&(payload[13]) = altitude;

    return skytraq_send_message(payload, length);
}

GPS_RETURNSTATE skytraq_query_software_version() {
    uint16_t length = 2;
    uint8_t payload[2];
    payload[0] = QUERY_SOFTWARE_VERSION;
    payload[1] = 1; // system code

    return skytraq_send_message(payload, length);
}

GPS_RETURNSTATE skytraq_query_software_CRC(uint8_t *reply, uint16_t reply_len) {
    uint16_t length = 2;
    uint8_t payload[2];
    payload[0] = QUERY_SOFTWARE_CRC;
    payload[1] = 1; // system code
    uint8_t reply[25] = {0};
    GPS_RETURNSTATE ret;

    return skytraq_send_message_with_reply(payload, length, reply, reply_len);
}

GPS_RETURNSTATE skytraq_restore_factory_defaults(void) {
    uint16_t length = 2;
    uint8_t payload[2];
    payload[0] = SET_FACTORY_DEFAULTS;
    payload[1] = 1; // system code

    return skytraq_send_message(payload, length);
}

GPS_RETURNSTATE skytraq_configure_serial_port(skytraq_baud_rate rate, skytraq_update_attributes attribute) {
    uint16_t length = 4;
    uint8_t payload[4];

    payload[0] = CONFIGURE_SERIAL_PORT;
    payload[1] = 0; // COM port is always 0, only one COM port on skytraq receiver
    payload[2] = rate;
    payload[3] = attribute;

    return skytraq_send_message(payload, length);
}

GPS_RETURNSTATE skytraq_configure_nmea_output_rate(uint8_t GGA_interval, uint8_t GSA_interval,
                                                   uint8_t GSV_interval, uint8_t GLL_interval,
                                                   uint8_t RMC_interval, uint8_t VTG_interval,
                                                   uint8_t ZDA_interval, skytraq_update_attributes attribute) {
    uint16_t length = 9;
    uint8_t payload[9];
    payload[0] = CONFIGURE_NMEA;
    payload[1] = GGA_interval;
    payload[2] = GSA_interval;
    payload[3] = GSV_interval;
    payload[4] = GLL_interval;
    payload[5] = RMC_interval;
    payload[6] = VTG_interval;
    payload[7] = ZDA_interval;
    payload[8] = attribute;

    return skytraq_send_message(payload, length);
}

GPS_RETURNSTATE configure_message_type(skytraq_message_type type, skytraq_update_attributes attribute) {
    uint16_t length = 3;
    uint8_t payload[3];

    payload[0] = CONFIGURE_MESSAGE_TYPE;
    payload[1] = type;
    payload[2] = attribute;

    return skytraq_send_message(payload, length);
}

GPS_RETURNSTATE skytraq_configure_power_mode(skytraq_power_mode mode, skytraq_update_attributes attribute) {
    uint16_t length = 3;
    uint8_t payload[3];

    payload[0] = CONFIGURE_POWER_MODE;
    payload[1] = mode;
    payload[2] = attribute;

    return skytraq_send_message(payload, length);
}

GPS_RETURNSTATE skytraq_get_gps_time(uint8_t *reply, uint16_t reply_len) {
    uint16_t length = 2;
    uint8_t payload[2];
    uint8_t reply[100];

    *(uint16_t *)&(payload[0]) = QUERY_GPS_TIME;

    return skytraq_send_message_with_reply(payload, length, reply, reply_len);
}

GPS_RETURNSTATE skytraq_configure_utc_reference(enable_disable status, uint16_t utc_year, uint8_t utc_month,
                                                uint8_t utc_day, skytraq_update_attributes attribute) {
    uint16_t length = 8;
    uint8_t payload[8];

    *(uint16_t *)&(payload[0]) = CONFIGURE_UTC_REFERENCE;
    payload[2] = status;
    *(uint16_t *)&(payload[3]) = utc_year;
    payload[5] = utc_month;
    payload[6] = utc_day;
    payload[7] = attribute;

    return skytraq_send_message(payload, length);
}

/*
void configure_system_position_rate(uint8_t rate, skytraq_update_attributes attribute) {
    uint16_t length = 3;
    uint8_t payload[3];

    payload[0] = CONFIGURE_UPDATE_RATE;
    payload[1] = rate;
    payload[2] = attribute;


    skytraq_send_message(payload, length);
}

void query_position_update_rate() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_UPDATE_RATE;


    skytraq_send_message(payload, length);
}

void query_power_mode() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_POWER_MODE;


    skytraq_send_message(payload, length);
}

void query_datum() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_DATUM;


    skytraq_send_message(payload, length);
}

void query_dop_mask() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_DOP_MASK;


    skytraq_send_message(payload, length);
}

void query_elevation_cnr_mask() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_ELEVATION_CNR_MASK;


    skytraq_send_message(payload, length);
}

void get_gps_ephemeris() {
    uint16_t length = 2;
    uint8_t payload[2];

    payload[0] = GET_GPS_EPHEMERIS;
    payload[1] = 0;


    skytraq_send_message(payload, length);
}

void get_glonass_ephemeris() {
    uint16_t length = 2;
    uint8_t payload[2];

    payload[0] = GET_GLONASS_EPHEMERIS;
    payload[1] = 0;


    skytraq_send_message(payload, length);
}

// may be unnecessary after using skytraq's own software
void configure_position_pinning(position_pinning mode, skytraq_update_attributes attribute) {
    uint16_t length = 3;
    uint8_t payload[3];

    payload[0] = CONFIGURE_POSITION_PINNING;
    payload[1] = mode;
    payload[2] = attribute;

    skytraq_send_message(payload, length);
}

void query_position_pinning() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_POSITION_PINNING;


    skytraq_send_message(payload, length);
}

void query_1pps_timing() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_1PPS_TIMING;


    skytraq_send_message(payload, length);
}

void query_1pps_cable_delay() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_1PPS_CABLE_DELAY;


    skytraq_send_message(payload, length);
}

void configure_nmea_talker_ID(nmea_talker_IDs id, skytraq_update_attributes attribute) {
    uint16_t length = 3;
    uint8_t payload[3];

    payload[0] = CONFIGURE_NMEA_TALKER_ID;
    payload[1] = id;
    payload[2] = attribute;


    skytraq_send_message(payload, length);
}

void query_nmea_talker_id() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_NMEA_TALKER_ID;


    skytraq_send_message(payload, length);
}

void get_gps_almanac() {
    uint16_t length = 2;
    uint8_t payload[2];

    payload[0] = GET_GPS_ALMANAC;
    payload[1] = 0;

    skytraq_send_message(payload, length);
}

void get_glonass_almanac() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = GET_GPS_ALMANAC;
    payload[1] = 0;

    skytraq_send_message(payload, length);
}

void query_1pps_timing() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = QUERY_1PPS_TIMING;


    skytraq_send_message(payload, length);
}

void get_glonass_time_correction() {
    uint16_t length = 1;
    uint8_t payload[1];

    payload[0] = GET_GLONASS_TIME_CORRECT_;

    skytraq_send_message(payload, length);
}
*/
