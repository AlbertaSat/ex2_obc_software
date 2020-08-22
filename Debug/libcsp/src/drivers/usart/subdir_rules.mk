################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
libcsp/src/drivers/usart/%.obj: ../libcsp/src/drivers/usart/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/alexqi/ti/ccs1010/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/bin/armcl" -mv7R5 --code_state=32 --float_support=VFPv3D16 --include_path="/home/alexqi/Ex2_Code/ex2_obc_software" --include_path="/home/alexqi/Ex2_Code/ex2_obc_software/ex2_obc_temp_software" --include_path="/home/alexqi/Ex2_Code/ex2_obc_software/ex2_services/Platform/obc" --include_path="/home/alexqi/Ex2_Code/ex2_obc_software/include/ex2_os" --include_path="/home/alexqi/Ex2_Code/ex2_obc_software/ex2_obc_software" --include_path="/home/alexqi/Ex2_Code/ex2_obc_software/ex2_services/Platform/demo/hal" --include_path="/home/alexqi/Ex2_Code/ex2_obc_software/ex2_services/Platform/demo" --include_path="/home/alexqi/Ex2_Code/ex2_obc_software/ex2_services/Services" --include_path="/home/alexqi/Ex2_Code/ex2_obc_software/ex2_services/ex2_demo_software" --include_path="/home/alexqi/Ex2_Code/ex2_obc_software/ex2_services/Platform" --include_path="/home/alexqi/Ex2_Code/ex2_obc_software/libcsp/include" --include_path="/home/alexqi/Ex2_Code/ex2_obc_software/include" --include_path="/home/alexqi/ti/ccs1010/ccs/tools/compiler/ti-cgt-arm_20.2.1.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="libcsp/src/drivers/usart/$(basename $(<F)).d_raw" --obj_directory="libcsp/src/drivers/usart" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


