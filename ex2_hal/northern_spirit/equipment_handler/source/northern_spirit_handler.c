/*
 * Copyright (C) 2021  University of Alberta
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
 * @file northern_spirit_handler.c
 * @author Thomas Ganley
 * @date 2021-10-22
 */

#include "northern_spirit_handler.h"
#include "northern_spirit_io.h"
#include "base_64.h"
#include "xmodem.h"
#include <redposix.h>
#include <os_semphr.h>

static SemaphoreHandle_t ns_command_mutex;

static void convert_bytes_to_int16(int16_t *dest, uint8_t little_byte, uint8_t big_byte);

// Functions fulfilling functionality common to AuroraSat and YukonSat

NS_return NS_handler_init() {
    ns_command_mutex = xSemaphoreCreateMutex();
    if (ns_command_mutex == NULL) {
        return NS_FAIL;
    }
    if (init_ns_io() != NS_OK) {
        return NS_FAIL;
    }
    return NS_OK;
}

NS_return NS_upload_artwork(char *filename) {
    if (xSemaphoreTake(ns_command_mutex, NS_COMMAND_MUTEX_TIMEOUT) != pdTRUE) {
        return NS_HANDLER_BUSY;
    }
    uint8_t command[NS_STANDARD_CMD_LEN] = {'a', 'a', 'a'};
    uint8_t answer[NS_STANDARD_ANS_LEN];

    strcat(filename, ".bmp");

    // Open specified file
    int32_t file1 = red_open(filename, RED_O_RDONLY);
    if (file1 == -1) {
        sys_log(ERROR, "Error %d opening file %s in NS_capture_image\r\n", red_errno, filename);
        xSemaphoreGive(ns_command_mutex);
        return NS_FAIL;
    }

    REDSTAT stat;
    red_fstat(file1, &stat);

    // Initiate image transaction and receive first ack
    NS_return return_val =
        NS_sendAndReceive(command, NS_STANDARD_CMD_LEN, answer, NS_STANDARD_ANS_LEN, NS_UART_LONG_TIMEOUT_MS);
    if (return_val != NS_OK) {
        xSemaphoreGive(ns_command_mutex);
        red_close(file1);
        return return_val;
    }
    // Try 10 times to received the ack
    return_val = NS_expectResponse(answer, NS_STANDARD_ANS_LEN, NS_UPLOAD_ARTWORK_DELAY);
    if (return_val != NS_OK) {
        xSemaphoreGive(ns_command_mutex);
        red_close(file1);
        return return_val;
    }

    // Start xmodem transfer
    int status = xmodemTransmit(file1, stat.st_size);
    if (status == -1) {
        sys_log(ERROR, "Error %d during xmodem transfer of %s in NS_upload_artwork\r\n", red_errno, filename);
        return_val = NS_FAIL;
    } else if (status < -1) {
        sys_log(ERROR, "Unknown error during xmodem transfer of %s in NS_upload_artwork\r\n", red_errno, filename);
        return_val = NS_FAIL;
    } else {
        uint8_t last_command[1] = {ETB};
        return_val = NS_sendAndReceive(last_command, 1, answer, NS_STANDARD_ANS_LEN, NS_UART_LONG_TIMEOUT_MS);
    }
    red_close(file1);
    xSemaphoreGive(ns_command_mutex);
    return return_val;
}

NS_return NS_download_image() {
    if (xSemaphoreTake(ns_command_mutex, NS_COMMAND_MUTEX_TIMEOUT) != pdTRUE) {
        return NS_HANDLER_BUSY;
    }
    uint8_t command[NS_STANDARD_CMD_LEN] = {'f', 'f', 'f'};
    uint8_t answer[NS_STANDARD_ANS_LEN * 2];

    // Initiate image capture and receive first two acks
    NS_return return_val =
        NS_sendAndReceive(command, NS_STANDARD_CMD_LEN, answer, NS_STANDARD_ANS_LEN * 2, NS_UART_LONG_TIMEOUT_MS);
    if (return_val != NS_OK) {
        xSemaphoreGive(ns_command_mutex);
        return return_val;
    }
    if (answer[2] == 0x15) {
        xSemaphoreGive(ns_command_mutex);
        return (NS_return)(answer[3]);
    }

    const unsigned char *file_name = "NS_IMAGE.jpg"; // Hardcoded is probably not the best idea
    int recvd = xmodemReceive(file_name);

    sys_log(INFO, "NIM sent %d bytes", recvd);
    xSemaphoreGive(ns_command_mutex);

    return return_val;
}

