################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
hardwareSim/%.o: ../hardwareSim/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/erenyildiz/ti/ccs1120/ccs/tools/compiler/msp430-gcc-9.3.0.31_linux64/bin/msp430-elf-gcc-9.3.0" -c -mmcu=msp430fr5994 -mhwmult=f5series -I"/home/erenyildiz/ti/ccs1120/ccs/ccs_base/msp430/include_gcc" -I"/home/erenyildiz/CCS_Workspace/BodyOS/Mayfly/hardwareSim" -I"/home/erenyildiz/CCS_Workspace/BodyOS/Mayfly/libalpaca" -I"/home/erenyildiz/CCS_Workspace/BodyOS/Mayfly/libmsp" -I"/home/erenyildiz/CCS_Workspace/BodyOS/Mayfly" -I"/home/erenyildiz/ti/ccs1120/ccs/tools/compiler/msp430-gcc-9.3.0.31_linux64/msp430-elf/include" -Og -g -gdwarf-3 -gstrict-dwarf -Wall -mlarge -mcode-region=none -mdata-region=lower -MMD -MP -MF"hardwareSim/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


