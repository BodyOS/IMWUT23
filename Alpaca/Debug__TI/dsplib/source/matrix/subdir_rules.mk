################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
dsplib/source/matrix/%.obj: ../dsplib/source/matrix/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/bin/cl430" -vmspx --data_model=restricted --use_hw_mpy=F5 --include_path="C:/ti/ccs1220/ccs/ccs_base/msp430/include" --include_path="C:/Users/Saad/Downloads/All-fresh/Alpaca/dsplib/include" --include_path="C:/Users/Saad/Downloads/All-fresh/Alpaca/dsplib/source" --include_path="C:/Users/Saad/Downloads/All-fresh/Alpaca/hardwareSim" --include_path="C:/Users/Saad/Downloads/All-fresh/Alpaca/libalpaca" --include_path="C:/Users/Saad/Downloads/All-fresh/Alpaca/libio" --include_path="C:/Users/Saad/Downloads/All-fresh/Alpaca/libmsp" --include_path="C:/Users/Saad/Downloads/All-fresh/Alpaca/libwispbase" --include_path="C:/Users/Saad/Downloads/All-fresh/Alpaca" --include_path="C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS/include" --advice:power=all --advice:hw_config=all --define=__MSP430FR5994__ -g --printf_support=minimal --diag_warning=225 --diag_wrap=off --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU40 --preproc_with_compile --preproc_dependency="dsplib/source/matrix/$(basename $(<F)).d_raw" --obj_directory="dsplib/source/matrix" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


