################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/hw_api_tester.c \
../src/lm75_proxy.c \
../src/sample_temp_proxy.c \
../src/temp_sensor.c \
../src/virt_temp_proxy.c 

OBJS += \
./src/hw_api_tester.o \
./src/lm75_proxy.o \
./src/sample_temp_proxy.o \
./src/temp_sensor.o \
./src/virt_temp_proxy.o 

C_DEPS += \
./src/hw_api_tester.d \
./src/lm75_proxy.d \
./src/sample_temp_proxy.d \
./src/temp_sensor.d \
./src/virt_temp_proxy.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-angstrom-linux-gnueabi-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


