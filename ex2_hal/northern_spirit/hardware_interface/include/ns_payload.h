/*
 * Copyright (C) 2022  University of Alberta
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
 * @file ns_payload.h
 * @author Thomas Ganley
 * @date 2022-06-12
 */

#ifndef EX2_HAL_NORTHERN_SPIRIT_HARDWARE_INTERFACE_INCLUDE_NS_PAYLOAD_H_
#define EX2_HAL_NORTHERN_SPIRIT_HARDWARE_INTERFACE_INCLUDE_NS_PAYLOAD_H_

#include "northern_spirit_handler.h"

NS_return HAL_NS_upload_artwork(char *filename);
NS_return HAL_NS_capture_image(void);
NS_return HAL_NS_confirm_downlink(void);
NS_return HAL_NS_get_heartbeat(uint8_t *heartbeat);
NS_return HAL_NS_get_flag(char flag, bool *stat);
NS_return HAL_NS_get_telemetry(ns_telemetry *tlm);
NS_return HAL_NS_get_software_version(uint8_t *version);

#endif /* EX2_HAL_NORTHERN_SPIRIT_HARDWARE_INTERFACE_INCLUDE_NS_PAYLOAD_H_ */
