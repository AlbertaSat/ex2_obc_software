################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
libcsp/src/interfaces/%.obj: ../libcsp/src/interfaces/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs1000/ccs/tools/compiler/ti-cgt-arm_20.2.0.LTS/bin/armcl" -mv7R5 --code_state=32 --float_support=VFPv3D16 --include_path="C:/Users/Andrew/Code/my_workspace/can/reliance_edge/os/freertos/include" --include_path="C:/Users/Andrew/Code/my_workspace/can/ex2_system_tasks" --include_path="C:/Users/Andrew/Code/my_workspace/can/reliance_edge/core/include" --include_path="C:/Users/Andrew/Code/my_workspace/can" --include_path="C:/Users/Andrew/Code/my_workspace/can/include/athena_drivers" --include_path="C:/Users/Andrew/Code/my_workspace/can/Project/host" --include_path="C:/Users/Andrew/Code/my_workspace/can/source/athena_drivers" --include_path="C:/Users/Andrew/Code/my_workspace/can/reliance_edge/include" --include_path="C:/Users/Andrew/Code/my_workspace/can/ex2_services/Platform/obc" --include_path="C:/Users/Andrew/Code/my_workspace/can/include/ex2_os" --include_path="C:/Users/Andrew/Code/my_workspace/can/ex2_obc_software" --include_path="C:/Users/Andrew/Code/my_workspace/can/ex2_services/Platform/demo/hal" --include_path="C:/Users/Andrew/Code/my_workspace/can/ex2_services/Platform/demo" --include_path="C:/Users/Andrew/Code/my_workspace/can/ex2_services/Services" --include_path="C:/Users/Andrew/Code/my_workspace/can/ex2_services/ex2_demo_software" --include_path="C:/Users/Andrew/Code/my_workspace/can/ex2_services/Platform" --include_path="C:/Users/Andrew/Code/my_workspace/can/libcsp/include" --include_path="C:/Users/Andrew/Code/my_workspace/can/include" --include_path="C:/ti/ccs1000/ccs/tools/compiler/ti-cgt-arm_20.2.0.LTS/include" --define=SYSTEM_APP_ID=_OBC_APP_ID_ --define=UHF_IS_STUBBED --define=SBAND_IS_STUBBED -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="libcsp/src/interfaces/$(basename $(<F)).d_raw" --obj_directory="libcsp/src/interfaces" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


