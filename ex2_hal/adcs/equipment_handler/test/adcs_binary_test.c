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

// Performs all tests used in TX2-AC-025 Ver 1.00 ADCS Binary Testing Plan with the OBC

// Make sure to define either "USE_UART" or "USE_I2C" in adcs_handler.c depending on which interface is being tested

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <redconf.h>
#include <redfs.h>
#include <redfse.h>
#include <redposix.h>
#include <redtests.h>
#include <redvolume.h>

#include "adcs_handler.h"
#include "adcs_io.h"
#include "FreeRTOS.h"

void CubeMag_Common_Test(bool);
void CubeTorquers_Common_Test(void);
void binaryTest_CubeSense1(void);
void ReactionWheels_Common_Test(uint8_t wheel_number);

void binaryTest(void) {//TODO: add enums for all adcs_handler functions called

//    printf("Running Bootloader Tests");
//    binaryTest_Bootloader();
//    printf("Bootloader Tests Complete!");
//
//    printf("Running CubeACP Tests");
//    binaryTest_CubeACP();
//    printf("CubeACP Tests Complete!");
//
//    printf("CubeSense 1 Tests");
//    binaryTest_CubeSense1();
//    printf("CubeSense 1 Tests Complete!");
//
//    printf("CubeSense 2 Tests");
//    binaryTest_CubeSense2();
//    printf("CubeSense 2 Tests Complete!");
//
//
    printf("CubeControl Tests");

//    printf("CubeControl Signal MCU Tests");
//    binaryTest_CubeControl_Sgn_MCU();
//    printf("CubeControl Signal MCU Tests Complete!");
//
//    printf("CubeMag Signal MCU Tests");
//    binaryTest_CubeMag_Sgn_MCU();
//    printf("CubeMag Signal MCU Tests Complete!");
//
//    printf("CubeTorquers Signal MCU Tests");
//    binaryTest_CubeTorquers_Sgn_MCU();
//    printf("CubeTorquers Signal MCU Tests Complete!");
//
    printf("CubeControl Motor MCU Tests");
    binaryTest_CubeControl_Motor_MCU();
    printf("CubeControl Motor MCU Tests Complete!");
//
//    printf("CubeMag Motor MCU Tests");
//    binaryTest_CubeMag_Motor_MCU();
//    printf("CubeMag Motor MCU Tests Complete!");
//
//    printf("CubeTorquers Motor MCU Tests");
//    binaryTest_CubeTorquers_Motor_MCU();
//    printf("CubeTorquers Motor MCU Tests Complete!");
//
//    printf("CubeControl Tests Complete!");
//
//
//
//    printf("CubeWheel Tests");
//
//    printf("CubeWheel BurnIn Tests");
//    binaryTest_CubeWheel_BurnIn_MCU();
//    printf("CubeWheel BurnIn Tests Complete!");
//
//    printf("CubeWheel 1  Tests");
//    binaryTest_CubeWheel1_MCU();
//    printf("CubeWheel 1 Tests Complete!");
//
//    printf("CubeWheel 2  Tests");
//    binaryTest_CubeWheel2_MCU();
//    printf("CubeWheel 2 Tests Complete!");
//
//    printf("CubeWheel 3  Tests");
//    binaryTest_CubeWheel3_MCU();
//    printf("CubeWheel 3 Tests Complete!");
//
//    printf("CubeWheel  Tests Complete!");


    //TODO: all the rest of test plan, including pause points for manual work done in test plan (including before this point)
    //TODO: add checks for "incrementing" and "idle" type values, since those are only checked once instantaneously now

}



void binaryTest_Bootloader(void){
    // Test corresponds to Section 4.1 Bootloader, Table 4-1 in test plan.
    // Test must be executed within 5 seconds of turning on ADCS
    ADCS_returnState test_returnState = ADCS_OK;
    uint16_t uptime = 0;
    uint8_t flags_arr[12];

    printf("Running ADCS_get_bootloader_state...\n");
    test_returnState = ADCS_get_bootloader_state(&uptime, flags_arr);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_bootloader_state returned %d \n", test_returnState);
        while(1);
    }

    printf("uptime = %d \n", uptime);
    printf("flags_arr:\n");
    printf("sram1 = %d \n", flags_arr[0]);
    printf("sram2 = %d \n", flags_arr[1]);
    printf("sram_latch_not_recovered = %d \n", flags_arr[2]);
    printf("sram_latch_recovered = %d \n", flags_arr[3]);
    printf("sd_initial_err = %d \n", flags_arr[4]);
    printf("sd_read_err = %d \n", flags_arr[5]);
    printf("sd_write_err = %d \n", flags_arr[6]);
    printf("external_flash_err = %d \n", flags_arr[7]);
    printf("internal_flash_err = %d \n", flags_arr[8]);
    printf("eeprom_err = %d \n", flags_arr[9]);
    printf("bad_boot_reg = %d \n", flags_arr[10]);
    printf("comms_radio_err = %d \n\n", flags_arr[11]);

    //Continuing test Section 4.1 Bootloader, Table 4-1 in test plan.
    uint8_t mcu_reset_cause = 0;
    uint8_t boot_cause = 0;
    uint16_t boot_count = 0;
    uint8_t boot_idx = 0;
    uint8_t major_firm_ver = 0;
    uint8_t minor_firm_ver = 0;

    printf("Running ADCS_get_boot_program_stat...\n");
    test_returnState = ADCS_get_boot_program_stat(&mcu_reset_cause, &boot_cause, &boot_count, &boot_idx, &major_firm_ver, &minor_firm_ver);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_boot_program_stat returned %d \n", test_returnState);
        while(1);
    }

    printf("mcu_reset_cause = %d \n", mcu_reset_cause);
    printf("boot_cause = %d \n", boot_cause);
    printf("boot_count = %d \n", boot_count);
    printf("boot_idx = %d \n\n", boot_idx);
    printf("major_firm_ver = %d \n", major_firm_ver);
    printf("minor_firm_ver = %d \n\n", minor_firm_ver);

    //Continuing test Section 4.1 Bootloader, Table 4-1 in test plan.
    uint8_t program_idx = 0;
    uint8_t boot_stat = 0;

    printf("Running ADCS_get_boot_index...\n");
    test_returnState = ADCS_get_boot_index(&program_idx, &boot_stat);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_boot_index returned %d \n", test_returnState);
        while(1);
    }

    printf("program_idx = %d \n", program_idx);
    printf("boot_stat = %d \n", boot_stat);

    //Continuing test Section 4.1 Bootloader, Table 4-1 in test plan.
    uint16_t TC_num = 0;
    uint16_t TM_num = 0;
    memset(&flags_arr, 0 , 12);

    printf("Running ADCS_get_comms_stat...\n");
    test_returnState = ADCS_get_comms_stat(&TC_num, &TM_num, flags_arr); // TODO: fix this
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_comms_stat returned %d \n", test_returnState);
        while(1);
    }

    printf("TC_num = %d \n", TC_num);
    printf("TM_num = %d \n", TM_num);
    printf("flags: %d %d %d %d %d %d", flags_arr[0],flags_arr[1],flags_arr[2],flags_arr[3],flags_arr[4],flags_arr[5]);

    printf("\n");
}

