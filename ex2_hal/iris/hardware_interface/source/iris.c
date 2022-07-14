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
#include <stdio.h>

#include "iris.h"
#include "iris_gio.h"
#include "iris_spi.h"
#include "logger.h"
#include "redposix.h"

/*
 * Optimization points
 * - Full error coverage is desired
 */

struct __attribute__((__packed__)) {
    uint16_t vis_temp;
    uint16_t nir_temp;
    uint16_t flash_temp;
    uint16_t gate_temp;
    uint8_t imagenum;
    uint8_t software_version;
    uint8_t errornum;
    uint16_t MAX_5V_voltage;
    uint16_t MAX_5V_power;
    uint16_t MAX_3V_voltage;
    uint16_t MAX_3V_power;
    uint16_t MIN_5V_voltage;
    uint16_t MIN_3V_voltage;
} iris_hk_buffer;

/**
 * @brief
 *   Initialize low-level spi driver settings
 **/
Iris_HAL_return iris_init() {
    iris_spi_init();
    iris_gio_init();

#if IS_ATHENA == 1
    IRIS_BOOT_LOW();

    IRIS_nRST_LOW();
    vTaskDelay(1);
    IRIS_nRST_HIGH();
#endif

    // TODO: Add quick iris loopback test
    return IRIS_HAL_OK;
}

/**
 * @brief
 *   Sends take a picture command to Iris
 *
 * @return
 *   Returns IRIS_HAL_OK if equipment handler returns IRIS_ACK, else IRIS_HAL_ERROR
 **/
Iris_HAL_return iris_take_pic() {
    IrisLowLevelReturn ret;

    controller_state = SEND_COMMAND;

    while (1) {
        switch (controller_state) {
        case SEND_COMMAND: {
            ret = iris_send_command(IRIS_TAKE_PIC);
            if (ret == IRIS_ACK) {
                controller_state = FINISH;
            } else {
                controller_state = ERROR_STATE;
            }
            break;
        }
        case FINISH: {
            sys_log(INFO, "Iris returns ACK on take a picture command");
            return IRIS_HAL_OK;
        }
        case ERROR_STATE: {
            sys_log(INFO, "Iris returns NACK on take a picture command");
            return IRIS_HAL_ERROR;
        }
        }
    }
}

/**
 * @brief
 *   Sends a get image length command to Iris, and expects to receive the
 *   image length
 *
 * @return
 *   Returns the length in bytes of image stored on Iris
 **/