NS_return NS_capture_image(void) {
    if (xSemaphoreTake(ns_command_mutex, NS_COMMAND_MUTEX_TIMEOUT) != pdTRUE) {
        return NS_HANDLER_BUSY;
    }
    uint8_t command[NS_STANDARD_CMD_LEN] = {'c', 'c', 'c'};
    uint8_t standard_answer[NS_STANDARD_ANS_LEN + NS_STANDARD_ANS_LEN];

    // Initiate image capture and receive first two acks
    NS_return return_val = NS_sendAndReceive(command, NS_STANDARD_CMD_LEN, standard_answer, NS_STANDARD_ANS_LEN,
                                             NS_UART_LONG_TIMEOUT_MS);
    if (return_val != NS_OK) {
        xSemaphoreGive(ns_command_mutex);
        return return_val;
    }

    // Receive final ack
    uint8_t final_ack[NS_STANDARD_ANS_LEN];
    return_val = NS_expectResponse(final_ack, NS_STANDARD_ANS_LEN, NS_IMAGE_COLLECTION_DELAY);
    if (return_val != NS_OK) {
        xSemaphoreGive(ns_command_mutex);
        return return_val;
    }
    if (final_ack[0] == NS_NAK_VAL) {
        return_val = (NS_return)final_ack[1];
    }

    xSemaphoreGive(ns_command_mutex);
    return return_val;
}

NS_return NS_confirm_downlink(uint8_t *conf) {
    if (xSemaphoreTake(ns_command_mutex, NS_COMMAND_MUTEX_TIMEOUT) != pdTRUE) {
        return NS_HANDLER_BUSY;
    }
    uint8_t command[NS_STANDARD_CMD_LEN] = {'g', 'g', 'g'};
    uint8_t answer[NS_STANDARD_ANS_LEN];

    NS_return return_val =
        NS_sendAndReceive(command, NS_STANDARD_CMD_LEN, answer, NS_STANDARD_ANS_LEN, NS_UART_LONG_TIMEOUT_MS);
    if (return_val != NS_OK) {
        xSemaphoreGive(ns_command_mutex);
        return return_val;
    }

    uint8_t confirmation[NS_STANDARD_ANS_LEN];
    return_val = NS_expectResponse(confirmation, NS_STANDARD_ANS_LEN, NS_CONFIRM_DOWNLINK_DELAY);
    if (confirmation[0] == NS_NAK_VAL) {
        return_val = confirmation[1];
    }
    *conf = (confirmation[0] == 'g') ? 0 : 1;

    xSemaphoreGive(ns_command_mutex);
    return return_val;
}

NS_return NS_get_heartbeat(uint8_t *heartbeat) {
    if (xSemaphoreTake(ns_command_mutex, NS_COMMAND_MUTEX_TIMEOUT) != pdTRUE) {
        return NS_HANDLER_BUSY;
    }
    uint8_t command[NS_STANDARD_CMD_LEN] = {'h', 'h', 'h'};
    uint8_t answer[NS_STANDARD_ANS_LEN];

    NS_return return_val =
        NS_sendAndReceive(command, NS_STANDARD_CMD_LEN, answer, NS_STANDARD_ANS_LEN, NS_UART_LONG_TIMEOUT_MS);

    *heartbeat = answer[0];
    if (answer[0] == NS_NAK_VAL) {
        return_val = answer[1];
    }
    xSemaphoreGive(ns_command_mutex);
    return return_val;
}