void binaryTest_CubeACP(void){

    //Test Section 4.2 CubeACP, Table 4-2 in test plan.
    uint16_t time = 0;
    uint8_t execution_point = 0;
    ADCS_returnState test_returnState = ADCS_OK;

    printf("ADCS_get_ACP_loop_stat...\n");
    test_returnState = ADCS_get_ACP_loop_stat(&time, &execution_point);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_comms_stat returned %d \n", test_returnState);
        while(1);
    }

    printf("time = %d \n", time);
    printf("execution_point = %d \n", execution_point);

    //Continuing test Section 4.2 CubeACP, Table 4-2 in test plan.
    uint8_t mcu_reset_cause = 0;
    uint8_t boot_cause = 0;
    uint16_t boot_count = 0;
    uint8_t boot_idx = 0;
    uint8_t major_firm_ver = 0;
    uint8_t minor_firm_ver = 0;

    printf("Running ADCS_get_boot_program_stat...\n");
    test_returnState = ADCS_get_boot_program_stat(&mcu_reset_cause, &boot_cause, &boot_count, &boot_idx, &major_firm_ver, &minor_firm_ver);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_boot_program_stat returned %d \n", test_returnState);
        while(1);
    }

    printf("mcu_reset_cause = %d \n", mcu_reset_cause);
    printf("boot_cause = %d \n", boot_cause);
    printf("boot_count = %d \n", boot_count);
    printf("boot_idx = %d \n\n", boot_idx);
    printf("major_firm_ver = %d \n", major_firm_ver);
    printf("minor_firm_ver = %d \n\n", minor_firm_ver);

    //Continuing test Section 4.2 CubeACP, Table 4-2 in test plan.
    uint8_t node_type = 0;
    uint8_t interface_ver = 0;
    major_firm_ver = 0;
    minor_firm_ver = 0;
    uint16_t runtime_s = 0;
    uint16_t runtime_ms = 0;

    printf("Running ADCS_get_node_identification...\n");
    test_returnState = ADCS_get_node_identification(&node_type, &interface_ver, &major_firm_ver, &minor_firm_ver, &runtime_s, &runtime_ms);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_node_identification returned %d \n", test_returnState);
        while(1);
    }

    printf("node_type = %d \n", node_type);
    printf("interface_ver = %d \n", interface_ver);
    printf("runtime_s = %d \n", runtime_s);
    printf("runtime_ms = %d \n\n", runtime_ms);

    //Continuing test Section 4.2 CubeACP, Table 4-2 in test plan.
    uint8_t program_idx = 0;
    uint8_t boot_stat = 0;

    printf("Running ADCS_get_boot_index...\n");
    test_returnState = ADCS_get_boot_index(&program_idx, &boot_stat);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_boot_index returned %d \n", test_returnState);
        while(1);
    }

    printf("program_idx = %d \n", program_idx);
    printf("boot_stat = %d \n", boot_stat);

    //Continuing test Section 4.2 CubeACP, Table 4-2 in test plan.
    uint16_t TC_num = 0;
    uint16_t TM_num = 0;
    uint8_t flags_arr = 0;

    printf("Running ADCS_get_comms_stat...\n");
    test_returnState = ADCS_get_comms_stat(&TC_num, &TM_num, &flags_arr);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_comms_stat returned %d \n", test_returnState);
        while(1);
    }

    printf("TC_num = %d \n", TC_num);
    printf("TM_num = %d \n", TM_num);
    printf("flags_arr (decimal form, not binary) = %d \n", flags_arr);

    //Continuing test Section 4.2 CubeACP, Table 4-2 in test plan.
    uint32_t unix_t = 0;
    uint16_t count_ms = 0;

    printf("Running ADCS_get_unix_t...\n");
    test_returnState = ADCS_get_unix_t(&unix_t, &count_ms);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_unix_t returned %d \n", test_returnState);
        while(1);
    }
    printf("unix_t = %d \n", unix_t);
    printf("count_ms = %d \n", count_ms);



    //Continuing test Section 4.2 CubeACP, Table 4-2 in test plan.
    uint16_t sram1 = 0;
    uint16_t sram2 = 0;

    printf("Running ADCS_get_SRAM_latchup_count...\n");
    test_returnState = ADCS_get_SRAM_latchup_count(&sram1, &sram2);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_SRAM_latchup_count %d \n", test_returnState);
        while(1);
    }

    printf("sram1 = %d \n", sram1);
    printf("sram2 = %d \n", sram2);

    //Continuing test Section 4.2 CubeACP, Table 4-2 in test plan.
    uint16_t single_sram = 0;
    uint16_t double_sram = 0;
    uint16_t multi_sram = 0;

    printf("Running ADCS_get_EDAC_err_count...\n");
    test_returnState = ADCS_get_EDAC_err_count(&single_sram, &double_sram,  &multi_sram);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_EDAC_err_count %d \n", test_returnState);
        while(1);
    }

    printf("single_sram = %d \n", single_sram);
    printf("double_sram = %d \n", double_sram);
    printf("multi_sram = %d \n", multi_sram);

    //Test Section 4.2 CubeACP, Table 4-3 in test plan.
    uint8_t *control = (uint8_t*)pvPortMalloc(10);
    if (control == NULL) {
        return ADCS_MALLOC_FAILED;
    }

    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while(1);
    }
    for(int i = 0; i<10; i++){
        printf("control[%d] = %d \n", i, control[i]);
    }

    vPortFree(control);

    //Test Section 4.2 CubeACP, Table 4-3 in test plan.
    adcs_state test_adcs_state;//init as 0?

    printf("Running ADCS_get_current_state...\n");
    test_returnState = ADCS_get_current_state(&test_adcs_state);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_current_state returned %d \n", test_returnState);
        while(1);
    }

    printf("att_estimate mode = %d \n", test_adcs_state.att_estimate_mode);
    printf("att_ctrl_mode = %d \n", test_adcs_state.att_ctrl_mode);
    printf("run_mode = %d \n", test_adcs_state.run_mode);

    //need to test if all other flags == 0. Simpler to do in code than via human.
    uint8_t all_other_adcs_states_equal_zero = 0;
    for(int i = 0; i<36; i++){//I think this is the right range.
        if(test_adcs_state.flags_arr[i] != 0){
            break;
        }
        if(i == 35){
            all_other_adcs_states_equal_zero = 1;
        }
    }
    if(all_other_adcs_states_equal_zero == 1){
        printf("all other states (frame offsets 12 to 47) == 0 \n");
    } else {
        printf("all other states (frame offsets 12 to 47) != 0... halting code execution\n");
        while(1);
    }

    //Enable ADCS
    printf("Running ADCS_set_enabled_state to set 1Hz loop (enabled)...\n");
    test_returnState = ADCS_set_enabled_state(1);
    if(test_returnState != ADCS_OK){
        printf("ADCS_set_enabled_state returned %d \n", test_returnState);
        while(1);
    }

    //Test Section 4.2 CubeACP, Table 4-4 in test plan.
    TC_num = 0;
    TM_num = 0;
    flags_arr = 0;

    printf("Running ADCS_get_comms_stat...\n");
    test_returnState = ADCS_get_comms_stat(&TC_num, &TM_num, &flags_arr);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_comms_stat returned %d \n", test_returnState);
        while(1);
    }

    printf("TC_num = %d \n", TC_num);

    //Test Section 4.2 CubeACP, Table 4-4 in test plan.
    uint16_t adcs_update = 0;
    uint16_t sensor_comms = 0;
    uint16_t sgp4_propag = 0;
    uint16_t igrf_model = 0;

    printf("Running ADCS_get_execution_times...\n");
    test_returnState = ADCS_get_execution_times(&adcs_update, &sensor_comms, &sgp4_propag, &igrf_model);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_execution_times returned %d \n", test_returnState);
        while(1);
    }

    printf("adcs_update = %d \n", adcs_update);
    printf("sensor_comms = %d \n", sensor_comms);
    printf("sgp4_propag = %d \n", sgp4_propag);
    printf("igrf_model = %d \n", igrf_model);

    //need to test if all other flags (with some exceptions) == 0. Simpler to do in code than via human.
    all_other_adcs_states_equal_zero = 0;
    for(int i = 0; i<36; i++){//I think this is the right range.
        if(i == 20){//Magnetometer Range Error
            //skip index
        }
        else if(i == 24){//cam2 Sensor Detection Error
            //skip index
        }
        else if(i == 25){//cam2 Sensor Range Error
            //skip index
        }
        else if(i == 29){//cam2 Sensor Detection Error
            //skip index
        }
        else if(i == 30){//Cam2 Sensor Range Error
            //skip index
        }
        else if(i == 33){//Coarse Sun Sensor Error
            //skip index
        }
        else{
            if(test_adcs_state.flags_arr[i] != 0){
                break;
            }
            if(i == 35){
                all_other_adcs_states_equal_zero = 1;
            }
        }
    }
    if(all_other_adcs_states_equal_zero == 1){
        printf("all other states** (frame offsets 12 to 47) == 0 \n");
    } else {
        printf("all other states** (frame offsets 12 to 47) != 0... halting code execution\n");
        while(1);
    }

    //Test Section 4.2 CubeACP, Table 4-4 in test plan.
    memset(&test_adcs_state, 0, sizeof(adcs_state));

    printf("Running ADCS_get_current_state...\n");
    test_returnState = ADCS_get_current_state(&test_adcs_state);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_current_state returned %d \n", test_returnState);
        while(1);
    }

    printf("att_estimate mode = %d \n", test_adcs_state.att_estimate_mode);
    printf("att_ctrl_mode = %d \n", test_adcs_state.att_ctrl_mode);
    printf("run_mode = %d \n", test_adcs_state.run_mode);
    printf("Sun is Above Local horizon = %d \n", test_adcs_state.flags_arr[11]);


    printf("\n");
}

