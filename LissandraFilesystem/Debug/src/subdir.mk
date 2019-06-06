################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Compactador.c \
../src/Consola.c \
../src/FileSistem.c \
../src/LFLExternals.c \
../src/Lissandra.c \
../src/LissandraFileSistem.c 

OBJS += \
./src/Compactador.o \
./src/Consola.o \
./src/FileSistem.o \
./src/LFLExternals.o \
./src/Lissandra.o \
./src/LissandraFileSistem.o 

C_DEPS += \
./src/Compactador.d \
./src/Consola.d \
./src/FileSistem.d \
./src/LFLExternals.d \
./src/Lissandra.d \
./src/LissandraFileSistem.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/tp-2019-1c-Macacos/SharedLibrary" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


