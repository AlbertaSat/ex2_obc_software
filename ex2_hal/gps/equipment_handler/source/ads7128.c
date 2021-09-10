

//1 LSB = 3.3V / 2^12 = 0.806 mV
//No calibration, averaging filter, currently implemented

//01M1002JF 10 kOhm thermistors in series w/ 4.7 kOhm: voltage via https://www.vishay.com/thermistors/ntc-rt-calculator/


#include "i2c_io.h"

//voltage in mV
int8_t voltageToTemperature(uint8_t voltage){//based off of https://embeddedgurus.com/stack-overflow/2010/01/a-tutorial-on-lookup-tables-in-c/

    //convert mV to index used in LUT

    static const int8_t lookup[128] = {//not including directive to store in non-RAM

    };

    int8_t temperature = lookup[voltage];
    return temperature;
}

void readSingleTemp(uint8_t channel){
    int i2c_Receive(ADS7128_PORT, ADS7128_ADDR, uint16_t size, void *buf)
    //voltageToTemperature
}

void readAllTemps(void){
    int i2c_Receive(i2cBASE_t *i2c, uint8_t addr, uint16_t size, void *buf)
    //voltageToTemperature
}
