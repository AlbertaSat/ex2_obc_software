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

// Make sure to define either "USE_UART" or "USE_I2C" in adcs_handler.c depending on which interface is being
// tested

#include "adcs_binary_test.h"

void binaryTest(void) { // TODO: add enums for all adcs_handler functions called

   ADCS_returnState test_returnState = ADCS_OK;

   //ADCS_reset();

//   printf("Enabling ADCS\n\n");
//   test_returnState = ADCS_set_enabled_state(1);
//   if (test_returnState != ADCS_OK)
//   {
//       printf("ADCS_set_enabled_state returned %d", test_returnState);
//       while(1);
//   }
//
//   adcs_state test_adcs_state;
//   printf("Running ADCS_get_current_state...\n");
//   test_returnState = ADCS_get_current_state(&test_adcs_state);
//   if (test_returnState != ADCS_OK) {
//       printf("ADCS_get_current_state returned %d \n", test_returnState);
//       while (1)
//           ;
//   }
//   printf("ADCS run mode = %d\n\n", test_adcs_state.run_mode);
//
//
//   // Power Control : CubeControl Signal and/or Motor Power = On (1), All others = Off (0)
//   uint8_t control[10] = {0};
//
//   // Verify Power State(s)
//   printf("Running ADCS_get_power_control...\n");
//   test_returnState = ADCS_get_power_control(control);
//   if (test_returnState != ADCS_OK) {
//       printf("ADCS_get_power_control returned %d \n", test_returnState);
//       while (1);
//   }
//   for (int i = 0; i < 10; i++) {
//       printf("control[%d] = %d \n", i, control[i]);
//   }
//
//   control[Set_CubeCTRLSgn_Power] = 1;
//   control[Set_CubeCTRLMtr_Power] = 1;
//
//   test_returnState = ADCS_set_power_control(control);
//   if (test_returnState != ADCS_OK) {
//       printf("ADCS_set_power_control returned %d \n", test_returnState);
//       while (1);
//   }
//

    // Bootloader command test
    // commandsTest_bootloader();


    //* ACP telemetry command test:
    // Acquires CubeACP State flags
    // Fires magnetorquer X at max duty cycle
    // Gets commanded actuator
    // commandsTest_ACP_telemetry();


    //* Attitude angle command test
    // Will Get-Set-Get commannded attitude
    // commandsTest_attitude();


    //* Concurrent logging test:
    // Will attempt logging using both LOG1 and LOG2
    // at different periods and w/ diff telemetry
    // for 1 min
     commandsTest_logtest();


    //* Configuration commands test:
    // Get default CSS Config, MTQ Config, and Wheel Configs
    // Modify CSS Config, MTQ Config, and Wheel Configs
    // Make sure to keep track of what you change!
    // Save new configurations 
    // Reset ADCS
    // Wait 6s for bootloader to finish
    // Re-enable ADCS
    // Verify modified configs
    // commandsTest_configs();


//    printf("Turning off CubeCTRL Signal/Motor\n");
//    control[Set_CubeCTRLSgn_Power] = 0;
//    control[Set_CubeCTRLMtr_Power] = 0;
//
//    test_returnState = ADCS_set_power_control(control);
//    if (test_returnState != ADCS_OK) {
//        printf("ADCS_set_power_control returned %d \n", test_returnState);
//        while (1);
//    }

    // TODO: add checks for "incrementing" and "idle" type values, since those are only checked once
    // instantaneously now
}

void binaryTest_Bootloader(void) {
   // Test corresponds to Section 4.1 Bootloader, Table 4-1 in test plan.
   // Test must be executed within 5 seconds of turning on ADCS
   ADCS_returnState test_returnState = ADCS_OK;
   uint16_t uptime = 0;
   uint8_t flags_arr[12];

   printf("Running ADCS_get_bootloader_state...\n");
   test_returnState = ADCS_get_bootloader_state(&uptime, flags_arr);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_bootloader_state returned %d \n", test_returnState);
       while (1)
           ;
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

   // Continuing test Section 4.1 Bootloader, Table 4-1 in test plan.
   uint8_t mcu_reset_cause = 0;
   uint8_t boot_cause = 0;
   uint16_t boot_count = 0;
   uint8_t boot_idx = 0;
   uint8_t major_firm_ver = 0;
   uint8_t minor_firm_ver = 0;

   printf("Running ADCS_get_boot_program_stat...\n");
   test_returnState = ADCS_get_boot_program_stat(&mcu_reset_cause, &boot_cause, &boot_count, &boot_idx,
                                                 &major_firm_ver, &minor_firm_ver);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_boot_program_stat returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("mcu_reset_cause = %d \n", mcu_reset_cause);
   printf("boot_cause = %d \n", boot_cause);
   printf("boot_count = %d \n", boot_count);
   printf("boot_idx = %d \n\n", boot_idx);
   printf("major_firm_ver = %d \n", major_firm_ver);
   printf("minor_firm_ver = %d \n\n", minor_firm_ver);

   // Continuing test Section 4.1 Bootloader, Table 4-1 in test plan.
   uint8_t program_idx = 0;
   uint8_t boot_stat = 0;

   printf("Running ADCS_get_boot_index...\n");
   test_returnState = ADCS_get_boot_index(&program_idx, &boot_stat);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_boot_index returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("program_idx = %d \n", program_idx);
   printf("boot_stat = %d \n", boot_stat);

   // Continuing test Section 4.1 Bootloader, Table 4-1 in test plan.
   uint16_t TC_num = 0;
   uint16_t TM_num = 0;
   memset(&flags_arr, 0, 12);

   printf("Running ADCS_get_comms_stat...\n");
   test_returnState = ADCS_get_comms_stat(&TC_num, &TM_num, flags_arr); // TODO: fix this
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_comms_stat returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("TC_num = %d \n", TC_num);
   printf("TM_num = %d \n", TM_num);
   printf("flags: %d %d %d %d %d %d", flags_arr[0], flags_arr[1], flags_arr[2], flags_arr[3], flags_arr[4],
          flags_arr[5]);

   printf("\n");
}

void binaryTest_CubeACP(void) {

   // Test Section 4.2 CubeACP, Table 4-2 in test plan.
   uint16_t time = 0;
   uint8_t execution_point = 0;
   ADCS_returnState test_returnState = ADCS_OK;

   printf("ADCS_get_ACP_loop_stat...\n");
   test_returnState = ADCS_get_ACP_loop_stat(&time, &execution_point);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_comms_stat returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("time = %d \n", time);
   printf("execution_point = %d \n", execution_point);

   // Continuing test Section 4.2 CubeACP, Table 4-2 in test plan.
   uint8_t mcu_reset_cause = 0;
   uint8_t boot_cause = 0;
   uint16_t boot_count = 0;
   uint8_t boot_idx = 0;
   uint8_t major_firm_ver = 0;
   uint8_t minor_firm_ver = 0;

   printf("Running ADCS_get_boot_program_stat...\n");
   test_returnState = ADCS_get_boot_program_stat(&mcu_reset_cause, &boot_cause, &boot_count, &boot_idx,
                                                 &major_firm_ver, &minor_firm_ver);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_boot_program_stat returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("mcu_reset_cause = %d \n", mcu_reset_cause);
   printf("boot_cause = %d \n", boot_cause);
   printf("boot_count = %d \n", boot_count);
   printf("boot_idx = %d \n\n", boot_idx);
   printf("major_firm_ver = %d \n", major_firm_ver);
   printf("minor_firm_ver = %d \n\n", minor_firm_ver);

   // Continuing test Section 4.2 CubeACP, Table 4-2 in test plan.
   uint8_t node_type = 0;
   uint8_t interface_ver = 0;
   major_firm_ver = 0;
   minor_firm_ver = 0;
   uint16_t runtime_s = 0;
   uint16_t runtime_ms = 0;

   printf("Running ADCS_get_node_identification...\n");
   test_returnState = ADCS_get_node_identification(&node_type, &interface_ver, &major_firm_ver, &minor_firm_ver,
                                                   &runtime_s, &runtime_ms);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_node_identification returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("node_type = %d \n", node_type);
   printf("interface_ver = %d \n", interface_ver);
   printf("runtime_s = %d \n", runtime_s);
   printf("runtime_ms = %d \n\n", runtime_ms);

   // Continuing test Section 4.2 CubeACP, Table 4-2 in test plan.
   uint8_t program_idx = 0;
   uint8_t boot_stat = 0;

   printf("Running ADCS_get_boot_index...\n");
   test_returnState = ADCS_get_boot_index(&program_idx, &boot_stat);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_boot_index returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("program_idx = %d \n", program_idx);
   printf("boot_stat = %d \n", boot_stat);

   // Continuing test Section 4.2 CubeACP, Table 4-2 in test plan.
   uint16_t TC_num = 0;
   uint16_t TM_num = 0;
   uint8_t flags_arr = 0;

   printf("Running ADCS_get_comms_stat...\n");
   test_returnState = ADCS_get_comms_stat(&TC_num, &TM_num, &flags_arr);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_comms_stat returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("TC_num = %d \n", TC_num);
   printf("TM_num = %d \n", TM_num);
   printf("flags_arr (decimal form, not binary) = %d \n", flags_arr);

   // Continuing test Section 4.2 CubeACP, Table 4-2 in test plan.
   uint32_t unix_t = 0;
   uint16_t count_ms = 0;

   printf("Running ADCS_get_unix_t...\n");
   test_returnState = ADCS_get_unix_t(&unix_t, &count_ms);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_unix_t returned %d \n", test_returnState);
       while (1)
           ;
   }
   printf("unix_t = %d \n", unix_t);
   printf("count_ms = %d \n", count_ms);

   // Continuing test Section 4.2 CubeACP, Table 4-2 in test plan.
   uint16_t sram1 = 0;
   uint16_t sram2 = 0;

   printf("Running ADCS_get_SRAM_latchup_count...\n");
   test_returnState = ADCS_get_SRAM_latchup_count(&sram1, &sram2);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_SRAM_latchup_count %d \n", test_returnState);
       while (1)
           ;
   }

   printf("sram1 = %d \n", sram1);
   printf("sram2 = %d \n", sram2);

   // Continuing test Section 4.2 CubeACP, Table 4-2 in test plan.
   uint16_t single_sram = 0;
   uint16_t double_sram = 0;
   uint16_t multi_sram = 0;

   printf("Running ADCS_get_EDAC_err_count...\n");
   test_returnState = ADCS_get_EDAC_err_count(&single_sram, &double_sram, &multi_sram);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_EDAC_err_count %d \n", test_returnState);
       while (1)
           ;
   }

   printf("single_sram = %d \n", single_sram);
   printf("double_sram = %d \n", double_sram);
   printf("multi_sram = %d \n", multi_sram);

   // Test Section 4.2 CubeACP, Table 4-3 in test plan.
   uint8_t *control = (uint8_t *)pvPortMalloc(10);
   if (control == NULL) {
       return ADCS_MALLOC_FAILED;
   }

   printf("Running ADCS_get_power_control...\n");
   test_returnState = ADCS_get_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
   for (int i = 0; i < 10; i++) {
       printf("control[%d] = %d \n", i, control[i]);
   }

   vPortFree(control);

   // Test Section 4.2 CubeACP, Table 4-3 in test plan.
   adcs_state test_adcs_state; // init as 0?

   printf("Running ADCS_get_current_state...\n");
   test_returnState = ADCS_get_current_state(&test_adcs_state);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("att_estimate mode = %d \n", test_adcs_state.att_estimate_mode);
   printf("att_ctrl_mode = %d \n", test_adcs_state.att_ctrl_mode);
   printf("run_mode = %d \n", test_adcs_state.run_mode);

   // need to test if all other flags == 0. Simpler to do in code than via human.
   uint8_t all_other_adcs_states_equal_zero = 0;
   for (int i = 0; i < 36; i++) { // I think this is the right range.
       if (test_adcs_state.flags_arr[i] != 0) {
           break;
       }
       if (i == 35) {
           all_other_adcs_states_equal_zero = 1;
       }
   }
   if (all_other_adcs_states_equal_zero == 1) {
       printf("all other states (frame offsets 12 to 47) == 0 \n");
   } else {
       printf("all other states (frame offsets 12 to 47) != 0... halting code execution\n");
       while (1)
           ;
   }

   // Enable ADCS
   printf("Running ADCS_set_enabled_state to set 1Hz loop (enabled)...\n");
   test_returnState = ADCS_set_enabled_state(1);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_enabled_state returned %d \n", test_returnState);
       while (1)
           ;
   }

   // Test Section 4.2 CubeACP, Table 4-4 in test plan.
   TC_num = 0;
   TM_num = 0;
   flags_arr = 0;

   printf("Running ADCS_get_comms_stat...\n");
   test_returnState = ADCS_get_comms_stat(&TC_num, &TM_num, &flags_arr);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_comms_stat returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("TC_num = %d \n", TC_num);

   // Test Section 4.2 CubeACP, Table 4-4 in test plan.
   uint16_t adcs_update = 0;
   uint16_t sensor_comms = 0;
   uint16_t sgp4_propag = 0;
   uint16_t igrf_model = 0;

   printf("Running ADCS_get_execution_times...\n");
   test_returnState = ADCS_get_execution_times(&adcs_update, &sensor_comms, &sgp4_propag, &igrf_model);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_execution_times returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("adcs_update = %d \n", adcs_update);
   printf("sensor_comms = %d \n", sensor_comms);
   printf("sgp4_propag = %d \n", sgp4_propag);
   printf("igrf_model = %d \n", igrf_model);

   // need to test if all other flags (with some exceptions) == 0. Simpler to do in code than via human.
   all_other_adcs_states_equal_zero = 0;
   for (int i = 0; i < 36; i++) { // I think this is the right range.
       if (i == 20) {             // Magnetometer Range Error
           // skip index
       } else if (i == 24) { // cam2 Sensor Detection Error
           // skip index
       } else if (i == 25) { // cam2 Sensor Range Error
           // skip index
       } else if (i == 29) { // cam2 Sensor Detection Error
           // skip index
       } else if (i == 30) { // Cam2 Sensor Range Error
           // skip index
       } else if (i == 33) { // Coarse Sun Sensor Error
           // skip index
       } else {
           if (test_adcs_state.flags_arr[i] != 0) {
               break;
           }
           if (i == 35) {
               all_other_adcs_states_equal_zero = 1;
           }
       }
   }
   if (all_other_adcs_states_equal_zero == 1) {
       printf("all other states** (frame offsets 12 to 47) == 0 \n");
   } else {
       printf("all other states** (frame offsets 12 to 47) != 0... halting code execution\n");
       while (1)
           ;
   }

   // Test Section 4.2 CubeACP, Table 4-4 in test plan.
   memset(&test_adcs_state, 0, sizeof(adcs_state));

   printf("Running ADCS_get_current_state...\n");
   test_returnState = ADCS_get_current_state(&test_adcs_state);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("att_estimate mode = %d \n", test_adcs_state.att_estimate_mode);
   printf("att_ctrl_mode = %d \n", test_adcs_state.att_ctrl_mode);
   printf("run_mode = %d \n", test_adcs_state.run_mode);
   printf("Sun is Above Local horizon = %d \n", test_adcs_state.flags_arr[11]);

   printf("\n");
}

