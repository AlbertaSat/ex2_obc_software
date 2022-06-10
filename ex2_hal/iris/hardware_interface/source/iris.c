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
#include <string.h>
#include <stdlib.h>

#include "iris.h"
#include "iris_spi.h"
#include "logger.h"

/*
 * Optimization points
 * - Full error coverage is desired
 */


/**
 * @brief
 *   Sends take a picture command to Iris
 *
 * @return
 *   Returns IRIS_HAL_OK if equipment handler returns IRIS_ACK, else IRIS_HAL_ERROR
 **/
IrisHALReturn iris_take_pic() {
    IrisLowLevelReturn ret;

    controller_state = SEND_COMMAND;

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
                ex2_log("Iris returns ACK on take a picture command");
                return IRIS_HAL_OK;
            }
            case ERROR_STATE:
            {
                ex2_log("Iris returns NACK on take a picture command");
                return IRIS_HAL_ERROR;
            }
        }
    }
    return IRIS_HAL_ERROR;
}

/**
 * @brief
 *   Sends a get image length command to Iris, and expects to receive the
 *   image length
 *
 * @return
 *   Returns the length in bytes of image stored on Iris
 **/
IrisHALReturn iris_get_image_length(uint32_t *image_length) {
    IrisLowLevelReturn ret;

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
                uint16_t * image_length_buffer = (uint16_t*) pvPortMalloc(MAX_IMAGE_LENGTH * sizeof(uint16_t));
                memset(image_length_buffer, 0, MAX_IMAGE_LENGTH);
                if (image_length_buffer == NULL) {
                    ex2_log("Failed attempt to dynamically allocate memory under iris get image length");
                    return IRIS_HAL_ERROR;;
                }
                ret = get_data(image_length_buffer, MAX_IMAGE_LENGTH);
                if (ret == IRIS_HAL_OK) {
                    controller_state = FINISH;
                } else {
                    controller_state = ERROR_STATE;
                }
                /* It is expected that the first byte in the buffer will be the LSB */
                *(image_length) = (uint32_t)((uint8_t)image_length_buffer[2]<<16 | (uint8_t)image_length_buffer[1]<<8 | (uint8_t)image_length_buffer[0]); // Concatenate image_length_buffer

                free(image_length_buffer);
                controller_state = FINISH;
                break;
            }
            case FINISH:
            {
                ex2_log("Iris returns ACK on get image length command");
                return IRIS_HAL_OK;
            }
            case ERROR_STATE:
            {
                ex2_log("Iris returns NACK on get image length command");
                return IRIS_HAL_ERROR;
            }
        }
    }
    return IRIS_HAL_ERROR;
}

/**
 * @brief
 *   Sends a transfer image command to Iris, and expects to receive the
 *   image data of size image_length
 *
 * @param[in] image_length
 *   Number of bytes expected to be received from Iris
 *
 * @return
 *   Returns IRIS_HAL_OK if equipment handler returns IRIS_ACK, else IRIS_HAL_ERROR
 **/
IrisHALReturn iris_transfer_image(uint32_t image_length) {
    uint32_t num_transfer;
    IrisLowLevelReturn ret;

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
                uint16_t * image_data_buffer = (uint16_t*) pvPortMalloc(IMAGE_TRANSFER_SIZE * sizeof(uint16_t));
                memset(image_data_buffer, 0, IMAGE_TRANSFER_SIZE);
                if (image_data_buffer == NULL) {
                    ex2_log("Failed attempt to dynamically allocate memory under iris get image data");
                    return IRIS_HAL_ERROR;;
                }
                num_transfer = (IMAGE_TRANSFER_SIZE + image_length) / IMAGE_TRANSFER_SIZE; // Ceiling division
                for (uint32_t count_transfer = 0; count_transfer < num_transfer; count_transfer++) {
                    ret = get_data(image_data_buffer, IMAGE_TRANSFER_SIZE);
                    // TODO: Do something with the received data (e.g transfer it to the SD card)
                    // Or just get the data and send it forward to the next stage. Prefer not to have too
                    // much data processing in driver code

                    memset(image_data_buffer, 0, IMAGE_TRANSFER_SIZE);
                }
                free(image_data_buffer);
                controller_state = FINISH;
                break;
            }
            case FINISH:
            {
                ex2_log("Iris returns ACK on transfer image command");
                return IRIS_HAL_OK;
            }
            case ERROR_STATE:
            {
                ex2_log("Iris returns NACK on transfer image command");
                return IRIS_HAL_ERROR;
            }
        }
    }
    return IRIS_HAL_ERROR;
}

/**
 * @brief
 *   Sends a get image count command to Iris, and expects to receive the
 *   number of images stored on iris
 *
 * @return
 *   Returns the number of images stored on Iris
 **/