void binaryTest_CubeSense1(void){


    //Test Section 5.1 CubeSense, Table 5-1 in test plan.
    //Using Command ADCS_get_power_control() - Table 184, ensure that all nodes are selected PowOff before proceeding.
    ADCS_returnState test_returnState = ADCS_OK;
    adcs_state test_adcs_state;//init as 0?
    printf("Running ADCS_get_current_state...\n");
    test_returnState = ADCS_get_current_state(&test_adcs_state);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_current_state returned %d \n", test_returnState);
        while(1);
    }

    //Section Variables
    uint8_t *control = (uint8_t*)pvPortMalloc(10);
    if (control == NULL) {
        return ADCS_MALLOC_FAILED;
    }

    //enable the ADCS
    ADCS_set_enabled_state(1);

    ADCS_set_unix_t(0,0);

    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while(1);
    }
    for(int i = 0; i<10; i++){
        printf("control[%d] = %d \n", i, control[i]);
    }


    //Using Command ADCS_set_power_control() - Table 184, switch on CubeSense1 by selecting PowOn.
    //Section Variables
    if (control == NULL) {
        return ADCS_MALLOC_FAILED;
    }
    control[Set_CubeSense1_Power] = 1;

    printf("Running ADCS_set_power_control...\n");
    test_returnState = ADCS_set_power_control(control);
    if(test_returnState != ADCS_OK){
        printf("ADCS_set_power_control returned %d \n", test_returnState);
        while(1);
    }

    //another read to make sure we are in the right state
    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while(1);
    }
    for(int i = 0; i<10; i++){
        printf("control[%d] = %d \n", i, control[i]);
    }

    vPortFree(control);

    //Ensure that the lens cap is on.


    //Using Command ADCS_set_cubesense_config() - Table 189, ensure that the Cam1 detection threshold is set to 150.
    //Next, ensure that the Cam1 sensor exposure time is set to 35 if Cam1 is a nadir  sensor, or 100 if Cam1 is a Sun sensor. Set the NadirMaxBadEdges to 30 to make  ground testing easier.
    //Section Variables]
    cubesense_config params;

    //Get all the current configuration parameters from the ADCS so that incorrect parameters aren't sent while testing.
    printf("Running ADCS_get_cubesense_config...\n");
    test_returnState = ADCS_get_cubesense_config(&params); //this function should be tested and checked before the command is sent
    if(test_returnState != ADCS_OK){
        printf("ADCS_set_cubesense_config returned %d \n", test_returnState);
        while(1);
    }


    //now set the parameters
    params.cam1_sense.detect_th = 150;
    params.cam1_sense.exposure_t = 100; //assuming Cam1 is a nadir sensor. If a sun sensor, use 100.
    params.nadir_max_bad_edge = 30;
    params.cam1_sense.boresight_x = 512;
    params.cam1_sense.boresight_y = 512;
//
    printf("Running ADCS_set_cubesense_config...\n");

    test_returnState = ADCS_set_cubesense_config(params); //this function should be tested and checked before the command is sent
    if(test_returnState != ADCS_OK){
        printf("ADCS_set_cubesense_config returned %d \n", test_returnState);
        while(1);
    }

    printf("Running ADCS_get_cubesense_config...\n");
    test_returnState = ADCS_get_cubesense_config(&params); //this function should be tested and checked before the command is sent
    if(test_returnState != ADCS_OK){
        printf("ADCS_set_cubesense_config returned %d \n", test_returnState);
        while(1);
    }
    //this is commented out because the dev board reads the wrong error state

    // Verify the following values in Table 5-1:

    printf("Running ADCS_get_current_state...\n");
    test_returnState = ADCS_get_current_state(&test_adcs_state);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_current_state returned %d \n", test_returnState);
        while(1);
    }

    printf("att_estimate mode = %d \n", test_adcs_state.att_estimate_mode);
    printf("att_ctrl_mode = %d \n", test_adcs_state.att_ctrl_mode);
    printf("run_mode = %d \n", test_adcs_state.run_mode);
    printf("CubeSense1 Enabled = %d \n", test_adcs_state.flags_arr[2]);
    printf("Sun is Above Local Horizon = %d \n", test_adcs_state.flags_arr[11]);
    //need to test if all flags other than CubeSense1 Enabled and Sun is Above Local Horizon are == 0. Simpler to do in code than via human.
    uint8_t all_other_adcs_states_equal_zero = 0;
    for(int i = 0; i<36; i++){//I think this is the right range.
        if(((i == 2) | (i == 11)) & (test_adcs_state.flags_arr[i] != 0)){
            break;
        }
        if(i == 35){
            all_other_adcs_states_equal_zero = 1;
        }
    }
    if(all_other_adcs_states_equal_zero == 1){
        printf("all other states (frame offsets 12 to 47) == 0 \n");
    } else {
        printf("all other states (frame offsets 12 to 47) != 0... halting code execution\n");
        while(1);
    }

    //ADCS_get_power_temp()
    adcs_pwr_temp *power_temp_measurements;
    power_temp_measurements = (adcs_pwr_temp *)pvPortMalloc(sizeof(adcs_pwr_temp));
    if (power_temp_measurements == NULL) {
        printf("malloc issues");
        while(1);
    }

    printf("Running ADCS_get_power_temp...\n");
    test_returnState = ADCS_get_power_temp(power_temp_measurements);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_power_temp returned %d \n", test_returnState);
        while(1);
    }

    printf("cubesense1_3v3_I = %f \n", power_temp_measurements->cubesense1_3v3_I);
    printf("cubesense1_camSram_I = %f \n", power_temp_measurements->cubesense1_camSram_I);

    vPortFree(power_temp_measurements);

    //ADCS_get_raw_sensor()
    adcs_raw_sensor *raw_sensor_measurements;
    raw_sensor_measurements = (adcs_raw_sensor *)pvPortMalloc(sizeof(adcs_raw_sensor));
    if (raw_sensor_measurements == NULL) {
        printf("malloc issues");
        while(1);
    }
    printf("Running ADCS_get_raw_sensor...\n");
    test_returnState = ADCS_get_raw_sensor(raw_sensor_measurements);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_raw_sensor returned %d \n", test_returnState);
        while(1);
    }

    printf("cam1.centroid_x = %d \n", raw_sensor_measurements->cam1.centroid_x);
    printf("cam1.centroid_y = %d \n", raw_sensor_measurements->cam1.centroid_y);
    printf("cam1.capture_stat = %d \n", raw_sensor_measurements->cam1.capture_stat);
    printf("cam1.detect_result = %d \n", raw_sensor_measurements->cam1.detect_result);

    //Take off the Cam1 camera�s lens cap.
    //Verify the following in Table 5-2 by testing the sensor with a light source (a dark n environment will prevent  false detections).
    //If Cam1 is a nadir sensor then a large light source should be used  (e.g. a desk lamp), or if Cam1 is a Sun sensor then a small light
    //source should be used  (e.g. narrow beam flashlight). Vary the distance between the light source and the sensor  until consistent measurements
    //are observed (normally �150mm). If difficulties are  experienced with the nadir sensor, the light source can be covered with white  paper/cloth
    //to create a more uniform light source. Finally, if no results are obtained for  the nadir or Sun sensors, the exposure value can be adjusted.

    //ADCS_get_raw_sensor() LIGHT BROUGHT CLOSE TO THE CAMERA
    printf("Running ADCS_get_raw_sensor...\n");
    test_returnState = ADCS_get_raw_sensor(raw_sensor_measurements);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_raw_sensor returned %d \n", test_returnState);
        while(1);
    }
    printf("LIGHT HELD CLOSE TO THE CAMERA");
    printf("cam1.capture_stat = %d \n", raw_sensor_measurements->cam1.capture_stat);
    printf("cam1.detect_result = %d \n", raw_sensor_measurements->cam1.detect_result);
    printf("cam1.centroid_x = %d \n", raw_sensor_measurements->cam1.centroid_x);
    printf("cam1.centroid_y = %d \n", raw_sensor_measurements->cam1.centroid_y);

    //ADCS_get_raw_sensor() MOVING THE LIGHT UP
    printf("Running ADCS_get_raw_sensor...\n");
    test_returnState = ADCS_get_raw_sensor(raw_sensor_measurements);
     if(test_returnState != ADCS_OK){
        printf("ADCS_get_raw_sensor returned %d \n", test_returnState);
        while(1);
    }
    printf("LIGHT MOVING UP");
    printf("cam1.capture_stat = %d \n", raw_sensor_measurements->cam1.capture_stat);
    printf("cam1.detect_result = %d \n", raw_sensor_measurements->cam1.detect_result);
    printf("cam1.centroid_x = %d \n", raw_sensor_measurements->cam1.centroid_x);
    printf("cam1.centroid_y = %d \n", raw_sensor_measurements->cam1.centroid_y);

    //ADCS_get_raw_sensor() MOVING THE LIGHT DOWN
    printf("Running ADCS_get_raw_sensor...\n");
    test_returnState = ADCS_get_raw_sensor(raw_sensor_measurements);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_raw_sensor returned %d \n", test_returnState);
        while(1);
    }
    printf("LIGHT MOVING DOWN");
    printf("cam1.capture_stat = %d \n", raw_sensor_measurements->cam1.capture_stat);
    printf("cam1.detect_result = %d \n", raw_sensor_measurements->cam1.detect_result);
    printf("cam1.centroid_x = %d \n", raw_sensor_measurements->cam1.centroid_x);
    printf("cam1.centroid_y = %d \n", raw_sensor_measurements->cam1.centroid_y);

    //ADCS_get_raw_sensor() MOVING THE LIGHT RIGHT
    printf("Running ADCS_get_raw_sensor...\n");
    test_returnState = ADCS_get_raw_sensor(raw_sensor_measurements);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_raw_sensor returned %d \n", test_returnState);
        while(1);
    }
    printf("LIGHT MOVING RIGHT");
    printf("cam1.capture_stat = %d \n", raw_sensor_measurements->cam1.capture_stat);
    printf("cam1.detect_result = %d \n", raw_sensor_measurements->cam1.detect_result);
    printf("cam1.centroid_x = %d \n", raw_sensor_measurements->cam1.centroid_x);
    printf("cam1.centroid_y = %d \n", raw_sensor_measurements->cam1.centroid_y);


    //ADCS_get_raw_sensor() MOVING THE LIGHT RIGHT
    printf("Running ADCS_get_raw_sensor...\n");
    test_returnState = ADCS_get_raw_sensor(raw_sensor_measurements);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_raw_sensor returned %d \n", test_returnState);
        while(1);
    }
    printf("LIGHT MOVING LEFT");
    printf("cam1.capture_stat = %d \n", raw_sensor_measurements->cam1.capture_stat);
    printf("cam1.detect_result = %d \n", raw_sensor_measurements->cam1.detect_result);
    printf("cam1.centroid_x = %d \n", raw_sensor_measurements->cam1.centroid_x);
    printf("cam1.centroid_y = %d \n", raw_sensor_measurements->cam1.centroid_y);

    vPortFree(raw_sensor_measurements);


    //While keeping the light in the field of view of CubeSense1, use Command ADCS_save_img() - Table 94 and select Cam1.
    //Select any desired size for Image size, but not selecting Size0 will reduce the downloading time by lowering the image quality (Size3 recommended).
    //Capture the image by sending the command by clicking on the green arrow. The camera will  capture an image after a delay of three seconds.
    //Continue to hold the light in front of  the camera for this duration.

    //ADCS_save_img()
    uint8_t camera = 0;
    uint8_t img_size = 3;

    printf("Running ADCS_save_img...\n");
    test_returnState = ADCS_save_img(camera, img_size);
    if(test_returnState != ADCS_OK){
        printf("ADCS_save_img returned %d \n", test_returnState);
        while(1);
    }

    //Using Command ADCS_get_img_save_progress() - Table 176, refresh Percentage Complete, which will increase slowly and indicate the progress of
    //the image being saved to the SD card from CubeSense�s memory.

    //ADCS_get_img_save_progress() - to run almost immediately after the image is taken
    uint8_t percentage = 0;
    uint8_t status = 0;

    printf("Running ADCS_get_img_save_progress...\n");
    test_returnState = ADCS_get_img_save_progress(&percentage, &status);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_img_save_progress returned %d \n", test_returnState);
        while(1);
    }

    printf("percentage = %d \n", percentage);
    printf("status = %d \n", status);

    //ADCS_get_img_save_progress() - to run a little while after the previous function call.
    percentage = 0;
    status = 0;

    printf("Running ADCS_get_img_save_progress...\n");
    test_returnState = ADCS_get_img_save_progress(&percentage, &status);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_img_save_progress returned %d \n", test_returnState);
        while(1);
    }

    printf("percentage = %d \n", percentage);
    printf("status = %d \n", status);

    // Steps to take to download the image file that was just created:
    // First, get the file list
    test_returnState = ADCS_get_file_list();
    if(test_returnState != ADCS_OK){
            printf("ADCS_get_file_list returned %d \n", test_returnState);
            while(1);
    }


    // 4. Load the image file that was just saved:
    //Variables:

//
//
//    //TODO: Receive all sent bytes from the download burst command. Check to see if the file is complete and if not,
//    //send the hole map back to the ADCS, set the ignore hole map to false, and initiate the download burst again.
//    //Once the whole file has been received, save this file and find a way to send it to the PC
//    // to read it as a bitmap file.
//
//    printf("\n");

}

void binaryTest_CubeSense2(void){

    //Test Section 5.2 CubeSense, Table 5-2 in test plan.
    //Using Command ADCS_get_power_control() - Table 184, ensure that all nodes are selected PowOff before proceeding.
    ADCS_returnState test_returnState = ADCS_OK;

    //Section Variables
    uint8_t *control = (uint8_t*)pvPortMalloc(10);
    if (control == NULL) {
        return ADCS_MALLOC_FAILED;
    }

    //enable the ADCS
    ADCS_set_enabled_state(1);
    ADCS_set_unix_t(0,0);

    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while(1);
    }
    for(int i = 0; i<10; i++){
        printf("control[%d] = %d \n", i, control[i]);
    }


    //Using Command ADCS_set_power_control() - Table 184, switch on CubeSense2 by selecting PowOn.
    //Section Variables
    control[Set_CubeSense1_Power] = 0;//ensures that cam1 is off
    control[Set_CubeSense2_Power] = 1;

    printf("Running ADCS_set_power_control...\n");
    test_returnState = ADCS_set_power_control(control);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while(1);
    }

    vPortFree(control);

    //Ensure that the lens cap is on.


    //Using Command ADCS_set_cubesense_config() - Table 189, ensure that the cam2 detection threshold is set to 150.
    //Next, ensure that the cam2 sensor exposure time is set to 35 if Cam2 is a nadir  sensor, or 100 if Cam2 is a Sun sensor. Set the NadirMaxBadEdges to 30 to make  ground testing easier.
    //Section Variables
    cubesense_config params;
    //Get all the current configuration parameters from the ADCS so that incorrect parameters aren't sent while testing.
    printf("Running ADCS_get_cubesense_config...\n");
    test_returnState = ADCS_get_cubesense_config(&params);
    if(test_returnState != ADCS_OK){
        printf("ADCS_set_cubesense_config returned %d \n", test_returnState);
        while(1);
    }

    //now set the parameters
    params.cam2_sense.detect_th = 150;
    params.cam2_sense.exposure_t = 35; //assuming Cam2 is a nadir sensor. If a sun sensor, use 100.
    params.nadir_max_bad_edge = 30;

    printf("Running ADCS_set_cubesense_config...\n");
    test_returnState = ADCS_set_cubesense_config(params); //this function should be tested and checked before the command is sent
    if(test_returnState != ADCS_OK){
        printf("ADCS_set_cubesense_config returned %d \n", test_returnState);
        while(1);
    }

    // Verify the following values in Table 5-1:
    adcs_state test_adcs_state;//init as 0?

    printf("Running ADCS_get_current_state...\n");
    test_returnState = ADCS_get_current_state(&test_adcs_state);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_current_state returned %d \n", test_returnState);
        while(1);
    }

    printf("att_estimate mode = %d \n", test_adcs_state.att_estimate_mode);
    printf("att_ctrl_mode = %d \n", test_adcs_state.att_ctrl_mode);
    printf("run_mode = %d \n", test_adcs_state.run_mode);
    printf("CubeSense2 Enabled = %d \n", test_adcs_state.flags_arr[2]);
    printf("Sun is Above Local Horizon = %d \n", test_adcs_state.flags_arr[11]);
    //need to test if all flags other than CubeSense2 Enabled and Sun is Above Local Horizon are == 0. Simpler to do in code than via human.
    uint8_t all_other_adcs_states_equal_zero = 0;
    for(int i = 0; i<36; i++){//I think this is the right range.
        if(((i == 2) | (i == 11)) & (test_adcs_state.flags_arr[i] != 0)){
            break;
        }
        if(i == 35){
            all_other_adcs_states_equal_zero = 1;
        }
    }
    if(all_other_adcs_states_equal_zero == 1){
        printf("all other states (frame offsets 12 to 47) == 0 \n");
    } else {
        printf("all other states (frame offsets 12 to 47) != 0... halting code execution\n");
        while(1);
    }

    //ADCS_get_power_temp()
    adcs_pwr_temp power_temp_measurements;

    printf("Running ADCS_get_power_temp...\n");
    test_returnState = ADCS_get_power_temp(&power_temp_measurements);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_power_temp returned %d \n", test_returnState);
        while(1);
    }

    printf("cubesense2_3v3_I = %f \n", power_temp_measurements.cubesense2_3v3_I);
    printf("cubesense2_camSram_I = %f \n", power_temp_measurements.cubesense2_camSram_I);


    //ADCS_get_raw_sensor()
    adcs_raw_sensor raw_sensor_measurements;

    printf("Running ADCS_get_raw_sensor...\n");
    test_returnState = ADCS_get_raw_sensor(&raw_sensor_measurements);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_raw_sensor returned %d \n", test_returnState);
        while(1);
    }

    printf("cam2.centroid_x = %d \n", raw_sensor_measurements.cam2.centroid_x);
    printf("cam2.centroid_y = %d \n", raw_sensor_measurements.cam2.centroid_y);
    printf("cam2.capture_stat = %d \n", raw_sensor_measurements.cam2.capture_stat);
    printf("cam2.detect_result = %d \n", raw_sensor_measurements.cam2.detect_result);

    //Take off the cam2 camera�s lens cap.
    //Verify the following in Table 5-2 by testing the sensor with a light source (a dark n environment will prevent  false detections).
    //If cam2 is a nadir sensor then a large light source should be used  (e.g. a desk lamp), or if cam2 is a Sun sensor then a small light
    //source should be used  (e.g. narrow beam flashlight). Vary the distance between the light source and the sensor  until consistent measurements
    //are observed (normally �150mm). If difficulties are  experienced with the nadir sensor, the light source can be covered with white  paper/cloth
    //to create a more uniform light source. Finally, if no results are obtained for  the nadir or Sun sensors, the exposure value can be adjusted.

    //ADCS_get_raw_sensor() LIGHT BROUGHT CLOSE TO THE CAMERA
    printf("Running ADCS_get_raw_sensor...\n");
    test_returnState = ADCS_get_raw_sensor(&raw_sensor_measurements);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_raw_sensor returned %d \n", test_returnState);
        while(1);
    }
