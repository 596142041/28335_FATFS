################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
bsp/src/CAN.obj: ../bsp/src/CAN.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -mt -ml --float_support=softlib --include_path="D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="H:/DSP__prj/28335_FATFS/hex_to_bin/inc" --include_path="H:/DSP__prj/28335_FATFS/CAN_ISP/inc" --include_path="H:/DSP__prj/28335_FATFS/Lib" --include_path="H:/DSP__prj/28335_FATFS/can_bootloader/inc" --include_path="H:/DSP__prj/28335_FATFS/bsp/inc" --include_path="H:/DSP__prj/28335_FATFS/ff12b/inc" --include_path="H:/DSP__prj/28335_FATFS/user/inc" --include_path="H:/DSP__prj/28335_FATFS/user_lib/asm" --include_path="H:/DSP__prj/28335_FATFS/user_lib/cmd" --include_path="H:/DSP__prj/28335_FATFS/user_lib/inc" --advice:performance=all -g --c99 --display_error_number --diag_wrap=off --diag_warning=225 --preproc_with_compile --preproc_dependency="bsp/src/CAN.d" --obj_directory="bsp/src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

bsp/src/SD.obj: ../bsp/src/SD.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -mt -ml --float_support=softlib --include_path="D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="H:/DSP__prj/28335_FATFS/hex_to_bin/inc" --include_path="H:/DSP__prj/28335_FATFS/CAN_ISP/inc" --include_path="H:/DSP__prj/28335_FATFS/Lib" --include_path="H:/DSP__prj/28335_FATFS/can_bootloader/inc" --include_path="H:/DSP__prj/28335_FATFS/bsp/inc" --include_path="H:/DSP__prj/28335_FATFS/ff12b/inc" --include_path="H:/DSP__prj/28335_FATFS/user/inc" --include_path="H:/DSP__prj/28335_FATFS/user_lib/asm" --include_path="H:/DSP__prj/28335_FATFS/user_lib/cmd" --include_path="H:/DSP__prj/28335_FATFS/user_lib/inc" --advice:performance=all -g --c99 --display_error_number --diag_wrap=off --diag_warning=225 --preproc_with_compile --preproc_dependency="bsp/src/SD.d" --obj_directory="bsp/src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

bsp/src/W25Q64BV.obj: ../bsp/src/W25Q64BV.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -mt -ml --float_support=softlib --include_path="D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="H:/DSP__prj/28335_FATFS/hex_to_bin/inc" --include_path="H:/DSP__prj/28335_FATFS/CAN_ISP/inc" --include_path="H:/DSP__prj/28335_FATFS/Lib" --include_path="H:/DSP__prj/28335_FATFS/can_bootloader/inc" --include_path="H:/DSP__prj/28335_FATFS/bsp/inc" --include_path="H:/DSP__prj/28335_FATFS/ff12b/inc" --include_path="H:/DSP__prj/28335_FATFS/user/inc" --include_path="H:/DSP__prj/28335_FATFS/user_lib/asm" --include_path="H:/DSP__prj/28335_FATFS/user_lib/cmd" --include_path="H:/DSP__prj/28335_FATFS/user_lib/inc" --advice:performance=all -g --c99 --display_error_number --diag_wrap=off --diag_warning=225 --preproc_with_compile --preproc_dependency="bsp/src/W25Q64BV.d" --obj_directory="bsp/src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


