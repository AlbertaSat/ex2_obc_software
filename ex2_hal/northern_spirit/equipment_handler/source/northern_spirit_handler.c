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

static SemaphoreHandle_t ns_command_mutex;

// Functions fulfilling functionality common to AuroraSat and YukonSat

NS_return NS_handler_init(){
    ns_command_mutex = xSemaphoreCreateMutex();
    if(ns_command_mutex == NULL){
        return NS_FAIL;
    }
    if(init_ns_io() != NS_OK){
        return NS_FAIL;
    }
    return NS_OK;
}

NS_return NS_upload_artwork(char *filename){
    if(xSemaphoreTake(ns_command_mutex, NS_COMMAND_MUTEX_TIMEOUT) != pdTRUE){
        return NS_HANDLER_BUSY;
    }
    uint8_t command[NS_STANDARD_CMD_LEN] = {'a', 'a', 'a'};
    uint8_t answer[NS_STANDARD_ANS_LEN + NS_STANDARD_ANS_LEN];

    strcat(filename, ".bmp");

    // Open specified file
    int32_t file1 = red_open(filename, RED_O_RDONLY);
    if(file1 == -1){
        sys_log(ERROR, "Error %d opening file %s in NS_capture_image\r\n", red_errno, filename);
        return NS_FAIL;
    }

    REDSTAT stat;
    red_fstat(file1, &stat);

    // Initiate image transaction and receive first two acks
    NS_return return_val = NS_sendAndReceive(command, NS_STANDARD_CMD_LEN, answer, NS_STANDARD_ANS_LEN + NS_STANDARD_ANS_LEN);
    if(return_val != NS_OK){
        xSemaphoreGive(ns_command_mutex);
        red_close(file1);
        return return_val;
    }

    // Start xmodem transfer
    int status = xmodemTransmit(file1, stat.st_size);
    if(status == -1){
        sys_log(ERROR, "Error %d during xmodem transfer of %s in NS_upload_artwork\r\n", red_errno, filename);
        return_val = NS_FAIL;
    }else if(status < -1){
        sys_log(ERROR, "Unknown error during xmodem transfer of %s in NS_upload_artwork\r\n", red_errno, filename);
        return_val = NS_FAIL;
    }

    uint8_t last_command[1] = {ETB};
    return_val = NS_sendAndReceive(last_command, 1, answer, NS_STANDARD_ANS_LEN);
    red_close(file1);
    return return_val;
}

NS_return NS_capture_image(void){
    if(xSemaphoreTake(ns_command_mutex, NS_COMMAND_MUTEX_TIMEOUT) != pdTRUE){
        return NS_HANDLER_BUSY;
    }
    uint8_t command[NS_STANDARD_CMD_LEN] = {'c', 'c', 'c'};
    uint8_t standard_answer[NS_STANDARD_ANS_LEN + NS_STANDARD_ANS_LEN];

    // Initiate image capture and receive first two acks
    NS_return return_val = NS_sendAndReceive(command, NS_STANDARD_CMD_LEN, standard_answer, NS_STANDARD_ANS_LEN + NS_STANDARD_ANS_LEN);
    if(return_val != NS_OK){
        xSemaphoreGive(ns_command_mutex);
        return return_val;
    }

    // Wait until image is captured
    vTaskDelay(NS_IMAGE_COLLECTION_DELAY);

    // Receive final ack
    uint8_t final_ack[NS_STANDARD_ANS_LEN];
    return_val = NS_expectResponse(final_ack, NS_STANDARD_ANS_LEN);
    if(return_val != NS_OK){
        xSemaphoreGive(ns_command_mutex);
        return return_val;
    }

    xSemaphoreGive(ns_command_mutex);
    return return_val;
}

NS_return NS_confirm_downlink(void){
    if(xSemaphoreTake(ns_command_mutex, NS_COMMAND_MUTEX_TIMEOUT) != pdTRUE){
        return NS_HANDLER_BUSY;
    }
    uint8_t command[NS_STANDARD_CMD_LEN] = {'g', 'g', 'g'};
    uint8_t answer[NS_STANDARD_ANS_LEN + NS_STANDARD_ANS_LEN];

    NS_return return_val = NS_sendAndReceive(command, NS_STANDARD_CMD_LEN, answer, NS_STANDARD_ANS_LEN + NS_STANDARD_ANS_LEN);

    xSemaphoreGive(ns_command_mutex);
    return return_val;
}

