################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Detector/confirmDevice.cpp \
../Detector/main.cpp 

OBJS += \
./Detector/confirmDevice.o \
./Detector/main.o 

CPP_DEPS += \
./Detector/confirmDevice.d \
./Detector/main.d 


# Each subdirectory must supply rules for building sources it contributes
Detector/%.o: ../Detector/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include/libusb-1.0 -I/home/cmk/workspace_c/JWalkingPrevetionProject/Detector -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


