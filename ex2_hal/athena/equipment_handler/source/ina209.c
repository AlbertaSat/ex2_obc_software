/*
 *  INA209.c
 *  Drivers for INA209 Current Sense
 *  See Datasheet for possible register values
 *  https://www.ti.com/lit/ds/symlink/ina209.pdf
 *
 *  INA209 registers are Most Significant Byte first - LSB PEEPS BEWARE
 *  (MSB peeps just toss the _flip_byte_order() function)
 *  Created on: May 10, 2022
 *  Author: Liam Droog
 */

#include <ina209.h>
#include <i2c_io.h>
#include <FreeRTOS.h>
#include <os_task.h>

uint16_t CALI_REG = 0xDA73;
uint16_t POWER_OLREG = 0x0100; // adapt me
uint16_t ZEROREG = 0x0000;
uint16_t FFREG = 0xFFFF;

int ina209_Write1ByteReg(uint8_t addr, uint8_t reg_addr, uint8_t data) {
    uint8_t buf[2];
    buf[0] = reg_addr;
    buf[1] = data;
    return i2c_Send(i2cREG2, addr, 2, &buf);
}

int ina209_Write2ByteReg(uint8_t addr, uint8_t reg_addr, uint16_t data) {
    uint8_t buf[3];
    buf[0] = reg_addr;
    buf[1] = data & 0x00FF;
    buf[2] = data & 0xFF00;
    return i2c_Send(i2cREG2, addr, 3, &buf);
}

int ina209_Read1ByteReg(uint8_t addr, uint8_t reg_addr, uint8_t *val) {
    // TODO: make this use error code return instead
    uint8_t value = 0;

    i2c_Send(i2cREG2, addr, 1, &reg_addr);

    i2c_Receive(i2cREG2, addr, 1, &value);

    return value;
}

int ina209_Read2ByteReg(uint8_t addr, uint8_t reg_addr, uint16_t *val) {
    // TODO: make this use error code return instead
    uint8_t data[2] = {0};
    uint16_t value = 0;

    if (i2c_Send(i2cREG2, addr, 1, &reg_addr) == -1) {
        return -1;
    }

    if (i2c_Receive(i2cREG2, addr, 2, &data) == -1) {
        return -1;
    }

    *val = (((uint16_t)(data[0])) << 8) | data[1];

    return 0;
}

void ina209_get_configuration(uint8_t addr, uint16_t *retval) {
    // POR is x399F
    ina209_Read2ByteReg(addr, 0x00, retval);
    return;
}

void ina209_set_configuration(uint8_t addr, uint16_t *val) {
    ina209_Write2ByteReg(addr, 0x00, *val);
    return;
}

void ina209_get_status_flags(uint8_t addr, uint16_t *retval) {
    //  POR is x0000
    ina209_Read2ByteReg(addr, 0x01, retval);
    return;
}

// probably not needed; designs have alert pin grounded
void ina209_get_control_register(uint8_t addr, uint16_t *retval) {
    ina209_Read2ByteReg(addr, 0x02, retval);
    return;
}

void ina209_set_control_register(uint8_t addr, uint16_t *val) { ina209_Write2ByteReg(addr, 0x02, *val); }

void ina209_get_shunt_voltage(uint8_t addr, uint16_t *retval) {
    ina209_Read2ByteReg(addr, 0x03, retval);
    return;
}

void ina209_get_bus_voltage(uint8_t addr, uint16_t *retval) {
    ina209_Read2ByteReg(addr, 0x04, retval);
    return;
}

void ina209_get_power(uint8_t addr, uint16_t *retval) {
    ina209_Read2ByteReg(addr, 0x05, retval);
    return;
}

// Current defaults to 0 on POR before calibration register (x16) is ina209_set
void ina209_get_current(uint8_t addr, uint16_t *retval) {
    ina209_Read2ByteReg(addr, 0x06, retval);
    return;
}

void ina209_get_shunt_voltage_peak_pos(uint8_t addr, uint16_t *retval) {
    ina209_Read2ByteReg(addr, 0x07, retval);
    return;
}

