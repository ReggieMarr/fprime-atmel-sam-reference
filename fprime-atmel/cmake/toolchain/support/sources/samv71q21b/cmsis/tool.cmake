#Tool chain file
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# GNU Toolchain settings
set(TOOLCHAIN arm-none-eabi)
set(CMAKE_C_COMPILER ${TOOLCHAIN}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN}-g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN}-gcc)
set(CMAKE_AR ${TOOLCHAIN}-ar)
set(CMAKE_OBJCOPY ${TOOLCHAIN}-objcopy)
set(CMAKE_OBJDUMP ${TOOLCHAIN}-objdump)
set(CMAKE_SIZE ${TOOLCHAIN}-size)

set(MCU_LINKER_FLAGS
    # newlib-nano is a minimal C library.
    # "-specs=nano.specs"
    # Minimal system calls implementation
    "-specs=nosys.specs"
)

set(MCU_DEFINES
    "-D__${MCU_MODEL}__"
    "-DARM_MATH_CM7"
    "-DARM_MATH_MATRIX_CHECK"
    "-DARM_MATH_ROUNDING"
    "-D__FPU_PRESENT=1"
)

set(MCU_ARCH_FLAGS
    "-mcpu=cortex-m7"
    "-mthumb"
    "-mfloat-abi=hard"
    "-mfpu=fpv5-d16"
)

# -----------------------------------------------------------------------------
# Base project setup
# -----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.26)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(ENABLE_DEBUGGING)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/target-config.cmake)

set(CMAKE_TOOLCHAIN_FILE ${PROJECT_ROOT}/Base/cmake/samv71q21b-gcc-toolchain.cmake)

# Project declaration
project(base_app_${MCU_MODEL}
    VERSION 1.0.0
    LANGUAGES C CXX ASM
)

# -----------------------------------------------------------------------------
# Compiler settings
# -----------------------------------------------------------------------------
# For details see: https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
set(DEFAULT_OPTIMIZATION_FLAGS "-O0")

set(DEBUG_OPTIMIZATION_FLAGS
    # Standard Debugging flags
    # TODO review this and see if we can optimize a bit here
    # - https://gcc.gnu.org/onlinedocs/gcc/Debugging-Options.html
    # - https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
    "-g3"
    "-Og"
)

set(RELEASE_OPTIMIZATION_FLAGS
    # Aggressive optimization for speed and size
    # TODO tune this better
    "-O3"
    # Place functions and data in their own sections
    # Is leveraged for link time optimization with --gc-sections
    # NOTE: Apparently enabling this optimization can hide undefined symbols, causing the
    # build to seem to work until some symbol gets misdefined and the build
    # suddenly breaks with a linker error! Although most likely these symbols
    # are indeed unused most of the time.
    "-ffunction-sections"
    "-fdata-sections"
    "-Wl,--gc-sections"
)

# Select optimization level based on configuration
if(ENABLE_DEBUGGING)
    if(ENABLE_OPTIMIZATION)
        message(WARNING "Both optimization and debugging enabled. Using debug-friendly optimization level.")
    endif()
    set(OPTIMIZATION_FLAGS ${DEBUG_OPTIMIZATION_FLAGS})
else()
    # Debugging isn't enabled so apply flags based on optimization desired.
    if(ENABLE_OPTIMIZATION)
        set(OPTIMIZATION_FLAGS ${RELEASE_OPTIMIZATION_FLAGS})
    else()
        set(OPTIMIZATION_FLAGS ${DEFAULT_OPTIMIZATION_FLAGS})
    endif()
endif()

message(STATUS "Optimization flags: ${OPTIMIZATION_FLAGS}")

# Warning control flags
set(WARNING_FLAGS
    # Default turn all the errors on
    "-Wall"
    # Enable extra warnings
    "-Wextra"
    # Treat warnings as errors
    # NOTE there are a bunch of conversion and unused parameters
    # in harmony code, once we remove that we should enable this.
    # "-Werror"
    # Warn about implicit conversions
    "-Wconversion"
    # Warn about unused parameters
    "-Wunused-parameter"
    # Warn about unused functions
    "-Wunused-function"
)

set(COMMON_COMPILE_FLAGS
    -fdiagnostics-color=always
    # Make gdb and lsp happy
    -fdebug-prefix-map=/fprime-atmel-sam-reference=${HOST_PATH}
    ${OPTIMIZATION_FLAGS}
    ${WARNING_FLAGS})

set(ASM_FLAGS ${MCU_COMPILE_FLAGS} ${COMMON_COMPILE_FLAGS})

set(C_FLAGS ${MCU_COMPILE_FLAGS} ${COMMON_COMPILE_FLAGS})

