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
/**
 * @file housekeeping_service.c
 * @author Haoran Qi, Andrew Rooney, Yuan Wang, Dustin Wagner
 * @date 2020-07-07
 */

#ifndef HOUSEKEEPING_SERVICE_H
#define HOUSEKEEPING_SERVICE_H

#include <csp/csp.h>

#include "services.h"

/*includes to allow colleting hk from other devices*/
#include "housekeeping_athena.h"
#include "eps.h"
#include "uhf.h"
#include "sband.h"
//TODO:
//#include "adcs.h"

/* Housekeeping service address & port*/

#define HK_PARAMETERS_REPORT 25
//#define TM_HK_PARAMETERS_REPORT 21

#define HK_PR_ERR -1
#define HK_PR_OK 0

typedef enum { TM_HK_PARAMETERS_REPORT = 0 } Housekeeping_Subtype;






typedef enum {
  FILE_EXISTS = 1,
  FILE_NOT_EXIST = 0
} Found_file;

typedef enum {
  SUCCESS = 0,
  FAILURE = 1
} Result;

typedef enum {
  GET_HK = 0,
  SET_MAX_FILES = 1,
  GET_MAX_FILES = 2
} subservice;

/*hk data sample*/
typedef enum { EPS, ADCS, OBC, COMMS } hardware;

/*--------------hk data----------------*/

typedef struct __attribute__((packed)){
  /*placeholder timestamp structure. Not sure if we use UNIX time*/
  uint32_t UNIXtimestamp;              //Note when this data was collected
  uint16_t dataPosition;                  //Use to place datasets in chronological order
} hk_time_and_order;

typedef struct {
  hk_time_and_order hk_timeorder;        //debugging time and file order

  //TODO:
  //adcs_housekeeping ADCS_hk;           //ADCS housekeeping struct
  athena_housekeeping Athena_hk;         //Athena housekeeping struct
  eps_instantaneous_telemetry_t EPS_hk;  //EPS telemetry struct
  UHF_housekeeping UHF_hk;                     //UHF status struct
  Sband_Housekeeping S_band_hk;          //S-band housekeeping struct
} All_systems_housekeeping;

SAT_returnState start_housekeeping_service(void);

/*This function called every interval to collect data periodically*/
Result populate_and_store_hk_data(void);


Result load_historic_hk_data(uint16_t file_num, All_systems_housekeeping* all_hk_data);
Result set_max_files(uint16_t new_max);



#endif /* HOUSEKEEPING_SERVICE_H */