void binaryTest_CubeSense1(void) {

   // Test Section 5.1 CubeSense, Table 5-1 in test plan.
   // Using Command ADCS_get_power_control() - Table 184, ensure that all nodes are selected PowOff before
   // proceeding.
   ADCS_returnState test_returnState = ADCS_OK;
   adcs_state test_adcs_state; // init as 0?
   printf("Running ADCS_get_current_state...\n");
   test_returnState = ADCS_get_current_state(&test_adcs_state);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }

   // Section Variables
   uint8_t *control = (uint8_t *)pvPortMalloc(10);
   if (control == NULL) {
       return ADCS_MALLOC_FAILED;
   }

   // enable the ADCS
   ADCS_set_enabled_state(1);

   ADCS_set_unix_t(0, 0);

   printf("Running ADCS_get_power_control...\n");
   test_returnState = ADCS_get_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
   for (int i = 0; i < 10; i++) {
       printf("control[%d] = %d \n", i, control[i]);
   }

   // Using Command ADCS_set_power_control() - Table 184, switch on CubeSense1 by selecting PowOn.
   // Section Variables
   if (control == NULL) {
       return ADCS_MALLOC_FAILED;
   }
   control[Set_CubeSense1_Power] = 1;

   printf("Running ADCS_set_power_control...\n");
   test_returnState = ADCS_set_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }

   // another read to make sure we are in the right state
   printf("Running ADCS_get_power_control...\n");
   test_returnState = ADCS_get_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
   for (int i = 0; i < 10; i++) {
       printf("control[%d] = %d \n", i, control[i]);
   }

   vPortFree(control);

   // Ensure that the lens cap is on.

   // Using Command ADCS_set_cubesense_config() - Table 189, ensure that the Cam1 detection threshold is set to
   // 150. Next, ensure that the Cam1 sensor exposure time is set to 35 if Cam1 is a nadir  sensor, or 100 if Cam1
   // is a Sun sensor. Set the NadirMaxBadEdges to 30 to make  ground testing easier. Section Variables]
   cubesense_config params;

   // Get all the current configuration parameters from the ADCS so that incorrect parameters aren't sent while
   // testing.
   printf("Running ADCS_get_cubesense_config...\n");
   test_returnState = ADCS_get_cubesense_config(
       &params); // this function should be tested and checked before the command is sent
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_cubesense_config returned %d \n", test_returnState);
       while (1)
           ;
   }

   // now set the parameters
   params.cam1_sense.detect_th = 150;
   params.cam1_sense.exposure_t = 100; // assuming Cam1 is a nadir sensor. If a sun sensor, use 100.
   params.nadir_max_bad_edge = 30;
   params.cam1_sense.boresight_x = 512;
   params.cam1_sense.boresight_y = 512;
   //
   printf("Running ADCS_set_cubesense_config...\n");

   test_returnState =
       ADCS_set_cubesense_config(params); // this function should be tested and checked before the command is sent
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_cubesense_config returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("Running ADCS_get_cubesense_config...\n");
   test_returnState = ADCS_get_cubesense_config(
       &params); // this function should be tested and checked before the command is sent
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_cubesense_config returned %d \n", test_returnState);
       while (1)
           ;
   }
   // this is commented out because the dev board reads the wrong error state

   // Verify the following values in Table 5-1:

   printf("Running ADCS_get_current_state...\n");
   test_returnState = ADCS_get_current_state(&test_adcs_state);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("att_estimate mode = %d \n", test_adcs_state.att_estimate_mode);
   printf("att_ctrl_mode = %d \n", test_adcs_state.att_ctrl_mode);
   printf("run_mode = %d \n", test_adcs_state.run_mode);
   printf("CubeSense1 Enabled = %d \n", test_adcs_state.flags_arr[2]);
   printf("Sun is Above Local Horizon = %d \n", test_adcs_state.flags_arr[11]);

   // need to test if all flags other than CubeSense1 Enabled and Sun is Above Local Horizon are == 0. Simpler to
   // do in code than via human.
   //    uint8_t all_other_adcs_states_equal_zero = 0;
   //    for(int i = 0; i<36; i++){//I think this is the right range.
   //        if(((i == 2) | (i == 11)) & (test_adcs_state.flags_arr[i] != 0)){
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

   // ADCS_get_power_temp()
   adcs_pwr_temp *power_temp_measurements;
   power_temp_measurements = (adcs_pwr_temp *)pvPortMalloc(sizeof(adcs_pwr_temp));
   if (power_temp_measurements == NULL) {
       printf("malloc issues");
       while (1)
           ;
   }

   printf("Running ADCS_get_power_temp...\n");
   test_returnState = ADCS_get_power_temp(power_temp_measurements);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_temp returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("cubesense1_3v3_I = %f \n", power_temp_measurements->cubesense1_3v3_I);
   printf("cubesense1_camSram_I = %f \n", power_temp_measurements->cubesense1_camSram_I);

   vPortFree(power_temp_measurements);

   // ADCS_get_raw_sensor()
   adcs_raw_sensor *raw_sensor_measurements;
   raw_sensor_measurements = (adcs_raw_sensor *)pvPortMalloc(sizeof(adcs_raw_sensor));
   if (raw_sensor_measurements == NULL) {
       printf("malloc issues");
       while (1)
           ;
   }
   printf("Running ADCS_get_raw_sensor...\n");
   test_returnState = ADCS_get_raw_sensor(raw_sensor_measurements);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_raw_sensor returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("cam1.centroid_x = %d \n", raw_sensor_measurements->cam1.centroid_x);
   printf("cam1.centroid_y = %d \n", raw_sensor_measurements->cam1.centroid_y);
   printf("cam1.capture_stat = %d \n", raw_sensor_measurements->cam1.capture_stat);
   printf("cam1.detect_result = %d \n", raw_sensor_measurements->cam1.detect_result);

       //Take off the Cam1 cameraÃ¯Â¿Â½s lens cap.
       //Verify the following in Table 5-2 by testing the sensor with a light source (a dark n environment will
       //prevent  false detections).
       //If Cam1 is a nadir sensor then a large light source should be used  (e.g. a desk lamp), or if Cam1 is a
       //Sun sensor then a small light
       //source should be used  (e.g. narrow beam flashlight). Vary the distance between the light source and
       //the sensor  until consistent measurements
       //are observed (normally Ã¯Â¿Â½150mm). If difficulties are  experienced with the nadir sensor, the light
       //source can be covered with white  paper/cloth
       //to create a more uniform light source. Finally, if no results are obtained for  the nadir or Sun
       //sensors, the exposure value can be adjusted.
    
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

   // While keeping the light in the field of view of CubeSense1, use Command ADCS_save_img() - Table 94 and
   // select Cam1. Select any desired size for Image size, but not selecting Size0 will reduce the downloading
   // time by lowering the image quality (Size3 recommended). Capture the image by sending the command by clicking
   // on the green arrow. The camera will  capture an image after a delay of three seconds. Continue to hold the
   // light in front of  the camera for this duration.

   // ADCS_save_img()
   uint8_t camera = 0;
   uint8_t img_size = 3;

   printf("Running ADCS_save_img...\n");
   test_returnState = ADCS_save_img(camera, img_size);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_save_img returned %d \n", test_returnState);
       while (1)
           ;
   }

   // Using Command ADCS_get_img_save_progress() - Table 176, refresh Percentage Complete, which will increase
   // slowly and indicate the progress of the image being saved to the SD card from CubeSenseÃ¯Â¿Â½s memory.

   // ADCS_get_img_save_progress() - to run almost immediately after the image is taken
   uint8_t percentage = 0;
   uint8_t status = 0;

   printf("Running ADCS_get_img_save_progress...\n");
   test_returnState = ADCS_get_img_save_progress(&percentage, &status);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_img_save_progress returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("percentage = %d \n", percentage);
   printf("status = %d \n", status);

   // ADCS_get_img_save_progress() - to run a little while after the previous function call.
   percentage = 0;
   status = 0;

   printf("Running ADCS_get_img_save_progress...\n");
   test_returnState = ADCS_get_img_save_progress(&percentage, &status);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_img_save_progress returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("percentage = %d \n", percentage);
   printf("status = %d \n", status);

   // Steps to take to download the image file that was just created:
   // First, get the file list
   test_returnState = ADCS_get_file_list();
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_file_list returned %d \n", test_returnState);
       // while(1);
   }

   test_returnState = ADCS_download_file(0, 0);
}

void binaryTest_CubeSense2(void) {

   // Test Section 5.2 CubeSense, Table 5-2 in test plan.
   // Using Command ADCS_get_power_control() - Table 184, ensure that all nodes are selected PowOff before
   // proceeding.
   ADCS_returnState test_returnState = ADCS_OK;

   // Section Variables
   uint8_t *control = (uint8_t *)pvPortMalloc(10);
   if (control == NULL) {
       return ADCS_MALLOC_FAILED;
   }

   // enable the ADCS
   ADCS_set_enabled_state(1);
   ADCS_set_unix_t(0, 0);

   printf("Running ADCS_get_power_control...\n");
   test_returnState = ADCS_get_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
   for (int i = 0; i < 10; i++) {
       printf("control[%d] = %d \n", i, control[i]);
   }

   // Using Command ADCS_set_power_control() - Table 184, switch on CubeSense2 by selecting PowOn.
   // Section Variables
   control[Set_CubeSense1_Power] = 0; // ensures that cam1 is off
   control[Set_CubeSense2_Power] = 1;

   printf("Running ADCS_set_power_control...\n");
   test_returnState = ADCS_set_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }

   vPortFree(control);

   // Ensure that the lens cap is on.

   // Using Command ADCS_set_cubesense_config() - Table 189, ensure that the cam2 detection threshold is set to
   // 150. Next, ensure that the cam2 sensor exposure time is set to 35 if Cam2 is a nadir  sensor, or 100 if Cam2
   // is a Sun sensor. Set the NadirMaxBadEdges to 30 to make  ground testing easier. Section Variables
   cubesense_config params;
   // Get all the current configuration parameters from the ADCS so that incorrect parameters aren't sent while
   // testing.
   printf("Running ADCS_get_cubesense_config...\n");
   test_returnState = ADCS_get_cubesense_config(&params);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_cubesense_config returned %d \n", test_returnState);
       while (1)
           ;
   }

   // now set the parameters
   params.cam2_sense.detect_th = 150;
   params.cam2_sense.exposure_t = 35; // assuming Cam2 is a nadir sensor. If a sun sensor, use 100.
   params.nadir_max_bad_edge = 30;

   printf("Running ADCS_set_cubesense_config...\n");
   test_returnState =
       ADCS_set_cubesense_config(params); // this function should be tested and checked before the command is sent
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_cubesense_config returned %d \n", test_returnState);
       while (1)
           ;
   }

   // Verify the following values in Table 5-1:
   adcs_state test_adcs_state; // init as 0?

   printf("Running ADCS_get_current_state...\n");
   test_returnState = ADCS_get_current_state(&test_adcs_state);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("att_estimate mode = %d \n", test_adcs_state.att_estimate_mode);
   printf("att_ctrl_mode = %d \n", test_adcs_state.att_ctrl_mode);
   printf("run_mode = %d \n", test_adcs_state.run_mode);
   printf("CubeSense2 Enabled = %d \n", test_adcs_state.flags_arr[2]);
   printf("Sun is Above Local Horizon = %d \n", test_adcs_state.flags_arr[11]);
   // need to test if all flags other than CubeSense2 Enabled and Sun is Above Local Horizon are == 0. Simpler to
   // do in code than via human.
   uint8_t all_other_adcs_states_equal_zero = 0;
   for (int i = 0; i < 36; i++) { // I think this is the right range.
       if (((i == 2) | (i == 11)) & (test_adcs_state.flags_arr[i] != 0)) {
           break;
       }
       if (i == 35) {
           all_other_adcs_states_equal_zero = 1;
       }
   }
   if (all_other_adcs_states_equal_zero == 1) {
       printf("all other states (frame offsets 12 to 47) == 0 \n");
   } else {
       printf("all other states (frame offsets 12 to 47) != 0... halting code execution\n");
       while (1)
           ;
   }

   // ADCS_get_power_temp()
   adcs_pwr_temp power_temp_measurements;

   printf("Running ADCS_get_power_temp...\n");
   test_returnState = ADCS_get_power_temp(&power_temp_measurements);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_temp returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("cubesense2_3v3_I = %f \n", power_temp_measurements.cubesense2_3v3_I);
   printf("cubesense2_camSram_I = %f \n", power_temp_measurements.cubesense2_camSram_I);

   // ADCS_get_raw_sensor()
   adcs_raw_sensor raw_sensor_measurements;

   printf("Running ADCS_get_raw_sensor...\n");
   test_returnState = ADCS_get_raw_sensor(&raw_sensor_measurements);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_raw_sensor returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("cam2.centroid_x = %d \n", raw_sensor_measurements.cam2.centroid_x);
   printf("cam2.centroid_y = %d \n", raw_sensor_measurements.cam2.centroid_y);
   printf("cam2.capture_stat = %d \n", raw_sensor_measurements.cam2.capture_stat);
   printf("cam2.detect_result = %d \n", raw_sensor_measurements.cam2.detect_result);

   // Take off the cam2 cameraï¿½s lens cap.
   // Verify the following in Table 5-2 by testing the sensor with a light source (a dark n environment will
   // prevent  false detections). If cam2 is a nadir sensor then a large light source should be used  (e.g. a desk
   // lamp), or if cam2 is a Sun sensor then a small light source should be used  (e.g. narrow beam flashlight).
   // Vary the distance between the light source and the sensor  until consistent measurements are observed
   // (normally ï¿½150mm). If difficulties are  experienced with the nadir sensor, the light source can be covered
   // with white  paper/cloth to create a more uniform light source. Finally, if no results are obtained for  the
   // nadir or Sun sensors, the exposure value can be adjusted.

   // ADCS_get_raw_sensor() LIGHT BROUGHT CLOSE TO THE CAMERA
