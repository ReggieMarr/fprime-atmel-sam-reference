# -----------------------------------------------------------------------------
# Basic project setup
# -----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.26)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(ENABLE_DEBUGGING)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../" ABSOLUTE)
set(DEPS_ROOT "${PROJECT_ROOT}/deps")
set(CONFIG_ROOT "${PROJECT_ROOT}/Base/config")

set(CMAKE_TOOLCHAIN_FILE ${PROJECT_ROOT}/Base/cmake/samv71q21b-gcc-toolchain.cmake)

# Project declaration
project(base_app_${MCU_MODEL}
    VERSION 1.0.0
    LANGUAGES C CXX ASM
)

# Initialize the target sources and includes
set(TARGET_SOURCES "")
set(TARGET_INCLUDES "")

# -----------------------------------------------------------------------------
# Component configurations
# -----------------------------------------------------------------------------
verify_path(DEPS_ROOT "Dependency root")

include(cmake/harmony.cmake)
include(cmake/cmsis.cmake)
include(cmake/FreeRTOS.cmake)

# -----------------------------------------------------------------------------
# Compiler and language settings
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
    set(OPTIMIZATION_FLAGS ${ENABLE_OPTIMIZATION} ? ${RELEASE_OPTIMIZATION_FLAGS} : ${DEFAULT_OPTIMIZATION_FLAGS})
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

# C++ specific flags
set(CXX_FLAGS
    # Disable C++ exception handling
    "-fno-exceptions"
    # Disable runtime type information
    "-fno-rtti"
    # Warn about volatile usage
    "-Wvolatile"
)

# Base linker flags
set(LINKER_FLAGS
    # Use newlib-nano (minimal C library)
    "-specs=nano.specs"
    # Minimal system calls implementation
    "-specs=nosys.specs"
)

# Add garbage collection flag
if(ENABLE_DEBUGGING)
    list(APPEND LINKER_FLAGS "-Wl,--gc-sections")
endif()

# Debug-specific flags
if(ENABLE_DEBUGGING)
    # Show overall memory usage statistics in debug builds
    list(APPEND LINKER_FLAGS "-Wl,--print-memory-usage")

    # Print which sections were removed gc-sections only
    # if gc-sections is enabled and we're in debug
    if(";${LINKER_FLAGS};" MATCHES ";-Wl,--gc-sections;")
        list(APPEND LINKER_FLAGS "-Wl,--print-gc-sections")
    endif()
endif()

if(ENABLE_DEBUGGING)
    message(STATUS "Linker flags: ${LINKER_FLAGS}")
endif()

list(APPEND LINKER_FLAGS
    # Linker script to align memory properly
    ${MICROCHIP_PACK_FLASH_LINKER_FLAG}
)

# Convert to strings for global flags
string(JOIN " " OPTIMIZATION_FLAGS_STR ${OPTIMIZATION_FLAGS})
string(JOIN " " WARNING_FLAGS_STR ${WARNING_FLAGS})
string(JOIN " " CXX_FLAGS_STR ${CXX_FLAGS})
string(JOIN " " LINKER_FLAGS_STR ${LINKER_FLAGS})

# Global flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OPTIMIZATION_FLAGS_STR} ${WARNING_FLAGS_STR}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OPTIMIZATION_FLAGS_STR} ${WARNING_FLAGS_STR} ${CXX_FLAGS_STR}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINKER_FLAGS_STR}")

if(ENABLE_DEBUGGING)
    message(STATUS "CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
    message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
    message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
    message(STATUS "CMAKE_ASM_FLAGS: ${CMAKE_ASM_FLAGS}")
endif()

# -----------------------------------------------------------------------------
# Harmony library setup
# -----------------------------------------------------------------------------
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
# target_compile_options(harmony
#     PRIVATE
# )

# -----------------------------------------------------------------------------
# FreeRTOS library setup
# -----------------------------------------------------------------------------
add_library(freertos STATIC ${FREERTOS_SOURCES})
target_include_directories(freertos
    PUBLIC
        ${FREERTOS_INCLUDES}
)

target_compile_options(freertos
    PRIVATE
        # ${COMMON_FLAGS}
        ### From FreeRTOS/Source/cmake_example
        # Gnu/Clang C Options
        $<$<COMPILE_LANG_AND_ID:C,GNU>:-fdiagnostics-color=always>
        $<$<COMPILE_LANG_AND_ID:C,Clang>:-fcolor-diagnostics>

        $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wall>
        $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wextra>
        $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wpedantic>
        $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Werror>
        $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:-Wconversion>
        $<$<COMPILE_LANG_AND_ID:C,Clang>:-Weverything>

        # Suppressions required to build clean with clang.
        $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-unused-macros>
        $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-padded>
        $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-missing-variable-declarations>
        $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-covered-switch-default>
        $<$<COMPILE_LANG_AND_ID:C,Clang>:-Wno-cast-align>
)

# -----------------------------------------------------------------------------
# Main target configuration
# -----------------------------------------------------------------------------
list(APPEND TARGET_SOURCES
    ${MICROCHIP_PACK_SOURCES}
    # ${ARM_PACK_SOURCES}
)

list(APPEND TARGET_INCLUDES
    ${MICROCHIP_PACK_INCLUDES}
    ${ARM_PACK_INCLUDES}
)

# Add Custom startup sources
list(APPEND TARGET_SOURCES
    ${PROJECT_ROOT}/Base/config/startup_samv71q21b.c
    ${PROJECT_ROOT}/Base/config/freeRTOS_hooks.c
)

list(APPEND TARGET_INCLUDES
    ${PROJECT_ROOT}/Base/config
)

list(APPEND TARGET_DEFS
    ${ARM_PACK_DEFS}
    ${MICROCHIP_PACK_DEFS}
)
#   NOTE __DEBUG is a Microchip-specific define that adds some breakpoints,
#   and triggers sys_debug and more convience factors.
#   NOTE we may want to remove later but it's added by MPLAB X when in debug mode.
#   so kept for now for consistency.
if(ENABLE_DEBUGGING)
    list(APPEND TARGET_DEFS
        DEBUG
        __DEBUG
    )
endif()

# Create executable
add_executable(${PROJECT_NAME} main.cpp)

# Add sources and includes
target_sources(${PROJECT_NAME}
    PRIVATE
        ${TARGET_SOURCES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${TARGET_INCLUDES}
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        ${TARGET_DEFS}
)

# target_link_options(${PROJECT_NAME} PRIVATE
# )
# Link dependencies
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        freertos
        harmony
)

# -----------------------------------------------------------------------------
# Post-build operations
# -----------------------------------------------------------------------------

# Add memory layout information to build
if (CMAKE_VERBOSE_MAKEFILE)
    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
            COMMAND ${CMAKE_SIZE} --format=berkeley ${PROJECT_NAME}
            COMMAND ${CMAKE_OBJDUMP} -h ${PROJECT_NAME}
    )
endif()
