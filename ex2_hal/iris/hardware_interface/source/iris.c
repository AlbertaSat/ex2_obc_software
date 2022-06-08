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
#include <stdlib.h>

#include "iris.h"
#include "iris_spi.h"

//uint16_t * image_length_buffer; // 3 byte format
//uint16_t * image_count_buffer; // 1 byte format
//uint16_t * image_data_buffer; // 512 byte format
//housekeeping_data hk_data;
//uint16_t * housekeeping_buffer; // 10 byte format

/*
 * TODO:
 * - Change variables to static if required!
 */

void iris_take_pic() {
    IRIS_return ret;

    controller_state = SEND_COMMAND;

    // Basically try to run through the states one after another until finish is hit
    // TODO: Will need to check for errors (e.g. failed ACK)
    while (controller_state != FINISH && controller_state != ERROR_STATE) {
        switch (controller_state) {
            case SEND_COMMAND:
            {
                ret = send_command(IRIS_TAKE_PIC);
                if (ret == IRIS_ACK) {
                    controller_state = FINISH;
                } else {
                    controller_state = ERROR_STATE;
                }
                break;
            }
            case FINISH:
            {
                break;
            }
            case ERROR_STATE:
            {
                break;
                //TODO: ERROR_STATE handler
            }
        }
    }
}

uint32_t iris_get_image_length() {
    uint32_t image_length;
    IRIS_return ret;

    controller_state = SEND_COMMAND;

    while (controller_state != FINISH && controller_state != ERROR_STATE) {
        switch (controller_state) {
            case SEND_COMMAND:
            {
                ret = send_command(IRIS_GET_IMAGE_LENGTH);
                if (ret == IRIS_ACK) {
                    controller_state = GET_DATA;
                } else {
                    controller_state = FINISH;
                }
                break;
            }
            case GET_DATA:
            {
                uint16_t * image_length_buffer = (uint16_t*) calloc(MAX_IMAGE_LENGTH + 1, sizeof(uint16_t));
                if (image_length_buffer == NULL) {
                    // TODO: Log error
                    return;
                }
                ret = get_data(image_length_buffer, MAX_IMAGE_LENGTH + 1);
                if (ret == IRIS_OK) {
                    controller_state = FINISH;
                } else {
                    controller_state = ERROR_STATE;
                }
                image_length = (uint32_t)((uint8_t)image_length_buffer[3]<<16 | (uint8_t)image_length_buffer[2]<<8 | (uint8_t)image_length_buffer[2]); // Concatenate image_length_buffer

                free(image_length_buffer);
                controller_state = FINISH;
                break;
            }
            case FINISH:
            {
                break;
            }
            case ERROR_STATE:
            {
                break;
                //TODO: ERROR_STATE handler
            }
        }
    }

    if (controller_state == FINISH) {
        return image_length;
    }

    //return image_length; // TODO: Need to handle ERROR_STATE
}

void iris_transfer_image(uint32_t image_length) {
    uint32_t num_transfer;
    IRIS_return ret;

    controller_state = SEND_COMMAND;

    while (controller_state != FINISH && controller_state != ERROR_STATE) {
        switch (controller_state) {
            case SEND_COMMAND: // Send start image transfer command
            {
                ret = send_command(IRIS_TRANSFER_IMAGE);
                if (ret == IRIS_ACK) {
                    controller_state = GET_DATA;
                } else {
                    controller_state = FINISH;
                }
                break;
            }
            case GET_DATA: // Get image data in chunks/blocks
            {
                uint16_t * image_data_buffer = (uint16_t*) calloc(MAX_IMAGE_LENGTH, sizeof(uint16_t));
                num_transfer = (IMAGE_TRANSFER_SIZE + image_length) / IMAGE_TRANSFER_SIZE; // Ceiling division
                for (uint32_t count_transfer = 0; count_transfer < num_transfer; count_transfer++) {
                    ret = get_data(image_data_buffer, IMAGE_TRANSFER_SIZE);
                    // TODO: Do something with the received data (e.g transfer it to the SD card)
                    // Or just get the data and send it forward to the next stage. Prefer not to have too
                    // much data processing in driver code

                    for (int i = 0; i < MAX_IMAGE_LENGTH; i++) {
                        if (image_data_buffer[i] != i) {
                            i = 0;
                            return;
                        }
                    }


                    memset(image_data_buffer, 0, MAX_IMAGE_LENGTH);
                }
                free(image_data_buffer);
                controller_state = FINISH;
                break;
            }
            case FINISH:
            {
                break;
                //TODO: Send appropriate termination command
            }
            case ERROR_STATE:
            {
                break;
                //TODO: ERROR_STATE handler
            }
        }
    }

    if (controller_state == FINISH) {
        return;
    }
}

uint8_t iris_get_image_count() {
    uint8_t image_count;
    IRIS_return ret;

    controller_state = SEND_COMMAND;

    while(controller_state != FINISH && controller_state != ERROR_STATE) {
        switch (controller_state) {
            case SEND_COMMAND:
            {
                ret = send_command(IRIS_GET_IMAGE_COUNT);
                if (ret == IRIS_ACK) {
                    controller_state = GET_DATA;
                } else {
                    controller_state = FINISH;
                }
                break;
            }
            case GET_DATA:
            {
//                image_length_buffer = get_data(MAX_IMAGE_COUNT);
//                image_count = image_length_buffer[0];
                break;
            }
            case FINISH:
            {
                break;
                //TODO: Send appropriate termination command
            }
            case ERROR_STATE:
            {
                break;
                //TODO: ERROR_STATE handler
            }
        }
    }
    return image_count; // TODO: Need to handle ERROR_STATE
}

