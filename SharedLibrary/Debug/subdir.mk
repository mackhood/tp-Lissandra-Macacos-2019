################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../auxiliaryFunctions.c \
../conexiones.c \
../configs.c \
../loggers.c \
../protocolo.c 

OBJS += \
./auxiliaryFunctions.o \
./conexiones.o \
./configs.o \
./loggers.o \
./protocolo.o 

C_DEPS += \
./auxiliaryFunctions.d \
./conexiones.d \
./configs.d \
./loggers.d \
./protocolo.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


