# CMake file for FRDM-K22F_Simple
cmake_minimum_required(VERSION 3.15.3)

# Optional: print out extra messages to see what is going on. Comment it to have less verbose messages
set(CMAKE_VERBOSE_MAKEFILE ON)

# Path to toolchain file. This one has to be before 'project()' below
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/arm-none-eabi-gcc.cmake)

# Setup project, output and linker file
project(STM32F103C8T6_BluePill)
set(EXECUTABLE ${PROJECT_NAME}.elf)
set(LINKER_FILE ${CMAKE_SOURCE_DIR}/STM32F103C8Tx_FLASH.ld)

enable_language(C ASM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Optional: issue a message to be sure it uses the correct toolchain file.
message(STATUS "CMAKE_TOOLCHAIN_FILE is: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "CMAKE_SOURCE_DIR is: ${CMAKE_SOURCE_DIR}")

# List of source files
set(SRC_FILES
        Core/Src/main.c 
        Core/Src/stm32f1xx_it.c 
        Core/Src/stm32f1xx_hal_msp.c 
        Core/Src/RingBuffer/ringbuffer.c 
        Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c 
        Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c 
        Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c 
        Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c 
        Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c 
        Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c 
        Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c 
        Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c 
        Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c 
        Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c 
        Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c 
        Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c 
        Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c 
        Core/Src/system_stm32f1xx.c 
        Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c
        startup_stm32f103xb.s
        )

# Build the executable based on the source files
add_executable(${EXECUTABLE} ${SRC_FILES})

# List of compiler defines, prefix with -D compiler option
target_compile_definitions(${EXECUTABLE} PRIVATE
        -DUSE_HAL_DRIVER
        -DSTM32F103xB
        )

# List of includ directories
target_include_directories(${EXECUTABLE} PRIVATE
        Core/Inc 
        Core/Src/RingBuffer 
        Drivers/STM32F1xx_HAL_Driver/Inc 
        Drivers/STM32F1xx_HAL_Driver/Inc/Legacy 
        Drivers/CMSIS/Device/ST/STM32F1xx/Include 
        Drivers/CMSIS/Include
        )

# Compiler options
target_compile_options(${EXECUTABLE} PRIVATE
        -mcpu=cortex-m3
        -mthumb
        # -mfpu=fpv4-sp-d16
        # -mfloat-abi=hard

        -fdata-sections
        -ffunction-sections

        -Wall
        -O0
        -g3
        -g
        -gdwarf-2
        )

# Linker options
target_link_options(${EXECUTABLE} PRIVATE
        -T${LINKER_FILE}
        -mcpu=cortex-m3
        -mthumb
        # -mfpu=fpv4-sp-d16
        # -mfloat-abi=hard
        -specs=nano.specs
        -specs=nosys.specs
        -lc
        -lm
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
        -Xlinker -print-memory-usage -Xlinker
        )

# Optional: Print executable size as part of the post build process
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND ${CMAKE_SIZE_UTIL} ${EXECUTABLE})

# Optional: Create hex, bin and S-Record files after the build
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O srec --srec-len=64 ${EXECUTABLE} ${PROJECT_NAME}.s19
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
        COMMAND ${CMAKE_OBJCOPY} -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin)
