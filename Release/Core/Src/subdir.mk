################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/E&D.c \
../Core/Src/Flash.c \
../Core/Src/TML.c \
../Core/Src/adc.c \
../Core/Src/aes.c \
../Core/Src/bootloader.c \
../Core/Src/cJSON.c \
../Core/Src/crc.c \
../Core/Src/fdcan.c \
../Core/Src/gpdma.c \
../Core/Src/gpio.c \
../Core/Src/icache.c \
../Core/Src/iwdg.c \
../Core/Src/main.c \
../Core/Src/memorymap.c \
../Core/Src/rtc.c \
../Core/Src/spi.c \
../Core/Src/stm32h5xx_hal_msp.c \
../Core/Src/stm32h5xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32h5xx.c \
../Core/Src/tim.c \
../Core/Src/usart.c 

OBJS += \
./Core/Src/E&D.o \
./Core/Src/Flash.o \
./Core/Src/TML.o \
./Core/Src/adc.o \
./Core/Src/aes.o \
./Core/Src/bootloader.o \
./Core/Src/cJSON.o \
./Core/Src/crc.o \
./Core/Src/fdcan.o \
./Core/Src/gpdma.o \
./Core/Src/gpio.o \
./Core/Src/icache.o \
./Core/Src/iwdg.o \
./Core/Src/main.o \
./Core/Src/memorymap.o \
./Core/Src/rtc.o \
./Core/Src/spi.o \
./Core/Src/stm32h5xx_hal_msp.o \
./Core/Src/stm32h5xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32h5xx.o \
./Core/Src/tim.o \
./Core/Src/usart.o 

C_DEPS += \
./Core/Src/E&D.d \
./Core/Src/Flash.d \
./Core/Src/TML.d \
./Core/Src/adc.d \
./Core/Src/aes.d \
./Core/Src/bootloader.d \
./Core/Src/cJSON.d \
./Core/Src/crc.d \
./Core/Src/fdcan.d \
./Core/Src/gpdma.d \
./Core/Src/gpio.d \
./Core/Src/icache.d \
./Core/Src/iwdg.d \
./Core/Src/main.d \
./Core/Src/memorymap.d \
./Core/Src/rtc.d \
./Core/Src/spi.d \
./Core/Src/stm32h5xx_hal_msp.d \
./Core/Src/stm32h5xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32h5xx.d \
./Core/Src/tim.d \
./Core/Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32H573xx -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H5xx/Include -I../Drivers/CMSIS/Include -I"D:/sCAN PROJECTS/BKP10122024/TML_EMBEDDED-TML_CLONE_12112024/git/TML_EMBEDDED/SMS/Inc" -I"D:/sCAN PROJECTS/BKP10122024/TML_EMBEDDED-TML_CLONE_12112024/git/TML_EMBEDDED/GPIO/Inc" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/E&D.cyclo ./Core/Src/E&D.d ./Core/Src/E&D.o ./Core/Src/E&D.su ./Core/Src/Flash.cyclo ./Core/Src/Flash.d ./Core/Src/Flash.o ./Core/Src/Flash.su ./Core/Src/TML.cyclo ./Core/Src/TML.d ./Core/Src/TML.o ./Core/Src/TML.su ./Core/Src/adc.cyclo ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/adc.su ./Core/Src/aes.cyclo ./Core/Src/aes.d ./Core/Src/aes.o ./Core/Src/aes.su ./Core/Src/bootloader.cyclo ./Core/Src/bootloader.d ./Core/Src/bootloader.o ./Core/Src/bootloader.su ./Core/Src/cJSON.cyclo ./Core/Src/cJSON.d ./Core/Src/cJSON.o ./Core/Src/cJSON.su ./Core/Src/crc.cyclo ./Core/Src/crc.d ./Core/Src/crc.o ./Core/Src/crc.su ./Core/Src/fdcan.cyclo ./Core/Src/fdcan.d ./Core/Src/fdcan.o ./Core/Src/fdcan.su ./Core/Src/gpdma.cyclo ./Core/Src/gpdma.d ./Core/Src/gpdma.o ./Core/Src/gpdma.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/icache.cyclo ./Core/Src/icache.d ./Core/Src/icache.o ./Core/Src/icache.su ./Core/Src/iwdg.cyclo ./Core/Src/iwdg.d ./Core/Src/iwdg.o ./Core/Src/iwdg.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/memorymap.cyclo ./Core/Src/memorymap.d ./Core/Src/memorymap.o ./Core/Src/memorymap.su ./Core/Src/rtc.cyclo ./Core/Src/rtc.d ./Core/Src/rtc.o ./Core/Src/rtc.su ./Core/Src/spi.cyclo ./Core/Src/spi.d ./Core/Src/spi.o ./Core/Src/spi.su ./Core/Src/stm32h5xx_hal_msp.cyclo ./Core/Src/stm32h5xx_hal_msp.d ./Core/Src/stm32h5xx_hal_msp.o ./Core/Src/stm32h5xx_hal_msp.su ./Core/Src/stm32h5xx_it.cyclo ./Core/Src/stm32h5xx_it.d ./Core/Src/stm32h5xx_it.o ./Core/Src/stm32h5xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32h5xx.cyclo ./Core/Src/system_stm32h5xx.d ./Core/Src/system_stm32h5xx.o ./Core/Src/system_stm32h5xx.su ./Core/Src/tim.cyclo ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su ./Core/Src/usart.cyclo ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su

.PHONY: clean-Core-2f-Src

