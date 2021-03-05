################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
libcsp/src/drivers/can/%.obj: ../libcsp/src/drivers/can/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs1000/ccs/tools/compiler/ti-cgt-arm_20.2.0.LTS/bin/armcl" -mv7R5 --code_state=32 --float_support=VFPv3D16 --include_path="C:/Users/Andrew/Code/my_workspace/ex2_obc_software/ex2_hal/ex2_uhf_software/equipment_handler/include" --include_path="C:/Users/Andrew/Code/my_workspace/ex2_obc_software" --include_path="C:/Users/Andrew/Code/my_workspace/ex2_obc_software/ex2_hal/ex2_uhf_software/hardware_interface/include" --include_path="C:/ti/ccs1000/ccs/tools/compiler/ti-cgt-arm_20.2.0.LTS/include" --include_path="C:/Users/Andrew/Code/my_workspace/ex2_obc_software/include" --include_path="C:/Users/Andrew/Code/my_workspace/ex2_obc_software/libcsp/include" --include_path="C:/Users/Andrew/Code/my_workspace/ex2_obc_software/ex2_services/Services/include" --include_path="C:/Users/Andrew/Code/my_workspace/ex2_obc_software/ex2_hal/ex2_sband_software/hardware_interface/include" --include_path="C:/Users/Andrew/Code/my_workspace/ex2_obc_software/reliance_edge/core/include" --include_path="C:/Users/Andrew/Code/my_workspace/ex2_obc_software/ex2_hal/ex2_eps_software/hardware_interface/include" --include_path="C:/Users/Andrew/Code/my_workspace/ex2_obc_software/reliance_edge/os/freertos/include" --include_path="C:/Users/Andrew/Code/my_workspace/ex2_obc_software/ex2_system/include" --include_path="C:/Users/Andrew/Code/my_workspace/ex2_obc_software/ex2_hal/ex2_sband_software/equipment_handler/include" --include_path="C:/Users/Andrew/Code/my_workspace/ex2_obc_software/ex2_hal/ex2_athena_software/equipment_handler/include" --include_path="C:/Users/Andrew/Code/my_workspace/ex2_obc_software/reliance_edge/include" --define=SYSTEM_APP_ID=_OBC_APP_ID_ --define=UHF_IS_STUBBED --define=SBAND_IS_STUBBED -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="libcsp/src/drivers/can/$(basename $(<F)).d_raw" --obj_directory="libcsp/src/drivers/can" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