set(CXX_FLAGS
    # Disable C++ exception handling
    "-fno-exceptions"
    # Disable runtime type information
    "-fno-rtti"
    # Warn about volatile usage
    "-Wvolatile"
    ${MCU_COMPILE_FLAGS}
    ${COMMON_COMPILE_FLAGS}
)

# Set the linker flags based on the MCU flags set in the toolchain file
set(LINKER_FLAGS
    ${MCU_LINKER_FLAGS})

# Debug-specific flags
if(ENABLE_DEBUGGING)
    # Show overall memory usage statistics in debug builds
    list(APPEND LINKER_FLAGS "-Wl,--print-memory-usage")

    # Print which sections were removed gc-sections only
    # if gc-sections is enabled and we're in debug
    if(";${OPTIMIZATION_FLAGS};" MATCHES ";-Wl,--gc-sections;")
        list(APPEND LINKER_FLAGS "-Wl,--print-gc-sections")
    endif()
endif()

# -----------------------------------------------------------------------------
# Startup configuration
# -----------------------------------------------------------------------------

#NOTE these could be subdirectories instead
# arm pack files
set(PACK_ROOT ${DEPS_ROOT}/mplab_dev_packs)
verify_path(PACK_ROOT "MPLAB device packs")

set(CMSIS_VERSION "5.8.0")

set(ARM_PACK_DIR ${PACK_ROOT}/arm/CMSIS/5.8.0)
verify_path(ARM_PACK_DIR "ARM CMSIS pack")

# since we're using an arm cortex m7 with double precision
set(ARM_PACK_DEFS
    -DARMCM7_DP
)

set(ARM_PACK_DEV_DIR ${ARM_PACK_DIR}/Device/ARM/ARMCM7)
set(ARM_PACK_SOURCES
    # Device implementations
    ${ARM_PACK_DEV_DIR}/Source/GCC/startup_ARMCM7.S
    ${ARM_PACK_DEV_DIR}/Source/startup_ARMCM7.c
    ${ARM_PACK_DEV_DIR}/Source/system_ARMCM7.c
)

set(ARM_PACK_INCLUDES
    ${ARM_PACK_DEV_DIR}/Include
    ${ARM_PACK_DIR}/CMSIS/Core/Include
)

set(ARM_PACK_BOOT_LINKER_SCRIPT
    -T${ARM_PACK_DEV_DIR}/Source/GCC/startup_ARMC7.ld)

# Microchip pack files
set(MICROCHIP_PACK_DEFS
    -DUSE_CMSIS_INIT
)

set(DFP_VERSION "4.10.230")
set(MICROCHIP_PACK_DIR ${PACK_ROOT}/Microchip/SAMV71_DFP/${DFP_VERSION}/samv71b)
verify_path(MICROCHIP_PACK_DIR "Microchip SAM pack")

set(MICROCHIP_PACK_SOURCES
    ${MICROCHIP_PACK_DIR}/gcc/system_samv71q21b.c
    # ${MICROCHIP_PACK_DIR}/gcc/gcc/startup_samv71q21b.c
    # ${MICROCHIP_PACK_DIR}/x32/gcc/startup_samv71q21b.c
)

set(MICROCHIP_PACK_INCLUDES
    ${MICROCHIP_PACK_DIR}/include
)

# Use a custom linker
set(MICROCHIP_PACK_BOOT_LINKER_SCRIPT
    -Wl,-T${PROJECT_ROOT}/Base/startup/startup_samv71q21b.ld)

set(MICROCHIP_PACK_DEBUG_DEFS __DEBUG)

set(FREERTOS_SOURCE_DIR ${DEPS_ROOT}/FreeRTOS/FreeRTOS/Source)
set(FREERTOS_PORT_DIR ${FREERTOS_SOURCE_DIR}/portable/GCC/ARM_CM7/r0p1)

set(FREERTOS_SOURCES
    ${FREERTOS_SOURCE_DIR}/tasks.c
    ${FREERTOS_SOURCE_DIR}/queue.c
    ${FREERTOS_SOURCE_DIR}/list.c
    ${FREERTOS_SOURCE_DIR}/timers.c
    # From https://www.FreeRTOS.org/a00111.html
    # + heap_4:
    #    + Can be used even when the application repeatedly deletes tasks,
    #      queues, semaphores, mutexes, etc..
    #    + Is much less likely than the heap_2 implementation to result in a heap
    #      space that is badly fragmented into multiple small blocks - even when
    #      the memory being allocated and freed is of random size.
    #    + Is not deterministic - but is much more efficient that most standard
    #      C library malloc implementations.
    # NOTE should only be used when configSUPPORT_DYNAMIC_ALLOCATION=1
    # TODO compare heap_5, use FreeRTOS-kernel and set port/heap using
    # recommended mechanisms like (e.g. set(FREERTOS_PORT "TEMPLATE" CACHE STRING "" FORCE))

    ${FREERTOS_SOURCE_DIR}/portable/MemMang/heap_4.c
    ${FREERTOS_PORT_DIR}/port.c
)