//   printf("Running ADCS_get_raw_sensor...\n");
//   test_returnState = ADCS_get_raw_sensor(&raw_sensor_measurements);
//   if (test_returnState != ADCS_OK) {
//       printf("ADCS_get_raw_sensor returned %d \n", test_returnState);
//       while (1)
//           ;
//   }
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
   //    //While keeping the light in the field of view of CubeSense1, use Command ADCS_save_img() - Table 94 and
   //    select cam2.
   //    //Select any desired size for Image size, but not selecting Size0 will reduce the downloading time by
   //    lowering the image quality (Size3 recommended).
   //    //Capture the image by sending the command by clicking on the green arrow. The camera will  capture an
   //    image after a delay of three seconds.
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
   //    //Using Command ADCS_get_img_save_progress() - Table 176, refresh Percentage Complete, which will
   //    increase slowly and indicate the progress of
   //    //the image being saved to the SD card from CubeSenseï¿½s memory.
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
   // Test Section 6.1 CubeControl, Table 6-1,2 in test plan.
   uint8_t *control = (uint8_t *)pvPortMalloc(10);
   if (control == NULL) {
       return ADCS_MALLOC_FAILED;
   }

   // enable the ADCS
   ADCS_set_enabled_state(1);

   ADCS_set_unix_t(0, 0);

   // Using Command ADCS_get_power_control() - Table 184, ensure that all nodes are selected PowOff before
   // proceeding.
   printf("Running ADCS_get_power_control...\n");
   test_returnState = ADCS_get_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
   for (int i = 0; i < 10; i++) {
       printf("control[%d] = %d \n", i, control[i]);
   }

   // Using Command ADCS_set_power_control() - Table 184, switch on CubeControl Signal MCU by selecting PowOn.
   // Section Variables
   if (control == NULL) {
       return ADCS_MALLOC_FAILED;
   }
   control[Set_CubeCTRLSgn_Power] = 1;

   printf("Running ADCS_set_power_control...\n");
   test_returnState = ADCS_set_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }

   // another read to make sure we are in the right state
   printf("Running ADCS_get_power_control...\n");
   test_returnState = ADCS_get_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
   for (int i = 0; i < 10; i++) {
       printf("control[%d] = %d \n", i, control[i]);
   }

   vPortFree(control);

   adcs_state test_adcs_state;
   // Run ADCS_get_current_state()
   printf("Running ADCS_get_current_state...\n");
   test_returnState = ADCS_get_current_state(&test_adcs_state);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("att_estimate mode = %d \n", test_adcs_state.att_estimate_mode);
   printf("att_ctrl_mode = %d \n", test_adcs_state.att_ctrl_mode);
   printf("run_mode = %d \n", test_adcs_state.run_mode);
   printf("CubeControl Signal Enabled = %d \n", test_adcs_state.flags_arr[0]);

   // need to test if all other flags == 0. Simpler to do in code than via human.
   //  CHANGED TO A MANUAL HUMAN CHECK AT THIS POINT
   uint8_t all_other_adcs_states_equal_zero = 1;
   //    for(int i = 0; i<36; i++){//I think this is the right range.
   //        if(!((i == 0)) && test_adcs_state.flags_arr[i] != 0){
   //            break;
   //        }
   //        if(i == 35){
   //            all_other_adcs_states_equal_zero = 1;
   //        }
   //    }
   if (all_other_adcs_states_equal_zero == 1) {
       printf("all other states (frame offsets 12 to 47) == 0 \n");
   } else {
       printf("all other states (frame offsets 12 to 47) != 0... halting code execution\n");
       while (1)
           ;
   }

   // ADCS_get_power_temp()
   adcs_pwr_temp *power_temp_measurements;
   power_temp_measurements = (adcs_pwr_temp *)pvPortMalloc(sizeof(adcs_pwr_temp));
   if (power_temp_measurements == NULL) {
       printf("malloc issues");
       while (1)
           ;
   }

   printf("Running ADCS_get_power_temp...\n");
   test_returnState = ADCS_get_power_temp(power_temp_measurements);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_temp returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("cubecontrol_3v3_I = %f \n", power_temp_measurements->cubecontrol_3v3_I);
   printf("cubecontrol_5v_I = %f \n", power_temp_measurements->cubecontrol_5v_I);
   printf("cubecontrol_vBat_I = %f \n", power_temp_measurements->cubecontrol_vBat_I);
   printf("MCU_temp = %f \n", power_temp_measurements->MCU_temp);
   printf("MTM_temp = %f \n", power_temp_measurements->MTM_temp);
   printf("MTM2_temp = %f \n", power_temp_measurements->MTM2_temp);

   vPortFree(power_temp_measurements);

   for (int iteration = 0; iteration < 12; iteration++) {
       // ADCS_get_raw_sensor()

       // Second time around, expose the coarse Sun sensors to a bright light, one by one.
       // Verify the following in Table 6-2:
       adcs_raw_sensor *raw_sensor_measurements;
       raw_sensor_measurements = (adcs_raw_sensor *)pvPortMalloc(sizeof(adcs_raw_sensor));
       if (raw_sensor_measurements == NULL) {
           printf("malloc issues");
           while (1)
               ;
       }
       printf("Running ADCS_get_raw_sensor...\n");
       test_returnState = ADCS_get_raw_sensor(raw_sensor_measurements);
       if (test_returnState != ADCS_OK) {
           printf("ADCS_get_raw_sensor returned %d \n", test_returnState);
           while (1)
               ;
       }

       for (int i = 0; i < 10; i++) {
           printf("CSS%d = %d \n", i, raw_sensor_measurements->css[i]);
       }
   }
}

void binaryTest_CubeMag_Sgn_MCU(void) {
   // enable the ADCS
   ADCS_set_enabled_state(1);
   ADCS_set_unix_t(0, 0);

   CubeMag_Common_Test(1);
}

void binaryTest_CubeTorquers_Sgn_MCU(void) {

   // Test Section 6.1.3 CubeMag, Table 6-5 in test plan.

   // Using Command ADCS_set_magnetorquer_output() - Table 81. Command the magnetorquer coil (X-axis) to maximum
   // positive value.
   ADCS_returnState test_returnState = ADCS_OK;
   printf("Running ADCS_set_attitude_ctrl_mode...\n");
   test_returnState = ADCS_set_attitude_ctrl_mode(0, 0XFFFF); // no timeout (infinite time)
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_attitude_ctrl_mode returned %d \n", test_returnState);
       while (1)
           ;
   }

   CubeTorquers_Common_Test();

   // Using Command ADCS_set_power_control() - Table 184, switch off CubeControl Signal MCU by selecting PowOff.
   uint8_t *control = (uint8_t *)pvPortMalloc(10);
   if (control == NULL) {
       return ADCS_MALLOC_FAILED;
   }

   for (int i = 0; i < 10; i++) {
       control[i] = 0;
   }
   // I assume 0 = PowOff?

   printf("Running ADCS_set_power_control...\n");
   test_returnState = ADCS_set_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
}

void binaryTest_CubeControl_Motor_MCU(void) {
   // Test Section 6.2 CubeControl, Table 6-6 in test plan.

   ADCS_returnState test_returnState = ADCS_OK;

   ADCS_set_enabled_state(1);

   ADCS_set_unix_t(0, 0);

   // Using Command ADCS_get_power_control() - Table 184, ensure that all nodes are selected PowOff before
   // proceeding. Section Variables
   uint8_t *control = (uint8_t *)pvPortMalloc(10);
   if (control == NULL) {
       return ADCS_MALLOC_FAILED;
   }

   printf("Running ADCS_get_power_control...\n");
   test_returnState = ADCS_get_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
   for (int i = 0; i < 10; i++) {
       printf("control[%d] = %d \n", i, control[i]);
   }

   // Using Command ADCS_set_power_control() - Table 184, switch on CubeControlï¿½s Motor MCU by selecting PowOn.
   // Section Variables
   if (control == NULL) {
       return ADCS_MALLOC_FAILED;
   }
   control[Set_CubeCTRLMtr_Power] = 1;

   printf("Running ADCS_set_power_control...\n");
   test_returnState = ADCS_set_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }

   // another read to make sure we are in the right state
   printf("Running ADCS_get_power_control...\n");
   test_returnState = ADCS_get_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
   for (int i = 0; i < 10; i++) {
       printf("control[%d] = %d \n", i, control[i]);
   }

   vPortFree(control);

   // Verify the following in Table 6-6:

   adcs_state test_adcs_state;
   // Run ADCS_get_current_state()
   printf("Running ADCS_get_current_state...\n");
   test_returnState = ADCS_get_current_state(&test_adcs_state);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("att_estimate mode = %d \n", test_adcs_state.att_estimate_mode);
   printf("att_ctrl_mode = %d \n", test_adcs_state.att_ctrl_mode);
   printf("run_mode = %d \n", test_adcs_state.run_mode);
   printf("CubeControl Motor Enabled = %d \n", test_adcs_state.flags_arr[1]);

   // need to test if all other flags == 0. Simpler to do in code than via human.
   uint8_t all_other_adcs_states_equal_zero = 0;
   for (int i = 0; i < 36; i++) { // I think this is the right range.
       if (test_adcs_state.flags_arr[i] != 0) {
           break;
       }
       if (i == 35) {
           all_other_adcs_states_equal_zero = 1;
       }
   }
   if (all_other_adcs_states_equal_zero == 1) {
       printf("all other states (frame offsets 12 to 47) == 0 \n");
   } else {
       printf("all other states (frame offsets 12 to 47) != 0... halting code execution\n");
       // while(1);
   }

   // ADCS_get_power_temp()
   adcs_pwr_temp *power_temp_measurements;
   power_temp_measurements = (adcs_pwr_temp *)pvPortMalloc(sizeof(adcs_pwr_temp));
   if (power_temp_measurements == NULL) {
       printf("malloc issues");
       while (1)
           ;
   }

   printf("Running ADCS_get_power_temp...\n");
   test_returnState = ADCS_get_power_temp(power_temp_measurements);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_temp returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("X-Rate Sensor Temperature = %d \n", power_temp_measurements->rate_sensor_temp.x);
   printf("Y-Rate Sensor Temperature = %d \n", power_temp_measurements->rate_sensor_temp.y);
   printf("Z-Rate Sensor Temperature = %d \n", power_temp_measurements->rate_sensor_temp.z);

   vPortFree(power_temp_measurements);

   adcs_measures *measurements;
   measurements = (adcs_measures *)pvPortMalloc(sizeof(adcs_measures));
   if (measurements == NULL) {
       printf("malloc issues");
       while (1)
           ;
   }

   for (int i = 0; i < 15; i++) { // repeating 5 times for each axis = 15 times
       printf("Running ADCS_get_measurements...\n");
       test_returnState = ADCS_get_measurements(measurements);
       if (test_returnState != ADCS_OK) {
           printf("ADCS_get_measurements returned %d \n", test_returnState);
           while (1)
               ;
       }
       printf("Angular Rate X = %+f \n",
              measurements->angular_rate.x); // not 100% sure if this will print the sign of the float
       printf("Angular Rate Y = %+f \n", measurements->angular_rate.y);
       printf("Angular Rate Z = %+f \n", measurements->angular_rate.z);
   }

   vPortFree(measurements);
}

void binaryTest_CubeMag_Motor_MCU(void) {
   // Test Section 6.2.1 CubeMag, Table 6-7,8 in test plan.

   CubeMag_Common_Test(0);
}

void binaryTest_CubeTorquers_Motor_MCU(void) {
   // Test Section 6.2.2 CubeTorquers, Table 6-9 in test plan.

   CubeTorquers_Common_Test();
}

void binaryTest_CubeWheel_BurnIn_MCU(void) { // Omitted for current binary testing plan
   // Test Section 7 CubeControl, Table 7-1 in test plan.

   // Using Command ADCS_get_power_control() - Table 184, ensure that all nodes are selected PowOff before
   // proceeding.

   // Using Command ADCS_set_power_control() - Table 184, switch on CubeWheel1Power, CubeWheel2Power, and
   // CubeWheel3Power Power Selection by selecting PowOn.

   // Verify the following in Table 7-1
}

void binaryTest_CubeWheel1_MCU(void) {
   // Test Section 6.1 CubeControl, Table 6-1,2 in test plan.
   ReactionWheels_Common_Test(1);
}

void binaryTest_CubeWheel2_MCU(void) {
   // Test Section 6.1 CubeControl, Table 6-1,2 in test plan.
   ReactionWheels_Common_Test(2);
}

void binaryTest_CubeWheel3_MCU(void) {
   // Test Section 6.1 CubeControl, Table 6-1,2 in test plan.
   ReactionWheels_Common_Test(3);
}

void commissioning_initial_angular_rates_est(void)
{
   // Step 1 of 3-axis CubeADCS Commissioning
   // Determine Initial Angular Rates

   ADCS_returnState test_returnState = ADCS_OK;

   //* Command Sequence
   // ADCS Run Mode : State = Enabled (1)
   printf("Running ADCS_set_enabled_state to set 1Hz loop (enabled)...\n");
   test_returnState = ADCS_set_enabled_state(ONEHZ_LOOP_ENABLE);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_enabled_state returned %d \n", test_returnState);
       while (1)
           ;
   }

   // Power Control : CubeControl Signal and/or Motor Power = On (1), All others = Off (0)
   uint8_t control[10] = {0};

   // Verify Power State(s)
   printf("Running ADCS_get_power_control...\n");
   test_returnState = ADCS_get_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
   for (int i = 0; i < 10; i++) {
       printf("control[%d] = %d \n", i, control[i]);
   }

   control[Set_CubeCTRLSgn_Power] = 1;
   control[Set_CubeCTRLMtr_Power] = 1;

   test_returnState = ADCS_set_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }

   // Verify Power States again
   test_returnState = ADCS_get_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
   for (int i = 0; i < 10; i++) {
       printf("control[%d] = %d \n", i, control[i]);
   }

   // Set Estimation Mode : Mode = Magnetometer rate filter (2)
   test_returnState = ADCS_set_attitude_estimate_mode(MAG_RATE_FILTER_MODE);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }

   //* Telemetry Logging
   printf("Setting Telemetry Logging\n");

   uint8_t flags_arr[80] = {0};
   uint8_t *flags_arr_ptr = flags_arr;
   uint16_t period;
   uint8_t sd;

   flags_arr[54] = 1; // Estimated Angular Rates
   flags_arr[69] = 1; // Rate Sensor Rates
   flags_arr[65] = 1; // Magnetometer Measurement

   // Verify Bit Mask
   printf("0b");
   for (int i = 0; i < 80; i++) {
       printf("%d", flags_arr[i]);

       if ((i + 1) % 8 == 0) {
           printf(" ");
       }
   }
   printf("\n");

   test_returnState = ADCS_set_log_config(flags_arr_ptr, TLM_LOG_PERIOD_10s, TLM_LOG_SDCARD_0, TLM_LOG_1);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_log_config returned %d \n", test_returnState);
       while (1)
           ;
   }

   for(int k = 0; k<80; k++){
       flags_arr_ptr[k] = 0;
   }

   test_returnState = ADCS_get_log_config(flags_arr_ptr, &period, &sd, TLM_LOG_1);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_log_config returned %d \n", test_returnState);
       while (1)
           ;
   }

   // Verify Bit Mask again
   printf("0b");
   for (int i = 0; i < 80; i++) {
       printf("%d", flags_arr[i]);

       if ((i + 1) % 8 == 0) {
           printf(" ");
       }
   }
   printf("\n");

   // Stop logging after 1 minutes
   vTaskDelay(pdMS_TO_TICKS(60000));
   printf("Stopping Telemetry Logging\n");
   test_returnState = ADCS_set_log_config(flags_arr_ptr, TLM_LOG_PERIOD_STOP, TLM_LOG_SDCARD_0, TLM_LOG_1);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_log_config returned %d \n", test_returnState);
       while (1)
           ;
   }

   // Power All = Off(0)
   control[Set_CubeCTRLSgn_Power] = 0;
   control[Set_CubeCTRLMtr_Power] = 0;
   test_returnState = ADCS_set_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
}

