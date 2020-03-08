// /*
//  * Copyright (C) 2015  Brendan Bruner
//  *
//  * This program is free software; you can redistribute it and/or
//  * modify it under the terms of the GNU General Public License
//  * as published by the Free Software Foundation; either version 2
//  * of the License, or (at your option) any later version.
//  *
//  * This program is distributed in the hope that it will be useful,
//  * but WITHOUT ANY WARRANTY; without even the implied warranty of
//  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  * GNU General Public License for more details.
//  *
//  * bbruner@ualberta.ca
//  */
// /**
//  * @file driver_toolkit_base.c
//  * @author Brendan Bruner
//  * @date Jun 3, 2015
//  */

// #include <driver_toolkit/driver_toolkit.h>
// #include <portable_types.h>
// #include <core_defines.h>
// #include <stdio.h>

// /**
//  * @memberof driver_toolkit_t
//  * @protected
//  * @brief
//  * 		Constructor.
//  * @details
//  * 		Constructor.
//  */
// _Bool initialize_driver_toolkit_( driver_toolkit_t* toolkit, filesystem_t* fs )
// {
// 	DEV_ASSERT( toolkit );
// 	DEV_ASSERT( fs );

// 	logger_error_t tl_err;
// 	tp_error_t tp_err;

// 	if( initialize_ptimer_controller( ) == NULL )
// 	{
// 		return false;
// 	}

// 	toolkit->fs = fs;

// 	tp_err = initialize_telemetry_priority( &toolkit->priority, toolkit->fs, PRIORITY_LOG_FILE );
// 	if( tp_err != TP_OK )
// 	{
// 		return false;
// 	}

// 	/* Cache scv configuration from non volatile memory
// 	 * before initializing loggers.
// 	 */
// 	scv_config_cache( );
// 	tl_err = initialize_logger( &toolkit->_.athena_logger_mem, toolkit->fs, ATHENA_LOGGER_MASTER_TABLE, ATHENA_LOGGER_IDENTIFIER, ATHENA_LOGGER_MAX_CAPACITY );
// 	if( tl_err != LOGGER_OK )
// 	{
// 		return false;
// 	}
// 	tl_err = initialize_logger( &toolkit->_.dfgm_filt1_logger_mem, toolkit->fs, DFGM_FILT1_LOGGER_MASTER_TABLE, DFGM_FILT1_LOGGER_IDENTIFIER, DFGM_FILT1_LOGGER_MAX_CAPACITY );
// 	if( tl_err != LOGGER_OK )
// 	{
// 		return false;
// 	}
// 	tl_err = initialize_logger( &toolkit->_.dfgm_filt2_logger_mem, toolkit->fs, DFGM_FILT2_LOGGER_MASTER_TABLE, DFGM_FILT2_LOGGER_IDENTIFIER, DFGM_FILT2_LOGGER_MAX_CAPACITY );
// 	if( tl_err != LOGGER_OK )
// 	{
// 		return false;
// 	}
// 	tl_err = initialize_logger( &toolkit->_.dfgm_raw_logger_mem, toolkit->fs, DFGM_RAW_LOGGER_MASTER_TABLE, DFGM_RAW_LOGGER_IDENTIFIER, DFGM_RAW_LOGGER_MAX_CAPACITY );
// 	if( tl_err != LOGGER_OK )
// 	{
// 		return false;
// 	}
// 	tl_err = initialize_logger( &toolkit->_.dfgm_hk_logger_mem, toolkit->fs, DFGM_HK_LOGGER_MASTER_TABLE, DFGM_HK_LOGGER_IDENTIFIER, DFGM_HK_LOGGER_MAX_CAPACITY );
// 	if( tl_err != LOGGER_OK )
// 	{
// 		return false;
// 	}
// 	tl_err = initialize_logger( &toolkit->_.wod_logger_mem, toolkit->fs, WOD_LOGGER_MASTER_TABLE, WOD_LOGGER_IDENTIFIER, WOD_LOGGER_MAX_CAPACITY );
// 	if( tl_err != LOGGER_OK )
// 	{
// 		return false;
// 	}
// 	tl_err = initialize_logger( &toolkit->_.udos_logger_mem, toolkit->fs, UDOS_LOGGER_MASTER_TABLE, UDOS_LOGGER_IDENTIFIER, UDOS_LOGGER_MAX_CAPACITY );
// 	if( tl_err != LOGGER_OK )
// 	{
// 		return false;
// 	}

// 	toolkit->wod_logger = &toolkit->_.wod_logger_mem;
// 	toolkit->dfgm_filt1_logger = &toolkit->_.dfgm_filt1_logger_mem;
// 	toolkit->dfgm_filt2_logger = &toolkit->_.dfgm_filt2_logger_mem;
// 	toolkit->dfgm_raw_logger = &toolkit->_.dfgm_raw_logger_mem;
// 	toolkit->dfgm_hk_logger = &toolkit->_.dfgm_hk_logger_mem;
// 	toolkit->athena_logger = &toolkit->_.athena_logger_mem;
// 	toolkit->udos_logger = &toolkit->_.udos_logger_mem;

// 	return true;
// }

// void destroy_driver_toolkit( driver_toolkit_t *kit )
// {
// 	DEV_ASSERT( kit );

// 	destroy_adcs( &kit->adcs );
// 	destroy_ground_station( kit->gs );

// 	destroy_rtc( kit->rtc );

// 	kit->fs->destroy( kit->fs );

// 	destroy_telemetry_priority( &kit->priority );
// 	((logger_t *) &kit->_.wod_logger_mem)->destroy( (logger_t *) &kit->_.wod_logger_mem );
// 	((logger_t *) &kit->_.dfgm_filt1_logger_mem)->destroy( (logger_t *) &kit->_.dfgm_filt1_logger_mem );
// 	((logger_t *) &kit->_.dfgm_filt2_logger_mem)->destroy( (logger_t *) &kit->_.dfgm_filt2_logger_mem );
// 	((logger_t *) &kit->_.dfgm_raw_logger_mem)->destroy( (logger_t *) &kit->_.dfgm_raw_logger_mem );
// }
