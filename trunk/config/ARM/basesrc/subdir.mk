################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/home/mxx/aloka/device-agent/src/Config.cpp 

OBJS += \
./basesrc/Config.o 

CPP_DEPS += \
./basesrc/Config.d 


# Each subdirectory must supply rules for building sources it contributes
basesrc/Config.o: /home/mxx/aloka/device-agent/src/Config.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	/usr/local/arm/4.3.2/bin/arm-none-linux-gnueabi-g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