Iris_HAL_return iris_get_image_length(uint32_t *image_length) {
    IrisLowLevelReturn ret;

    controller_state = SEND_COMMAND;

    while (1) {
        switch (controller_state) {
        case SEND_COMMAND: {
            ret = iris_send_command(IRIS_GET_IMAGE_LENGTH);
            if (ret == IRIS_ACK) {
                controller_state = GET_DATA;
            } else {
                controller_state = ERROR_STATE;
            }
            break;
        }
        case GET_DATA: {
            static uint16_t image_length_buffer[MAX_IMAGE_LENGTH];
            ret = iris_get_data(image_length_buffer, MAX_IMAGE_LENGTH);
            if (ret == IRIS_HAL_OK) {
                controller_state = FINISH;
            } else {
                controller_state = ERROR_STATE;
            }
            /* It is expected that the first byte in the buffer will be the LSB */
            *(image_length) =
                (uint32_t)((uint8_t)image_length_buffer[0] << 16 | (uint8_t)image_length_buffer[1] << 8 |
                           (uint8_t)image_length_buffer[2]); // Concatenate image_length_buffer

            controller_state = FINISH;
            break;
        }
        case FINISH: {
            sys_log(INFO, "Iris returns ACK on get image length command");
            return IRIS_HAL_OK;
        }
        case ERROR_STATE: {
            sys_log(INFO, "Iris returns NACK on get image length command");
            return IRIS_HAL_ERROR;
        }
        }
    }
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
Iris_HAL_return iris_transfer_image(uint32_t image_length) {
    uint16_t num_transfer;
    IrisLowLevelReturn ret;

#if IS_ATHENA == 1
    uint32_t *fptr;
    fptr = red_open("iris_image.jpg", RED_O_CREAT | RED_O_WRONLY);
#else
    uint32_t fptr;
    fptr = red_open("iris_image_2.jpg", RED_O_CREAT | RED_O_WRONLY);
//    FILE *fptr;
//    fptr = fopen("new_image.jpg", "wb");
#endif

    if (fptr == NULL) {
        return;
    }

    controller_state = SEND_COMMAND;

    while (1) {
        switch (controller_state) {
        case SEND_COMMAND: // Send start image transfer command
        {
            ret = iris_send_command(IRIS_TRANSFER_IMAGE);
            if (ret == IRIS_ACK) {
                controller_state = GET_DATA;
            } else {
                controller_state = ERROR_STATE;
            }
            IRIS_WAIT_FOR_STATE_TRANSITION;
            break;
        }
        case GET_DATA: // Get image data in chunks/blocks
        {
            static uint16_t image_data_buffer[IMAGE_TRANSFER_SIZE];
            static uint8_t image_data_buffer_8Bit[IMAGE_TRANSFER_SIZE];
            memset(image_data_buffer, 0, IMAGE_TRANSFER_SIZE);
            num_transfer = (uint16_t)((image_length + (IMAGE_TRANSFER_SIZE - 1)) /
                                      IMAGE_TRANSFER_SIZE); // TODO: Ceiling division not working 100%

            vTaskDelay(150);
            for (uint32_t count_transfer = 0; count_transfer < num_transfer; count_transfer++) {
                ret = iris_get_data(image_data_buffer, IMAGE_TRANSFER_SIZE);

                for (int i = 0; i < IMAGE_TRANSFER_SIZE; i++) {
                    image_data_buffer_8Bit[i] = (image_data_buffer[i] >> (8 * 0)) & 0xff;
                }

#if IS_ATHENA == 1
                red_write(fptr, image_data_buffer_8Bit, IMAGE_TRANSFER_SIZE);
#else
                red_write(fptr, image_data_buffer_8Bit, IMAGE_TRANSFER_SIZE);
                // fwrite(image_data_buffer_8Bit, 1, IMAGE_TRANSFER_SIZE, fptr);
#endif
                vTaskDelay(10);
            }
            red_close(fptr);
#if IS_ATHENA == 1
            red_close(fptr);
#endif
            controller_state = FINISH;
            break;
        }
        case FINISH: {
            sys_log(INFO, "Iris returns ACK on transfer image command");
            return IRIS_HAL_OK;
        }
        case ERROR_STATE: {
            sys_log(INFO, "Iris returns NACK on transfer image command");
            return IRIS_HAL_ERROR;
        }
        }
    }
}

/**
 * @brief
 *   Sends a get image count command to Iris, and expects to receive the
 *   number of images stored on iris
 *
 * @return
 *   Returns the number of images stored on Iris
 **/
Iris_HAL_return iris_get_image_count(uint16_t *image_count) {
    IrisLowLevelReturn ret;

    controller_state = SEND_COMMAND;

    while (1) {
        switch (controller_state) {
        case SEND_COMMAND: {
            ret = iris_send_command(IRIS_GET_IMAGE_COUNT);
            if (ret == IRIS_ACK) {
                controller_state = GET_DATA;
            } else {
                controller_state = ERROR_STATE;
            }
            break;
        }
        case GET_DATA: {
            ret = iris_get_data(image_count, MAX_IMAGE_COUNT);
            controller_state = FINISH;
            break;
        }
        case FINISH: {
            sys_log(INFO, "Iris returns ACK on transfer image command");
            return IRIS_HAL_OK;
        }
        case ERROR_STATE: {
            sys_log(INFO, "Iris returns NACK on transfer image command");
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
Iris_HAL_return iris_toggle_sensor(IRIS_SENSOR_TOGGLE toggle) {
    IrisLowLevelReturn ret;

    controller_state = SEND_COMMAND;

    while (1) {
        switch (controller_state) {
        case SEND_COMMAND: {
            if (toggle == IRIS_SENSOR_ON) {
                ret = iris_send_command(IRIS_ON_SENSOR_IDLE);
            } else if (toggle == IRIS_SENSOR_OFF) {
                ret = iris_send_command(IRIS_OFF_SENSOR_IDLE);
            }
            if (ret == IRIS_ACK) {
                controller_state = FINISH;
            } else {
                controller_state = ERROR_STATE;
            }
            break;
        }
        case FINISH: {
            sys_log(INFO, "Iris returns ACK on toggling sensor command");
            return IRIS_HAL_OK;
        }
        case ERROR_STATE: {
            sys_log(INFO, "Iris returns NACK on toggling sensor command");
            return IRIS_HAL_ERROR;
        }
        }
    }
}

/**
 * @brief
 *   Sends a housekeeping command to Iris and expects to receive housekeeping
 *   statistics which then is stored into internal housekeeping struct
 *
 * @return
 *   Returns a housekeeping data structure
 **/
Iris_HAL_return iris_get_housekeeping(IRIS_Housekeeping *hk_data) {
    IrisLowLevelReturn ret;

    controller_state = SEND_COMMAND;

    while (1) {
        switch (controller_state) {
        case SEND_COMMAND: {
            ret = iris_send_command(IRIS_SEND_HOUSEKEEPING);
            if (ret == IRIS_ACK) {
                controller_state = GET_DATA;
            } else {
                controller_state = ERROR_STATE;
            }
            IRIS_WAIT_FOR_STATE_TRANSITION;
            break;
        }
        case GET_DATA: {

            static uint16_t housekeeping_buffer[HOUSEKEEPING_SIZE];
            ret = iris_get_data(housekeeping_buffer, HOUSEKEEPING_SIZE);
            if (ret == IRIS_HAL_OK) {
                controller_state = FINISH;
            } else {
                controller_state = ERROR_STATE;
            }

            // Pre-processed housekeeping data
            iris_hk_buffer.vis_temp = housekeeping_buffer[1] << 8 | housekeeping_buffer[0];
            iris_hk_buffer.nir_temp = housekeeping_buffer[3] << 8 | housekeeping_buffer[2];
            iris_hk_buffer.flash_temp = housekeeping_buffer[5] << 8 | housekeeping_buffer[4];
            iris_hk_buffer.gate_temp = housekeeping_buffer[7] << 8 | housekeeping_buffer[6];
            iris_hk_buffer.imagenum = housekeeping_buffer[8];
            iris_hk_buffer.software_version = housekeeping_buffer[9];
            iris_hk_buffer.errornum = housekeeping_buffer[10];
            iris_hk_buffer.MAX_5V_voltage = housekeeping_buffer[12] << 8 | housekeeping_buffer[11];
            iris_hk_buffer.MAX_5V_power = housekeeping_buffer[14] << 8 | housekeeping_buffer[13];
            iris_hk_buffer.MAX_3V_voltage = housekeeping_buffer[16] << 8 | housekeeping_buffer[15];
            iris_hk_buffer.MAX_3V_power = housekeeping_buffer[18] << 8 | housekeeping_buffer[17];
            iris_hk_buffer.MIN_5V_voltage = housekeeping_buffer[20] << 8 | housekeeping_buffer[19];
            iris_hk_buffer.MIN_3V_voltage = housekeeping_buffer[22] << 8 | housekeeping_buffer[21];

            // Post-processed housekeeping data
            hk_data->vis_temp = iris_convert_hk_temperature(iris_hk_buffer.vis_temp);
            hk_data->nir_temp = iris_convert_hk_temperature(iris_hk_buffer.nir_temp);
            hk_data->flash_temp = iris_convert_hk_temperature(iris_hk_buffer.flash_temp);
            hk_data->gate_temp = iris_convert_hk_temperature(iris_hk_buffer.gate_temp);
            hk_data->imagenum = iris_hk_buffer.imagenum;
            hk_data->software_version = iris_hk_buffer.software_version;
            hk_data->errornum = iris_hk_buffer.errornum;
            hk_data->MAX_5V_voltage = iris_hk_buffer.MAX_5V_voltage;
            hk_data->MAX_5V_power = iris_hk_buffer.MAX_5V_power;
            hk_data->MAX_3V_voltage = iris_hk_buffer.MAX_3V_voltage;
            hk_data->MAX_3V_power = iris_hk_buffer.MAX_3V_power;
            hk_data->MIN_5V_voltage = iris_hk_buffer.MIN_5V_voltage;
            hk_data->MIN_3V_voltage = iris_hk_buffer.MIN_3V_voltage;

            controller_state = FINISH;

            IRIS_WAIT_FOR_STATE_TRANSITION;
            break;
        }
        case FINISH: {
            sys_log(INFO, "Iris returns ACK on housekeeping command");
            return IRIS_HAL_OK;
        }
        case ERROR_STATE: {
            sys_log(INFO, "Iris returns NACK on housekeeping command");
            return IRIS_HAL_ERROR;
        }
        }
    }
}

/**
 * @brief
 *   Sends a update sensor register command to Iris and then send the
 *   register address + data

 * @return
 *   Returns IRIS_HAL_OK if equipment handler returns IRIS_ACK, else IRIS_HAL_ERROR
 **/
Iris_HAL_return iris_update_sensor_i2c_reg() {
    IrisLowLevelReturn ret;

    controller_state = SEND_COMMAND;

    while (1) {
        switch (controller_state) {
        case SEND_COMMAND: {
            ret = iris_send_command(IRIS_UPDATE_SENSOR_I2C_REG);
            if (ret == IRIS_ACK) {
                controller_state = SEND_DATA;
            } else {
                controller_state = FINISH;
            }
            break;
        }
        case SEND_DATA: {
            // TODO: Convert sensor_reg into buffer/array/vector. Need to think a bit more on this
            // uint16_t sensor_reg_buffer[] = {0xFFFF, 0xFF};
            uint16_t tx_buffer[4] = {0x02, 0x06, 0x10, 0x14};
            ret = iris_send_data(tx_buffer, 4); // TODO: Need to take care of explicit declaration

            if (ret != IRIS_ACK) {
                sys_log(INFO, "Updating Iris sensor registers failed");
                return IRIS_HAL_ERROR;
            }
            break;
        }
        case FINISH: {
            sys_log(INFO, "Iris returns ACK on update sensor register command");
            return IRIS_HAL_OK;
        }
        case ERROR_STATE: {
            sys_log(INFO, "Iris returns NACK on update sensor register command");
            return IRIS_HAL_ERROR;
            // TODO: ERROR_STATE handler
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
Iris_HAL_return iris_update_current_limit(uint16_t current_limit) {
    IrisLowLevelReturn ret;

    controller_state = SEND_COMMAND;

    while (1) {
        switch (controller_state) {
        case SEND_COMMAND: {
            iris_send_command(IRIS_UPDATE_CURRENT_LIMIT);
            if (ret == IRIS_ACK) {
                controller_state = SEND_DATA;
            } else {
                controller_state = FINISH;
            }
            break;
        }
        case SEND_DATA: {
            // TODO: Convert sensor_reg into buffer/array/vector. Need to think a bit more on this
            uint16_t current_limit_buffer[] = {current_limit};
            iris_send_data(current_limit_buffer, 1); // TODO: Need to take care of explicit declaration

            controller_state = FINISH;
            break;
        }
        case FINISH: {
            sys_log(INFO, "Iris successful on update current limit command");
            return IRIS_HAL_OK;
        }
        case ERROR_STATE: {
            sys_log(WARN, "Iris failure on update current limit command");
            return IRIS_HAL_ERROR;
        }
        }
    }
    return IRIS_HAL_ERROR;
}

/*          Iris common functions           */
float iris_convert_hk_temperature(uint16_t temperature) {
    float upper = (float)(temperature >> 8);
    float lower = (float)((temperature & 0xFF) >> 4) * 0.0625;

    return (float)(upper + lower);
}
