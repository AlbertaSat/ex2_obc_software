################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
ex2_hal/ex2_eps_software/hardware_interface/source/%.obj: ../ex2_hal/ex2_eps_software/hardware_interface/source/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"D:/ti/ccs1040/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin/armcl" -mv7R5 --code_state=32 --float_support=VFPv3D16 --include_path="D:/AlbertaSat/repos/ex2_obc_software/main" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_hal/ex2_dfgm_software/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_hal/ex2_adcs_software/equipment_handler/inc" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_hal/ex2_adcs_software/hardware_interface/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_hal/ex2_hyperion_solar_panel_software/equipment_handler/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_services/Services/include/housekeeping" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_ftp/Program/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/libcsp/src" --include_path="D:/AlbertaSat/repos/ex2_obc_software/libcsp/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/include/ex2_os" --include_path="D:/AlbertaSat/repos/ex2_obc_software/F021_API" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_hal/ex2_hyperion_solar_panel_software/hardware_interface/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_services/Services/include/util" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_hal/ex2_uhf_software/equipment_handler/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_hal/ex2_gps_software/services" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_services/Services/include/communication" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_services/Services/include/util" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_hal/ex2_gps_software/hardware_interface/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_hal/ex2_gps_software/equipment_handler/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_system/include/beacon" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_system/include/coordinate_management" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_system/include/diagnostic" --include_path="D:/AlbertaSat/repos/ex2_obc_software" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_hal/ex2_uhf_software/hardware_interface/include" --include_path="D:/ti/ccs1040/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/libcsp/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_services/Services/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_hal/ex2_sband_software/hardware_interface/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/reliance_edge/core/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_hal/ex2_eps_software/hardware_interface/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/reliance_edge/os/freertos/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_system/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_hal/ex2_sband_software/equipment_handler/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_hal/ex2_athena_software/equipment_handler/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/reliance_edge/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/include/ex2_os" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_services/Services" --include_path="D:/AlbertaSat/repos/ex2_obc_software/ex2_services/ex2_demo_software" --include_path="D:/AlbertaSat/repos/ex2_obc_software/libcsp/include" --include_path="D:/AlbertaSat/repos/ex2_obc_software/include" --include_path="D:/ti/ccs1040/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/include" --define=SYSTEM_APP_ID=_OBC_APP_ID_ --define=IS_3U --define=IS_ATHENA --define=IS_FLATSAT --define=ADCS_IS_STUBBED --define=UHF_IS_STUBBED --define=WATCHDOG_IS_STUBBED --define=SBAND_IS_STUBBED -g --c99 --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="ex2_hal/ex2_eps_software/hardware_interface/source/$(basename $(<F)).d_raw" --obj_directory="ex2_hal/ex2_eps_software/hardware_interface/source" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


