################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Graphics/sed1335-driver.c \
../Core/Src/Graphics/sed1335-graphic.c \
../Core/Src/Graphics/sed1335-stm32.c 

OBJS += \
./Core/Src/Graphics/sed1335-driver.o \
./Core/Src/Graphics/sed1335-graphic.o \
./Core/Src/Graphics/sed1335-stm32.o 

C_DEPS += \
./Core/Src/Graphics/sed1335-driver.d \
./Core/Src/Graphics/sed1335-graphic.d \
./Core/Src/Graphics/sed1335-stm32.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Graphics/%.o Core/Src/Graphics/%.su: ../Core/Src/Graphics/%.c Core/Src/Graphics/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/Code/Dashboard-Firmware/Graphics_Dashboard/Core" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Graphics

clean-Core-2f-Src-2f-Graphics:
	-$(RM) ./Core/Src/Graphics/sed1335-driver.d ./Core/Src/Graphics/sed1335-driver.o ./Core/Src/Graphics/sed1335-driver.su ./Core/Src/Graphics/sed1335-graphic.d ./Core/Src/Graphics/sed1335-graphic.o ./Core/Src/Graphics/sed1335-graphic.su ./Core/Src/Graphics/sed1335-stm32.d ./Core/Src/Graphics/sed1335-stm32.o ./Core/Src/Graphics/sed1335-stm32.su

.PHONY: clean-Core-2f-Src-2f-Graphics

