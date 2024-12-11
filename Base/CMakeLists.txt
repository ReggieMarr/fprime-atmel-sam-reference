# -----------------------------------------------------------------------------
# Basic project setup
# -----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.26)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake/arm-none-eabi-toolchain.cmake)

# Project declaration
project(samv71_base
    VERSION 1.0.0
    LANGUAGES C CXX ASM
)

# Initialize the target sources and includes
set(TARGET_SOURCES "")
set(TARGET_INCLUDES "")

# -----------------------------------------------------------------------------
# Compiler and language settings
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Common compilation flags
set(COMMON_FLAGS
    "-ffunction-sections"
    "-fdata-sections"
    "-Wno-unused-parameter"
    "-Wno-unused-function"
)
string(JOIN " " COMMON_FLAGS ${COMMON_FLAGS})

# Language-specific flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS} -fno-exceptions -fno-rtti -Wno-volatile")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")

# -----------------------------------------------------------------------------
# Component configurations
# -----------------------------------------------------------------------------
include(cmake/samv71-config.cmake)
include(cmake/harmony.cmake)
include(cmake/cmsis.cmake)
include(cmake/FreeRTOS.cmake)

# Add Custom startup sources
list(APPEND TARGET_SOURCES
    ${PROJECT_ROOT}/Base/config/startup_samv71q21b.c
    ${PROJECT_ROOT}/Base/config/freeRTOS_hooks.c
)

list(APPEND TARGET_INCLUDES
    ${PROJECT_ROOT}/Base/config
)

# -----------------------------------------------------------------------------
# Harmony library setup
# -----------------------------------------------------------------------------
add_library(harmony STATIC ${HARMONY_SOURCES})
target_include_directories(harmony
    PRIVATE
        ${HARMONY_INCLUDES}
)

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
        -Wno-unused-parameter
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

# Add target specific compile options and definitions
target_compile_options(${PROJECT_NAME}
    PRIVATE
        # $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
        # $<$<COMPILE_LANGUAGE:CXX>:-Wno-expansion-to-defined>
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        ${TARGET_DEFINITIONS}
)

# Link dependencies
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        freertos
        harmony
)

# -----------------------------------------------------------------------------
# Post-build operations
# -----------------------------------------------------------------------------
add_custom_command(TARGET ${PROJECT_NAME}
    POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary ${PROJECT_NAME} ${PROJECT_NAME}.bin
        COMMAND ${CMAKE_OBJCOPY} -O ihex ${PROJECT_NAME} ${PROJECT_NAME}.hex
        COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}
)

#Post build print the flags we applied
message(STATUS "CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
message(STATUS "CMAKE_ASM_FLAGS: ${CMAKE_ASM_FLAGS}")

# -----------------------------------------------------------------------------
# Optional: Build configuration specific settings
# -----------------------------------------------------------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE
            DEBUG
    )
endif()
