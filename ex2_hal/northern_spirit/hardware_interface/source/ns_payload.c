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

NS_return HAL_NS_upload_artwork(char *filename){
#ifndef PAYLOAD_IS_STUBBED
    return NS_upload_artwork(filename);
#else
    return NS_IS_STUBBED;
#endif
}

NS_return HAL_NS_capture_image(){
#ifndef PAYLOAD_IS_STUBBED
    return NS_capture_image();
#else
    return NS_IS_STUBBED;
#endif
}

NS_return HAL_NS_confirm_downlink(uint8_t *conf){
#ifndef PAYLOAD_IS_STUBBED
    return NS_confirm_downlink(conf);
#else
    return NS_IS_STUBBED;
#endif
}

NS_return HAL_NS_get_heartbeat(uint8_t *heartbeat){
#ifndef PAYLOAD_IS_STUBBED
    return NS_get_heartbeat(heartbeat);
#else
    return NS_IS_STUBBED;
#endif
}

NS_return HAL_NS_get_flag(char flag, bool *stat){
#ifndef PAYLOAD_IS_STUBBED
    return NS_get_flag(flag, stat);
#else
    return NS_IS_STUBBED;
#endif
}

NS_return HAL_NS_get_filename(char subcode, char *filename){
#ifndef PAYLOAD_IS_STUBBED
    return NS_get_filename(subcode, filename);
#else
    return NS_IS_STUBBED;
#endif
}

NS_return HAL_NS_get_telemetry(ns_telemetry *tlm){
#ifndef PAYLOAD_IS_STUBBED
    return NS_get_telemetry(tlm);
#else
    return NS_IS_STUBBED;
#endif
}

NS_return HAL_NS_get_software_version(uint8_t *version){
#ifndef PAYLOAD_IS_STUBBED
    return NS_get_software_version(version);
#else
    return NS_IS_STUBBED;
#endif
}


