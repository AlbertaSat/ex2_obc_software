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
 * @author Andrew R. Rooney, Arash Yazdani
 * @date Mar. 6, 2021
 */

#ifndef EX2_SYSTEM_INCLUDE_SYSTEM_STATE_STATE_TASK_H_
#define EX2_SYSTEM_INCLUDE_SYSTEM_STATE_STATE_TASK_H_

#include <FreeRTOS.h>
#include <stdbool.h>

#include "eps.h"
#include "uhf.h"
#include "main/system.h"


typedef enum {
  hw_critical_state = 0,  // EPS will shut OBC down prior to informing it
  critical_state = 1,     // EPS will shut OBC down prior to informing it
  safe_state = 2,
  operational_state = 3
} sat_state_e;

/* It might be a good idea to replace it with SAT_returnState. */
typedef enum { SYS_OFF = 0, SYS_ON = 1, SYS_NO_RESPONSE = -1 } sys_returnstate_e;

typedef struct {
  bool heater;
  bool eps;
  bool obc;
  bool uhf;
  bool stx;
  bool iris;
  bool dfgm;
  bool adcs;
} systems_status_t;

SAT_returnState start_state_daemon();
void stop_state_daemon();
sat_state_e eps2sat_mode_cnv(eps_mode_e batt_mode);

void change_systems_status(systems_status_t subsystem_target_state);
UHF_return power_switch_uhf(const bool uhf_status);
sys_returnstate_e power_switch_sys(const uint8_t channel, const bool target_state);

#endif /* EX2_SYSTEM_INCLUDE_SYSTEM_STATE_STATE_TASK_H_ */
