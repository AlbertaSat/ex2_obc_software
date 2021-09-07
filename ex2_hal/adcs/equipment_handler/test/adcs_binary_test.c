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

#include "adcs_handler.h"
#include "adcs_io.h"


void binaryTest_Bootloader(void){
    //Test corresponds to Section 4.1 Bootloader, Table 4-1 in test plan.
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
    uint8_t flags_arr = 0;

    printf("Running ADCS_get_comms_stat...\n");
    test_returnState = ADCS_get_comms_stat(&TC_num, &TM_num, &flags_arr);//should give compile error until function itself is fixed
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_comms_stat returned %d \n", test_returnState);
        while(1);
    }

    printf("TC_num = %d \n", TC_num);
    printf("TM_num = %d \n", TM_num);
    printf("flags_arr (decimal form, not binary) = %d \n", flags_arr);

    printf("\n");
}

void binaryTest_CubeACP(void){

    //Test Section 4.2 CubeACP, Table 4-2 in test plan.
    uint16_t time = 0;
    uint8_t execution_point = 0;

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
    uint8_t major_firm_ver = 0;
    uint8_t minor_firm_ver = 0;
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
    test_returnState = ADCS_get_comms_stat(&TC_num, &TM_num, &flags_arr);//should give compile error until function itself is fixed
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
    uint8_t control[10] = {0};

    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(&control);
    if(test_returnState != ADCS_OK){
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while(1);
    }
    for(int i = 0; i<10; i++){
        printf("control[%d] = %d \n", i, control[i]);
    }

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
    uint16_t TC_num = 0;
    uint16_t TM_num = 0;
    uint8_t flags_arr = 0;

    printf("Running ADCS_get_comms_stat...\n");
    test_returnState = ADCS_get_comms_stat(&TC_num, &TM_num, &flags_arr);//should give compile error until function itself is fixed
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
    test_returnState = ADCS_get_execution_times(&adcs_update, &sensor_comms, &sgp4_propag, igrf_model);
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
        else if(i == 24){//Cam1 Sensor Detection Error
            //skip index
        }
        else if(i == 25){//Cam1 Sensor Range Error
            //skip index
        }
        else if(i == 29){//Cam1 Sensor Detection Error
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
    test_adcs_state = {0};

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

void binaryTest(void) {//TODO: add enums for all adcs_handler functions called

    printf("Running Bootloader Tests");
    binaryTest_Bootloader();
    printf("Bootloader Tests Complete!");

    printf("Running CubeACP Tests");
    binaryTest_CubeACP();
    printf("CubeACP Tests Complete!");

    //TODO: all the rest of test plan, including pause points for manual work done in test plan (including before this point)
    //TODO: add checks for "incrementing" and "idle" type values, since those are only checked once instantaneously now


}
