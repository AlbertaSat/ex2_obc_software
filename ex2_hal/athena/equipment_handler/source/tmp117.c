#include "tmp117.h"
#include "HL_i2c.h"
#include "i2c_io.h"
#include "stdio.h"

int tmp117_Read2ByteReg(uint8_t addr, uint8_t reg_addr, uint16_t *val) {
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

// returns temp in deg C
int tmp117_read(uint8_t sadd, int16_t *val) {
    uint16_t regval = 0;
    tmp117_Read2ByteReg(sadd, 0, &regval);
    uint8_t negative = (regval >> 15) & 1;
    float scaling_const = 0.0078125;
    if (negative == 1) {
        *val = (int16_t)(scaling_const * (float)((~regval) & 0b0111111111111111));
    } else {
        *val = (int16_t)(scaling_const * (float)regval);
    }
    return 0;
}

int tmp117_init(uint8_t sadd) {
    // shouldn't need to configure this device
    return 1;
}
