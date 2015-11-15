################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Logger.cpp \
../src/MainClass.cpp \
../src/RouteEntry.cpp \
../src/RoutingTable.cpp 

OBJS += \
./src/Logger.o \
./src/MainClass.o \
./src/RouteEntry.o \
./src/RoutingTable.o 

CPP_DEPS += \
./src/Logger.d \
./src/MainClass.d \
./src/RouteEntry.d \
./src/RoutingTable.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


