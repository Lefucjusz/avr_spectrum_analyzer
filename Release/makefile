################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
spectrum_analyzer.lss \

FLASH_IMAGE += \
spectrum_analyzer.hex \

EEPROM_IMAGE += \
spectrum_analyzer.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: spectrum_analyzer.elf secondary-outputs

# Tool invocations
spectrum_analyzer.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,spectrum_analyzer.map -mmcu=atmega8 -o "spectrum_analyzer.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

spectrum_analyzer.lss: spectrum_analyzer.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S spectrum_analyzer.elf  >"spectrum_analyzer.lss"
	@echo 'Finished building: $@'
	@echo ' '

spectrum_analyzer.hex: spectrum_analyzer.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex spectrum_analyzer.elf  "spectrum_analyzer.hex"
	@echo 'Finished building: $@'
	@echo ' '

spectrum_analyzer.eep: spectrum_analyzer.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex spectrum_analyzer.elf  "spectrum_analyzer.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: spectrum_analyzer.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega8 spectrum_analyzer.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(EEPROM_IMAGE)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) spectrum_analyzer.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