//    printf("LIGHT HELD CLOSE TO THE CAMERA\n");
//    printf("cam2.capture_stat = %d \n", raw_sensor_measurements.cam2.capture_stat);
//    printf("cam2.detect_result = %d \n", raw_sensor_measurements.cam2.detect_result);
//    printf("cam2.centroid_x = %d \n", raw_sensor_measurements.cam2.centroid_x);
//    printf("cam2.centroid_y = %d \n", raw_sensor_measurements.cam2.centroid_y);
//
//    //ADCS_get_raw_sensor() MOVING THE LIGHT UP
//    printf("Running ADCS_get_raw_sensor...\n");
//    test_returnState = ADCS_get_raw_sensor(&raw_sensor_measurements);
//    if(test_returnState != ADCS_OK){
//        printf("ADCS_get_raw_sensor returned %d \n", test_returnState);
//        while(1);
//    }
//    printf("LIGHT MOVING UP\n");
//    printf("cam2.capture_stat = %d \n", raw_sensor_measurements.cam2.capture_stat);
//    printf("cam2.detect_result = %d \n", raw_sensor_measurements.cam2.detect_result);
//    printf("cam2.centroid_x = %d \n", raw_sensor_measurements.cam2.centroid_x);
//    printf("cam2.centroid_y = %d \n", raw_sensor_measurements.cam2.centroid_y);
//
//    //ADCS_get_raw_sensor() MOVING THE LIGHT DOWN
//    printf("Running ADCS_get_raw_sensor...\n");
//    test_returnState = ADCS_get_raw_sensor(&raw_sensor_measurements);
//    if(test_returnState != ADCS_OK){
//        printf("ADCS_get_raw_sensor returned %d \n", test_returnState);
//        while(1);
//    }
//    printf("LIGHT MOVING DOWN\n");
//    printf("cam2.capture_stat = %d \n", raw_sensor_measurements.cam2.capture_stat);
//    printf("cam2.detect_result = %d \n", raw_sensor_measurements.cam2.detect_result);
//    printf("cam2.centroid_x = %d \n", raw_sensor_measurements.cam2.centroid_x);
//    printf("cam2.centroid_y = %d \n", raw_sensor_measurements.cam2.centroid_y);
//
//    //ADCS_get_raw_sensor() MOVING THE LIGHT RIGHT
//    printf("Running ADCS_get_raw_sensor...\n");
//    test_returnState = ADCS_get_raw_sensor(&raw_sensor_measurements);
//    if(test_returnState != ADCS_OK){
//        printf("ADCS_get_raw_sensor returned %d \n", test_returnState);
//        while(1);
//    }
//    printf("LIGHT MOVING RIGHT\n");
//    printf("cam2.capture_stat = %d \n", raw_sensor_measurements.cam2.capture_stat);
//    printf("cam2.detect_result = %d \n", raw_sensor_measurements.cam2.detect_result);
//    printf("cam2.centroid_x = %d \n", raw_sensor_measurements.cam2.centroid_x);
//    printf("cam2.centroid_y = %d \n", raw_sensor_measurements.cam2.centroid_y);
//
//
//    //ADCS_get_raw_sensor() MOVING THE LIGHT RIGHT
//    printf("Running ADCS_get_raw_sensor...\n");
//    test_returnState = ADCS_get_raw_sensor(&raw_sensor_measurements);
//    if(test_returnState != ADCS_OK){
//        printf("ADCS_get_raw_sensor returned %d \n", test_returnState);
//        while(1);
//    }
//    printf("LIGHT MOVING LEFT\n");
//    printf("cam2.capture_stat = %d \n", raw_sensor_measurements.cam2.capture_stat);
//    printf("cam2.detect_result = %d \n", raw_sensor_measurements.cam2.detect_result);
//    printf("cam2.centroid_x = %d \n", raw_sensor_measurements.cam2.centroid_x);
//    printf("cam2.centroid_y = %d \n", raw_sensor_measurements.cam2.centroid_y);
//
//
//    //While keeping the light in the field of view of CubeSense1, use Command ADCS_save_img() - Table 94 and select cam2.
//    //Select any desired size for Image size, but not selecting Size0 will reduce the downloading time by lowering the image quality (Size3 recommended).
//    //Capture the image by sending the command by clicking on the green arrow. The camera will  capture an image after a delay of three seconds.
//    //Continue to hold the light in front of  the camera for this duration.
//
//    //ADCS_save_img()
//    uint8_t camera = 0;
//    uint8_t img_size = 3;
//
//    printf("Running ADCS_save_img...\n");
//    test_returnState = ADCS_save_img(camera, img_size);
//    if(test_returnState != ADCS_OK){
//        printf("ADCS_save_img returned %d \n", test_returnState);
//        while(1);
//    }
//
//    //Using Command ADCS_get_img_save_progress() - Table 176, refresh Percentage Complete, which will increase slowly and indicate the progress of
//    //the image being saved to the SD card from CubeSense�s memory.
//
//    //ADCS_get_img_save_progress() - to run almost immediately after the image is taken
//    uint8_t percentage = 0;
//    uint8_t status = 0;
//
//    printf("Running ADCS_get_img_save_progress...\n");
//    test_returnState = ADCS_get_img_save_progress(&percentage, &status);
//    if(test_returnState != ADCS_OK){
//        printf("ADCS_get_img_save_progress returned %d \n", test_returnState);
//        while(1);
//    }
//
//    printf("percentage = %d \n", percentage);
//    printf("status = %d \n", status);
//
//    //ADCS_get_img_save_progress() - to run a little while after the previous function call.
//    percentage = 0;
//    status = 0;
//
//    printf("Running ADCS_get_img_save_progress...\n");
//    test_returnState = ADCS_get_img_save_progress(&percentage, &status);
//    if(test_returnState != ADCS_OK){
//        printf("ADCS_get_img_save_progress returned %d \n", test_returnState);
//        while(1);
//    }
//
//    printf("percentage = %d \n", percentage);
//    printf("status = %d \n", status);
//
//    // TODO: Steps to take to download the image file that was just created.
//    //Might be able to copy and adjust the code for cam1, but that should be tested before copying.
//
//    printf("\n");
}

void binaryTest_CubeControl_Sgn_MCU(void) {

    ADCS_returnState test_returnState = ADCS_OK;
    //Test Section 6.1 CubeControl, Table 6-1,2 in test plan.
    uint8_t *control = (uint8_t*)pvPortMalloc(10);
    if (control == NULL) {
        return ADCS_MALLOC_FAILED;
    }

    //enable the ADCS
    ADCS_set_enabled_state(1);

    ADCS_set_unix_t(0,0);

    // Using Command ADCS_get_power_control() - Table 184, ensure that all nodes are selected PowOff before proceeding.
    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while(1);
    }
    for(int i = 0; i<10; i++){
        printf("control[%d] = %d \n", i, control[i]);
    }

    // Using Command ADCS_set_power_control() - Table 184, switch on CubeControl Signal MCU by selecting PowOn.
    //Section Variables
    if (control == NULL) {
        return ADCS_MALLOC_FAILED;
    }
    control[Set_CubeCTRLSgn_Power] = 1;

    printf("Running ADCS_set_power_control...\n");
    test_returnState = ADCS_set_power_control(control);
    if(test_returnState != ADCS_OK){
        printf("ADCS_set_power_control returned %d \n", test_returnState);
        while(1);
    }

    //another read to make sure we are in the right state
    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while(1);
    }
    for(int i = 0; i<10; i++){
        printf("control[%d] = %d \n", i, control[i]);
    }

    vPortFree(control);

    adcs_state test_adcs_state;
    //Run ADCS_get_current_state()
    printf("Running ADCS_get_current_state...\n");
    test_returnState = ADCS_get_current_state(&test_adcs_state);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_current_state returned %d \n", test_returnState);
        while(1);
    }

    printf("att_estimate mode = %d \n", test_adcs_state.att_estimate_mode);
    printf("att_ctrl_mode = %d \n", test_adcs_state.att_ctrl_mode);
    printf("run_mode = %d \n", test_adcs_state.run_mode);
    printf("CubeControl Signal Enabled = %d \n", test_adcs_state.flags_arr[0]);

    //need to test if all other flags == 0. Simpler to do in code than via human.
    // CHANGED TO A MANUAL HUMAN CHECK AT THIS POINT
    uint8_t all_other_adcs_states_equal_zero = 1;
