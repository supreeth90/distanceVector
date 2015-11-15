################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Logger.cpp \
../MainClass.cpp \
../RouteEntry.cpp \
../RoutingTable.cpp 

OBJS += \
./Logger.o \
./MainClass.o \
./RouteEntry.o \
./RoutingTable.o 

CPP_DEPS += \
./Logger.d \
./MainClass.d \
./RouteEntry.d \
./RoutingTable.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


