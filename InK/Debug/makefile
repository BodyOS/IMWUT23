################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /home/erenyildiz/ti/ccs1120/ccs/tools/compiler/msp430-gcc-9.3.0.31_linux64

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./appinit.o" \
"./main.o" \
"./covid/thread1.o" \
"./hardwareSim/clock.o" \
"./hardwareSim/energyHarvester.o" \
"./hardwareSim/sensorDataRead.o" \
"./isr/isrmanager.o" \
"./isr/persistentqueue.o" \
"./mcu/msp430/dma.o" \
"./mcu/msp430/msp430fr5969.o" \
"./scheduler/priority.o" \
"./scheduler/scheduler.o" \
"./scheduler/thread.o" \
-T"../msp430fr5994.ld" \
$(GEN_CMDS__FLAG) \
-Wl,--start-group \
-lgcc \
-lc \
-Wl,--end-group \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include covid/subdir_vars.mk
-include hardwareSim/subdir_vars.mk
-include isr/subdir_vars.mk
-include mcu/msp430/subdir_vars.mk
-include scheduler/subdir_vars.mk
-include subdir_rules.mk
-include covid/subdir_rules.mk
-include hardwareSim/subdir_rules.mk
-include isr/subdir_rules.mk
-include mcu/msp430/subdir_rules.mk
-include scheduler/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
InK.out \

EXE_OUTPUTS__QUOTED += \
"InK.out" \

BIN_OUTPUTS += \
InK.hex \

BIN_OUTPUTS__QUOTED += \
"InK.hex" \


# All Target
all: $(OBJS) $(LD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "InK.out"

# Tool invocations
InK.out: $(OBJS) $(LD_SRCS) $(GEN_CMDS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: GNU Linker'
	"/home/erenyildiz/ti/ccs1120/ccs/tools/compiler/msp430-gcc-9.3.0.31_linux64/bin/msp430-elf-gcc-9.3.0" -mhwmult=f5series -Og -g -gdwarf-3 -gstrict-dwarf -Wall -mcode-region=none -mlarge -Wl,-Map,"InK.map" -Wl,--gc-sections -L"/home/erenyildiz/ti/ccs1120/ccs/ccs_base/msp430/include_gcc" -mmcu=msp430fr5994 -o"InK.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

InK.hex: $(EXE_OUTPUTS)
	@echo 'Building secondary target: "$@"'
	@echo 'Invoking: GNU Objcopy Utility'
	"/home/erenyildiz/ti/ccs1120/ccs/tools/compiler/msp430-gcc-9.3.0.31_linux64/bin/msp430-elf-objcopy"  $(EXE_OUTPUTS__QUOTED) "InK.hex"
	@echo 'Finished building secondary target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "appinit.o" "main.o" "covid/thread1.o" "hardwareSim/clock.o" "hardwareSim/energyHarvester.o" "hardwareSim/sensorDataRead.o" "isr/isrmanager.o" "isr/persistentqueue.o" "mcu/msp430/dma.o" "mcu/msp430/msp430fr5969.o" "scheduler/priority.o" "scheduler/scheduler.o" "scheduler/thread.o" 
	-$(RM) "appinit.d" "main.d" "covid/thread1.d" "hardwareSim/clock.d" "hardwareSim/energyHarvester.d" "hardwareSim/sensorDataRead.d" "isr/isrmanager.d" "isr/persistentqueue.d" "mcu/msp430/dma.d" "mcu/msp430/msp430fr5969.d" "scheduler/priority.d" "scheduler/scheduler.d" "scheduler/thread.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

