#include "ads7128.h"
#include "pcal9538a.h"
#include <stdbool.h>
#include <stdio.h>

bool charon_binary_test(void){

    int temperatures[8] = {0};
    uint8_t threshold = 40;
    uint8_t max_measured_temp = 0;
    printf("Raise temperature of one sensor over 40 deg C to exit loop.\n");
    printf("Thermistor temperatures: \n");
    while(max_measured_temp < threshold){
        if(readAllTemps(temperatures)){
            return 1;
        }
        for(int i = 0; i<8; i++){
            printf("Channel 0 = %d deg C \n", temperatures[i]);
            if(temperatures[i] > max_measured_temp){
                max_measured_temp = temperatures[i];
            }
        }
        printf("\n");
        for(int j = 0; j<100000; j++);//not using freeRTOS delay for basic testing
    }

    printf("Resetting ADCS power channel in ~10 seconds... \n");
    for(int j = 0; j<100000; j++);//not using freeRTOS delay for basic testing

    if(setuppcal9538a()){
        return 1;
    }

    Power_Channel channel_to_reset = ADCS;

    for(int i = 0; i<6; i++){
        printf("Resetting power channel %d for 2.2 s \n", channel_to_reset);
        if(triggerPowerChannelReset(channel_to_reset)){
            return 1;
        }
        printf("Delaying for ~10 seconds... \n");
        for(int j = 0; j<100000; j++);//not using freeRTOS delay for basic testing
        channel_to_reset = (Power_Channel)i;
    }

    return 0;
}