//    for(int i = 0; i<36; i++){//I think this is the right range.
//        if(!((i == 0)) && test_adcs_state.flags_arr[i] != 0){
//            break;
//        }
//        if(i == 35){
//            all_other_adcs_states_equal_zero = 1;
//        }
//    }
    if(all_other_adcs_states_equal_zero == 1){
        printf("all other states (frame offsets 12 to 47) == 0 \n");
    } else {
        printf("all other states (frame offsets 12 to 47) != 0... halting code execution\n");
        while(1);
    }

    //ADCS_get_power_temp()
    adcs_pwr_temp *power_temp_measurements;
    power_temp_measurements = (adcs_pwr_temp *)pvPortMalloc(sizeof(adcs_pwr_temp));
    if (power_temp_measurements == NULL) {
        printf("malloc issues");
        while(1);
    }

    printf("Running ADCS_get_power_temp...\n");
    test_returnState = ADCS_get_power_temp(power_temp_measurements);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_power_temp returned %d \n", test_returnState);
        while(1);
    }

    printf("cubecontrol_3v3_I = %f \n", power_temp_measurements->cubecontrol_3v3_I);
    printf("cubecontrol_5v_I = %f \n", power_temp_measurements->cubecontrol_5v_I);
    printf("cubecontrol_vBat_I = %f \n", power_temp_measurements->cubecontrol_vBat_I);
    printf("MCU_temp = %f \n", power_temp_measurements->MCU_temp);
    printf("MTM_temp = %f \n", power_temp_measurements->MTM_temp);
    printf("MTM2_temp = %f \n", power_temp_measurements->MTM2_temp);

    vPortFree(power_temp_measurements);

    for(int iteration = 0; iteration<12; iteration++){
        //ADCS_get_raw_sensor()

        //Second time around, expose the coarse Sun sensors to a bright light, one by one.
        //Verify the following in Table 6-2:
        adcs_raw_sensor *raw_sensor_measurements;
        raw_sensor_measurements = (adcs_raw_sensor *)pvPortMalloc(sizeof(adcs_raw_sensor));
        if (raw_sensor_measurements == NULL) {
            printf("malloc issues");
            while(1);
        }
        printf("Running ADCS_get_raw_sensor...\n");
        test_returnState = ADCS_get_raw_sensor(raw_sensor_measurements);
        if(test_returnState != ADCS_OK){
            printf("ADCS_get_raw_sensor returned %d \n", test_returnState);
            while(1);
        }

        for(int i = 0; i<10; i++){
            printf("CSS%d = %d \n", i, raw_sensor_measurements->css[i]);
        }
    }
}

void binaryTest_CubeMag_Sgn_MCU(void) {
    //enable the ADCS
    ADCS_set_enabled_state(1);
    ADCS_set_unix_t(0,0);

    CubeMag_Common_Test(1);
}



void binaryTest_CubeTorquers_Sgn_MCU(void) {

    //Test Section 6.1.3 CubeMag, Table 6-5 in test plan.

    //Using Command ADCS_set_magnetorquer_output() - Table 81. Command the magnetorquer coil (X-axis) to maximum positive value.
    ADCS_returnState test_returnState = ADCS_OK;
    printf("Running ADCS_set_attitude_ctrl_mode...\n");
    test_returnState = ADCS_set_attitude_ctrl_mode(0, 0XFFFF);//no timeout (infinite time)
    if(test_returnState != ADCS_OK){
        printf("ADCS_set_attitude_ctrl_mode returned %d \n", test_returnState);
        while(1);
    }

    CubeTorquers_Common_Test();

    //Using Command ADCS_set_power_control() - Table 184, switch off CubeControl Signal MCU by selecting PowOff.
    uint8_t *control = (uint8_t*)pvPortMalloc(10);
    if (control == NULL) {
        return ADCS_MALLOC_FAILED;
    }

    for(int i = 0; i<10; i++){
       control[i] = 0;
    }
    //I assume 0 = PowOff?

    printf("Running ADCS_set_power_control...\n");
    test_returnState = ADCS_set_power_control(control);
    if(test_returnState != ADCS_OK){
        printf("ADCS_set_power_control returned %d \n", test_returnState);
        while(1);
    }


}

void binaryTest_CubeControl_Motor_MCU(void) {
    //Test Section 6.2 CubeControl, Table 6-6 in test plan.

    ADCS_returnState test_returnState = ADCS_OK;

    //Using Command ADCS_get_power_control() - Table 184, ensure that all nodes are selected PowOff before proceeding.
    //Section Variables
    uint8_t *control = (uint8_t*)pvPortMalloc(10);
    if (control == NULL) {
        return ADCS_MALLOC_FAILED;
    }


    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while(1);
    }
    for(int i = 0; i<10; i++){
        printf("control[%d] = %d \n", i, control[i]);
    }


    //Using Command ADCS_set_power_control() - Table 184, switch on CubeControl�s Motor MCU by selecting PowOn.
    //Section Variables
    if (control == NULL) {
        return ADCS_MALLOC_FAILED;
    }
    control[Set_CubeCTRLMtr_Power] = 1;

    printf("Running ADCS_set_power_control...\n");
    test_returnState = ADCS_set_power_control(control);
    if(test_returnState != ADCS_OK){
        printf("ADCS_set_power_control returned %d \n", test_returnState);
        while(1);
    }

    //another read to make sure we are in the right state
    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while(1);
    }
    for(int i = 0; i<10; i++){
        printf("control[%d] = %d \n", i, control[i]);
    }

    vPortFree(control);


    //Verify the following in Table 6-6:

    adcs_state test_adcs_state;
    //Run ADCS_get_current_state()
    printf("Running ADCS_get_current_state...\n");
    test_returnState = ADCS_get_current_state(&test_adcs_state);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_current_state returned %d \n", test_returnState);
        while(1);
    }

    printf("att_estimate mode = %d \n", test_adcs_state.att_estimate_mode);
    printf("att_ctrl_mode = %d \n", test_adcs_state.att_ctrl_mode);
    printf("run_mode = %d \n", test_adcs_state.run_mode);
    printf("CubeControl Motor Enabled = %d \n", test_adcs_state.flags_arr[1]);

    //need to test if all other flags == 0. Simpler to do in code than via human.
    uint8_t all_other_adcs_states_equal_zero = 0;
    for(int i = 0; i<36; i++){//I think this is the right range.
        if(test_adcs_state.flags_arr[i] != 0){
            break;
        }
        if(i == 35){
            all_other_adcs_states_equal_zero = 1;
        }
    }
    if(all_other_adcs_states_equal_zero == 1){
        printf("all other states (frame offsets 12 to 47) == 0 \n");
    } else {
        printf("all other states (frame offsets 12 to 47) != 0... halting code execution\n");
        //while(1);
    }

    //ADCS_get_power_temp()
    adcs_pwr_temp *power_temp_measurements;
    power_temp_measurements = (adcs_pwr_temp *)pvPortMalloc(sizeof(adcs_pwr_temp));
    if (power_temp_measurements == NULL) {
        printf("malloc issues");
        while(1);
    }

    printf("Running ADCS_get_power_temp...\n");
    test_returnState = ADCS_get_power_temp(power_temp_measurements);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_power_temp returned %d \n", test_returnState);
        while(1);
    }

    printf("X-Rate Sensor Temperature = %d \n", power_temp_measurements->rate_sensor_temp.x);
    printf("Y-Rate Sensor Temperature = %d \n", power_temp_measurements->rate_sensor_temp.y);
    printf("Z-Rate Sensor Temperature = %d \n", power_temp_measurements->rate_sensor_temp.z);

    vPortFree(power_temp_measurements);


    adcs_measures *measurements;
    measurements = (adcs_measures *)pvPortMalloc(sizeof(adcs_measures));
    if (measurements == NULL) {
        printf("malloc issues");
        while(1);
    }

    for(int i = 0; i<15; i++){//repeating 5 times for each axis = 15 times
        printf("Running ADCS_get_measurements...\n");
        test_returnState = ADCS_get_measurements(measurements);
        if(test_returnState != ADCS_OK){
            printf("ADCS_get_measurements returned %d \n", test_returnState);
            while(1);
        }
        printf("Angular Rate X = %+f \n", measurements->angular_rate.x);//not 100% sure if this will print the sign of the float
        printf("Angular Rate Y = %+f \n", measurements->angular_rate.y);
        printf("Angular Rate Z = %+f \n", measurements->angular_rate.z);
    }

    vPortFree(measurements);

}

void binaryTest_CubeMag_Motor_MCU(void) {
    //Test Section 6.2.1 CubeMag, Table 6-7,8 in test plan.

    CubeMag_Common_Test(0);

}

void binaryTest_CubeTorquers_Motor_MCU(void) {
    //Test Section 6.2.2 CubeTorquers, Table 6-9 in test plan.

    CubeTorquers_Common_Test();

}

void binaryTest_CubeWheel_BurnIn_MCU(void) {//Omitted for current binary testing plan
    //Test Section 7 CubeControl, Table 7-1 in test plan.

    //Using Command ADCS_get_power_control() - Table 184, ensure that all nodes are selected PowOff before proceeding.

    //Using Command ADCS_set_power_control() - Table 184, switch on CubeWheel1Power, CubeWheel2Power, and CubeWheel3Power Power Selection by selecting PowOn.

    //Verify the following in Table 7-1
}

void binaryTest_CubeWheel1_MCU(void) {
    //Test Section 6.1 CubeControl, Table 6-1,2 in test plan.
    ReactionWheels_Common_Test(1);
}

void binaryTest_CubeWheel2_MCU(void) {
    //Test Section 6.1 CubeControl, Table 6-1,2 in test plan.
    ReactionWheels_Common_Test(2);

}

void binaryTest_CubeWheel3_MCU(void) {
    //Test Section 6.1 CubeControl, Table 6-1,2 in test plan.
    ReactionWheels_Common_Test(3);

}