void commissioning_initial_detumbling(void) {
   // Step 2 & 3 of ADCS Commissioning
   // Initial Detumbling

   ADCS_returnState test_returnState = ADCS_OK;

   //* Command Sequence
   // ADCS Run Mode : State = Enabled (1)
   printf("Running ADCS_set_enabled_state to set 1Hz loop (enabled)...\n");
   test_returnState = ADCS_set_enabled_state(ONEHZ_LOOP_ENABLE);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_enabled_state returned %d \n", test_returnState);
       while (1)
           ;
   }

   // Power Control : CubeControl Signal and/or Motor Power = On (1), All others = Off (0)
   uint8_t control[10] = {0};

   // Verify Power State(s)
   printf("Running ADCS_get_power_control...\n");
   test_returnState = ADCS_get_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
   for (int i = 0; i < 10; i++) {
       printf("control[%d] = %d \n", i, control[i]);
   }

   control[Set_CubeCTRLSgn_Power] = 1;
   control[Set_CubeCTRLMtr_Power] = 1;

   test_returnState = ADCS_set_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }

   // Verify Power States again
   test_returnState = ADCS_get_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }

   for (int i = 0; i < 10; i++) {
       printf("control[%d] = %d \n", i, control[i]);
   }

   // Set Estimation Mode : Mode = Magnetometer rate filter (2) or MEMS rate sensing (1)
   test_returnState = ADCS_set_attitude_estimate_mode(MEMS_RATE_SENSING_MODE);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }

   adcs_state test_adcs_state;
   printf("Running ADCS_get_current_state...\n");
   test_returnState = ADCS_get_current_state(&test_adcs_state);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }
   printf("att_ctrl_mode = %d \n", test_adcs_state.att_ctrl_mode);

   //* Telemetry Logging
   printf("Setting Telemetry Logging\n");

   uint8_t flags_arr[80] = {0};

   flags_arr[26] = 1; // Estimated Angular Rates
   flags_arr[10] = 1; // Rate Sensor Rates
   flags_arr[14] = 1; // Magnetometer Measurement
   flags_arr[0] = 1; // Magnetorquer Commands

   // Verify Bit Mask
   printf("0b");
   for (int i = 0; i < 80; i++) {
       printf("%d", flags_arr[i]);

       if ((i + 1) % 8 == 0) {
           printf(" ");
       }
   }
   printf("\n");

   test_returnState = ADCS_set_log_config(flags_arr, TLM_LOG_PERIOD_10s, TLM_LOG_SDCARD_0, TLM_LOG_1);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_log_config returned %d \n", test_returnState);
       while (1)
           ;
   }

   test_returnState = ADCS_get_log_config(flags_arr, TLM_LOG_PERIOD_10s, TLM_LOG_SDCARD_0, TLM_LOG_1);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_log_config returned %d \n", test_returnState);
       while (1)
           ;
   }

   // Verify Bit Mask again
   printf("0b");
   for (int i = 0; i < 80; i++) {
       printf("%d", flags_arr[i]);

       if ((i + 1) % 8 == 0) {
           printf(" ");
       }
   }
   printf("\n");

   // Initial Activation 1 (Detumbling, 600s)
   printf("Initial Activation 1 : Detumbling\n");
   printf("Running ADCS_set_attitude_ctrl_mode...\n");
   test_returnState = ADCS_set_attitude_ctrl_mode(DETUMBLING_MODE, 600);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_attitude_ctrl_mode returned %d \n", test_returnState);
       while (1)
           ;
   }

   // Verify Control Mode
   printf("Running ADCS_get_current_state...\n");
   test_returnState = ADCS_get_current_state(&test_adcs_state);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }
   printf("att_ctrl_mode = %d \n", test_adcs_state.att_ctrl_mode);

   // Begin verifying angular rates
   adcs_measures *measurements;
   measurements = (adcs_measures *)pvPortMalloc(sizeof(adcs_measures));
   if (measurements == NULL) {
       printf("malloc issues");
       while (1)
           ;
   }

   // Tilt ADCS 45 degrees about each axis for 12*5000ms = 1 min
   // Output rates on serial monitor every 5000ms
   for (int i = 0; i < 12; i++) {
       printf("Running ADCS_get_measurements...\n");
       test_returnState = ADCS_get_measurements(measurements);
       if (test_returnState != ADCS_OK) {
           printf("ADCS_get_measurements returned %d \n", test_returnState);
           while (1)
               ;
       }
       printf("Angular Rate X = %+f \n",
              measurements->angular_rate.x); // not 100% sure if this will print the sign of the float
       printf("Angular Rate Y = %+f \n", measurements->angular_rate.y);
       printf("Angular Rate Z = %+f \n", measurements->angular_rate.z);
       vTaskDelay(pdMS_TO_TICKS(5000));
   }

   free(measurements);

   printf("Stopping Telemetry Logging\n");
   test_returnState = ADCS_set_log_config(flags_arr, TLM_LOG_PERIOD_STOP, TLM_LOG_SDCARD_0, TLM_LOG_1);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_log_config returned %d \n", test_returnState);
       while (1)
           ;
   }

   //// Final Activation (Y-Thomson, 0s)
   //// printf("Final Activation : Y-Thomson\n")
   //// printf("Running ADCS_set_attitude_ctrl_mode...\n");
   //// test_returnState = ADCS_set_attitude_ctrl_mode(2, 0);//no timeout (infinite time)
   //// if(test_returnState != ADCS_OK){
   ////     printf("ADCS_set_attitude_ctrl_mode returned %d \n", test_returnState);
   ////     while(1);
   //// }

   //// Verify Control Mode
   //// printf("Running ADCS_get_current_state...\n");
   //// test_returnState = ADCS_get_current_state(&test_adcs_state);
   //// if(test_returnState != ADCS_OK){
   ////     printf("ADCS_get_current_state returned %d \n", test_returnState);
   ////     while(1);
   //// }
   //// printf("att_ctrl_mode = %d \n", test_adcs_state.att_ctrl_mode);

   // All = Off(0)
   control[Set_CubeCTRLSgn_Power] = 0;
   control[Set_CubeCTRLMtr_Power] = 0;
   test_returnState = ADCS_set_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
}

void commissioning_mag_calibration(void)
{
   // Step 5 of ADCS Commissioning
   // Magnetometer Calibration
   // Prerequisite: All angular rate vector components in range -3 to +3 deg/s

   ADCS_returnState test_returnState = ADCS_OK;

   //* Command Sequence
   // ADCS Run Mode : State = Enabled (1)
   printf("Running ADCS_set_enabled_state to set 1Hz loop (enabled)...\n");
   test_returnState = ADCS_set_enabled_state(ONEHZ_LOOP_ENABLE);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_enabled_state returned %d \n", test_returnState);
       while (1)
           ;
   }


   adcs_state test_adcs_state;
   printf("Running ADCS_get_current_state...\n");
   test_returnState = ADCS_get_current_state(&test_adcs_state);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }
   printf("run_mode = %d \n", test_adcs_state.run_mode);


   // Power Control : CubeControl Signal and/or Motor Power = On (1), All others = Off (0)
   uint8_t control[10] = {0};

   // Verify Power State(s)
   printf("Running ADCS_get_power_control...\n");
   test_returnState = ADCS_get_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
   for (int i = 0; i < 10; i++) {
       printf("control[%d] = %d \n", i, control[i]);
   }

   control[Set_CubeCTRLSgn_Power] = 1;
   control[Set_CubeCTRLMtr_Power] = 1;

   test_returnState = ADCS_set_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }

   // Verify Power States again
   test_returnState = ADCS_get_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
   for (int i = 0; i < 10; i++) {
       printf("control[%d] = %d \n", i, control[i]);
   }



   // Set Estimation Mode  : Mode =  Magnetometer Rate Estimator (2)
   test_returnState = ADCS_set_attitude_estimate_mode(MAG_RATE_FILTER_MODE);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_attitude_estimate_mode returned %d \n", test_returnState);
       while (1)
           ;
   }


   printf("Running ADCS_get_current_state...\n");
   test_returnState = ADCS_get_current_state(&test_adcs_state);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }
   printf("att_estimate_mode = %d \n", test_adcs_state.att_estimate_mode);



   //* Telemetry Logging
   printf("Setting Telemetry Logging\n");

   uint8_t flags_arr[80] = {0};
   uint16_t period;
   uint8_t sd;

   flags_arr[60] = 1; // Fine Estimated Angular Rates
   flags_arr[10] = 1; // Rate Sensor Rates
   flags_arr[14] = 1; // Magnetometer Measurements (Calibrated at this point)

   // Verify Bit Mask
   printf("0b");
   for (int i = 0; i < 80; i++) {
       printf("%d", flags_arr[i]);

       if ((i + 1) % 8 == 0) {
           printf(" ");
       }
   }
   printf("\n");

   test_returnState = ADCS_set_log_config(flags_arr, TLM_LOG_PERIOD_10s, TLM_LOG_SDCARD_0, TLM_LOG_1);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_log_config returned %d \n", test_returnState);
       while (1)
           ;
   }

   test_returnState = ADCS_get_log_config(flags_arr, &period, &sd, TLM_LOG_1);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_log_config returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("period = %d, sd = %d\n", period, sd);

   // Verify Bit Mask again
   printf("0b");
   for (int i = 0; i < 80; i++) {
       printf("%d", flags_arr[i]);

       if ((i + 1) % 8 == 0) {
           printf(" ");
       }
   }
   printf("\n");

   // Delay '1 orbit'
   // On-ground Processing

   //*  Confirm following:
   // Set Mag Config
   // mtm_config mag_config;

   // Deployed Mag angles
   // mag_config.mounting_angle.z = 0;    // Alpha  = 0 Deg
   // mag_config.mounting_angle.y = 90;   // Beta   = 90 Deg
   // mag_config.mounting_angle.x = 90;   // Gamma  = 90 Deg

   // mag_config.channel_offset.z = 0;
   // mag_config.channel_offset.y = 0;
   // mag_config.channel_offset.x = 0;
   // mag_config.sensitivity_mat = {0};

   // test_returnState = ADCS_set_mtm_config(mag_config, 1);
   // if(test_returnState != ADCS_OK){
   //     printf("ADCS_set_mtm_config returned %d \n", test_returnState);
   //     while(1);
   // }

   // // Save Configuration
   // test_returnState = ADCS_save_config();
   // if(test_returnState != ADCS_OK){
   //     printf("ADCS_save_config returned %d \n", test_returnState);
   //     while(1);
   // }

   // Log data for 1 minute
   printf("Stopping Telemetry Logging\n");
   vTaskDelay(ONE_MINUTE);
   test_returnState = ADCS_set_log_config(flags_arr, TLM_LOG_PERIOD_STOP, TLM_LOG_SDCARD_0, TLM_LOG_1);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_log_config returned %d \n", test_returnState);
       while (1)
           ;
   }

   // All = Off(0)
   control[Set_CubeCTRLSgn_Power] = 0;
   control[Set_CubeCTRLMtr_Power] = 0;
   test_returnState = ADCS_set_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
}

void commissioning_ang_rate_pitch_angle_est(void)
{
   // Step 6 in ADCS Commissioning
   // Angular Rate and Pitch Angle Estimation

   ADCS_returnState test_returnState = ADCS_OK;

   //* Command Sequence
   // ADCS Run Mode : State = Enabled (1)
   printf("Running ADCS_set_enabled_state to set 1Hz loop (enabled)...\n");
   test_returnState = ADCS_set_enabled_state(ONEHZ_LOOP_ENABLE);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_enabled_state returned %d \n", test_returnState);
       while (1)
           ;
   }

   adcs_state test_adcs_state;
   printf("Running ADCS_get_current_state...\n");
   test_returnState = ADCS_get_current_state(&test_adcs_state);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }



   // Power Control : CubeControl Signal and/or Motor Power = On (1), All others = Off (0)
   uint8_t control[10] = {0};

   // Verify Power State(s)
   printf("Running ADCS_get_power_control...\n");
   test_returnState = ADCS_get_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
   for (int i = 0; i < 10; i++) {
       printf("control[%d] = %d \n", i, control[i]);
   }

   control[Set_CubeCTRLSgn_Power] = 1;
   control[Set_CubeCTRLMtr_Power] = 1;

   test_returnState = ADCS_set_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }

   // Verify Power States again
   test_returnState = ADCS_get_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
   for (int i = 0; i < 10; i++) {
       printf("control[%d] = %d \n", i, control[i]);
   }



   // Set Estimation Mode  : Mode =  Mag. Rate Filter w/ Pitch Estimation (3)
   test_returnState = ADCS_set_attitude_estimate_mode(MAG_RATE_FILTER_W_PITCH_EST);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_attitude_estimate_mode returned %d \n", test_returnState);
       while (1)
           ;
   }


   printf("Running ADCS_get_current_state...\n");
   test_returnState = ADCS_get_current_state(&test_adcs_state);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }
   printf("att_estimate_mode = %d \n", test_adcs_state.att_estimate_mode);



   // Set Attitude Control Mode : Y-Thomson (2) (timeout : 0s)
   printf("Running ADCS_set_attitude_ctrl_mode...\n");
   test_returnState = ADCS_set_attitude_ctrl_mode(Y_THOMSON_MODE, 0); // 0 Timeout == infinite
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_attitude_ctrl_mode returned %d \n", test_returnState);
       while (1)
           ;
   }

   // Verify Control Mode
   printf("Running ADCS_get_current_state...\n");
   test_returnState = ADCS_get_current_state(&test_adcs_state);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }



   //* Telemetry Logging
   printf("Setting Telemetry Logging\n");

   uint8_t flags_arr[80] = {0};
   uint16_t period;
   uint8_t sd;

   flags_arr[56] = 1; // Fine Estimated Angular Rates
   flags_arr[27] = 1; // Estimated Attitude Angles
   flags_arr[10] = 1; // Rate Sensor Rates
   flags_arr[14] = 1; // Magnetometer Measurements (Calibrated at this point)

   // Verify Bit Mask
   printf("0b");
   for (int i = 0; i < 80; i++) {
       printf("%d", flags_arr[i]);

       if ((i + 1) % 8 == 0) {
           printf(" ");
       }
   }
   printf("\n");

   test_returnState = ADCS_set_log_config(flags_arr, TLM_LOG_PERIOD_10s, TLM_LOG_SDCARD_0, TLM_LOG_1);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_log_config returned %d \n", test_returnState);
       while (1)
           ;
   }

   test_returnState = ADCS_get_log_config(flags_arr, &period, &sd, TLM_LOG_1);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_log_config returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("period = %d, sd = %d\n", period, sd);

   // Verify Bit Mask again
   printf("0b");
   for (int i = 0; i < 80; i++) {
       printf("%d", flags_arr[i]);

       if ((i + 1) % 8 == 0) {
           printf(" ");
       }
   }
   printf("\n");

   // Log data for 1 minute
   printf("Stopping Telemetry Logging\n");
   vTaskDelay(ONE_MINUTE);
   test_returnState = ADCS_set_log_config(flags_arr, TLM_LOG_PERIOD_STOP, TLM_LOG_SDCARD_0, TLM_LOG_1);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_log_config returned %d \n", test_returnState);
       while (1)
           ;
   }



   // All = Off(0)
   control[Set_CubeCTRLSgn_Power] = 0;
   control[Set_CubeCTRLMtr_Power] = 0;
   test_returnState = ADCS_set_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }

}

