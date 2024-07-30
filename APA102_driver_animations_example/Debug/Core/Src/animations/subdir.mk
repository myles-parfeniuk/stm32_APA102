################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Src/animations/ChaseAnimation.cpp \
../Core/Src/animations/CometAnimation.cpp \
../Core/Src/animations/FlickerAnimation.cpp \
../Core/Src/animations/PulseAnimation.cpp 

OBJS += \
./Core/Src/animations/ChaseAnimation.o \
./Core/Src/animations/CometAnimation.o \
./Core/Src/animations/FlickerAnimation.o \
./Core/Src/animations/PulseAnimation.o 

CPP_DEPS += \
./Core/Src/animations/ChaseAnimation.d \
./Core/Src/animations/CometAnimation.d \
./Core/Src/animations/FlickerAnimation.d \
./Core/Src/animations/PulseAnimation.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/animations/%.o Core/Src/animations/%.su Core/Src/animations/%.cyclo: ../Core/Src/animations/%.cpp Core/Src/animations/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++20 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L475xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-animations

clean-Core-2f-Src-2f-animations:
	-$(RM) ./Core/Src/animations/ChaseAnimation.cyclo ./Core/Src/animations/ChaseAnimation.d ./Core/Src/animations/ChaseAnimation.o ./Core/Src/animations/ChaseAnimation.su ./Core/Src/animations/CometAnimation.cyclo ./Core/Src/animations/CometAnimation.d ./Core/Src/animations/CometAnimation.o ./Core/Src/animations/CometAnimation.su ./Core/Src/animations/FlickerAnimation.cyclo ./Core/Src/animations/FlickerAnimation.d ./Core/Src/animations/FlickerAnimation.o ./Core/Src/animations/FlickerAnimation.su ./Core/Src/animations/PulseAnimation.cyclo ./Core/Src/animations/PulseAnimation.d ./Core/Src/animations/PulseAnimation.o ./Core/Src/animations/PulseAnimation.su

.PHONY: clean-Core-2f-Src-2f-animations

