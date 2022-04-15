################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Test.cpp 

CPP_DEPS += \
./src/Test.d 

OBJS += \
./src/Test.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	aarch64-none-linux-gnu-g++ -I/home/ubu/backup/maaxboard/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/lib/glib-2.0/include -I/home/ubu/Downloads/include -I/home/ubu/backup/maaxboard/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/include/glib-2.0 -I/home/ubu/backup/maaxboard/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/lib/gcc/aarch64-none-linux-gnu/10.3.1/include -O0 -g3 -Wall -Wmissing-declarations -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/Test.d ./src/Test.o

.PHONY: clean-src