void commissioning_ywheel_ramp_up_test(void)
{
   // Step 7 in ADCS Commissioning
   // Y-Wheel Ramp-Up Test


   ADCS_returnState test_returnState = ADCS_OK;

   //* Command Sequence
   // ADCS Run Mode : State = Enabled (1)
   printf("Running ADCS_set_enabled_state to set 1Hz loop (enabled)...\n");
   test_returnState = ADCS_set_enabled_state(ONEHZ_LOOP_ENABLE);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_enabled_state returned %d \n", test_returnState);
       while (1)
           ;
   }

   adcs_state test_adcs_state;
   printf("Running ADCS_get_current_state...\n");
   test_returnState = ADCS_get_current_state(&test_adcs_state);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("ADCS run mode = %d\n", test_adcs_state.run_mode);


   // Power Control : CubeControl Signal and/or Motor Power = On (1), All others = Off (0)
   uint8_t control[10] = {0};

   // Verify Power State(s)
   printf("Running ADCS_get_power_control...\n");
   test_returnState = ADCS_get_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
   for (int i = 0; i < 10; i++) {
       printf("control[%d] = %d \n", i, control[i]);
   }

   control[Set_CubeCTRLSgn_Power] = 1;
   control[Set_CubeCTRLMtr_Power] = 1;
   control[Set_CubeWheel1_Power] = 1; // Y-Wheel

   test_returnState = ADCS_set_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }

   // Verify Power States again
   test_returnState = ADCS_get_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
   for (int i = 0; i < 10; i++) {
       printf("control[%d] = %d \n", i, control[i]);
   }



   // Set Estimation Mode : Mtm Rate Filter w/ pitch estimation (3)
   test_returnState = ADCS_set_attitude_estimate_mode(MAG_RATE_FILTER_W_PITCH_EST);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_attitude_estimate_mode returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("Running ADCS_get_current_state...\n");
   test_returnState = ADCS_get_current_state(&test_adcs_state);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }
   printf("att_estimate_mode = %d \n", test_adcs_state.att_estimate_mode);



   // Set Control Mode : None (0)
   printf("Running ADCS_set_attitude_ctrl_mode...\n");
   test_returnState = ADCS_set_attitude_ctrl_mode(0, 0); // 0 Timeout == infinite
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_attitude_ctrl_mode returned %d \n", test_returnState);
       while (1)
           ;
   }

   // Verify Control Mode
   printf("Running ADCS_get_current_state...\n");
   test_returnState = ADCS_get_current_state(&test_adcs_state);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }
   printf("att_ctrl_mode = %d \n", test_adcs_state.att_ctrl_mode);



   //* Telemetry Logging
   printf("Setting Telemetry Logging\n");

   uint8_t flags_arr[80] = {0};
   uint16_t period;
   uint8_t sd;

   flags_arr[60] = 1; // Fine Estimated Angular Rates
   flags_arr[27] = 1; // Estimated Attitude Angles
   flags_arr[10] = 1; // Rate Sensor Rates
   flags_arr[14] = 1; // Magnetometer Measurements (Calibrated at this point)
   flags_arr[9]  = 1; // Wheel Speed

   // Verify Bit Mask
   printf("0b");
   for (int i = 0; i < 80; i++) {
       printf("%d", flags_arr[i]);

       if ((i + 1) % 8 == 0) {
           printf(" ");
       }
   }
   printf("\n");

   test_returnState = ADCS_set_log_config(flags_arr, TLM_LOG_PERIOD_1s, TLM_LOG_SDCARD_0, TLM_LOG_1);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_log_config returned %d \n", test_returnState);
       while (1)
           ;
   }

   test_returnState = ADCS_get_log_config(flags_arr, &period, &sd, TLM_LOG_1);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_log_config returned %d \n", test_returnState);
       while (1)
           ;
   }

   printf("period = %d, sd = %d\n", period, sd);

   // Verify Bit Mask again
   printf("0b");
   for (int i = 0; i < 80; i++) {
       printf("%d", flags_arr[i]);

       if ((i + 1) % 8 == 0) {
           printf(" ");
       }
   }
   printf("\n");



   // Commanded Wheel Speed : 1000 rpm (arbitrary test speed)
   xyz16 speed;
   speed.x = 0;
   speed.y = 1000; // ?
   speed.z = 0;

   test_returnState = ADCS_set_wheel_speed(speed);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }

   adcs_measures *measurements;
   measurements = (adcs_measures *)pvPortMalloc(sizeof(adcs_measures));
   if (measurements == NULL) {
       printf("malloc issues");
       while (1)
           ;
   }



   // Delay 2 minutes
   vTaskDelay(pdMS_TO_TICKS(2*60000));



   // Commanded Wheel Speed : 0rpm
   speed.y = 0;
   test_returnState = ADCS_set_wheel_speed(speed);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }



   // Set Control Mode : Y-Thomson (2)
   printf("Running ADCS_set_attitude_ctrl_mode...\n");
   test_returnState = ADCS_set_attitude_ctrl_mode(Y_THOMSON_MODE, 0); // 0 Timeout == infinite
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_attitude_ctrl_mode returned %d \n", test_returnState);
       while (1)
           ;
   }

   // Verify Control Mode
   printf("Running ADCS_get_current_state...\n");
   test_returnState = ADCS_get_current_state(&test_adcs_state);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_get_current_state returned %d \n", test_returnState);
       while (1)
           ;
   }
   printf("att_ctrl_mode = %d \n", test_adcs_state.att_ctrl_mode);



   // Stop Telemetry Log
   printf("Stopping Telemetry Logging\n");
   test_returnState = ADCS_set_log_config(flags_arr, TLM_LOG_PERIOD_STOP, TLM_LOG_SDCARD_0, TLM_LOG_1);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_log_config returned %d \n", test_returnState);
       while (1)
           ;
   }



   // All = Off(0)
   control[Set_CubeCTRLSgn_Power] = 0;
   control[Set_CubeCTRLMtr_Power] = 0;
   test_returnState = ADCS_set_power_control(control);
   if (test_returnState != ADCS_OK) {
       printf("ADCS_set_power_control returned %d \n", test_returnState);
       while (1)
           ;
   }
}

void commissioning_initial_y_momentum_activation(void) 
{
    // Step 8 of ADCS Commissioning
    // Initial Y-Momentum Mode Activation
    
    ADCS_returnState test_returnState = ADCS_OK;

    //* Command Sequence
    // ADCS Run Mode : State = Enabled (1)
    printf("Running ADCS_set_enabled_state to set 1Hz loop (enabled)...\n");
    test_returnState = ADCS_set_enabled_state(ONEHZ_LOOP_ENABLE);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_set_enabled_state returned %d \n", test_returnState);
        while (1)
            ;
    }


    adcs_state test_adcs_state;
    printf("Running ADCS_get_current_state...\n");
    test_returnState = ADCS_get_current_state(&test_adcs_state);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_current_state returned %d \n", test_returnState);
        while (1)
            ;
    }
    printf("run_mode = %d \n", test_adcs_state.run_mode);



    // Power Control : CubeControl Signal and/or Motor Power = On (1), All others = Off (0)
    uint8_t control[10] = {0};

    // Verify Power State(s)
    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while (1)
            ;
    }
    for (int i = 0; i < 10; i++) {
        printf("control[%d] = %d \n", i, control[i]);
    }

    control[Set_CubeCTRLSgn_Power] = 1;
    control[Set_CubeCTRLMtr_Power] = 1;

    test_returnState = ADCS_set_power_control(control);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_set_power_control returned %d \n", test_returnState);
        while (1)
            ;
    }

    // Verify Power States again
    test_returnState = ADCS_get_power_control(control);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while (1)
            ;
    }
    for (int i = 0; i < 10; i++) {
        printf("control[%d] = %d \n", i, control[i]);
    }




    // Set Unix Time
    uint32_t time = 1651165869;

    test_returnState = ADCS_set_unix_t(time, 0);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_set_unix_t returned %d \n", test_returnState);
        while (1)
            ;
    }

    test_returnState = ADCS_get_unix_t(&time, 0);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while (1)
            ;
    }
    printf("Unix Time : %ul(s)\n", time);

    //Tried to send the set sgp4 orbital params byte by byte. Did not succeed but will leave it here if needed for other commands.
//  static uint8_t cmd[71] = {0x1F, 0x7F, 0x2D, 0x8A, 0x8E, 0xE4, 0xF2, 0x1F, 0x1F, 0xD2, 0x49, 0x40, 0x6F, 0xB8, 0x8F, 0xDC, 0x9A, 0x74, 0x7B,
//                                      0x3F, 0x7F, 0xFB, 0x3A, 0x70, 0xCE, 0xEE, 0x6E, 0x40, 0xFE, 0xD4, 0x78, 0xE9, 0x26, 0x51, 0x60, 0x40, 0x85,
//                                      0x42, 0x04, 0x1C, 0x42, 0x95, 0x6A, 0x3F, 0xCF, 0x1F, 0x1F, 0x47, 0x31, 0x48, 0x71, 0x2F, 0x40, 0xFF, 0x21,
//                                      0xFD, 0xF6, 0x75, 0x50, 0x74, 0x40, 0x83, 0xC0, 0xCA, 0xA1, 0x45, 0x88, 0x70, 0x40, 0x1F, 0xFF};
//  sciSend(ADCS_SCI, 71, cmd);


    // Set Orbit Parameters
        adcs_sgp4 orbital_params;


        test_returnState = ADCS_get_sgp4_orbit_params(&orbital_params);
        if (test_returnState != ADCS_OK)
        {
            printf("ADCS_get_sgp4_orbit_params returned %d", test_returnState);
            while(1);
        }
        printf("Orbit params before:\n");
        printf("Inclination: %f\n", orbital_params.inclination);
        printf("ECC: %f\n", orbital_params.ECC);
        printf("RAAN: %f\n", orbital_params.RAAN);
        printf("AOP: %f\n", orbital_params.AOP);
        printf("Bstar: %f\n", orbital_params.Bstar);
        printf("MM: %f\n", orbital_params.MM);
        printf("MA: %f\n", orbital_params.MA);
        printf("Epoch: %f\n\n", orbital_params.epoch);


        orbital_params.inclination = 94.745900;
        orbital_params.RAAN = 334.171400;
        orbital_params.ECC = 0.001148;
        orbital_params.AOP = 296.502100;
        orbital_params.Bstar = 0.000220;
        orbital_params.MA = 51.501000;
        orbital_params.MM = 14.972134;
        orbital_params.epoch = 18147.166300;

        test_returnState = ADCS_set_sgp4_orbit_params(orbital_params);
        if (test_returnState != ADCS_OK)
        {
            printf("ADCS_set_sgp4_orbit_params returned %d", test_returnState);
            while(1);
        }

        test_returnState = ADCS_get_sgp4_orbit_params(&orbital_params);
        if (test_returnState != ADCS_OK)
        {
            printf("ADCS_get_sgp4_orbit_params returned %d", test_returnState);
            while(1);
        }
        printf("Orbit params after:\n");
        printf("Inclination: %f\n", orbital_params.inclination);
        printf("ECC: %f\n", orbital_params.ECC);
        printf("RAAN: %f\n", orbital_params.RAAN);
        printf("AOP: %f\n", orbital_params.AOP);
        printf("Bstar: %f\n", orbital_params.Bstar);
        printf("MM: %f\n", orbital_params.MM);
        printf("MA: %f\n", orbital_params.MA);
        printf("Epoch: %f\n\n", orbital_params.epoch);

    // ADCS_set_sgp4_orbit_params();
    // Save Orbit Parameters
    ADCS_save_orbit_params();

//    ADCS_reset();

//    test_returnState = ADCS_reset();
//    if (test_returnState != ADCS_OK)
//    {
//        printf("ADCS_reset returned %d", test_returnState);
//        while(1);
//    }

    // Set Estimation Parameters
    estimation_config est_params;
    adcs_config ADCS_config;

    test_returnState = ADCS_get_full_config(&ADCS_config);
    if (test_returnState != ADCS_OK){
        printf("ADCS_get_full_config returned %d\n", test_returnState);
        while(1);
    }

    est_params.select_arr[0] = 0;           // Use Sun Sensor   = 0 (false)
    est_params.select_arr[1] = 0;           // Use Nadir Sensor = 0 (false)
    est_params.select_arr[2] = 0;           // Use CSS Sensor   = 0 (false)
    est_params.select_arr[3] = 0;           // Use Star Tracker = 0 (false)
    est_params = ADCS_config.estimation;    // All others : Default

    test_returnState = ADCS_set_estimation_config(est_params);
    if (test_returnState != ADCS_OK)
    {
        printf("ADCS_set_estimation_config returned %d", test_returnState);
        while(1);
    }

    // Set Estimation Mode : Magnetometer Rate Filter with Pitch Rate Estimatino (3)
    test_returnState = ADCS_set_attitude_estimate_mode(MAG_RATE_FILTER_W_PITCH_EST);
    if (test_returnState != ADCS_OK){
        printf("ADCS_set_attitude_estimate_mode returned %d\n", test_returnState);
        while(1);
    }

    printf("Running ADCS_get_current_state...\n");
    test_returnState = ADCS_get_current_state(&test_adcs_state);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_current_state returned %d \n", test_returnState);
        while (1)
            ;
    }
    printf("att_estimate_mode = %d \n\n\n", test_adcs_state.att_estimate_mode);



    //* Telemetry Logging
    printf("Setting Telemetry Logging\n");

    uint8_t flags_arr[80] = {0};
    uint16_t period; 
    uint8_t sd;

    flags_arr[60] = 1;  // Fine Estimated Angular Rates
    flags_arr[27] = 1;  // Estimated Attitude Angles
    flags_arr[10] = 1;  // Rate Sensor Rates
    flags_arr[14] = 1;  // Magnetometer Measurements (Calibrated at this point)
    flags_arr[9]  = 1;  // Wheel Speed
    flags_arr[38]  = 1; // Satellite Position

    // Verify Bit Mask
    printf("0b");
    for (int i = 0; i < 80; i++) {
        printf("%d", flags_arr[i]);

        if ((i + 1) % 8 == 0) {
            printf(" ");
        }
    }
    printf("\n");

    test_returnState = ADCS_set_log_config(flags_arr, TLM_LOG_PERIOD_10s, TLM_LOG_SDCARD_0, TLM_LOG_1);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_set_log_config returned %d \n", test_returnState);
        while (1)
            ;
    }

    test_returnState = ADCS_get_log_config(flags_arr, &period, &sd, TLM_LOG_1);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_log_config returned %d \n", test_returnState);
        while (1)
            ;
    }

    printf("period = %d, sd = %d\n", period, sd);

    // Verify Bit Mask again
    printf("0b");
    for (int i = 0; i < 80; i++) {
        printf("%d", flags_arr[i]);

        if ((i + 1) % 8 == 0) {
            printf(" ");
        }
    }
    printf("\n");



    // First Activation: 
    // Mode = Initial Y-Momentum (3)
    // Duration = 1 Orbit (for testing, 60s)
    printf("First Activation: Initial Y-Momentum (60s)\n");
    test_returnState = ADCS_set_attitude_ctrl_mode(3, 60);
    if (test_returnState != ADCS_OK){
        printf("ADCS_set_attitude_ctrl_mode returned %d\n", test_returnState);
        while(1);
    }

    printf("Running ADCS_get_current_state...\n");
    test_returnState = ADCS_get_current_state(&test_adcs_state);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_current_state returned %d \n", test_returnState);
        while (1)
            ;
    }
    printf("att_estimate_mode = %d \n\n\n", test_adcs_state.att_ctrl_mode);



    // Subsequent Activation: 
    // Mode = Steady Y-Momentum (4)
    // Duration = Indefinite 
    printf("Subsequent Activation: Steady Y-Momentum (Indefinite)\n");
    test_returnState = ADCS_set_attitude_ctrl_mode(4, 0);
    if (test_returnState != ADCS_OK){
        printf("ADCS_set_attitude_ctrl_mode returned %d\n", test_returnState);
        while(1);
    }

    printf("Running ADCS_get_current_state...\n");
    test_returnState = ADCS_get_current_state(&test_adcs_state);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_current_state returned %d \n", test_returnState);
        while (1)
            ;
    }
    printf("att_estimate_mode = %d \n", test_adcs_state.att_ctrl_mode);

    

    // Step 9 in 2U ADCS Commissioning
    // Magnetometer EKF
    // Set Estimation Mode : Full-State EKF (5)
    //! Estimator mode 6 will be used for purposes of testing
    test_returnState = ADCS_set_attitude_estimate_mode(6);
    if (test_returnState != ADCS_OK){
        printf("ADCS_set_attitude_estimate_mode returned %d\n", test_returnState);
        while(1);
    }

    printf("Running ADCS_get_current_state...\n");
    test_returnState = ADCS_get_current_state(&test_adcs_state);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_current_state returned %d \n", test_returnState);
        while (1)
            ;
    }
    printf("att_estimate_mode = %d \n\n\n", test_adcs_state.att_estimate_mode);
    vTaskDelay(pdMS_TO_TICKS(5000));

    
    // Stop Telemetry Log
    printf("Stopping Telemetry Logging\n");
    test_returnState = ADCS_set_log_config(flags_arr, TLM_LOG_PERIOD_STOP, TLM_LOG_SDCARD_0, TLM_LOG_1);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_set_log_config returned %d \n", test_returnState);
        while (1)
            ;
    }



    // All = Off(0)
    control[Set_CubeCTRLSgn_Power] = 0;
    control[Set_CubeCTRLMtr_Power] = 0;
    test_returnState = ADCS_set_power_control(control);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_set_power_control returned %d \n", test_returnState);
        while (1)
            ;
    }
}

