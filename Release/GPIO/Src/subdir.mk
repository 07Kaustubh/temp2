################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../GPIO/Src/gpio_handler.c \
../GPIO/Src/ledblink.c 

OBJS += \
./GPIO/Src/gpio_handler.o \
./GPIO/Src/ledblink.o 

C_DEPS += \
./GPIO/Src/gpio_handler.d \
./GPIO/Src/ledblink.d 


# Each subdirectory must supply rules for building sources it contributes
GPIO/Src/%.o GPIO/Src/%.su GPIO/Src/%.cyclo: ../GPIO/Src/%.c GPIO/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m33 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32H573xx -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc -I../Drivers/STM32H5xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H5xx/Include -I../Drivers/CMSIS/Include -I"D:/sCAN PROJECTS/BKP10122024/TML_EMBEDDED-TML_CLONE_12112024/git/TML_EMBEDDED/SMS/Inc" -I"D:/sCAN PROJECTS/BKP10122024/TML_EMBEDDED-TML_CLONE_12112024/git/TML_EMBEDDED/GPIO/Inc" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-GPIO-2f-Src

clean-GPIO-2f-Src:
	-$(RM) ./GPIO/Src/gpio_handler.cyclo ./GPIO/Src/gpio_handler.d ./GPIO/Src/gpio_handler.o ./GPIO/Src/gpio_handler.su ./GPIO/Src/ledblink.cyclo ./GPIO/Src/ledblink.d ./GPIO/Src/ledblink.o ./GPIO/Src/ledblink.su

.PHONY: clean-GPIO-2f-Src

