################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../buzzer.c \
../control_ECU_main.c \
../dcmotor.c \
../external_eeprom.c \
../gpio.c \
../pir.c \
../pwm.c \
../timer.c \
../twi.c \
../uart.c 

OBJS += \
./buzzer.o \
./control_ECU_main.o \
./dcmotor.o \
./external_eeprom.o \
./gpio.o \
./pir.o \
./pwm.o \
./timer.o \
./twi.o \
./uart.o 

C_DEPS += \
./buzzer.d \
./control_ECU_main.d \
./dcmotor.d \
./external_eeprom.d \
./gpio.d \
./pir.d \
./pwm.d \
./timer.d \
./twi.d \
./uart.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


