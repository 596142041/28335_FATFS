################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
user_lib/asm/DSP2833x_ADC_cal.obj: ../user_lib/asm/DSP2833x_ADC_cal.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -mt -ml --float_support=softlib --include_path="D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="H:/DSP__prj/28335_FATFS/hex_to_bin/inc" --include_path="H:/DSP__prj/28335_FATFS/CAN_ISP/inc" --include_path="H:/DSP__prj/28335_FATFS/Lib" --include_path="H:/DSP__prj/28335_FATFS/can_bootloader/inc" --include_path="H:/DSP__prj/28335_FATFS/bsp/inc" --include_path="H:/DSP__prj/28335_FATFS/ff12b/inc" --include_path="H:/DSP__prj/28335_FATFS/user/inc" --include_path="H:/DSP__prj/28335_FATFS/user_lib/asm" --include_path="H:/DSP__prj/28335_FATFS/user_lib/cmd" --include_path="H:/DSP__prj/28335_FATFS/user_lib/inc" --advice:performance=all -g --c99 --display_error_number --diag_wrap=off --diag_warning=225 --preproc_with_compile --preproc_dependency="user_lib/asm/DSP2833x_ADC_cal.d" --obj_directory="user_lib/asm" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

user_lib/asm/DSP2833x_CodeStartBranch.obj: ../user_lib/asm/DSP2833x_CodeStartBranch.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -mt -ml --float_support=softlib --include_path="D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="H:/DSP__prj/28335_FATFS/hex_to_bin/inc" --include_path="H:/DSP__prj/28335_FATFS/CAN_ISP/inc" --include_path="H:/DSP__prj/28335_FATFS/Lib" --include_path="H:/DSP__prj/28335_FATFS/can_bootloader/inc" --include_path="H:/DSP__prj/28335_FATFS/bsp/inc" --include_path="H:/DSP__prj/28335_FATFS/ff12b/inc" --include_path="H:/DSP__prj/28335_FATFS/user/inc" --include_path="H:/DSP__prj/28335_FATFS/user_lib/asm" --include_path="H:/DSP__prj/28335_FATFS/user_lib/cmd" --include_path="H:/DSP__prj/28335_FATFS/user_lib/inc" --advice:performance=all -g --c99 --display_error_number --diag_wrap=off --diag_warning=225 --preproc_with_compile --preproc_dependency="user_lib/asm/DSP2833x_CodeStartBranch.d" --obj_directory="user_lib/asm" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

user_lib/asm/DSP2833x_usDelay.obj: ../user_lib/asm/DSP2833x_usDelay.asm $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C2000 Compiler'
	"D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -mt -ml --float_support=softlib --include_path="D:/Program Files/TI/CCS710/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --include_path="H:/DSP__prj/28335_FATFS/hex_to_bin/inc" --include_path="H:/DSP__prj/28335_FATFS/CAN_ISP/inc" --include_path="H:/DSP__prj/28335_FATFS/Lib" --include_path="H:/DSP__prj/28335_FATFS/can_bootloader/inc" --include_path="H:/DSP__prj/28335_FATFS/bsp/inc" --include_path="H:/DSP__prj/28335_FATFS/ff12b/inc" --include_path="H:/DSP__prj/28335_FATFS/user/inc" --include_path="H:/DSP__prj/28335_FATFS/user_lib/asm" --include_path="H:/DSP__prj/28335_FATFS/user_lib/cmd" --include_path="H:/DSP__prj/28335_FATFS/user_lib/inc" --advice:performance=all -g --c99 --display_error_number --diag_wrap=off --diag_warning=225 --preproc_with_compile --preproc_dependency="user_lib/asm/DSP2833x_usDelay.d" --obj_directory="user_lib/asm" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


