/*
 * Copyright (C) 2020  University of Alberta
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
 * @file state_task.h
 * @author Andrew R. Rooney
 * @date Mar. 6, 2021
 */

#ifndef EX2_SYSTEM_INCLUDE_SYSTEM_STATE_STATE_TASK_H_
#define EX2_SYSTEM_INCLUDE_SYSTEM_STATE_STATE_TASK_H_

#include <stdbool.h>

#include "eps.h"
#include "main/system.h"

enum SAT_state {
  hw_critical_state = 0,  // EPS will shut OBC down prior to informing it
  critical_state = 1,     // EPS will shut OBC down prior to informing it
  safe_state = 2,
  operational_state = 3
};

/* It might be a good idea to replace it with SAT_returnState. */
typedef enum { SYS_OFF = 0, SYS_ON = 1, SYS_NO_RESPONSE = -1 } SYS_returnState;

typedef struct __attribute__((packed)) {
  bool batHeater;
  bool EPS;
  bool OBC;
  bool UHF;
  bool STX;  // S-band Transmitter
  bool Iris;
  bool DFGM;
  bool ADCS;
} systems_status;

typedef enum SAT_state SAT_state_e;

SAT_returnState start_state_daemon();
SAT_state_e eps2sat_mode_cnv(eps_mode_e batt_mode);

void change_systems_status(uint8_t ctrl_word);
SYS_returnState power_switch_uhf(bool *uhf_status);
SYS_returnState power_switch_stx(bool *stx_status);
SYS_returnState power_switch_iris(bool *iris_status);
SYS_returnState power_switch_dfgm(bool *dfgm_status);
SYS_returnState power_switch_adcs(bool *adcs_status);

#endif /* EX2_SYSTEM_INCLUDE_SYSTEM_STATE_STATE_TASK_H_ */
