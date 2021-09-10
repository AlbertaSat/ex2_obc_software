#define ADS7128_ADDR              (0x32)//TODO
#ifdef IS_ATHENA
#define ADS7128_PORT              i2cREG2
#else
#define ADS7128_PORT              i2cREG1 // port used on dev board
#endif
