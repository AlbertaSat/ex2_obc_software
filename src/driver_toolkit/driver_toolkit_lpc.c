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
// #include <driver_toolkit/driver_toolkit_lpc.h>
// #include <portable_types.h>
// #include <filesystems/fatfs/filesystem_fatfs_lpc.h>
// #include <eps/eps_local.h>
// #include <uart/uart_lpc.h>

// _Bool initialize_driver_toolkit_lpc( driver_toolkit_lpc_t *toolkit )
// {
// 	DEV_ASSERT( toolkit );

// 	uint8_t fs_err;
// 	_Bool	super_err;
// 	gpio_t* eps_state_gpios[EPS_LOCAL_STATE_GPIO_COUNT];
// 	gpio_t*	eps_power_line_gpios[EPS_TOTAL_POWER_LINES];
// 	gpio_t* hub_deployment_gpios[HUB_TOTAL_DEPLOYMENT_LINES];
// 	gpio_t*	hub_power_line_gpios[HUB_TOTAL_POWER_LINES];

// 	fs_err = initialize_filesystem_fatfs_lpc( &toolkit->_.fs );
// 	if( fs_err != FILESYSTEM_SUCCESS )
// 	{
// 		return false;
// 	}

// 	/* Initialize RTC */
// 	((driver_toolkit_t*) toolkit)->rtc = get_rtc_lpc( );
// 	if( ((driver_toolkit_t*) toolkit)->rtc == NULL )
// 	{
// 		return false;
// 	}

// 	/* Initialize super with valid file system. */
// 	extern _Bool initialize_driver_toolkit_( driver_toolkit_t*, filesystem_t* );
// 	super_err = initialize_driver_toolkit_( (driver_toolkit_t*) toolkit, (filesystem_t*) &toolkit->_.fs );

// 	/* Initialize ADCS */
// 	initialize_adcs_lpc_local( &((driver_toolkit_t*) toolkit)->adcs );

// 	/* Initilizing GPIOs for the EPS. */
// 	/* State GPIOs. */
// //	initialize_gpio_lpc( &toolkit->_.eps_state_gpios_mem[0], 0, 18, GPIO_BARE );
// //	eps_state_gpios[0] = (gpio_t*) &toolkit->_.eps_state_gpios_mem[0];
// //	initialize_gpio_lpc( &toolkit->_.eps_state_gpios_mem[1], 0, 15, GPIO_BARE );
// //	eps_state_gpios[1] = (gpio_t*) &toolkit->_.eps_state_gpios_mem[1];
// //	initialize_gpio_lpc( &toolkit->_.eps_state_gpios_mem[2], 0, 17, GPIO_BARE );
// //	eps_state_gpios[2] = (gpio_t*) &toolkit->_.eps_state_gpios_mem[2];

// 	/* Power line GPIOs. */
// //	initialize_gpio_lpc( &toolkit->_.eps_power_line_gpios_mem[0], 2, 13, GPIO_BARE );
// //	eps_power_line_gpios[0] = (gpio_t*) &toolkit->_.eps_power_line_gpios_mem[0];
// //	initialize_gpio_lpc( &toolkit->_.eps_power_line_gpios_mem[1], 0, 23, GPIO_BARE );
// //	eps_power_line_gpios[1] = (gpio_t*) &toolkit->_.eps_power_line_gpios_mem[1];
// //	initialize_gpio_lpc( &toolkit->_.eps_power_line_gpios_mem[2], 0, 25, GPIO_BARE );
// //	eps_power_line_gpios[2] = (gpio_t*) &toolkit->_.eps_power_line_gpios_mem[2];
// //	initialize_gpio_lpc( &toolkit->_.eps_power_line_gpios_mem[3], 1, 30, GPIO_BARE );
// //	eps_power_line_gpios[3] = (gpio_t*) &toolkit->_.eps_power_line_gpios_mem[3];
// //	initialize_gpio_lpc( &toolkit->_.eps_power_line_gpios_mem[4], 0, 24, GPIO_BARE );
// //	eps_power_line_gpios[4] = (gpio_t*) &toolkit->_.eps_power_line_gpios_mem[4];
// //	initialize_gpio_lpc( &toolkit->_.eps_power_line_gpios_mem[5], 2, 13, GPIO_BARE );
// //	eps_power_line_gpios[5] = (gpio_t*) &toolkit->_.eps_power_line_gpios_mem[5];

// 	/* Initialize the eps with the GPIOs. */
// 	initialize_eps_local( &toolkit->_.eps, NULL, NULL );
// 	((driver_toolkit_t*) toolkit)->eps = (eps_t*) &toolkit->_.eps;

// 	/* Initialize COMM. */
// 	initialize_comm_local( &toolkit->_.comm );
// 	((driver_toolkit_t*) toolkit)->comm = (comm_t*) &toolkit->_.comm;

// 	/* Initialize ground station. */
// 	initialize_ground_station_uart( &toolkit->_.gs, (uart_t*) initialize_uart_lpc_simple( UART_PORT2 ) );
// 	((driver_toolkit_t*) toolkit)->gs = (ground_station_t*) &toolkit->_.gs;

// 	/* Initialize GPIOs for hub. */
// 	/* Deployment GPIOs. */
// //	initialize_gpio_lpc( &toolkit->_.hub_deployment_lines_mem[0], 0, 2, GPIO_BARE );
// //	hub_deployment_gpios[0] = (gpio_t*) &toolkit->_.hub_deployment_lines_mem[0];
// //	initialize_gpio_lpc( &toolkit->_.hub_deployment_lines_mem[1], 1, 31, GPIO_BARE );
// //	hub_deployment_gpios[1] = (gpio_t*) &toolkit->_.hub_deployment_lines_mem[1];

// 	/* Power line GPIOs. */
// //	initialize_gpio_lpc( &toolkit->_.hub_power_line_gpios_mem[0], 0, 26, GPIO_BARE );
// //	hub_power_line_gpios[0] = (gpio_t*) &toolkit->_.hub_power_line_gpios_mem[0];
// //	initialize_gpio_lpc( &toolkit->_.hub_power_line_gpios_mem[1], 0, 26, GPIO_BARE );
// //	hub_power_line_gpios[1] = (gpio_t*) &toolkit->_.hub_power_line_gpios_mem[1];

// 	/* Initialize hub with the GPIOs. */
// 	initialize_hub_local( &toolkit->_.hub, NULL, NULL );
// 	((driver_toolkit_t*) toolkit)->hub = (hub_t*) &toolkit->_.hub;

// 	/* DFGM. */
// 	initialize_dfgm_local( &toolkit->_.dfgm );
// 	((driver_toolkit_t*) toolkit)->dfgm = (dfgm_t*) &toolkit->_.dfgm;

// 	/* MNLP. */
// 	initialize_mnlp_local( &toolkit->_.mnlp );
// 	((driver_toolkit_t*) toolkit)->mnlp = (mnlp_t*) &toolkit->_.mnlp;

// 	/* Teledyne. */
// 	initialize_teledyne_local( &toolkit->_.teledyne );
// 	((driver_toolkit_t*) toolkit)->teledyne = (teledyne_t*) &toolkit->_.teledyne;

// 	return super_err;
// }
