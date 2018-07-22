################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/main.cpp \
../src/miscellaneous.cpp \
../src/partition.cpp \
../src/problem.cpp \
../src/process.cpp 

OBJS += \
./src/main.o \
./src/miscellaneous.o \
./src/partition.o \
./src/problem.o \
./src/process.o 

CPP_DEPS += \
./src/main.d \
./src/miscellaneous.d \
./src/partition.d \
./src/problem.d \
./src/process.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11  -fopenmp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


