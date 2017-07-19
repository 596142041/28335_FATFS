################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
can_bootloader/src/can_bootloader.obj: ../can_bootloader/src/can_bootloader.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -mt -ml --float_support=softlib --include_path="D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="H:/DSP__prj/28335_FATFS/hex_to_bin/inc" --include_path="H:/DSP__prj/28335_FATFS/CAN_ISP/inc" --include_path="H:/DSP__prj/28335_FATFS/Lib" --include_path="H:/DSP__prj/28335_FATFS/can_bootloader/inc" --include_path="H:/DSP__prj/28335_FATFS/bsp/inc" --include_path="H:/DSP__prj/28335_FATFS/ff12b/inc" --include_path="H:/DSP__prj/28335_FATFS/user/inc" --include_path="H:/DSP__prj/28335_FATFS/user_lib/asm" --include_path="H:/DSP__prj/28335_FATFS/user_lib/cmd" --include_path="H:/DSP__prj/28335_FATFS/user_lib/inc" --advice:performance=all -g --c99 --display_error_number --diag_wrap=off --diag_warning=225 --preproc_with_compile --preproc_dependency="can_bootloader/src/can_bootloader.d" --obj_directory="can_bootloader/src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

can_bootloader/src/can_driver.obj: ../can_bootloader/src/can_driver.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -mt -ml --float_support=softlib --include_path="D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="H:/DSP__prj/28335_FATFS/hex_to_bin/inc" --include_path="H:/DSP__prj/28335_FATFS/CAN_ISP/inc" --include_path="H:/DSP__prj/28335_FATFS/Lib" --include_path="H:/DSP__prj/28335_FATFS/can_bootloader/inc" --include_path="H:/DSP__prj/28335_FATFS/bsp/inc" --include_path="H:/DSP__prj/28335_FATFS/ff12b/inc" --include_path="H:/DSP__prj/28335_FATFS/user/inc" --include_path="H:/DSP__prj/28335_FATFS/user_lib/asm" --include_path="H:/DSP__prj/28335_FATFS/user_lib/cmd" --include_path="H:/DSP__prj/28335_FATFS/user_lib/inc" --advice:performance=all -g --c99 --display_error_number --diag_wrap=off --diag_warning=225 --preproc_with_compile --preproc_dependency="can_bootloader/src/can_driver.d" --obj_directory="can_bootloader/src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


