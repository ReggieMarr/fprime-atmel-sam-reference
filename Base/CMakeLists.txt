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

set(RELEASE_OPTIMIZATION_FLAGS
    # Aggressive optimization for speed and size
    # TODO tune this better
    "-O3"
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
include(${PROJECT_ROOT}/Base/cmake/startup.cmake)
include(${PROJECT_ROOT}/Base/cmake/harmony.cmake)

add_library(harmony STATIC ${HARMONY_SOURCES})
target_include_directories(harmony
    PRIVATE
        ${HARMONY_INCLUDES}
        ${ARM_PACK_INCLUDES}
        ${MICROCHIP_PACK_INCLUDES}
        ${FREERTOS_INCLUDES}
)

target_compile_definitions(harmony
    PRIVATE
        ${ARM_PACK_DEFS}
        ${MICROCHIP_PACK_DEFS}
)

target_compile_options(harmony
    PRIVATE
        ${COMMON_COMPILE_FLAGS}
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

# -----------------------------------------------------------------------------
# Target configuration
# -----------------------------------------------------------------------------
set(PROJECT_BIN ${PROJECT_NAME}.elf)

add_executable(${PROJECT_BIN} ${PROJECT_ROOT}/Base/startup/main.cpp)

target_sources(${PROJECT_BIN}
    PRIVATE
        ${MICROCHIP_PACK_SOURCES}
        ${PROJECT_ROOT}/Base/startup/startup_samv71q21b.c
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

# -Map: generate a linker report (mapfile) for the program. This contains
#   info such as memory layout, list of symbols, linked libraries etc. Useful for
#   solving linker errors or finding who's hogging memory.
# Refs:
# - https://interrupt.memfault.com/blog/get-the-most-out-of-the-linker-map-file
target_link_options(${PROJECT_BIN}
    PRIVATE
        ${LINKER_FLAGS}
        ${MICROCHIP_PACK_BOOT_LINKER_SCRIPT}
        -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map
)

# Link dependencies
target_link_libraries(${PROJECT_BIN}
    PRIVATE
        freertos
        harmony
)

# -----------------------------------------------------------------------------
# Post-build operations
# -----------------------------------------------------------------------------

# Add memory layout information to build
if (CMAKE_VERBOSE_MAKEFILE)
    add_custom_command(TARGET ${PROJECT_BIN}
        POST_BUILD
            COMMAND ${CMAKE_SIZE} --format=berkeley ${PROJECT_BIN}
            COMMAND ${CMAKE_OBJDUMP} -h ${PROJECT_BIN}
    )
endif()
