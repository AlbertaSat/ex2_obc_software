
#include "tmp421.h"
#include "HL_i2c.h"
#include "i2c_io.h"
#include "stdio.h"

//returns temp in deg C
int tmp117_read(uint8_t sadd, float *val){
    uint16_t regval = i2cSlaveRead2ByteReg(sadd, 0);
    uint8_t negative = (regval >> 15) & 1;
    float scaling_const = 0.0078125;
    if(negative == 1){
        *val = scaling_const*(float)((~regval) & 0b0111111111111111);
    } else {
        *val = scaling_const*(float)regval;
    }
    return 0;
}

int tmp117_init(uint8_t sadd){
    //shouldn't need to configure this device
}
