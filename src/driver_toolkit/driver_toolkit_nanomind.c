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
//  * @file driver_toolkit
//  * @author Brendan Bruner
//  * @date Feb 4, 2015
//  */
// #include <driver_toolkit/driver_toolkit_nanomind.h>
// #include <portable_types.h>
// #include <printing.h>
// #include <telecommands/telecommand_server.h>
// #include <mnlp.h>
// #include <dfgm.h>
// #include <athena_gateway/athena.h>

// _Bool initialize_driver_toolkit_nanomind( driver_toolkit_nanomind_t *toolkit )
// {
// 	DEV_ASSERT( toolkit );

// 	uint8_t fs_err;
// 	_Bool	super_err;

// 	fs_err = initialize_filesystem_fatfs_nanomind( &toolkit->_.fs );
// 	if( fs_err != FILESYSTEM_SUCCESS )
// 	{
// 		return false;
// 	}

// 	/* Initialize RTC */
// 	((driver_toolkit_t*) toolkit)->rtc = get_rtc_nanomind( );
// 	if( ((driver_toolkit_t*) toolkit)->rtc == NULL )
// 	{
// 		return false;
// 	}

// 	/* Initialize super with valid filesystem. */
// 	extern _Bool initialize_driver_toolkit_( driver_toolkit_t*, filesystem_t* );
// 	super_err = initialize_driver_toolkit_( (driver_toolkit_t*) toolkit, (filesystem_t*) &toolkit->_.fs );

// 	/* Initialize ADCS */
// 	initialize_adcs_nanomind( &((driver_toolkit_t*) toolkit)->adcs );

// 	/* Initialize EPS */
// 	initialize_eps_nanomind( &toolkit->_.eps );
// 	((driver_toolkit_t*) toolkit)->eps = (eps_t*) &toolkit->_.eps;

// 	/* Initialize COMM. */
// 	initialize_comm_nanomind( &toolkit->_.comm );
// 	((driver_toolkit_t*) toolkit)->comm = (comm_t*) &toolkit->_.comm;

// 	/* Initialize ground station. */
// 	initialize_ground_station_nanomind( &toolkit->_.gs);
// 	((driver_toolkit_t*) toolkit)->gs = (ground_station_t*) &toolkit->_.gs;

// 	/* Nanohub. */
// 	initialize_nanohub( &toolkit->_.hub );
// 	((driver_toolkit_t*) toolkit)->hub = (hub_t*) &toolkit->_.hub;

// 	/* DFGM. */
// 	initialize_dfgm_external( &toolkit->_.dfgm );
// 	((driver_toolkit_t*) toolkit)->dfgm = (dfgm_t*) &toolkit->_.dfgm;

// 	/* MNLP. */
// 	initialize_mnlp_external( &toolkit->_.mnlp );
// 	((driver_toolkit_t*) toolkit)->mnlp = (mnlp_t*) &toolkit->_.mnlp;

// 	/* Teledyne. */
// 	initialize_teledyne_external( &toolkit->_.teledyne );
// 	((driver_toolkit_t*) toolkit)->teledyne = (teledyne_t*) &toolkit->_.teledyne;

// 	mnlp_preinit(); // Initialize semaphores
// 	DFGM_init(); // Initialize semaphores and queues
// 	dosimeter_start(((driver_toolkit_t*) toolkit)->teledyne); // Dosimeter runs always
// 	athena_init( ); // Start athena task.

// 	start_telecommand_server((driver_toolkit_t*) toolkit);

// 	return super_err;
// }
