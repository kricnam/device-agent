################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/CRC16.cpp \
../src/CommunicationCommand.cpp \
../src/Packet.cpp \
../src/SerialPort.cpp \
../src/device-agent.cpp 

OBJS += \
./src/CRC16.o \
./src/CommunicationCommand.o \
./src/Packet.o \
./src/SerialPort.o \
./src/device-agent.o 

CPP_DEPS += \
./src/CRC16.d \
./src/CommunicationCommand.d \
./src/Packet.d \
./src/SerialPort.d \
./src/device-agent.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