NS_return NS_get_heartbeat(uint8_t *heartbeat){
    if(xSemaphoreTake(ns_command_mutex, NS_COMMAND_MUTEX_TIMEOUT) != pdTRUE){
        return NS_HANDLER_BUSY;
    }
    uint8_t command[NS_STANDARD_CMD_LEN] = {'h', 'h', 'h'};
    uint8_t answer[NS_STANDARD_ANS_LEN];

    NS_return return_val = NS_sendAndReceive(command, NS_STANDARD_CMD_LEN, answer, NS_STANDARD_ANS_LEN);

    *heartbeat = answer[0];
    xSemaphoreGive(ns_command_mutex);
    return return_val;
}

NS_return NS_get_flag(char flag, bool *stat){
    if(xSemaphoreTake(ns_command_mutex, NS_COMMAND_MUTEX_TIMEOUT) != pdTRUE){
        return NS_HANDLER_BUSY;
    }

    uint8_t command[NS_SUBCODED_CMD_LEN] = {'k', 'k', 'k', flag, flag, flag};
    uint8_t answer[NS_STANDARD_ANS_LEN + NS_FLAG_DATA_LEN + NS_STANDARD_ANS_LEN];

    NS_return return_val = NS_sendAndReceive(command, NS_STANDARD_CMD_LEN, answer, NS_STANDARD_ANS_LEN + NS_FLAG_DATA_LEN + NS_STANDARD_ANS_LEN);

    *stat = answer[NS_STANDARD_ANS_LEN];

    xSemaphoreGive(ns_command_mutex);
    return return_val;
}

NS_return NS_get_telemetry(uint8_t *telemetry){
    if(xSemaphoreTake(ns_command_mutex, NS_COMMAND_MUTEX_TIMEOUT) != pdTRUE){
        return NS_HANDLER_BUSY;
    }
    uint8_t command[NS_STANDARD_CMD_LEN] = {'t', 't', 't'};
    uint8_t answer[NS_STANDARD_ANS_LEN];

    // Initiate telemetry command and receive ack
    NS_return return_val = NS_sendAndReceive(command, NS_STANDARD_CMD_LEN, answer, NS_STANDARD_ANS_LEN);

    if(return_val != NS_OK){
        xSemaphoreGive(ns_command_mutex);
        return return_val;
    }

    // Wait until telemetry is collected
    vTaskDelay(NS_TELEMETRY_COLLECTION_DELAY);

    // Receive telemetry data
    char response_data[NS_ENCODED_TELEMETRY_DATA_LEN + NS_STANDARD_ANS_LEN];
    return_val = NS_expectResponse((uint8_t *)response_data, NS_ENCODED_TELEMETRY_DATA_LEN + NS_STANDARD_ANS_LEN);

    if(return_val != NS_OK){
        xSemaphoreGive(ns_command_mutex);
        return return_val;
    }

    size_t decoded_len;
    unsigned char *decoded_data = base64_decode(response_data, NS_ENCODED_TELEMETRY_DATA_LEN, &decoded_len);
    if((decoded_data == NULL) || (decoded_len != NS_DECODED_TELEMETRY_DATA_LEN)){
        return NS_FAIL;
    }

    memcpy(telemetry, decoded_data, decoded_len);

    xSemaphoreGive(ns_command_mutex);
    return return_val;
}

NS_return NS_get_software_version(uint8_t *version){
    if(xSemaphoreTake(ns_command_mutex, NS_COMMAND_MUTEX_TIMEOUT) != pdTRUE){
        return NS_HANDLER_BUSY;
    }
    uint8_t command[NS_STANDARD_CMD_LEN] = {'v', 'v', 'v'};
    uint8_t answer[NS_STANDARD_ANS_LEN + NS_SWVERSION_DATA_LEN + NS_STANDARD_ANS_LEN];

    NS_return return_val = NS_sendAndReceive(command, NS_STANDARD_CMD_LEN, answer, NS_STANDARD_ANS_LEN + NS_SWVERSION_DATA_LEN + NS_STANDARD_ANS_LEN);

    memcpy(version, (answer + NS_STANDARD_ANS_LEN), NS_SWVERSION_DATA_LEN);
    xSemaphoreGive(ns_command_mutex);
    return return_val;
}

#ifdef IS_YUKONSAT
// Functions fulfilling functionality specific to YukonSat

#endif

#ifdef IS_AURORASAT
// Functions fulfilling functionality specific to AuroraSat


#endif