//BELOW HERE LIES CODE THAT IS COMMON FOR MULTIPLE PARTS OF BINARY TEST PLAN. THESE FUNCTIONS
//ARE CALLED BY FUNCTIONS ABOVE HERE, AND SHOULD NOT BE RUN IN ISOLATION

void CubeMag_Common_Test(bool signalMCU){

    ADCS_returnState test_returnState = ADCS_OK;
    adcs_state test_adcs_state;

// Using Command ADCS_get_power_control() - Table 184, ensure that all nodes are selected PowOff before proceeding.
    uint8_t *control = (uint8_t*)pvPortMalloc(10);
    if (control == NULL) {
        return ADCS_MALLOC_FAILED;
    }

    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if(test_returnState != ADCS_OK){
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while(1);
    }
    for(int i = 0; i<10; i++){
       printf("control[%d] = %d \n", i, control[i]);
    }

    // Using Command ADCS_set_power_control() - Table 184, switch on CubeControl Signal MCU by selecting PowOn.
    //Section Variables
    if (control == NULL) {
       return ADCS_MALLOC_FAILED;
    }
    control[Set_CubeCTRLSgn_Power] = 1;

    printf("Running ADCS_set_power_control...\n");
    test_returnState = ADCS_set_power_control(control);
    if(test_returnState != ADCS_OK){
       printf("ADCS_set_power_control returned %d \n", test_returnState);
       while(1);
    }

    //another read to make sure we are in the right state
    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if(test_returnState != ADCS_OK){
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while(1);
    }
    for(int i = 0; i<10; i++){
       printf("control[%d] = %d \n", i, control[i]);
    }

    vPortFree(control);

    uint8_t MTM_op_mode;
    //Test Section 6.1.1 CubeControl, Table 6-3,4 in test plan.
    //Using Command ADCS_get_current_state() - Table 89, select SigMainMag.
    if(signalMCU == 1){
        printf("Running ADCS_set_MTM_op_mode to Main MTM Sampled Through Signal...\n");
        MTM_op_mode = 0;
    }
    else if (signalMCU ==0){
        printf("Running ADCS_set_MTM_op_mode to Main MTM Sampled Through motor...\n");
        MTM_op_mode = 2;
    }

    test_returnState = ADCS_set_MTM_op_mode(MTM_op_mode);
    if(test_returnState != ADCS_OK){
        printf("ADCS_set_MTM_op_mode returned %d \n", test_returnState);
        while(1);
    }

    if(signalMCU == 1){
        //Using Command ADCS_get_current_state() - Table 98,  ensure that the  Magnetometer Range Error is not checked.
        //If it is checked, then the magnetometer is  unable to measure a sufficient/overpowering magnetic field.
        //This can be solved by  ensuring that there is no contact to an anti-static mat or by placing the magnetometers
        //away from motors, power supplies, large ferromagnetic objects, etc.
        //Run ADCS_get_current_state()
        printf("Running ADCS_get_current_state...\n");
        test_returnState = ADCS_get_current_state(&test_adcs_state);
        if(test_returnState != ADCS_OK){
            printf("ADCS_get_current_state returned %d \n", test_returnState);
            while(1);
        }

        printf("Magnetometer Range Error = %d \n", test_adcs_state.flags_arr[10]);
    }

    // Familiarise the axes of both magnetometers, as shown in Appendix A at the end of  this document.

    //Verify the operation of the  magnetometers in Table 6-3 by using Commands
    //ADCS_get_raw_sensor()/ADCS_get_MTM2_measurements() - Table 128 and Table 163.
    //Choose an axis on the main magnetometer and point it in the positive direction of the magnetic field lines
    //(north) to align the axis with the magnetic vector. Now rotate the main magnetometer around  this axis.
    //The chosen axis must remain positive while the other two axes will both go  negative and positive through the rotation.
    //Repeat this for all three axes to verify  polarities.
    adcs_raw_sensor *raw_sensor_measurements;
    raw_sensor_measurements = (adcs_raw_sensor *)pvPortMalloc(sizeof(adcs_raw_sensor));
    if (raw_sensor_measurements == NULL) {
        printf("malloc issues");
        while(1);
    }
    xyz16 mtm2;
    for(int i = 0; i<15; i++){//repeating 5 times for each axis = 15 times

        //ADCS_get_raw_sensor()
        printf("Running ADCS_get_raw_sensor...\n");
        test_returnState = ADCS_get_raw_sensor(raw_sensor_measurements);
        if(test_returnState != ADCS_OK){
            printf("ADCS_get_raw_sensor returned %d \n", test_returnState);
            while(1);
        }

        printf("Raw Magnetometer X = %d \n", raw_sensor_measurements->MTM.x);//not 100% sure if this will print the sign of the float
        printf("Raw Magnetometer Y = %d \n", raw_sensor_measurements->MTM.y);
        printf("Raw Magnetometer Z = %d \n", raw_sensor_measurements->MTM.z);

        //ADCS_get_raw_sensor()
        printf("Running ADCS_get_MTM2_measurement...\n");
        test_returnState = ADCS_get_MTM2_measurements(&mtm2);
        if(test_returnState != ADCS_OK){
            printf("ADCS_get_MTM2_measurement returned %d \n", test_returnState);
            while(1);
        }

        printf("Raw Secondary Mag X = %d \n", mtm2.x);//not 100% sure if this will print the sign of the float
        printf("Raw Secondary Mag Y = %d \n", mtm2.y);
        printf("Raw Secondary Mag Z = %d \n", mtm2.z);
    }


    vPortFree(raw_sensor_measurements);


    //Rotate the  main magnetometer and verify in Table 6-4 that the magnetic field vector displays both positive
    //and negative in X, Y, and Z directions correctly. Fill the following table accordingly:

    adcs_measures *measurements;
    measurements = (adcs_measures *)pvPortMalloc(sizeof(adcs_measures));
    if (measurements == NULL) {
        printf("malloc issues");
        while(1);
    }

    for(int i = 0; i<15; i++){//repeating 5 times for each axis = 15 times
        printf("Running ADCS_get_measurements...\n");
        test_returnState = ADCS_get_measurements(measurements);
        if(test_returnState != ADCS_OK){
            printf("ADCS_get_measurements returned %d \n", test_returnState);
            while(1);
        }
        printf("Magnetic Field X = %+f \n", measurements->magnetic_field.x);//not 100% sure if this will print the sign of the float
        printf("Magnetic Field Y = %+f \n", measurements->magnetic_field.y);
        printf("Magnetic Field Z = %+f \n", measurements->magnetic_field.z);
    }

    vPortFree(measurements);

}