NS_return NS_get_flag(char flag, bool *stat) {
    if (xSemaphoreTake(ns_command_mutex, NS_COMMAND_MUTEX_TIMEOUT) != pdTRUE) {
        return NS_HANDLER_BUSY;
    }

    uint8_t command[NS_SUBCODED_CMD_LEN] = {'k', 'k', 'k', flag, flag, flag};
    uint8_t answer[NS_STANDARD_ANS_LEN];

    NS_return return_val = NS_sendAndReceive(command, NS_STANDARD_CMD_LEN + NS_STANDARD_CMD_LEN, answer,
                                             NS_STANDARD_ANS_LEN, NS_UART_LONG_TIMEOUT_MS);
    if (return_val != NS_OK) {
        xSemaphoreGive(ns_command_mutex);
        return return_val;
    }

    uint8_t flag_ans[NS_FLAG_DATA_LEN + NS_STANDARD_ANS_LEN];
    return_val = NS_expectResponse(flag_ans, NS_FLAG_DATA_LEN + NS_STANDARD_ANS_LEN, NS_GETFLAG_DELAY);
    if (flag_ans[0] == NS_NAK_VAL) {
        return_val = flag_ans[1];
    }
    *stat = (flag_ans[0] != '0') ? 1 : 0;

    xSemaphoreGive(ns_command_mutex);
    return return_val;
}

NS_return NS_get_filename(char subcode, char *filename) {
    if (xSemaphoreTake(ns_command_mutex, NS_COMMAND_MUTEX_TIMEOUT) != pdTRUE) {
        return NS_HANDLER_BUSY;
    }

    uint8_t command[NS_SUBCODED_CMD_LEN] = {'l', 'l', 'l', subcode, subcode, subcode};
    uint8_t answer[NS_STANDARD_ANS_LEN];

    NS_return return_val = NS_sendAndReceive(command, NS_STANDARD_CMD_LEN + NS_STANDARD_CMD_LEN, answer,
                                             NS_STANDARD_ANS_LEN, NS_UART_LONG_TIMEOUT_MS);
    if (return_val != NS_OK) {
        xSemaphoreGive(ns_command_mutex);
        return return_val;
    }

    uint8_t filename_ans[NS_FILENAME_DATA_LEN];
    return_val = NS_expectResponse(filename_ans, NS_FILENAME_DATA_LEN, NS_GETFILENAME_DELAY);

    memcpy(filename, filename_ans, 11);
    xSemaphoreGive(ns_command_mutex);
    return return_val;
}

