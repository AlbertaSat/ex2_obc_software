/*
 * Copyright (C) 2015  University of Alberta
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

#include "FreeRTOS.h"
#include "iris.h"
#include "iris_spi.h"

uint16_t * image_length_buffer; // 3 byte format
uint16_t * image_count_buffer; // 1 byte format
uint16_t * image_data_buffer; // 514 byte format
housekeeping_data hk_data;
uint16_t * housekeeping_buffer; // 10 byte format

enum {
    SEND_COMMAND,
    SEND_DATA,
    GET_DATA,
    FINISH, // May implement this state after getting advice from Iris team
    ERROR, // TODO: Potentially used for error handling
} controller_state;

/*
 * TODO:
 * - Change variables to static if required!
 */

void iris_take_pic() {
    //controller_state = SEND_COMMAND;

    // Basically try to run through the states one after another until finish is hit
    // TODO: Will need to check for errors (e.g. failed ACK)
    while (controller_state != FINISH && controller_state != ERROR) {
        switch (controller_state) {
            case SEND_COMMAND:
            {
                send_command(IRIS_TAKE_PIC);
                controller_state = FINISH;
                break;
            }
            case FINISH:
            {
                break;
            }
            case ERROR:
            {
                break;
                //TODO: Error handler
            }
        }
    }
}

uint32_t iris_get_image_length() {
    uint32_t image_length;
    uint16_t *image_length_buffer;
    int ret;

    controller_state = SEND_COMMAND;

    while (controller_state != FINISH && controller_state != ERROR) {
        switch (controller_state) {
            case SEND_COMMAND:
            {
                ret = send_command(IRIS_GET_IMAGE_LENGTH);
                if (ret != -1) {
                    controller_state = GET_DATA;
                } else {
                    controller_state = FINISH;
                }
                break;
            }
            case GET_DATA:
            {
                image_length_buffer = get_data(MAX_IMAGE_LENGTH);
                image_length = ((uint32_t)image_length_buffer[2]<<16) | ((uint32_t)image_length_buffer[1]<<8) | (uint32_t)image_length_buffer[0]; // Concatenate image_length_buffer

                free(image_length_buffer);
                controller_state = FINISH;
                break;
            }
            case FINISH:
            {
                break;
            }
            case ERROR:
            {
                break;
                //TODO: Error handler
            }
        }
    }

    if (controller_state == FINISH) {
        return image_length;
    }

    //return image_length; // TODO: Need to handle error
}

void iris_transfer_image(uint32_t image_length) {
    uint8_t num_transfer;
    uint8_t count_transfer;

    while (controller_state != FINISH && controller_state != ERROR) {
        switch (controller_state) {
            case SEND_COMMAND: // Send start image transfer command
            {
                send_command(IRIS_TRANSFER_IMAGE);
                controller_state = GET_DATA;
                break;
            }
            case GET_DATA: // Get image data in chunks/blocks
            {
                num_transfer = (IMAGE_TRANSFER_SIZE + image_length - 1) / IMAGE_TRANSFER_SIZE; // Ceiling division
                for (count_transfer = 0; count_transfer < num_transfer; count_transfer++) {
                    image_data_buffer = get_data(IMAGE_TRANSFER_SIZE);
                    // TODO: Do something with the received data (e.g transfer it to the SD card)
                    // Or just get the data and send it forward to the next stage. Prefer not to have too
                    // much data processing in driver code
                }
                break;
            }
            case FINISH:
            {
                break;
                //TODO: Send appropriate termination command
            }
            case ERROR:
            {
                break;
                //TODO: Error handler
            }
        }
    }
}