void commandsTest_bootloader(void)
{
//    //* Bootloader Telecommands testing
//    // Testing running bootloader with ADCS_reset
//    uint8_t node_type = 0;
//    uint8_t interface_ver = 0;
//    uint16_t runtime_s = 0;
//    uint16_t runtime_ms = 0;
//    uint8_t major_firm_ver = 0;
//    uint8_t minor_firm_ver = 0;
//
//    ADCS_reset();
//
//    ADCS_returnState test_returnState = ADCS_OK;
//
//    printf("Running ADCS_get_boot_program_stat...\n");
//    test_returnState = ADCS_get_node_identification(&node_type, &interface_ver, &major_firm_ver,
//                                                    &minor_firm_ver, &runtime_s, &runtime_ms);
//    if (test_returnState != ADCS_OK) {
//       printf("ADCS_get_boot_program_stat returned %d \n", test_returnState);
//       while (1)
//           ;
//    }
//
//    printf("node_type = %d \n", node_type);
//    printf("interface_ver = %d \n", interface_ver);
//    printf("runtime_s = %d \n", runtime_s);
//    printf("runtime_ms = %d \n\n", runtime_ms);
//    printf("major_firm_ver = %d \n", major_firm_ver);
//    printf("minor_firm_ver = %d \n\n", minor_firm_ver);

      //getting boot index to check if we are in bootloader
      uint8_t mcu_reset_cause = 0;
      uint8_t boot_cause = 0;
      uint16_t boot_count = 0;
      uint8_t boot_idx = 0;
      uint8_t major_firm_ver = 0;
      uint8_t minor_firm_ver = 0;

      ADCS_returnState test_returnState = ADCS_OK;

      printf("Running ADCS_get_boot_program_stat...\n");
      test_returnState = ADCS_get_boot_program_stat(&mcu_reset_cause, &boot_cause, &boot_count, &boot_idx,
                                                     &major_firm_ver, &minor_firm_ver);
      if (test_returnState != ADCS_OK) {
           printf("ADCS_get_boot_program_stat returned %d \n", test_returnState);
           while (1)
               ;
       }

      printf("mcu_reset_cause = %d \n", mcu_reset_cause);
      printf("boot_cause = %d \n", boot_cause);
      printf("boot_count = %d \n", boot_count);
      printf("boot_idx = %d \n\n", boot_idx);
      printf("major_firm_ver = %d \n", major_firm_ver);
      printf("minor_firm_ver = %d \n\n", minor_firm_ver);

//    // Clear Bootloader Error Flags
//    printf("Clearing error flags\n\n");
//    test_returnState = ADCS_clear_err_flags();
//    if (test_returnState != ADCS_OK)
//    {
//        printf("ADCS_clear_err_flags returned %d\n", test_returnState);
//        while(1);
//    }


    // Set Boot Index

//    printf("Setting boot index\n\n");
//    test_returnState = ADCS_set_boot_index(INTERNAL_FLASH_PROGRAM);
//    if (test_returnState != ADCS_OK)
//    {
//        printf("ADCS_set_boot_index returned %d\n", test_returnState);
//        while(1);
//    }


    // Read Program Info
//    printf("Reading program info (Bootloader)\n\n");
//    test_returnState = ADCS_read_program_info(BOOTLOADER);
//    if (test_returnState != ADCS_OK)
//    {
//        printf("ADCS_read_program_info returned %d", test_returnState);
//        while(1);
//    }

    printf("Reading Program Info (Internal Flash Program)\n\n");
    test_returnState = ADCS_read_program_info(INTERNAL_FLASH_PROGRAM);
    if (test_returnState != ADCS_OK)
    {
        printf("ADCS_read_program_info returned %d", test_returnState);
        while(1);
    }


//    // Clear latched errors
//    printf("Clearing latched errors\n\n");
//    test_returnState = ADCS_clear_latched_errs(true, true);
//    if (test_returnState != ADCS_OK)
//    {
//        printf("ADCS_clear_latched_err returned %d", test_returnState);
//        while(1);
//    }
}

void commandsTest_ACP_telemetry(void)
{
    //* ACP Telemetry commands testing

    ADCS_returnState test_returnState = ADCS_OK;



    // CubeACP State flags
    uint8_t *flags_arr = (uint8 *)pvPortMalloc(6*sizeof(int8_t));
    test_returnState = ADCS_get_cubeACP_state(flags_arr);
    if (test_returnState != ADCS_OK)
    {
        printf("ADCS_reset returned %d", test_returnState);
        while(1);
    }

    for (int i = 0; i < 6; i++)
    {
        printf("CubeACP State Flag %d = %d\n", i, flags_arr[i]);
    }

    vPortFree(flags_arr);



    // Actuator Commands
    xyz16 dutycycle = {0, 0, 0};
    dutycycle.y = 800;
    dutycycle.z = 800;
    printf("Running Magnetorquer X at max duty cycle\n\n");
    test_returnState = ADCS_set_magnetorquer_output(dutycycle);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_set_magnetorquer_output returned %d \n", test_returnState);
        while (1);
    }

    printf("Getting actuator commands\n");
    adcs_actuator *actuator_commands = (adcs_actuator *)pvPortMalloc(sizeof(adcs_actuator));
    test_returnState = ADCS_get_actuator(actuator_commands);
    if (test_returnState != ADCS_OK)
    {
        printf("ADCS_get_actuator returned %d", test_returnState);
        while(1);
    }

    printf("Magnetorquer X cmd = %f\n", actuator_commands->magnetorquer.x);
    printf("Magnetorquer Y cmd = %f\n", actuator_commands->magnetorquer.y);
    printf("Magnetorquer Z cmd = %f\n", actuator_commands->magnetorquer.z);


    printf("Wheel Speed X cmd = %f\n", actuator_commands->wheel_speed.x);
    printf("Wheel Speed Y cmd = %f\n", actuator_commands->wheel_speed.y);
    printf("Wheel Speed Z cmd = %f\n", actuator_commands->wheel_speed.z);

    vPortFree(actuator_commands);
}

void commandsTest_attitude(void)
{
    ADCS_returnState test_returnState = ADCS_OK;


    // Get attitude angles
    xyz att_angle;
    printf("Getting commanded attitude angles\n");
    test_returnState = ADCS_get_attitude_angle(&att_angle);
    if (test_returnState != ADCS_OK)
    {
        printf("ADCS_clear_err_flags returned %d", test_returnState);
        while(1);
    }

    printf("X angle: %+f\n", att_angle.x);
    printf("Y angle: %+f\n", att_angle.y);
    printf("Z angle: %+f\n\n", att_angle.z);

    // Set attitude angles
    att_angle.x = 10;
    att_angle.y = 10;
    att_angle.z = 10;

    printf("Setting attitude angles\n");
    test_returnState = ADCS_set_attitude_angle(att_angle);
    if (test_returnState != ADCS_OK)
    {
        printf("ADCS_clear_err_flags returned %d", test_returnState);
        while(1);
    }

    // Get attitude angles
    printf("Getting commanded attitude angles\n");
    test_returnState = ADCS_get_attitude_angle(&att_angle);
    if (test_returnState != ADCS_OK)
    {
        printf("ADCS_clear_err_flags returned %d", test_returnState);
        while(1);
    }

    printf("X angle: %f\n", att_angle.x);
    printf("Y angle: %f\n", att_angle.y);
    printf("Z angle: %f\n\n", att_angle.z);

    // Reset attitude angles
    att_angle.x = 0;
    att_angle.y = 0;
    att_angle.z = 0;

    printf("Resetting attitude angles\n");
    test_returnState = ADCS_set_attitude_angle(att_angle);
    if (test_returnState != ADCS_OK)
    {
        printf("ADCS_clear_err_flags returned %d", test_returnState);
        while(1);
    }

    // Get attitude angles
    printf("Getting commanded attitude angles\n");
    test_returnState = ADCS_get_attitude_angle(&att_angle);
    if (test_returnState != ADCS_OK)
    {
        printf("ADCS_clear_err_flags returned %d", test_returnState);
        while(1);
    }

    printf("X angle: %+f\n", att_angle.x);
    printf("Y angle: %+f\n", att_angle.y);
    printf("Z angle: %+f\n\n", att_angle.z);
}

void commandsTest_logtest(void)
{
    
    // Function for testing LOG1 and LOG2 parallel telemetry logging
    printf("Starting log test:\n\n");

    ADCS_returnState test_returnState = ADCS_OK;


    // Set Estimation Mode : Mode = Magnetometer rate filter (2)
    test_returnState = ADCS_set_attitude_estimate_mode(MAG_RATE_FILTER_MODE);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while (1);
    }

    adcs_state test_adcs_state;
    printf("Running ADCS_get_current_state...\n");
    test_returnState = ADCS_get_current_state(&test_adcs_state);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_current_state returned %d \n", test_returnState);
        while (1);
    }
    printf("att_estimate_mode = %d \n", test_adcs_state.att_estimate_mode);


    //* Telemetry Logging

    // LOG1 setting
    printf("Setting LOG1 (Magnetometer Measurement, 10s)\n");

    uint8_t flags_arr1[80] = {0};
    flags_arr1[14] = 1; // Magnetometer Measurement

    test_returnState = ADCS_set_log_config(flags_arr1, TLM_LOG_PERIOD_10s, TLM_LOG_SDCARD_0, TLM_LOG_1);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_set_log_config returned %d \n", test_returnState);
        while (1);
    }

    // LOG2 setting
    printf("Setting LOG2 (Estimated Angular Rates, 1s)\n");

    uint8_t flags_arr2[80] = {0};
    flags_arr2[27] = 1; // Estimated Angular Rates

    test_returnState = ADCS_set_log_config(flags_arr2, TLM_LOG_PERIOD_1s, TLM_LOG_SDCARD_0, TLM_LOG_2);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_set_log_config returned %d \n", test_returnState);
        while (1);
    }


    // Verify log settings
    printf("Verifying log settings:\n");

    uint16_t period_log1, period_log2;
    uint8_t sd_log1, sd_log2;

    test_returnState = ADCS_get_log_config(flags_arr1, &period_log1, &sd_log1, TLM_LOG_1);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_set_log_config returned %d \n", test_returnState);
        while (1);
    }

    printf("LOG1 period: %d\n", period_log1);
    printf("LOG1 SD dest: %d\n", sd_log1);

    test_returnState = ADCS_get_log_config(flags_arr1, &period_log2, &sd_log2, TLM_LOG_2);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_set_log_config returned %d \n", test_returnState);
        while (1);
    }

    printf("LOG2 period: %d\n", period_log2);
    printf("LOG2 SD dest: %d\n\n", sd_log2);

    
    // Log for one minute
    printf("Logging for 1 min...\n");
    vTaskDelay(ONE_MINUTE);


    // Stop logging
    printf("Stopping LOG1\n");
    test_returnState = ADCS_set_log_config(flags_arr1, TLM_LOG_PERIOD_STOP, TLM_LOG_SDCARD_0, TLM_LOG_1);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_set_log_config returned %d \n", test_returnState);
        while (1);
    }

    printf("Stopping LOG2\n");
    test_returnState = ADCS_set_log_config(flags_arr2, TLM_LOG_PERIOD_STOP, TLM_LOG_SDCARD_0, TLM_LOG_2);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_set_log_config returned %d \n", test_returnState);
        while (1);
    }

}

void commandsTest_configs(void)
{
    // Function for testing config commands 
    // and saving them

    ADCS_returnState test_returnState = ADCS_OK;

    adcs_config  *ADCS_config = (adcs_config *)pvPortMalloc(sizeof(adcs_config));
    printf("Getting default configs:\n");
    test_returnState = ADCS_get_full_config(ADCS_config);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_full_config returned %d \n", test_returnState);
        while (1);
    }

    // Set MTQ config
    printf("Default MTQ config:\n");
    printf("Magnetorquer 1 config: %u\n", ADCS_config->MTQ.x);
    printf("Magnetorquer 2 config: %u\n", ADCS_config->MTQ.y);
    printf("Magnetorquer 3 config: %u\n", ADCS_config->MTQ.z);


    printf("Setting MTQ Config:\n");

    xyzu8 params;
    params.x = 0;
    params.y = 3;
    params.z = 5;

    test_returnState = ADCS_set_MTQ_config(params);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_full_config returned %d \n", test_returnState);
        while (1);
    }

    printf("Verfying sent MTQ config:\n");
    test_returnState = ADCS_get_full_config(ADCS_config);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_full_config returned %d \n", test_returnState);
        while (1);
    }

    printf("Magnetorquer 1 config: %u\n", ADCS_config->MTQ.x);
    printf("Magnetorquer 2 config: %u\n", ADCS_config->MTQ.y);
    printf("Magnetorquer 3 config: %u\n\n", ADCS_config->MTQ.z);



