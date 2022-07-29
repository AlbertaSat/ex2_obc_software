/*
 * ina209.h
 *
 *  Created on: Jun 23, 2022
 *      Author: joshd
 */

#ifndef EX2_HAL_ATHENA_EQUIPMENT_HANDLER_INCLUDE_INA209_H_
#define EX2_HAL_ATHENA_EQUIPMENT_HANDLER_INCLUDE_INA209_H_

#include <stdint.h>
#include <FreeRTOS.h>
#include <os_task.h>

#define SOLAR_INA209_ADDR 0x40
#define CORE_V_INA209_ADDR 0x45

#define SOLAR_INA209_STACK_LEN 1000 // arbitrary for now

int ina209_get_configuration(uint8_t addr, uint16_t *retval);
int ina209_set_configuration(uint8_t addr, uint16_t *val);
int ina209_get_status_flags(uint8_t addr, uint16_t *retval);
int ina209_get_control_register(uint8_t addr, uint16_t *retval);
int ina209_set_control_register(uint8_t addr, uint16_t *val);
int ina209_get_shunt_voltage(uint8_t addr, uint16_t *retval);
int ina209_get_bus_voltage(uint8_t addr, uint16_t *retval);
int ina209_get_power(uint8_t addr, uint16_t *retval);
int ina209_get_current(uint8_t addr, uint16_t *retval);
int ina209_get_shunt_voltage_peak_pos(uint8_t addr, uint16_t *retval);
int ina209_get_shunt_voltage_peak_neg(uint8_t addr, uint16_t *retval);
int ina209_get_bus_voltage_peak_max(uint8_t addr, uint16_t *retval);
int ina209_get_bus_voltage_peak_min(uint8_t addr, uint16_t *retval);
int ina209_get_power_peak(uint8_t addr, uint16_t *retval);
int ina209_get_power_overlimit(uint8_t addr, uint16_t *retval);
int ina209_set_power_overlimit(uint8_t addr, uint16_t *val);
int ina209_get_bus_voltage_overlimit(uint8_t addr, uint16_t *retval);
int ina209_set_bus_voltage_overlimit(uint8_t addr, uint16_t *val);
int ina209_get_bus_voltage_underlimit(uint8_t addr, uint16_t *retval);
int ina209_set_bus_voltage_underlimit(uint8_t addr, uint16_t *val);
int ina209_get_calibration(uint8_t addr, uint16_t *retval);
int ina209_set_calibration(uint8_t addr, uint16_t *val);
int init_ina209(uint8_t addr);
int reset_ina209(uint8_t addr);

#endif /* EX2_HAL_ATHENA_EQUIPMENT_HANDLER_INCLUDE_INA209_H_ */