uint8_t iris_get_image_count() {
    uint8_t image_count;

    while(controller_state != FINISH && controller_state != ERROR) {
        switch (controller_state) {
            case SEND_COMMAND:
            {
                send_command(IRIS_GET_IMAGE_COUNT);
                controller_state = GET_DATA;
                break;
            }
            case GET_DATA:
            {
                image_length_buffer = get_data(MAX_IMAGE_COUNT);
                image_count = image_length_buffer[0];
                break;
            }
            case FINISH:
            {
                break;
                //TODO: Send appropriate termination command
            }
            case ERROR:
            {
                break;
                //TODO: Error handler
            }
        }
    }
    return image_count; // TODO: Need to handle error
}

void iris_toggle_sensor_idle(uint8_t toggle) {

    while (controller_state != FINISH && controller_state != ERROR) {
        switch (controller_state) {
            case SEND_COMMAND:
            {
                if (toggle == 1) {
                    send_command(IRIS_ON_SENSOR_IDLE);
                } else {
                    send_command(IRIS_OFF_SENSOR_IDLE);
                }
                break;
            }
            case FINISH:
            {
                break;
                //TODO: Send appropriate termination command
            }
            case ERROR:
            {
                //TODO: Error handler
            }
        }
    }
}

housekeeping_data iris_get_housekeeping() {

    while (controller_state != FINISH && controller_state != ERROR) {
        switch (controller_state) {
            case SEND_COMMAND:
            {
                send_command(IRIS_SEND_HOUSEKEEPING);
                controller_state = GET_DATA;
                break;
            }
            case GET_DATA:
            {
                // Retrieve housekeeping data and store it in a struct
                housekeeping_buffer = get_data(HOUSEKEEPING_SIZE);

                // Transfer data from buffer to struct
                // TODO: Verify Endianness and correct order of storage
                hk_data.vis_temp = housekeeping_buffer[0] << 8 | housekeeping_buffer[1];
                hk_data.nir_temp = housekeeping_buffer[2] << 8 | housekeeping_buffer[3];
                hk_data.flash_temp = housekeeping_buffer[4] << 8 | housekeeping_buffer[5];
                hk_data.gate_temp = housekeeping_buffer[6] << 8 | housekeeping_buffer[7];
                hk_data.imagenum = housekeeping_buffer[8];
                hk_data.software_version = housekeeping_buffer[9];
                break;
            }
            case FINISH:
            {
                break;
                //TODO: Send appropriate termination command
            }
            case ERROR:
            {
                break;
                //TODO: Error handler
            }
        }
    }
    return hk_data;
}

void iris_update_sensor_i2c_reg(sensor_reg sr[]) {

    while (controller_state != FINISH && controller_state != ERROR) {
        switch (controller_state) {
            case SEND_COMMAND:
            {
                send_command(IRIS_UPDATE_SENSOR_I2C_REG);
                controller_state = SEND_DATA;
                break;
            }
            case SEND_DATA:
            {
                // TODO: Convert sensor_reg into buffer/array/vector. Need to think a bit more on this
                uint16_t sensor_reg_buffer[] = {0xFFFF, 0xFF};
                send_data(sensor_reg_buffer, 2); // TODO: Need to take care of explicit declaration
                break;
            }
            case FINISH:
            {
                break;
                //TODO: Send appropriate termination command
            }
            case ERROR:
            {
                break;
                //TODO: Error handler
            }
        }
    }
}

void iris_update_current_limit(uint16_t current_limit) {

    while(controller_state != FINISH && controller_state != ERROR) {
        switch (controller_state) {
            case SEND_COMMAND:
            {
                send_command(IRIS_UPDATE_CURRENT_LIMIT);
                controller_state = SEND_DATA;
                break;
            }
            case SEND_DATA:
            {
                // TODO: Convert sensor_reg into buffer/array/vector. Need to think a bit more on this
                uint16_t current_limit_buffer[] = {current_limit};
                send_data(current_limit_buffer, 1); // TODO: Need to take care of explicit declaration
                break;
            }
            case FINISH:
            {
                break;
                //TODO: Send appropriate termination command
            }
            case ERROR:
            {
                break;
                //TODO: Error handler
            }
        }
    }
}


