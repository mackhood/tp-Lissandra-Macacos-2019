################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Consola.c \
../src/Memoria.c \
../src/Request.c \
../src/config_memoria.c \
../src/estructurasMemoria.c 

OBJS += \
./src/Consola.o \
./src/Memoria.o \
./src/Request.o \
./src/config_memoria.o \
./src/estructurasMemoria.o 

C_DEPS += \
./src/Consola.d \
./src/Memoria.d \
./src/Request.d \
./src/config_memoria.d \
./src/estructurasMemoria.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2019-1c-Macacos/SharedLibrary" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


