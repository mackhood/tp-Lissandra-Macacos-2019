################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Consola.c \
../src/InicializacionPcb.c \
../src/Kernel.c \
../src/adminColasPlanificacion.c \
../src/conexionMemoria.c \
../src/kernelConfig.c \
../src/main.c \
../src/planificador.c 

OBJS += \
./src/Consola.o \
./src/InicializacionPcb.o \
./src/Kernel.o \
./src/adminColasPlanificacion.o \
./src/conexionMemoria.o \
./src/kernelConfig.o \
./src/main.o \
./src/planificador.o 

C_DEPS += \
./src/Consola.d \
./src/InicializacionPcb.d \
./src/Kernel.d \
./src/adminColasPlanificacion.d \
./src/conexionMemoria.d \
./src/kernelConfig.d \
./src/main.d \
./src/planificador.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2019-1c-Macacos/SharedLibrary" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


