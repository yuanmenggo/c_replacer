################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/memwatch.c \
../src/re_palloc.c \
../src/re_string.c \
../src/re_util.c \
../src/replacer.c 

OBJS += \
./src/memwatch.o \
./src/re_palloc.o \
./src/re_string.o \
./src/re_util.o \
./src/replacer.o 

C_DEPS += \
./src/memwatch.d \
./src/re_palloc.d \
./src/re_string.d \
./src/re_util.d \
./src/replacer.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


