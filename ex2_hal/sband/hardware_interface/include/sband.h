#ifndef SBAND_H
#define SBAND_H

/*
 * Copyright (C) 2022 University of Alberta
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
 * @file sband.h
 * @author Ron Unrau
 * @date 2022-07-01
 */
#include <hal_sband.h>
#include <spi.h>

#if SBAND_COMMERCIAL_FREQUENCY == 1
#define SBAND_FREQUENCY 2228.0
#endif

/* Send an S-Band Sync word every sync interval bytes */
#define SBAND_SYNC_INTERVAL 8*1024

/* The depth of our TX FIFO */
#define SBAND_FIFO_DEPTH 20*1024

int sband_init(void);

bool sband_enter_conf_mode(void);
bool sband_enter_sync_mode(void);
bool sband_enter_data_mode(void);

void sband_sync(void);

int sband_transmit_ready(void);

bool sband_buffer_count(uint16_t *cnt);

#endif /* SBAND_H */
