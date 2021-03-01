################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
libcsp/src/drivers/can/%.obj: ../libcsp/src/drivers/can/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/robert/bin/ti/ccs1011/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/bin/armcl" -mv7R5 --code_state=32 --float_support=VFPv3D16 --include_path="/home/robert/Documents/AbSat/ex2_obc_software/reliance_edge/os/freertos/include" --include_path="/home/robert/Documents/AbSat/ex2_obc_software/ex2_system_tasks" --include_path="/home/robert/Documents/AbSat/ex2_obc_software/reliance_edge/core/include" --include_path="/home/robert/Documents/AbSat/ex2_obc_software" --include_path="/home/robert/Documents/AbSat/ex2_obc_software/include/athena_drivers" --include_path="/home/robert/Documents/AbSat/ex2_obc_software/Project/host" --include_path="/home/robert/Documents/AbSat/ex2_obc_software/source/athena_drivers" --include_path="/home/robert/Documents/AbSat/ex2_obc_software/reliance_edge/include" --include_path="/home/robert/Documents/AbSat/ex2_obc_software/ex2_services/Platform/obc" --include_path="/home/robert/Documents/AbSat/ex2_obc_software/include/ex2_os" --include_path="/home/robert/Documents/AbSat/ex2_obc_software/ex2_obc_software" --include_path="/home/robert/Documents/AbSat/ex2_obc_software/ex2_services/Platform/demo/hal" --include_path="/home/robert/Documents/AbSat/ex2_obc_software/ex2_services/Platform/demo" --include_path="/home/robert/Documents/AbSat/ex2_obc_software/ex2_services/Services" --include_path="/home/robert/Documents/AbSat/ex2_obc_software/ex2_services/ex2_demo_software" --include_path="/home/robert/Documents/AbSat/ex2_obc_software/ex2_services/Platform" --include_path="/home/robert/Documents/AbSat/ex2_obc_software/libcsp/include" --include_path="/home/robert/Documents/AbSat/ex2_obc_software/include" --include_path="/home/robert/bin/ti/ccs1011/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/include" --define=SYSTEM_APP_ID=_OBC_APP_ID_ --define=UHF_IS_STUBBED --define=SBAND_IS_STUBBED -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="libcsp/src/drivers/can/$(basename $(<F)).d_raw" --obj_directory="libcsp/src/drivers/can" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


