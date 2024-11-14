cmake_minimum_required(VERSION 3.20)

# 프로젝트 이름 설정
set(PROJECT_NAME GUI_COM_test)
project(${PROJECT_NAME} C ASM)

# MCU 관련 설정
set(CPU_PARAMETERS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

# 컴파일러 플래그 설정
add_compile_options(
    ${CPU_PARAMETERS}
    -Wall
    -fdata-sections
    -ffunction-sections
    $<$<CONFIG:Debug>:-g3>
    $<$<CONFIG:Release>:-O2>
)

# 링커 플래그 설정
add_link_options(
    ${CPU_PARAMETERS}
    -Wl,--gc-sections
    -T${CMAKE_SOURCE_DIR}/STM32F303VCTX_FLASH.ld
)

# 소스 파일 추가
file(GLOB_RECURSE SOURCES 
    "Core/Src/*.c"
    "Core/Startup/*.s"
    "Drivers/STM32F3xx_HAL_Driver/Src/*.c"
    "Component/Uart_Class/*.c"
    "Middlewares/ST/STM32_USB_Device_Library/Core/Src/*.c"
    "Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/*.c"    
    "Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c"
    "Middlewares/Third_Party/FreeRTOS/Source/croutine.c"
    "Middlewares/Third_Party/FreeRTOS/Source/event_groups.c"
    "Middlewares/Third_Party/FreeRTOS/Source/list.c"
    "Middlewares/Third_Party/FreeRTOS/Source/queue.c"
    "Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c"
    "Middlewares/Third_Party/FreeRTOS/Source/tasks.c"
    "Middlewares/Third_Party/FreeRTOS/Source/timers.c"
    "Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/*.c"
    "Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/*.c"
    "Middlewares/Third_Party/FatFs/src/*.c"
    "Middlewares/Third_Party/FatFs/src/option/*.c"    
)

# 헤더 파일 경로 추가
include_directories(
    Core/Inc
    Drivers/CMSIS/Include
    Drivers/CMSIS/Device/ST/STM32F3xx/Include
    Drivers/STM32F3xx_HAL_Driver/Inc
    Component/Uart_Class
)

# 매크로 정의
add_definitions(
    -DUSE_HAL_DRIVER
    -DSTM32F303xC
)

# 실행 파일 생성
add_executable(${PROJECT_NAME}.elf ${SOURCES})

# 바이너리 파일 생성을 위한 post-build 명령어
add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> ${PROJECT_NAME}.bin
) 