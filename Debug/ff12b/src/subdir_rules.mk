################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
ff12b/src/diskio.obj: ../ff12b/src/diskio.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -mt -ml --float_support=softlib --include_path="D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="H:/DSP__prj/28335_FATFS/hex_to_bin/inc" --include_path="H:/DSP__prj/28335_FATFS/CAN_ISP/inc" --include_path="H:/DSP__prj/28335_FATFS/Lib" --include_path="H:/DSP__prj/28335_FATFS/can_bootloader/inc" --include_path="H:/DSP__prj/28335_FATFS/bsp/inc" --include_path="H:/DSP__prj/28335_FATFS/ff12b/inc" --include_path="H:/DSP__prj/28335_FATFS/user/inc" --include_path="H:/DSP__prj/28335_FATFS/user_lib/asm" --include_path="H:/DSP__prj/28335_FATFS/user_lib/cmd" --include_path="H:/DSP__prj/28335_FATFS/user_lib/inc" --advice:performance=all -g --c99 --display_error_number --diag_wrap=off --diag_warning=225 --preproc_with_compile --preproc_dependency="ff12b/src/diskio.d" --obj_directory="ff12b/src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

ff12b/src/ff.obj: ../ff12b/src/ff.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -mt -ml --float_support=softlib --include_path="D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="H:/DSP__prj/28335_FATFS/hex_to_bin/inc" --include_path="H:/DSP__prj/28335_FATFS/CAN_ISP/inc" --include_path="H:/DSP__prj/28335_FATFS/Lib" --include_path="H:/DSP__prj/28335_FATFS/can_bootloader/inc" --include_path="H:/DSP__prj/28335_FATFS/bsp/inc" --include_path="H:/DSP__prj/28335_FATFS/ff12b/inc" --include_path="H:/DSP__prj/28335_FATFS/user/inc" --include_path="H:/DSP__prj/28335_FATFS/user_lib/asm" --include_path="H:/DSP__prj/28335_FATFS/user_lib/cmd" --include_path="H:/DSP__prj/28335_FATFS/user_lib/inc" --advice:performance=all -g --c99 --display_error_number --diag_wrap=off --diag_warning=225 --preproc_with_compile --preproc_dependency="ff12b/src/ff.d" --obj_directory="ff12b/src" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