//    // Set Wheel config
//    printf("Default Reaction Wheel config:\n");
//    printf("Wheel 1 config: %u\n", ADCS_config->RW[0]);
//    printf("Wheel 2 config: %u\n", ADCS_config->RW[1]);
//    printf("Wheel 3 config: %u\n", ADCS_config->RW[2]);
//    printf("Wheel 4 config: %u\n\n", ADCS_config->RW[3]); //? 4th wheel?
//
//    printf("Setting Wheel config\n");
//    uint8_t RW[4];
//    RW[0] = 1; // -X
//    RW[1] = 6; // -Y
//    RW[2] = 6; // -Z
//    RW[3] = 6; // Not used
//
//    test_returnState = ADCS_set_RW_config(RW);
//    if (test_returnState != ADCS_OK) {
//        printf("ADCS_set_RW_config returned %d \n", test_returnState);
//        while (1);
//    }
//
//    printf("Verifying sent RW config:\n");
//    test_returnState = ADCS_get_full_config(ADCS_config);
//    if (test_returnState != ADCS_OK) {
//        printf("ADCS_get_full_config returned %d \n", test_returnState);
//        while (1);
//    }
//
//    printf("Wheel 1 config: %u\n", ADCS_config->RW[0]);
//    printf("Wheel 2 config: %u\n", ADCS_config->RW[1]);
//    printf("Wheel 3 config: %u\n", ADCS_config->RW[2]);
//    printf("Wheel 4 config: %u\n\n", ADCS_config->RW[3]);
//
//
//
//    // Set CSS config
//    printf("Default CSS config:\n");
//    for(int i = 0; i < 10; i++)
//    {
//        printf("CSS%d config = %d\n", i, ADCS_config->css.config[i]);
//
//    }
//    for(int i = 0; i < 10; i++)
//    {
//        printf("CSS%d relative scale = %f\n", i, ADCS_config->css.rel_scale[i]);
//    }
//    printf("Threshhold: %d\n\n", ADCS_config->css.threshold);
//
//    printf("Setting CSS config\n");
//    css_config *test_css_config = (css_config *)pvPortMalloc(sizeof(css_config));
//
//    test_css_config->config[0] = 0; test_css_config->rel_scale[0] = 2;
//    test_css_config->config[1] = 1; test_css_config->rel_scale[1] = 2;
//    test_css_config->config[2] = 2; test_css_config->rel_scale[2] = 2;
//    test_css_config->config[3] = 3; test_css_config->rel_scale[3] = 2;
//    test_css_config->config[4] = 4; test_css_config->rel_scale[4] = 2;
//    test_css_config->config[5] = 5; test_css_config->rel_scale[5] = 2;
//    test_css_config->config[6] = 6; test_css_config->rel_scale[6] = 2;
//    test_css_config->config[7] = 0; test_css_config->rel_scale[7] = 2;
//    test_css_config->config[8] = 1; test_css_config->rel_scale[8] = 2;
//    test_css_config->config[9] = 2; test_css_config->rel_scale[9] = 2;
//
//    test_css_config->threshold = ADCS_config->css.threshold;
//
//    test_returnState = ADCS_set_css_config(*test_css_config);
//    if (test_returnState != ADCS_OK)
//    {
//        printf("ADCS_set_css_config returned %d", test_returnState);
//        while(1);
//    }
//
//    printf("Verifying sent CSS config:\n");
//    test_returnState = ADCS_get_full_config(ADCS_config);
//    if (test_returnState != ADCS_OK) {
//        printf("ADCS_get_full_config returned %d \n", test_returnState);
//        while (1);
//    }
//    for(int i = 0; i < 10; i++)
//    {
//        printf("CSS%d config = %d\n", i, ADCS_config->css.config[i]);
//    }
//    for(int i = 0; i < 10; i++)
//    {
//        printf("CSS%d relative scale = %f\n", i, ADCS_config->css.rel_scale[i]);
//    }
//    printf("Threshhold: %d\n\n", ADCS_config->css.threshold);

    printf("Saving configuration...\n\n");
    test_returnState = ADCS_save_config();
    if (test_returnState != ADCS_OK)
    {
        printf("ADCS_save_config returned %d", test_returnState);
        while(1);
    }

    ADCS_reset();

    printf("Waiting 6s for bootloader to finish...\n\n");
    vTaskDelay(pdMS_TO_TICKS(6000)); // Wait 6s for bootloader duration



    printf("Re-enabling ADCS\n\n");
    test_returnState = ADCS_set_enabled_state(1);
    if (test_returnState != ADCS_OK)
    {
        printf("ADCS_set_enabled_state returned %d", test_returnState);
        while(1);
    }

    adcs_state test_adcs_state;
    printf("Running ADCS_get_current_state...\n");
    test_returnState = ADCS_get_current_state(&test_adcs_state);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_current_state returned %d \n", test_returnState);
        while (1)
            ;
    }
    printf("ADCS run mode = %d\n\n", test_adcs_state.run_mode);



    printf("Verifying config saved after reset:\n");
    test_returnState = ADCS_get_full_config(ADCS_config);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_full_config returned %d \n", test_returnState);
        while (1);
    }
    for(int i = 0; i < 10; i++)
    {
        printf("CSS%d config = %d\n", i, ADCS_config->css.config[i]);
    }
    for(int i = 0; i < 10; i++)
    {
        printf("CSS%d relative scale = %f\n", i, ADCS_config->css.rel_scale[i]);
    }
    printf("Threshhold: %d\n\n", ADCS_config->css.threshold);

    printf("Wheel 1 config: %u\n", ADCS_config->RW[0]);
    printf("Wheel 2 config: %u\n", ADCS_config->RW[1]);
    printf("Wheel 3 config: %u\n", ADCS_config->RW[2]);
    printf("Wheel 4 config: %u\n\n", ADCS_config->RW[3]); 

    printf("Magnetorquer 1 config: %u\n", ADCS_config->MTQ.x);
    printf("Magnetorquer 2 config: %u\n", ADCS_config->MTQ.y);
    printf("Magnetorquer 3 config: %u\n\n", ADCS_config->MTQ.z);

    printf("Config test finished.");
}

void commandsTest_configs_unsaved(void)
{
    ADCS_returnState test_returnState = ADCS_OK;
    adcs_config adcs_Config;
    test_returnState = ADCS_get_full_config(&adcs_Config);
    if (test_returnState != ADCS_OK)
    {
        printf("ADCS_get_full_config returned %d", test_returnState);
        while(1);
    }


    printf("Default MoI matrix config:\n");
    printf("Ixx: %f\n", adcs_Config.MoI.diag.x);
    printf("Iyy: %f\n", adcs_Config.MoI.diag.y);
    printf("Iyy: %f\n", adcs_Config.MoI.diag.x);
    printf("Ixy: %f\n", adcs_Config.MoI.nondiag.x);
    printf("Ixz: %f\n", adcs_Config.MoI.nondiag.x);
    printf("Iyz: %f\n\n", adcs_Config.MoI.nondiag.x);
    

    printf("Default Rate Gyro config:\n");
    printf("Rate Gyro Config1: %u\n", adcs_Config.rate_gyro.gyro.x);
    printf("Rate Gyro Config2: %u\n", adcs_Config.rate_gyro.gyro.y);
    printf("Rate Gyro Config3: %u\n", adcs_Config.rate_gyro.gyro.z);
    printf("X Rate sensor Offset: %f\n", adcs_Config.rate_gyro.sensor_offset.x);
    printf("Y Rate sensor Offset: %f\n", adcs_Config.rate_gyro.sensor_offset.y);
    printf("Z Rate sensor Offset: %f\n", adcs_Config.rate_gyro.sensor_offset.z);
    printf("Rate Sensor Mult: %u\n\n", adcs_Config.rate_gyro.rate_sensor_mult);

    
    // Set MoI Matrix
    printf("Setting MoI Matrix:\n\n");
    moment_inertia_config moi_config;
    moi_config.diag.x  = 2;
    moi_config.diag.y = 2;
    moi_config.diag.z = 2;
    moi_config.nondiag.x  = 0.5;
    moi_config.nondiag.y  = -0.5;
    moi_config.nondiag.z  = 0.5;

    test_returnState = ADCS_set_MoI_mat(moi_config);
    if (test_returnState != ADCS_OK)
    {
        printf("ADCS_set_MoI_mat returned %d", test_returnState);
        while(1);
    }


    // Set Rate Gyro config
    printf("Setting Rate Gyro config:\n\n");
    rate_gyro_config rate_gyro_params;
    rate_gyro_params.gyro.x = 1;
    rate_gyro_params.gyro.y = 3;
    rate_gyro_params.gyro.z = 5;
    rate_gyro_params.sensor_offset.x = 0.16;
    rate_gyro_params.sensor_offset.y = -0.16;
    rate_gyro_params.sensor_offset.z = 0.16;
    rate_gyro_params.rate_sensor_mult = 1;

    test_returnState = ADCS_set_rate_gyro(rate_gyro_params);
    if (test_returnState != ADCS_OK)
    {
        printf("ADCS_set_rate_gyro returned %d", test_returnState);
        while(1);
    }



    test_returnState = ADCS_get_full_config(&adcs_Config);
    if (test_returnState != ADCS_OK)
    {
        printf("ADCS_get_full_config returned %d", test_returnState);
        while(1);
    }


    
    printf("Verifying modified configs:\n");
    test_returnState = ADCS_get_full_config(&adcs_Config);
    if (test_returnState != ADCS_OK)
    {
        printf("ADCS_get_full_config returned %d", test_returnState);
        while(1);
    }
    printf("Modified MoI matrix config:\n");
    printf("Ixx: %f\n", adcs_Config.MoI.diag.x);
    printf("Iyy: %f\n", adcs_Config.MoI.diag.y);
    printf("Iyy: %f\n", adcs_Config.MoI.diag.x);
    printf("Ixy: %f\n", adcs_Config.MoI.nondiag.x);
    printf("Ixz: %f\n", adcs_Config.MoI.nondiag.x);
    printf("Iyz: %f\n\n", adcs_Config.MoI.nondiag.x);
    

    printf("Modified Rate Gyro config:\n");
    printf("Rate Gyro Config1: %u\n", adcs_Config.rate_gyro.gyro.x);
    printf("Rate Gyro Config2: %u\n", adcs_Config.rate_gyro.gyro.y);
    printf("Rate Gyro Config3: %u\n", adcs_Config.rate_gyro.gyro.z);
    printf("X Rate sensor Offset: %f\n", adcs_Config.rate_gyro.sensor_offset.x);
    printf("Y Rate sensor Offset: %f\n", adcs_Config.rate_gyro.sensor_offset.y);
    printf("Z Rate sensor Offset: %f\n", adcs_Config.rate_gyro.sensor_offset.z);
    printf("Rate Sensor Mult: %u\n", adcs_Config.rate_gyro.rate_sensor_mult);

}

