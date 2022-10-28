#include <stdint.h>

#define MCU_TEMP_SENSOR_ADDR 0x4A
#define CONVERTER_TEMP_SENSOR_ADDR 0x49

int tmp117_read(uint8_t sadd, int16_t *val);

int tmp421_init(uint8_t sadd);