IrisHALReturn iris_get_image_count(uint8_t *image_count) {
    IrisLowLevelReturn ret;

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
                //TODO: write code to receive image count
                break;
            }
            case FINISH:
            {
                ex2_log("Iris returns ACK on transfer image command");
                return IRIS_HAL_OK;
            }
            case ERROR_STATE:
            {
                ex2_log("Iris returns NACK on transfer image command");
                return IRIS_HAL_ERROR;
            }
        }
    }
    return IRIS_HAL_ERROR;
}

/**
 * @brief
 *   Sends a toggle sensors command to Iris
 *
 * @param[in] toggle
 *   Flag to either turn on or off Iris sensors
 *
 * @return
 *   Returns IRIS_HAL_OK if equipment handler returns IRIS_ACK, else IRIS_HAL_ERROR
 **/
IrisHALReturn iris_toggle_sensor_idle(IRIS_SENSOR_TOOGGLE toggle) {
    IrisLowLevelReturn ret;

    controller_state = SEND_COMMAND;

    while (controller_state != FINISH && controller_state != ERROR_STATE) {
        switch (controller_state) {
            case SEND_COMMAND:
            {
                if (toggle == IRIS_SENSOR_ON) {
                    ret = send_command(IRIS_ON_SENSOR_IDLE);
                } else if (toggle == IRIS_SENSOR_OFF) {
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
                ex2_log("Iris returns ACK on toggling sensor command");
                return IRIS_HAL_OK;
            }
            case ERROR_STATE:
            {
                ex2_log("Iris returns NACK on toggling sensor command");
                return IRIS_HAL_ERROR;
            }
        }
    }
    return IRIS_HAL_ERROR;
}

/**
 * @brief
 *   Sends a housekeeping command to Iris and expects to receive housekeeping
 *   statistics which then is stored into internal housekeeping struct
 *
 * @return
 *   Returns a housekeeping data structure
 **/
IrisHALReturn iris_get_housekeeping(iris_housekeeping_data hk_data) {
    IrisLowLevelReturn ret;

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

                uint16_t * housekeeping_buffer = (uint16_t*) pvPortMalloc(HOUSEKEEPING_SIZE * sizeof(uint16_t));
                memset(housekeeping_buffer, 0, HOUSEKEEPING_SIZE);
                if (housekeeping_buffer == NULL) {
                    ex2_log("Failed attempt to dynamically allocate memory under iris get housekeeping");
                    return IRIS_HAL_ERROR;;
                }
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
                ex2_log("Iris returns ACK on housekeeping command");
                return IRIS_HAL_OK;
            }
            case ERROR_STATE:
            {
                ex2_log("Iris returns NACK on housekeeping command");
                return IRIS_HAL_ERROR;
            }
        }
    }
    return IRIS_HAL_ERROR;
}

/**
 * @brief
 *   Sends a update sensor register command to Iris and then send the
 *   register address + data

 * @return
 *   Returns IRIS_HAL_OK if equipment handler returns IRIS_ACK, else IRIS_HAL_ERROR
 **/
IrisHALReturn iris_update_sensor_i2c_reg() {
    IrisLowLevelReturn ret;

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
                //uint16_t sensor_reg_buffer[] = {0xFFFF, 0xFF};
                uint16_t tx_buffer[4] = {0x02, 0x06, 0x10, 0x14};
                ret = send_data(tx_buffer, 4); // TODO: Need to take care of explicit declaration

                if (ret != IRIS_ACK) {
                    ex2_log("Updating Iris sensor registers failed");
                    return IRIS_HAL_ERROR;
                }
                break;
            }
            case FINISH:
            {
                ex2_log("Iris returns ACK on update sensor register command");
                return IRIS_HAL_OK;
            }
            case ERROR_STATE:
            {
                ex2_log("Iris returns NACK on update sensor register command");
                return IRIS_HAL_ERROR;
                //TODO: ERROR_STATE handler
            }
        }
    }
    return IRIS_HAL_ERROR;
}

/**
 * @brief
 *   Sends a update current limit command to Iris
 *
 * @param[in] current limit
 *   Current limit set point
 *
 * @return
 *   Returns IRIS_HAL_OK if equipment handler returns IRIS_ACK, else IRIS_HAL_ERROR
 **/
IrisHALReturn iris_update_current_limit(uint16_t current_limit) {
    IrisLowLevelReturn ret;

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
                ex2_log("Iris returns ACK on update current limit command");
                return IRIS_HAL_OK;
            }
            case ERROR_STATE:
            {
                ex2_log("Iris returns NACK on update current limit command");
                return IRIS_HAL_ERROR;
            }
        }
    }
    return IRIS_HAL_ERROR;
}