void iris_toggle_sensor_idle(uint8_t toggle) {
    IRIS_return ret;

    controller_state = SEND_COMMAND;

    while (controller_state != FINISH && controller_state != ERROR_STATE) {
        switch (controller_state) {
            case SEND_COMMAND:
            {
                if (toggle == 1) {
                    ret = send_command(IRIS_ON_SENSOR_IDLE);
                } else {
                    ret = send_command(IRIS_OFF_SENSOR_IDLE);
                }
                if (ret == IRIS_ACK) {
                    controller_state = FINISH;
                } else {
                    controller_state = ERROR_STATE;
                }
                break;
            }
            case FINISH:
            {
                break;
                //TODO: Send appropriate termination command
            }
            case ERROR_STATE:
            {
                //TODO: ERROR_STATE handler
            }
        }
    }
}

housekeeping_data iris_get_housekeeping() {
    IRIS_return ret;
    housekeeping_data hk_data;

    controller_state = SEND_COMMAND;

    while (controller_state != FINISH && controller_state != ERROR_STATE) {
        switch (controller_state) {
            case SEND_COMMAND:
            {
                ret = send_command(IRIS_SEND_HOUSEKEEPING);
                if (ret == IRIS_ACK) {
                    vTaskDelay(1000);
                    controller_state = GET_DATA;
                } else {
                    controller_state = FINISH;
                }
                break;
            }
            case GET_DATA:
            {
                // Retrieve housekeeping data and store it in a struct
                uint16_t * housekeeping_buffer = (uint16_t*) calloc(HOUSEKEEPING_SIZE, sizeof(uint16_t));
                ret = get_data(housekeeping_buffer, HOUSEKEEPING_SIZE);


                // Transfer data from buffer to struct
                // TODO: Verify Endianness and correct order of storage
                hk_data.vis_temp = housekeeping_buffer[1] << 8 | housekeeping_buffer[0];
                hk_data.nir_temp = housekeeping_buffer[3] << 8 | housekeeping_buffer[2];
                hk_data.flash_temp = housekeeping_buffer[5] << 8 | housekeeping_buffer[4];
                hk_data.gate_temp = housekeeping_buffer[7] << 8 | housekeeping_buffer[6];
                hk_data.imagenum = housekeeping_buffer[8];
                hk_data.software_version = housekeeping_buffer[9];
                hk_data.errornum = housekeeping_buffer[10];
                hk_data.MAX_5V_voltage = housekeeping_buffer[12] << 8 | housekeeping_buffer[11];
                hk_data.MAX_5V_power = housekeeping_buffer[14] << 8 | housekeeping_buffer[13];
                hk_data.MAX_3V_voltage = housekeeping_buffer[16] << 8 | housekeeping_buffer[15];
                hk_data.MAX_3V_power = housekeeping_buffer[18] << 8 | housekeeping_buffer[17];
                hk_data.MIN_5V_voltage = housekeeping_buffer[20] << 8 | housekeeping_buffer[19];
                hk_data.MIN_3V_voltage = housekeeping_buffer[22] << 8 | housekeeping_buffer[21];

                free(housekeeping_buffer);
                controller_state = FINISH;
                break;
            }
            case FINISH:
            {
                break;
                //TODO: Send appropriate termination command
            }
            case ERROR_STATE:
            {
                break;
                //TODO: ERROR_STATE handler
            }
        }
    }

    if (controller_state == FINISH) {
        return;
    }
}

void iris_update_sensor_i2c_reg() {
    IRIS_return ret;

    controller_state = SEND_COMMAND;

    while (controller_state != FINISH && controller_state != ERROR_STATE) {
        switch (controller_state) {
            case SEND_COMMAND:
            {
                ret = send_command(IRIS_UPDATE_SENSOR_I2C_REG);
                if (ret == IRIS_ACK) {
                    controller_state = SEND_DATA;
                } else {
                    controller_state = FINISH;
                }
                break;
            }
            case SEND_DATA:
            {
                // TODO: Convert sensor_reg into buffer/array/vector. Need to think a bit more on this
                uint16_t sensor_reg_buffer[] = {0xFFFF, 0xFF};
                uint16_t tx_buffer[4] = {0x02, 0x06, 0x10, 0x14};
                send_data(tx_buffer, 4); // TODO: Need to take care of explicit declaration
                break;
            }
            case FINISH:
            {
                break;
                //TODO: Send appropriate termination command
            }
            case ERROR_STATE:
            {
                break;
                //TODO: ERROR_STATE handler
            }
        }
    }
}

void iris_update_current_limit(uint16_t current_limit) {
    IRIS_return ret;

    controller_state = SEND_COMMAND;

    while(controller_state != FINISH && controller_state != ERROR_STATE) {
        switch (controller_state) {
            case SEND_COMMAND:
            {
                send_command(IRIS_UPDATE_CURRENT_LIMIT);
                if (ret == IRIS_ACK) {
                    controller_state = SEND_DATA;
                } else {
                    controller_state = FINISH;
                }
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
            case ERROR_STATE:
            {
                break;
                //TODO: ERROR_STATE handler
            }
        }
    }
}


