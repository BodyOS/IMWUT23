################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
dsplib/source/vector/%.o: ../dsplib/source/vector/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"C:/ti/bin/msp430-elf-gcc-9.3.1.exe" -c -mmcu=msp430fr5994 -mhwmult=f5series -I"C:/ti/ccs1220/ccs/ccs_base/msp430/include_gcc" -I"C:/Users/Saad/Downloads/BodyOS/BodyOS" -I"C:/Users/Saad/Downloads/BodyOS/BodyOS/dsplib/include" -I"C:/Users/Saad/Downloads/BodyOS/BodyOS/dsplib/source" -I"C:/Users/Saad/Downloads/BodyOS/BodyOS/hardwareSim" -I"C:/Users/Saad/Downloads/BodyOS/BodyOS/mcu/msp430" -I"C:/Users/Saad/Downloads/BodyOS/BodyOS/scheduler" -I"C:/Users/Saad/Downloads/BodyOS/BodyOS" -I"C:/ti/msp430-elf/include" -Og -g -gdwarf-3 -gstrict-dwarf -Wall -mlarge -mcode-region=none -mdata-region=lower -MMD -MP -MF"dsplib/source/vector/$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