void CubeTorquers_Common_Test(void){

    ADCS_returnState test_returnState = ADCS_OK;

// Using Command ADCS_get_power_control() - Table 184, ensure that all nodes are selected PowOff before proceeding.
    uint8_t *control = (uint8_t*)pvPortMalloc(10);
    if (control == NULL) {
        return ADCS_MALLOC_FAILED;
    }

    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if(test_returnState != ADCS_OK){
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while(1);
    }
    for(int i = 0; i<10; i++){
       printf("control[%d] = %d \n", i, control[i]);
    }

    // Using Command ADCS_set_power_control() - Table 184, switch on CubeControl Signal MCU by selecting PowOn.
    //Section Variables
    if (control == NULL) {
       return ADCS_MALLOC_FAILED;
    }
    control[Set_CubeCTRLSgn_Power] = 1;

    printf("Running ADCS_set_power_control...\n");
    test_returnState = ADCS_set_power_control(control);
    if(test_returnState != ADCS_OK){
       printf("ADCS_set_power_control returned %d \n", test_returnState);
       while(1);
    }

    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if(test_returnState != ADCS_OK){
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while(1);
    }
    for(int i = 0; i<10; i++){
       printf("control[%d] = %d \n", i, control[i]);
    }

    adcs_pwr_temp *power_temp_measurements;
    power_temp_measurements = (adcs_pwr_temp *)pvPortMalloc(sizeof(adcs_pwr_temp));
    if (power_temp_measurements == NULL) {
        printf("malloc issues");
        while(1);
    }
    int16_t maxDuty = 800;
    xyz16 dutycycle = {maxDuty, 0, 0};//TODO Verify this!
    for (int i = 0; i<3; i++){

        switch (i) {
            case 0:
                printf("Testing X-axis magnetorquer");
                break;
            case 1:
                dutycycle.x = 0;
                dutycycle.y = maxDuty;
                dutycycle.z = 0;
                printf("Testing Y-axis magnetorquer");
                break;
            case 2:
                dutycycle.x = 0;
                dutycycle.y = 0;
                dutycycle.z = maxDuty;
                printf("Testing Z-axis magnetorquer");
                break;
        }

        //ADCS_set_magnetorquer_output
        printf("Running ADCS_set_magnetorquer_output -POSITIVE DIRECTION-...\n");
        test_returnState = ADCS_set_magnetorquer_output(dutycycle);
        if(test_returnState != ADCS_OK){
            printf("ADCS_set_magnetorquer_output returned %d \n", test_returnState);
            while(1);
        }
        //Confirm the current measurement and the direction of the magnetic field in the table  below.
        //The direction can be confirmed by either placing a compass directly in line with  the magnetorquer and observing the field
        //direction or by using an external  magnetometer. Note that the magnetorquer pulses on for a maximum of 0.8 seconds  and then switches off.

        //ADCS_get_power_temp()
        printf("Running ADCS_get_power_temp...\n");
        test_returnState = ADCS_get_power_temp(power_temp_measurements);
        if(test_returnState != ADCS_OK){
            printf("ADCS_get_power_temp returned %d \n", test_returnState);
            while(1);
        }

        printf("magnetorquer_I = %f \n", power_temp_measurements->magnetorquer_I);

        //Change the command to maximum negative and confirm the current and direction  again.
        dutycycle.x = -(dutycycle.x);
        dutycycle.y = -(dutycycle.y);
        dutycycle.z = -(dutycycle.z);
        //ADCS_set_magnetorquer_output
        printf("Running ADCS_set_magnetorquer_output (NEGATIVE DIRECTION)...\n");
        test_returnState = ADCS_set_magnetorquer_output(dutycycle);
        if(test_returnState != ADCS_OK){
            printf("ADCS_set_magnetorquer_output returned %d \n", test_returnState);
            while(1);
        }
        //Confirm the current measurement and the direction of the magnetic field in the table  below.
        //The direction can be confirmed by either placing a compass directly in line with  the magnetorquer and observing the field
        //direction or by using an external  magnetometer. Note that the magnetorquer pulses on for a maximum of 0.8 seconds  and then switches off.

        //ADCS_get_power_temp()
        printf("Running ADCS_get_power_temp...\n");
        test_returnState = ADCS_get_power_temp(power_temp_measurements);
        if(test_returnState != ADCS_OK){
            printf("ADCS_get_power_temp returned %d \n", test_returnState);
            while(1);
        }

        printf("magnetorquer_I = %f \n", power_temp_measurements->magnetorquer_I);

        //Command the magnetorquer to zero to turn off the magnetorquer.
        dutycycle.x = 0;
        dutycycle.y = 0;
        dutycycle.z = 0;
        //ADCS_set_magnetorquer_output
        printf("Running ADCS_set_magnetorquer_output (OFF)...\n");
        test_returnState = ADCS_set_magnetorquer_output(dutycycle);
        if(test_returnState != ADCS_OK){
            printf("ADCS_set_magnetorquer_output returned %d \n", test_returnState);
            while(1);
        }

        //Ensure all magnetorquers are turned off before continuing

        //ADCS_get_power_temp()
        printf("Running ADCS_get_power_temp...\n");
        test_returnState = ADCS_get_power_temp(power_temp_measurements);
        if(test_returnState != ADCS_OK){
            printf("ADCS_get_power_temp returned %d \n", test_returnState);
            while(1);
        }

        printf("magnetorquer_I = %f \n", power_temp_measurements->magnetorquer_I);

        //Repeat these steps for the Y and Z magnetorquer rods and record the required values in table 6-5.
    }

    vPortFree(power_temp_measurements);

}

void ReactionWheels_Common_Test(uint8_t wheel_number){
    ADCS_returnState test_returnState = ADCS_OK;

    //Using Command ADCS_get_power_control() - Table 184, ensure that all nodes are selected PowOff before proceeding.
    //Section Variables
    uint8_t *control = (uint8_t*)pvPortMalloc(10);
    if (control == NULL) {
        return ADCS_MALLOC_FAILED;
    }


    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while(1);
    }
    for(int i = 0; i<10; i++){
        printf("control[%d] = %d \n", i, control[i]);
    }

    if(wheel_number == 1){
        control[Set_CubeWheel1_Power] = 1;
    }
    else if(wheel_number == 2){
        control[Set_CubeWheel2_Power] = 1;
    }
    else if(wheel_number == 3){
        control[Set_CubeWheel3_Power] = 1;
    }


    printf("Running ADCS_set_power_control...\n");
    test_returnState = ADCS_set_power_control(control);
    if(test_returnState != ADCS_OK){
        printf("ADCS_set_power_control returned %d \n", test_returnState);
        while(1);
    }

    //another read to make sure we are in the right state
    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while(1);
    }
    for(int i = 0; i<10; i++){
        printf("control[%d] = %d \n", i, control[i]);
    }




    adcs_state test_adcs_state;
    //Run ADCS_get_current_state()
    printf("Running ADCS_get_current_state...\n");
    test_returnState = ADCS_get_current_state(&test_adcs_state);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_current_state returned %d \n", test_returnState);
        while(1);
    }

    printf("att_estimate mode = %d \n", test_adcs_state.att_estimate_mode);
    printf("att_ctrl_mode = %d \n", test_adcs_state.att_ctrl_mode);
    printf("run_mode = %d \n", test_adcs_state.run_mode);

    if(wheel_number == 1){
        printf("CubeWheel1 Signal Enabled = %d \n", test_adcs_state.flags_arr[4]);
    }
    else if(wheel_number == 2){
        printf("CubeWheel2 Signal Enabled = %d \n", test_adcs_state.flags_arr[5]);
    }
    else if(wheel_number == 3){
        printf("CubeWheel3 Signal Enabled = %d \n", test_adcs_state.flags_arr[6]);
    }


    //need to test if all other flags == 0. Simpler to do in code than via human.
//    uint8_t all_other_adcs_states_equal_zero = 0;
//    for(int i = 0; i<36; i++){//I think this is the right range.
//        if(test_adcs_state.flags_arr[i] != 0){
//            break;
//        }
//        if(i == 35){
//            all_other_adcs_states_equal_zero = 1;
//        }
//    }
//    if(all_other_adcs_states_equal_zero == 1){
//        printf("all other states (frame offsets 12 to 47) == 0 \n");
//    } else {
//        printf("all other states (frame offsets 12 to 47) != 0... halting code execution\n");
//        while(1);
//    }

    xyz16 speed;
    speed.x = 0;
    speed.y = 0;
    speed.z = 0;
    int16_t rpm = 0;

    adcs_measures *measurements;
    measurements = (adcs_measures *)pvPortMalloc(sizeof(adcs_measures));
    if (measurements == NULL) {
        printf("malloc issues");
        while(1);
    }

    adcs_pwr_temp *power_temp_measurements;
    power_temp_measurements = (adcs_pwr_temp *)pvPortMalloc(sizeof(adcs_pwr_temp));
    if (power_temp_measurements == NULL) {
        printf("malloc issues");
        while(1);
    }

    for (int i = 0; i<3; i++){

        if(i == 0){
            rpm = 4000;
        }
        else if(i == 1){
            rpm = -2000;
        }
        else if(i == 2){
            rpm = 0;
        }

        if(wheel_number == 1){
            speed.x = rpm;
        }
        else if(wheel_number == 2){
            speed.y = rpm;
        }
        else if(wheel_number == 3){
            speed.z = rpm;
        }

        //Using command ADCS_set_wheel_speed() - Table 82. Set the  commanded X speed to 4000 rpm.
        printf("Running ADCS_set_wheel_speed...\n");
        test_returnState = ADCS_set_wheel_speed(speed);
        if(test_returnState != ADCS_OK){
            printf("ADCS_get_current_state returned %d \n", test_returnState);
            while(1);
        }

        // After 10 seconds the wheel will settle to the commanded speed. Listen to the wheel to  ensure that the correct one is spinning up and that the other two wheels are silent.
        // Verify the following in Table 7-5

        for(int i = 0; i<15; i++){//repeating 5 times for each axis = 15 times
            printf("Running ADCS_get_measurements...\n");
            test_returnState = ADCS_get_measurements(measurements);
            if(test_returnState != ADCS_OK){
                printf("ADCS_get_measurements returned %d \n", test_returnState);
                while(1);
            }
            printf("X Wheel Speed = %+f \n", measurements->wheel_speed.x);//not 100% sure if this will print the sign of the float
            printf("Y Wheel Speed = %+f \n", measurements->wheel_speed.y);
            printf("Z Wheel Speed = %+f \n", measurements->wheel_speed.z);
        }



        //ADCS_get_power_temp()

        printf("Running ADCS_get_power_temp...\n");
        test_returnState = ADCS_get_power_temp(power_temp_measurements);
        if(test_returnState != ADCS_OK){
            printf("ADCS_get_power_temp returned %d \n", test_returnState);
            while(1);
        }

        printf("Wheel 1 Current [mA] = %f \n", power_temp_measurements->wheel1_I);
        printf("Wheel 2 Current [mA] = %f \n", power_temp_measurements->wheel2_I);
        printf("Wheel 3 Current [mA] = %f \n", power_temp_measurements->wheel3_I);

        speed.x = 0;
        speed.y = 0;
        speed.z = 0;


    }
    vPortFree(measurements);
    vPortFree(power_temp_measurements);

    if(wheel_number == 1){
        control[Set_CubeWheel1_Power] = 0;
    }
    else if(wheel_number == 2){
        control[Set_CubeWheel2_Power] = 0;
    }
    else if(wheel_number == 3){
        control[Set_CubeWheel3_Power] = 0;
    }


    printf("Running ADCS_set_power_control...\n");
    test_returnState = ADCS_set_power_control(control);
    if(test_returnState != ADCS_OK){
        printf("ADCS_set_power_control returned %d \n", test_returnState);
        while(1);
    }
    vPortFree(control);
}