NS_return NS_get_telemetry(ns_telemetry *telemetry) {
    if (xSemaphoreTake(ns_command_mutex, NS_COMMAND_MUTEX_TIMEOUT) != pdTRUE) {
        return NS_HANDLER_BUSY;
    }
    uint8_t command[NS_STANDARD_CMD_LEN] = {'t', 't', 't'};
    uint8_t answer[NS_STANDARD_ANS_LEN];

    // Initiate telemetry command and receive ack
    NS_return return_val =
        NS_sendAndReceive(command, NS_STANDARD_CMD_LEN, answer, NS_STANDARD_ANS_LEN, NS_UART_LONG_TIMEOUT_MS);
    if (answer[0] != 't') {
        xSemaphoreGive(ns_command_mutex);
        return NS_FAIL;
    }

    if (return_val != NS_OK) {
        xSemaphoreGive(ns_command_mutex);
        return return_val;
    }

    // Receive telemetry data
    char response_data[NS_ENCODED_TELEMETRY_DATA_LEN];
    return_val =
        NS_expectResponse((uint8_t *)response_data, NS_ENCODED_TELEMETRY_DATA_LEN, NS_UART_LONG_TIMEOUT_MS);

    if (return_val != NS_OK) {
        xSemaphoreGive(ns_command_mutex);
        return return_val;
    }

    size_t decoded_len;
    unsigned char *decoded_data = base64_decode(response_data, NS_ENCODED_TELEMETRY_DATA_LEN, &decoded_len);
    if ((decoded_data == NULL) || (decoded_len != NS_DECODED_TELEMETRY_DATA_LEN)) {
        vPortFree(decoded_data);
        xSemaphoreGive(ns_command_mutex);
        return NS_FAIL;
    }

    xSemaphoreGive(ns_command_mutex);

    convert_bytes_to_int16(&telemetry->temp0, decoded_data[0], decoded_data[1]);
    convert_bytes_to_int16(&telemetry->temp1, decoded_data[2], decoded_data[3]);
    convert_bytes_to_int16(&telemetry->temp2, decoded_data[4], decoded_data[5]);
    convert_bytes_to_int16(&telemetry->temp3, decoded_data[6], decoded_data[7]);
    convert_bytes_to_int16(&telemetry->eNIM0, decoded_data[16], decoded_data[17]);
    convert_bytes_to_int16(&telemetry->eNIM1, decoded_data[18], decoded_data[19]);
    convert_bytes_to_int16(&telemetry->eNIM2, decoded_data[20], decoded_data[21]);
    convert_bytes_to_int16(&telemetry->eNIM3, decoded_data[22], decoded_data[23]);
    convert_bytes_to_int16(&telemetry->ram_avail, decoded_data[32], decoded_data[33]);
    convert_bytes_to_int16(&telemetry->lowest_img_num, decoded_data[34], decoded_data[35]);
    convert_bytes_to_int16(&telemetry->first_blank_img_num, decoded_data[36], decoded_data[37]);
    vPortFree(decoded_data);
    return return_val;
}

NS_return NS_get_software_version(uint8_t *version) {
    if (xSemaphoreTake(ns_command_mutex, NS_COMMAND_MUTEX_TIMEOUT) != pdTRUE) {
        return NS_HANDLER_BUSY;
    }
    uint8_t command[NS_STANDARD_CMD_LEN] = {'v', 'v', 'v'};
    uint8_t answer[NS_STANDARD_ANS_LEN + NS_SWVERSION_DATA_LEN + NS_STANDARD_ANS_LEN];

    NS_return return_val = NS_sendAndReceive(command, NS_STANDARD_CMD_LEN, answer,
                                             NS_STANDARD_ANS_LEN + NS_SWVERSION_DATA_LEN + NS_STANDARD_ANS_LEN,
                                             NS_UART_LONG_TIMEOUT_MS);

    memcpy(version, (answer + NS_STANDARD_ANS_LEN), NS_SWVERSION_DATA_LEN);
    xSemaphoreGive(ns_command_mutex);
    return return_val;
}

NS_return NS_clear_sd_card() {
    return NS_STUBBED;
    if (xSemaphoreTake(ns_command_mutex, NS_COMMAND_MUTEX_TIMEOUT) != pdTRUE) {
        return NS_HANDLER_BUSY;
    }
    uint8_t command[2 * NS_STANDARD_CMD_LEN] = {'l', 'l', 'l', 'r', 'r', 'r'};
    uint8_t answer[NS_STANDARD_ANS_LEN];

    NS_return return_val =
        NS_sendAndReceive(command, 2 * NS_STANDARD_CMD_LEN, answer, NS_STANDARD_ANS_LEN, NS_UART_LONG_TIMEOUT_MS);
    // TODO: check the NAK value
    if (answer[0] != 'l' || answer[1] != 0x06) {
        return NS_FAIL;
    }
    xSemaphoreGive(ns_command_mutex);
    return return_val;
}

static void convert_bytes_to_int16(int16_t *dest, uint8_t little_byte, uint8_t big_byte) {
    uint16_t temp = (big_byte << 8) | little_byte;
    memcpy(dest, &temp, sizeof(int16_t));
}

#ifdef IS_YUKONSAT
// Functions fulfilling functionality specific to YukonSat

#endif

#ifdef IS_AURORASAT
// Functions fulfilling functionality specific to AuroraSat

#endif