set(FREERTOS_INCLUDES
    ${FREERTOS_SOURCE_DIR}/include
    ${FREERTOS_PORT_DIR}
    # Sets our freeRTOS config
    ${THIRD_PARTY_ROOT}/peak/
)

add_library(freertos STATIC ${FREERTOS_SOURCES})
target_include_directories(freertos
    PUBLIC
        ${FREERTOS_INCLUDES}
)

target_compile_options(freertos
    PRIVATE
        ${COMMON_COMPILE_FLAGS}
)

add_library(Base STATIC ${PROJECT_ROOT}/Base/main.cpp)
target_include_directories(Base
    PRIVATE
        ${MICROCHIP_PACK_INCLUDES}
        ${ARM_PACK_INCLUDES}
        ${HARMONY_INCLUDES}
        ${FREERTOS_INCLUDES}
        ${PROJECT_ROOT}/Base/startup
)
target_compile_options(Base
    PRIVATE
        ${COMMON_COMPILE_FLAGS}
)

set(HARMONY_DIR ${PROJECT_ROOT}/Base/third_party/harmony/sam_v71_xult)
verify_path(HARMONY_DIR "Harmony generated files")
set(HARMONY_SOURCES
    # Core files
    ${HARMONY_DIR}/initialization.c
    ${HARMONY_DIR}/interrupts.c
    ${HARMONY_DIR}/exceptions.c
    ${HARMONY_DIR}/libc_syscalls.c
    ${HARMONY_DIR}/freertos_hooks.c

    # OSAL
    ${HARMONY_DIR}/osal/osal_freertos.c

    # Peripheral implementations
    ${HARMONY_DIR}/peripheral/clk/plib_clk.c
    ${HARMONY_DIR}/peripheral/nvic/plib_nvic.c
    ${HARMONY_DIR}/peripheral/pio/plib_pio.c
    ${HARMONY_DIR}/peripheral/efc/plib_efc.c
    ${HARMONY_DIR}/peripheral/usart/plib_usart1.c
    ${HARMONY_DIR}/peripheral/twihs/master/plib_twihs0_master.c

    # Harmony Drivers
    ${HARMONY_DIR}/driver/i2c/src/drv_i2c.c
    ${HARMONY_DIR}/driver/usart/src/drv_usart.c
)

# Add include directories
set(HARMONY_INCLUDES
    ${HARMONY_DIR}
)

# -----------------------------------------------------------------------------
# Target configuration
# -----------------------------------------------------------------------------
set(PROJECT_BIN ${PROJECT_NAME}.elf)

add_executable(${PROJECT_BIN} ${PROJECT_ROOT}/Base/startup/startup_samv71q21b.c)

target_sources(${PROJECT_BIN}
    PRIVATE
        ${HARMONY_SOURCES}
        ${MICROCHIP_PACK_SOURCES}
        ${PROJECT_ROOT}/Base/startup/freeRTOS_hooks.c
)

target_include_directories(${PROJECT_BIN}
    PRIVATE
        ${MICROCHIP_PACK_INCLUDES}
        ${ARM_PACK_INCLUDES}
        ${HARMONY_INCLUDES}
        ${PROJECT_ROOT}/Base/startup
)

target_compile_options(${PROJECT_BIN}
    PRIVATE
        $<$<COMPILE_LANGUAGE:ASM>:${ASM_FLAGS}>
        $<$<COMPILE_LANGUAGE:C>:${C_FLAGS}>
        $<$<COMPILE_LANGUAGE:CXX>:${CXX_FLAGS}>
)

target_compile_definitions(${PROJECT_BIN}
    PRIVATE
        ${ARM_PACK_DEFS}
        ${MICROCHIP_PACK_DEFS}
        $<$<BOOL:${ENABLE_DEBUGGING}>:DEBUG>
        $<$<BOOL:${ENABLE_DEBUGGING}>:${MICROCHIP_PACK_DEBUG_DEFS}>
)

target_link_options(${PROJECT_BIN}
    PRIVATE
        ${LINKER_FLAGS}
        ${MICROCHIP_PACK_BOOT_LINKER_SCRIPT}
)

# Link dependencies
target_link_libraries(${PROJECT_BIN}
    PRIVATE
        freertos
        Base
)
