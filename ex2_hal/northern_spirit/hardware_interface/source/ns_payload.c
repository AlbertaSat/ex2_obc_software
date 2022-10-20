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

#include "ns_payload.h"

NS_return HAL_NS_upload_artwork(char *filename) {
#if NS_IS_STUBBED == 0
    return NS_upload_artwork(filename);
#else
    return NS_STUBBED;
#endif
}

NS_return HAL_NS_download_image() {
#if NS_IS_STUBBED == 0
    return NS_download_image();
#else
    return NS_STUBBED;
#endif
}

NS_return HAL_NS_clear_sd_card() {
#if NS_IS_STUBBED == 0
    return NS_clear_sd_card();
#else
    return NS_STUBBED;
#endif
}

NS_return HAL_NS_capture_image() {
#if NS_IS_STUBBED == 0
    return NS_capture_image();
#else
    return NS_STUBBED;
#endif
}

NS_return HAL_NS_confirm_downlink(uint8_t *conf) {
#if NS_IS_STUBBED == 0
    return NS_confirm_downlink(conf);
#else
    return NS_STUBBED;
#endif
}

NS_return HAL_NS_get_heartbeat(uint8_t *heartbeat) {
#if NS_IS_STUBBED == 0
    return NS_get_heartbeat(heartbeat);
#else
    return NS_STUBBED;
#endif
}

NS_return HAL_NS_get_flag(char flag, bool *stat) {
#if NS_IS_STUBBED == 0
    return NS_get_flag(flag, stat);
#else
    return NS_STUBBED;
#endif
}

NS_return HAL_NS_get_filename(char subcode, char *filename) {
#if NS_IS_STUBBED == 0
    return NS_get_filename(subcode, filename);
#else
    return NS_STUBBED;
#endif
}

NS_return HAL_NS_get_telemetry(ns_telemetry *tlm) {
#if NS_IS_STUBBED == 0
    return NS_get_telemetry(tlm);
#else
    return NS_STUBBED;
#endif
}

NS_return HAL_NS_reset_mcu() {
#if NS_IS_STUBBED == 0
    return NS_reset_mcu();
#else
    return NS_STUBBED;
#endif
}

NS_return HAL_NS_get_software_version(uint8_t *version) {
#if NS_IS_STUBBED == 0
    return NS_get_software_version(version);
#else
    return NS_STUBBED;
#endif
}
