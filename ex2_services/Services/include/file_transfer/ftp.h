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

#ifndef FTP_H
#define FTP_H

#include <stdint.h>

#include "services.h"

typedef enum {FTP_GET_FILE_SIZE, FTP_REQUEST_BURST_DOWNLOAD, FTP_DATA_PACKET, FTP_START_UPLOAD, FTP_UPLOAD_PACKET } FTP_Subtype;

SAT_returnState start_FTP_service(void);

#endif /* FTP_H */