void commandsTest_upload(void)
{
    //* Upload firmware test 
    ADCS_returnState test_returnState = ADCS_OK;
    
    
    //Initiate file upload
    uint8_t file_dest = 3;  // External flash program 1 
    uint8_t block_size = 0; // Block size ignored, set to 0 (Firmware ref page.159)
    printf("Initiating file uploadn\n"\n);
    test_returnState = ADCS_initiate_file_upload(file_dest, block_size);
    if (test_returnState != ADCS_OK)
    {
        pritnf("initiate file upload returned %d", test_returnState);
        while(1);
    }
    
    // Above code will erase selected flash program area
    // Suggested wait time: 8s
    printf("Waiting 8s.\n\n");
    vTaskDelay(pdMS_TO_TICKS(8000));
    // Poll completion using Initialize Upload Complete TLM
    bool busy = true;

    while (busy)
    {
        test_returnState = ADCS_get_init_upload_stat(&busy);
        if (test_returnState != ADCS_OK)
        {
            printf("get upload status returned error: %d", test_returnState);
            while(1);
        }
        if (busy) 
        {
            printf("Still erasing flash, waiting 1s\n");
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        else{ 
            printf("Flash erase complete\n\n");
        }
    }


    // Send reset upload block
    printf("Resetting upload block.\n\n");
    test_returnState = ADCS_reset_upload_block();
    if (test_returnState != ADCS_OK)
    {
        printf("reset upload block returned error: %d", test_returnState);
        while(1);
    }



    // Continuous file packet upload
    printf("Uploading packets...\n\n");
    for (uint16_t packet_no = 0; packet_no < 1024, packet_no++)
    {
        ADCS_file_upload_packet(packet_no, "");
    }
    

    // Obtain hole map
    uint8_t hole_map[16];
    uint8_t num;
    printf("Getting hole map\n");
    test_returnState = ADCS_get_hole_map
}



// BELOW HERE LIES CODE THAT IS COMMON FOR MULTIPLE PARTS OF BINARY TEST PLAN. THESE FUNCTIONS
// ARE CALLED BY FUNCTIONS ABOVE HERE, AND SHOULD NOT BE RUN IN ISOLATION

void CubeMag_Common_Test(bool signalMCU) {

    ADCS_returnState test_returnState = ADCS_OK;
    adcs_state test_adcs_state;

    // Using Command ADCS_get_power_control() - Table 184, ensure that all nodes are selected PowOff before
    // proceeding.
    uint8_t *control = (uint8_t *)pvPortMalloc(10);
    if (control == NULL) {
        return ADCS_MALLOC_FAILED;
    }

    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while (1)
            ;
    }
    for (int i = 0; i < 10; i++) {
        printf("control[%d] = %d \n", i, control[i]);
    }

    // Using Command ADCS_set_power_control() - Table 184, switch on CubeControl Signal MCU by selecting PowOn.
    // Section Variables
    if (control == NULL) {
        return ADCS_MALLOC_FAILED;
    }
    control[Set_CubeCTRLSgn_Power] = 1;

    printf("Running ADCS_set_power_control...\n");
    test_returnState = ADCS_set_power_control(control);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_set_power_control returned %d \n", test_returnState);
        while (1)
            ;
    }

    // another read to make sure we are in the right state
    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while (1)
            ;
    }
    for (int i = 0; i < 10; i++) {
        printf("control[%d] = %d \n", i, control[i]);
    }

    vPortFree(control);

    uint8_t MTM_op_mode;
    // Test Section 6.1.1 CubeControl, Table 6-3,4 in test plan.
    // Using Command ADCS_get_current_state() - Table 89, select SigMainMag.
    if (signalMCU == 1) {
        printf("Running ADCS_set_MTM_op_mode to Main MTM Sampled Through Signal...\n");
        MTM_op_mode = 0;
    } else if (signalMCU == 0) {
        printf("Running ADCS_set_MTM_op_mode to Main MTM Sampled Through motor...\n");
        MTM_op_mode = 2;
    }

    test_returnState = ADCS_set_MTM_op_mode(MTM_op_mode);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_set_MTM_op_mode returned %d \n", test_returnState);
        while (1)
            ;
    }

    if (signalMCU == 1) {
        // Using Command ADCS_get_current_state() - Table 98,  ensure that the  Magnetometer Range Error is not
        // checked. If it is checked, then the magnetometer is  unable to measure a sufficient/overpowering
        // magnetic field. This can be solved by  ensuring that there is no contact to an anti-static mat or by
        // placing the magnetometers away from motors, power supplies, large ferromagnetic objects, etc. Run
        // ADCS_get_current_state()
        printf("Running ADCS_get_current_state...\n");
        test_returnState = ADCS_get_current_state(&test_adcs_state);
        if (test_returnState != ADCS_OK) {
            printf("ADCS_get_current_state returned %d \n", test_returnState);
            while (1)
                ;
        }

        printf("Magnetometer Range Error = %d \n", test_adcs_state.flags_arr[10]);
    }

    // Familiarise the axes of both magnetometers, as shown in Appendix A at the end of  this document.

    // Verify the operation of the  magnetometers in Table 6-3 by using Commands
    // ADCS_get_raw_sensor()/ADCS_get_MTM2_measurements() - Table 128 and Table 163.
    // Choose an axis on the main magnetometer and point it in the positive direction of the magnetic field lines
    //(north) to align the axis with the magnetic vector. Now rotate the main magnetometer around  this axis.
    // The chosen axis must remain positive while the other two axes will both go  negative and positive through
    // the rotation. Repeat this for all three axes to verify  polarities.
    adcs_raw_sensor *raw_sensor_measurements;
    raw_sensor_measurements = (adcs_raw_sensor *)pvPortMalloc(sizeof(adcs_raw_sensor));
    if (raw_sensor_measurements == NULL) {
        printf("malloc issues");
        while (1)
            ;
    }
    xyz16 mtm2;
    for (int i = 0; i < 15; i++) { // repeating 5 times for each axis = 15 times

        // ADCS_get_raw_sensor()
        printf("Running ADCS_get_raw_sensor...\n");
        test_returnState = ADCS_get_raw_sensor(raw_sensor_measurements);
        if (test_returnState != ADCS_OK) {
            printf("ADCS_get_raw_sensor returned %d \n", test_returnState);
            while (1)
                ;
        }

        printf("Raw Magnetometer X = %d \n",
               raw_sensor_measurements->MTM.x); // not 100% sure if this will print the sign of the float
        printf("Raw Magnetometer Y = %d \n", raw_sensor_measurements->MTM.y);
        printf("Raw Magnetometer Z = %d \n", raw_sensor_measurements->MTM.z);

        // ADCS_get_raw_sensor()
        printf("Running ADCS_get_MTM2_measurement...\n");
        test_returnState = ADCS_get_MTM2_measurements(&mtm2);
        if (test_returnState != ADCS_OK) {
            printf("ADCS_get_MTM2_measurement returned %d \n", test_returnState);
            while (1)
                ;
        }

        printf("Raw Secondary Mag X = %d \n", mtm2.x); // not 100% sure if this will print the sign of the float
        printf("Raw Secondary Mag Y = %d \n", mtm2.y);
        printf("Raw Secondary Mag Z = %d \n", mtm2.z);
    }

    vPortFree(raw_sensor_measurements);

    // Rotate the  main magnetometer and verify in Table 6-4 that the magnetic field vector displays both positive
    // and negative in X, Y, and Z directions correctly. Fill the following table accordingly:

    adcs_measures *measurements;
    measurements = (adcs_measures *)pvPortMalloc(sizeof(adcs_measures));
    if (measurements == NULL) {
        printf("malloc issues");
        while (1)
            ;
    }

    for (int i = 0; i < 15; i++) { // repeating 5 times for each axis = 15 times
        printf("Running ADCS_get_measurements...\n");
        test_returnState = ADCS_get_measurements(measurements);
        if (test_returnState != ADCS_OK) {
            printf("ADCS_get_measurements returned %d \n", test_returnState);
            while (1)
                ;
        }
        printf("Magnetic Field X = %+f \n",
               measurements->magnetic_field.x); // not 100% sure if this will print the sign of the float
        printf("Magnetic Field Y = %+f \n", measurements->magnetic_field.y);
        printf("Magnetic Field Z = %+f \n", measurements->magnetic_field.z);
    }

    vPortFree(measurements);
}

void CubeTorquers_Common_Test(void) {

    ADCS_returnState test_returnState = ADCS_OK;

    ADCS_set_enabled_state(1);

    ADCS_set_unix_t(0, 0);

    // Using Command ADCS_get_power_control() - Table 184, ensure that all nodes are selected PowOff before
    // proceeding.
    uint8_t *control = (uint8_t *)pvPortMalloc(10);
    if (control == NULL) {
        return ADCS_MALLOC_FAILED;
    }

    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while (1)
            ;
    }
    for (int i = 0; i < 10; i++) {
        printf("control[%d] = %d \n", i, control[i]);
    }

    // Using Command ADCS_set_power_control() - Table 184, switch on CubeControl Signal MCU by selecting PowOn.
    // Section Variables
    if (control == NULL) {
        return ADCS_MALLOC_FAILED;
    }
    control[Set_CubeCTRLSgn_Power] = 1;

    printf("Running ADCS_set_power_control...\n");
    test_returnState = ADCS_set_power_control(control);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_set_power_control returned %d \n", test_returnState);
        while (1)
            ;
    }

    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while (1)
            ;
    }
    for (int i = 0; i < 10; i++) {
        printf("control[%d] = %d \n", i, control[i]);
    }

    adcs_pwr_temp *power_temp_measurements;
    power_temp_measurements = (adcs_pwr_temp *)pvPortMalloc(sizeof(adcs_pwr_temp));
    if (power_temp_measurements == NULL) {
        printf("malloc issues");
        while (1)
            ;
    }
    int16_t maxDuty = 800;
    xyz16 dutycycle = {maxDuty, 0, 0}; // TODO Verify this!
    for (int i = 0; i < 3; i++) {

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

        // ADCS_set_magnetorquer_output
        printf("Running ADCS_set_magnetorquer_output -POSITIVE DIRECTION-...\n");
        test_returnState = ADCS_set_magnetorquer_output(dutycycle);
        if (test_returnState != ADCS_OK) {
            printf("ADCS_set_magnetorquer_output returned %d \n", test_returnState);
            while (1)
                ;
        }
        // Confirm the current measurement and the direction of the magnetic field in the table  below.
        // The direction can be confirmed by either placing a compass directly in line with  the magnetorquer and
        // observing the field direction or by using an external  magnetometer. Note that the magnetorquer pulses
        // on for a maximum of 0.8 seconds  and then switches off.

        // ADCS_get_power_temp()
        printf("Running ADCS_get_power_temp...\n");
        test_returnState = ADCS_get_power_temp(power_temp_measurements);
        if (test_returnState != ADCS_OK) {
            printf("ADCS_get_power_temp returned %d \n", test_returnState);
            while (1)
                ;
        }

        printf("magnetorquer_I = %f \n", power_temp_measurements->magnetorquer_I);

        // Change the command to maximum negative and confirm the current and direction  again.
        dutycycle.x = -(dutycycle.x);
        dutycycle.y = -(dutycycle.y);
        dutycycle.z = -(dutycycle.z);
        // ADCS_set_magnetorquer_output
        printf("Running ADCS_set_magnetorquer_output (NEGATIVE DIRECTION)...\n");
        test_returnState = ADCS_set_magnetorquer_output(dutycycle);
        if (test_returnState != ADCS_OK) {
            printf("ADCS_set_magnetorquer_output returned %d \n", test_returnState);
            while (1)
                ;
        }
        // Confirm the current measurement and the direction of the magnetic field in the table  below.
        // The direction can be confirmed by either placing a compass directly in line with  the magnetorquer and
        // observing the field direction or by using an external  magnetometer. Note that the magnetorquer pulses
        // on for a maximum of 0.8 seconds  and then switches off.

        // ADCS_get_power_temp()
        printf("Running ADCS_get_power_temp...\n");
        test_returnState = ADCS_get_power_temp(power_temp_measurements);
        if (test_returnState != ADCS_OK) {
            printf("ADCS_get_power_temp returned %d \n", test_returnState);
            while (1)
                ;
        }

        printf("magnetorquer_I = %f \n", power_temp_measurements->magnetorquer_I);

        // Command the magnetorquer to zero to turn off the magnetorquer.
        dutycycle.x = 0;
        dutycycle.y = 0;
        dutycycle.z = 0;
        // ADCS_set_magnetorquer_output
        printf("Running ADCS_set_magnetorquer_output (OFF)...\n");
        test_returnState = ADCS_set_magnetorquer_output(dutycycle);
        if (test_returnState != ADCS_OK) {
            printf("ADCS_set_magnetorquer_output returned %d \n", test_returnState);
            while (1)
                ;
        }

        // Ensure all magnetorquers are turned off before continuing

        // ADCS_get_power_temp()
        printf("Running ADCS_get_power_temp...\n");
        test_returnState = ADCS_get_power_temp(power_temp_measurements);
        if (test_returnState != ADCS_OK) {
            printf("ADCS_get_power_temp returned %d \n", test_returnState);
            while (1)
                ;
        }

        printf("magnetorquer_I = %f \n", power_temp_measurements->magnetorquer_I);

        // Repeat these steps for the Y and Z magnetorquer rods and record the required values in table 6-5.
    }

    vPortFree(power_temp_measurements);
}

void ReactionWheels_Common_Test(uint8_t wheel_number) {
    ADCS_returnState test_returnState = ADCS_OK;

    // Using Command ADCS_get_power_control() - Table 184, ensure that all nodes are selected PowOff before
    // proceeding. Section Variables
    uint8_t *control = (uint8_t *)pvPortMalloc(10);
    if (control == NULL) {
        return ADCS_MALLOC_FAILED;
    }

    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while (1)
            ;
    }
    for (int i = 0; i < 10; i++) {
        printf("control[%d] = %d \n", i, control[i]);
    }

    if (wheel_number == 1) {
        control[Set_CubeWheel1_Power] = 1;
        control[Set_CubeWheel2_Power] = 0;
        control[Set_CubeWheel3_Power] = 0;
    } else if (wheel_number == 2) {
        control[Set_CubeWheel1_Power] = 0;
        control[Set_CubeWheel2_Power] = 1;
        control[Set_CubeWheel3_Power] = 0;
    } else if (wheel_number == 3) {
        control[Set_CubeWheel1_Power] = 0;
        control[Set_CubeWheel2_Power] = 0;
        control[Set_CubeWheel3_Power] = 1;
    }

    printf("Running ADCS_set_power_control...\n");
    test_returnState = ADCS_set_power_control(control);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_set_power_control returned %d \n", test_returnState);
        while (1)
            ;
    }

    // Delay so that the get isn't too soon after the set
    for (int k = 0; k < 100000; k++) {
    }

    // another read to make sure we are in the right state
    printf("Running ADCS_get_power_control...\n");
    test_returnState = ADCS_get_power_control(control);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_power_control returned %d \n", test_returnState);
        while (1)
            ;
    }
    for (int i = 0; i < 10; i++) {
        printf("control[%d] = %d \n", i, control[i]);
    }

    adcs_state test_adcs_state;
    // Run ADCS_get_current_state()
    printf("Running ADCS_get_current_state...\n");
    test_returnState = ADCS_get_current_state(&test_adcs_state);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_get_current_state returned %d \n", test_returnState);
        while (1)
            ;
    }

    printf("att_estimate mode = %d \n", test_adcs_state.att_estimate_mode);
    printf("att_ctrl_mode = %d \n", test_adcs_state.att_ctrl_mode);
    printf("run_mode = %d \n", test_adcs_state.run_mode);

    if (wheel_number == 1) {
        printf("CubeWheel1 Signal Enabled = %d \n", test_adcs_state.flags_arr[4]);
    } else if (wheel_number == 2) {
        printf("CubeWheel2 Signal Enabled = %d \n", test_adcs_state.flags_arr[5]);
    } else if (wheel_number == 3) {
        printf("CubeWheel3 Signal Enabled = %d \n", test_adcs_state.flags_arr[6]);
    }

    // need to test if all other flags == 0. Simpler to do in code than via human.
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
        while (1)
            ;
    }

    adcs_pwr_temp *power_temp_measurements;
    power_temp_measurements = (adcs_pwr_temp *)pvPortMalloc(sizeof(adcs_pwr_temp));
    if (power_temp_measurements == NULL) {
        printf("malloc issues");
        while (1)
            ;
    }

    for (int i = 0; i < 3; i++) {

        if (i == 0) {
            rpm = 0;
        } else if (i == 1) {
            rpm = -2000;
        } else if (i == 2) {
            rpm = 0;
        }

        if (wheel_number == 1) {
            speed.y = rpm; // 2U ADCS CubeWheel 1 is set to the y direction
        } else if (wheel_number == 2) {
            speed.y = rpm;
        } else if (wheel_number == 3) {
            speed.z = rpm;
        }

        // Using command ADCS_set_wheel_speed() - Table 82. Set the  commanded X speed to 4000 rpm.
        printf("Running ADCS_set_wheel_speed...\n");
        test_returnState = ADCS_set_wheel_speed(speed);
        if (test_returnState != ADCS_OK) {
            printf("ADCS_get_current_state returned %d \n", test_returnState);
            while (1)
                ;
        }

        // After 10 seconds the wheel will settle to the commanded speed. Listen to the wheel to  ensure that the
        // correct one is spinning up and that the other two wheels are silent. Verify the following in Table 7-5

        for (int i = 0; i < 15; i++) { // repeating 5 times for each axis = 15 times
            printf("Running ADCS_get_measurements...\n");
            test_returnState = ADCS_get_measurements(measurements);
            if (test_returnState != ADCS_OK) {
                printf("ADCS_get_measurements returned %d \n", test_returnState);
                while (1)
                    ;
            }
            printf("X Wheel Speed = %+f \n",
                   measurements->wheel_speed.x); // not 100% sure if this will print the sign of the float
            printf("Y Wheel Speed = %+f \n", measurements->wheel_speed.y);
            printf("Z Wheel Speed = %+f \n", measurements->wheel_speed.z);
        }

        // ADCS_get_power_temp()

        printf("Running ADCS_get_power_temp...\n");
        test_returnState = ADCS_get_power_temp(power_temp_measurements);
        if (test_returnState != ADCS_OK) {
            printf("ADCS_get_power_temp returned %d \n", test_returnState);
            while (1)
                ;
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

    if (wheel_number == 1) {
        control[Set_CubeWheel1_Power] = 0;
    } else if (wheel_number == 2) {
        control[Set_CubeWheel2_Power] = 0;
    } else if (wheel_number == 3) {
        control[Set_CubeWheel3_Power] = 0;
    }

    printf("Running ADCS_set_power_control...\n");
    test_returnState = ADCS_set_power_control(control);
    if (test_returnState != ADCS_OK) {
        printf("ADCS_set_power_control returned %d \n", test_returnState);
        while (1)
            ;
    }
    vPortFree(control);
}
