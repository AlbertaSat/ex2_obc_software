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
 * @file    deployablecontrol.c
 * @author  Josh Lazaruk
 * @date    2020-08-13
 */

#include <FreeRTOS.h>
#include <os_task.h>
#include "HL_gio.h"
#include "HL_het.h"
#include "deployablescontrol.h"
#include "eps.h"

int activate(Deployable_t knife, uint16_t *burnwire_current) {
    switch (knife) {
    case Port:{
        // Activate burnwire
        gioSetBit(hetPORT1, 8, 1);

        // Wait
        vTaskDelay(DEPLOYABLE_BURNWIRE_DELAY_MS);

        // Read the current through the burnwire
        eps_refresh_instantaneous_telemetry();
        eps_instantaneous_telemetry_t eps = get_eps_instantaneous_telemetry();

        // Deactivate burnwire
        gioSetBit(hetPORT1, 8, 0);

        // Store the current through the burnwire
        *burnwire_current = eps.curOutput[DEPLOYABLES_CURRENT_INDEX];
        break;
    }
    case UHF_P:{
        gioSetBit(hetPORT2, 5, 1);
        vTaskDelay(DEPLOYABLE_BURNWIRE_DELAY_MS);
        eps_refresh_instantaneous_telemetry();
        eps_instantaneous_telemetry_t eps = get_eps_instantaneous_telemetry();
        gioSetBit(hetPORT2, 5, 0);

        *burnwire_current = eps.curOutput[DEPLOYABLES_CURRENT_INDEX];
        break;
    }
    case UHF_Z:{
        gioSetBit(hetPORT1, 26, 1);
        vTaskDelay(DEPLOYABLE_BURNWIRE_DELAY_MS);
        eps_refresh_instantaneous_telemetry();
        eps_instantaneous_telemetry_t eps = get_eps_instantaneous_telemetry();
        gioSetBit(hetPORT1, 26, 0);

        *burnwire_current = eps.curOutput[DEPLOYABLES_CURRENT_INDEX];
        break;
    }
    case Payload:{
        gioSetBit(hetPORT1, 14, 1);
        vTaskDelay(DEPLOYABLE_BURNWIRE_DELAY_MS);
        eps_refresh_instantaneous_telemetry();
        eps_instantaneous_telemetry_t eps = get_eps_instantaneous_telemetry();
        gioSetBit(hetPORT1, 14, 0);

        *burnwire_current = eps.curOutput[DEPLOYABLES_CURRENT_INDEX];
        break;
    }
    case UHF_S:{
        gioSetBit(gioPORTA, 0, 1);
        vTaskDelay(DEPLOYABLE_BURNWIRE_DELAY_MS);
        eps_refresh_instantaneous_telemetry();
        eps_instantaneous_telemetry_t eps = get_eps_instantaneous_telemetry();
        gioSetBit(gioPORTA, 0, 0);

        *burnwire_current = eps.curOutput[DEPLOYABLES_CURRENT_INDEX];
        break;
    }
    case UHF_N:{
        gioSetBit(hetPORT1, 22, 1);
        vTaskDelay(DEPLOYABLE_BURNWIRE_DELAY_MS);
        eps_refresh_instantaneous_telemetry();
        eps_instantaneous_telemetry_t eps = get_eps_instantaneous_telemetry();
        gioSetBit(hetPORT1, 22, 0);

        *burnwire_current = eps.curOutput[DEPLOYABLES_CURRENT_INDEX];
        break;
    }
    case Starboard:{
        gioSetBit(hetPORT2, 1, 1);
        vTaskDelay(DEPLOYABLE_BURNWIRE_DELAY_MS);
        eps_refresh_instantaneous_telemetry();
        eps_instantaneous_telemetry_t eps = get_eps_instantaneous_telemetry();
        gioSetBit(hetPORT2, 1, 0);

        *burnwire_current = eps.curOutput[DEPLOYABLES_CURRENT_INDEX];
        break;
    }
    case DFGM:{
        gioSetBit(hetPORT1, 16, 1);
        vTaskDelay(DEPLOYABLE_BURNWIRE_DELAY_MS);
        eps_refresh_instantaneous_telemetry();
        eps_instantaneous_telemetry_t eps = get_eps_instantaneous_telemetry();
        gioSetBit(hetPORT1, 16, 0);

        *burnwire_current = eps.curOutput[DEPLOYABLES_CURRENT_INDEX];
        break;
    }
    default:{
        return -1;
    }
    }
    return 0;
}

bool switchstatus(Deployable_t sw) {
    switch (sw) {
    case Port:
        return gioGetBit(hetPORT2, 4);
    case UHF_P:
        return gioGetBit(hetPORT1, 10);
    case UHF_Z:
        return gioGetBit(hetPORT1, 30);
    case Payload:
        return gioGetBit(hetPORT1, 4);
    case UHF_S:
        return gioGetBit(hetPORT2, 2);
    case UHF_N:
        return gioGetBit(hetPORT1, 12);
    case Starboard:
        return gioGetBit(gioPORTA, 4);
    case DFGM:
        return gioGetBit(hetPORT1, 29);
    default:
        return 1; // check this
    }
}

// This should return 1, assuming switches are not connected to ground when undepressed
bool deploy(Deployable_t deployable, uint16_t *burnwire_current) {
    activate(deployable, burnwire_current);
    return switchstatus(deployable);
}