void ina209_get_shunt_voltage_peak_neg(uint8_t addr, uint16_t *retval) {
    ina209_Read2ByteReg(addr, 0x08, retval);
    return;
}

void ina209_get_bus_voltage_peak_max(uint8_t addr, uint16_t *retval) {
    ina209_Read2ByteReg(addr, 0x09, retval);
    return;
}

void ina209_get_bus_voltage_peak_min(uint8_t addr, uint16_t *retval) {
    ina209_Read2ByteReg(addr, 0x0A, retval);
    return;
}

void ina209_get_power_peak(uint8_t addr, uint16_t *retval) {
    ina209_Read2ByteReg(addr, 0x0B, retval);
    return;
}

void ina209_get_power_overlimit(uint8_t addr, uint16_t *retval) {
    ina209_Read2ByteReg(addr, 0x11, retval);
    return;
}

void ina209_set_power_overlimit(uint8_t addr, uint16_t *val) {
    ina209_Write2ByteReg(addr, 0x11, *val);
    return;
}

void ina209_get_bus_voltage_overlimit(uint8_t addr, uint16_t *retval) {
    ina209_Read2ByteReg(addr, 0x12, retval);
    return;
}

void ina209_set_bus_voltage_overlimit(uint8_t addr, uint16_t *val) {
    ina209_Write2ByteReg(addr, 0x12, *val);
    return;
}

void ina209_get_bus_voltage_underlimit(uint8_t addr, uint16_t *retval) {
    ina209_Read2ByteReg(addr, 0x13, retval);
    return;
}

void ina209_set_bus_voltage_underlimit(uint8_t addr, uint16_t *val) {
    ina209_Write2ByteReg(addr, 0x13, *val);
    return;
}

void ina209_get_calibration(uint8_t addr, uint16_t *retval) {
    ina209_Read2ByteReg(addr, 0x16, retval);
    return;
}

void ina209_set_calibration(uint8_t addr, uint16_t *val) {
    ina209_Write2ByteReg(addr, 0x16, *val);
    return;
}

void _flip_byte_order(uint16_t *input) {
    // Data is transmitted MSB first, but STM is LSB.
    // This flips the byte order.
    uint8_t rtn = 0x0000;
    uint8_t lsb = *input >> 8;
    uint8_t msb = *input & 0x00FF;
    rtn = msb << 8 | lsb;
    *input = rtn;
    return;
}

void init_ina209(uint8_t addr) {
    // clear POR flags
    uint16_t retval;
    for (uint8_t i = 0; i < 5; i++) {
        ina209_get_status_flags(addr, &retval);
    }
    // ina209_set calibration register
    ina209_set_calibration(addr, &CALI_REG);
    // ina209_set power overlimit
    ina209_set_power_overlimit(addr, &POWER_OLREG);
    // ina209_set bit masks
    ina209_set_control_register(addr, &ZEROREG);
    vTaskDelay(2);
    ina209_set_control_register(addr, &FFREG);
    return;
}

void reset_ina209(uint8_t addr) {
    ina209_set_control_register(addr, &ZEROREG);
    vTaskDelay(2);
    ina209_set_control_register(addr, &FFREG);
}

int test_currentsense(uint8_t addr) {
    int rtn = 0;
    printf("Initializing INA209\r\n");
    init_ina209(SOLAR_INA209_ADDR);
    vTaskDelay(500);
    uint16_t current, current_reg, shunt_voltage;
    // current = (1/4096) * (shunt voltage * calibration register)
    for (int i = 0; i < 30; i++) {
        // Read current from register
        ina209_get_current(SOLAR_INA209_ADDR, &current_reg);
        ina209_get_shunt_voltage(SOLAR_INA209_ADDR, &shunt_voltage);
        current = (current_reg * shunt_voltage) / 4096;
        printf("Current Register:\t0x%.4x\r\n", current_reg);
        printf("Shunt Voltage Register:\t0x%.4x\r\n", shunt_voltage);
        printf("Calculated Current: 0x%.4x -> %d\r\n\n", current, current);
        vTaskDelay(500);
    }
    printf("Current Test Complete\r\n");
    return rtn;
}
