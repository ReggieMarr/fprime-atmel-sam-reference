####
# atmel-support.cmake:
#
# Support for Atmel SAM boards using the ARM GCC toolchain. This toolchain provides
# the following functions:
#
# - finalize_atmel_executable: called on a deployment target to attach the Atmel specific
#   steps and libraries to it.
#
####
cmake_minimum_required(VERSION 3.19)

# Include existing configuration files
include(${CMAKE_CURRENT_LIST_DIR}/target-config.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/samv71q21b-gcc-toolchain.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/samv71q21b-xplained.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/startup.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/harmony.cmake)
# set(EXTRA_LIBRARY_SOURCE "${CMAKE_CURRENT_LIST_DIR}/sources/extras.cpp")

set(FPRIME_ATMEL TRUE)

# Set the executable suffix if not defined
if (NOT DEFINED FPRIME_EXECUTABLE_SUFFIX)
    set(FPRIME_EXECUTABLE_SUFFIX ".elf")
endif()

####
# Function `setup_atmel_base_libraries`:
#
# Sets up the basic libraries needed for Atmel support
####
function(setup_atmel_base_libraries)
    if (NOT TARGET freertos)
        add_library(freertos STATIC ${FREERTOS_SOURCES})
        target_include_directories(freertos
            PUBLIC
            ${FREERTOS_INCLUDES}
        )
        target_compile_options(freertos
            PRIVATE
            ${COMMON_COMPILE_FLAGS}
        )
    endif()

    if (NOT TARGET atmel_base)
        add_library(atmel_base STATIC ${PROJECT_ROOT}/Base/main.cpp)
        target_include_directories(atmel_base
            PRIVATE
            ${MICROCHIP_PACK_INCLUDES}
            ${ARM_PACK_INCLUDES}
            ${HARMONY_INCLUDES}
            ${FREERTOS_INCLUDES}
            ${PROJECT_ROOT}/Base/startup
        )
        target_compile_options(atmel_base
            PRIVATE
            ${COMMON_COMPILE_FLAGS}
        )
    endif()
endfunction(setup_atmel_base_libraries)

####
# Function `setup_atmel_fprime_target`:
#
# Sets up fprime-specific configurations for Atmel targets
####
function(setup_atmel_fprime_target TARGET)
    target_sources(${TARGET}
        PRIVATE
        ${HARMONY_SOURCES}
        ${MICROCHIP_PACK_SOURCES}
        ${PROJECT_ROOT}/Base/startup/startup_samv71q21b.c
        ${PROJECT_ROOT}/Base/startup/freeRTOS_hooks.c
    )

    target_compile_options(${TARGET}
        PRIVATE
        $<$<COMPILE_LANGUAGE:ASM>:${ASM_FLAGS}>
        $<$<COMPILE_LANGUAGE:C>:${C_FLAGS}>
        $<$<COMPILE_LANGUAGE:CXX>:${CXX_FLAGS}>
    )

    target_compile_definitions(${TARGET}
        PRIVATE
        ${ARM_PACK_DEFS}
        ${MICROCHIP_PACK_DEFS}
        $<$<BOOL:${ENABLE_DEBUGGING}>:DEBUG>
        $<$<BOOL:${ENABLE_DEBUGGING}>:${MICROCHIP_PACK_DEBUG_DEFS}>
    )

    target_include_directories(${TARGET}
        PRIVATE
        ${MICROCHIP_PACK_INCLUDES}
        ${ARM_PACK_INCLUDES}
        ${HARMONY_INCLUDES}
        ${PROJECT_ROOT}/Base/startup
    )

    target_link_options(${TARGET}
        PRIVATE
        ${LINKER_FLAGS}
        ${MICROCHIP_PACK_BOOT_LINKER_SCRIPT}
        -Wl,-Map=${CMAKE_BINARY_DIR}/${TARGET}.map
    )

    # Link with base libraries
    target_link_libraries(${TARGET}
        PRIVATE
        freertos
        atmel_base
    )
endfunction(setup_atmel_fprime_target)

####
# Function `finalize_atmel_executable`:
#
# Sets up the final executable with necessary post-build steps
####
function(finalize_atmel_executable)
    if (DEFINED FPRIME_SUBBOUILD_TARGETS)
        return()
    endif()

    # Ensure base libraries are set up
    setup_atmel_base_libraries()

    # Setup the fprime target with our Atmel-specific configuration
    setup_atmel_fprime_target(${FPRIME_CURRENT_MODULE})

    # Add post build commands to generate binary and hex files
    add_custom_command(
        TARGET "${FPRIME_CURRENT_MODULE}"
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary
            $<TARGET_FILE:${FPRIME_CURRENT_MODULE}>
            ${CMAKE_BINARY_DIR}/${FPRIME_CURRENT_MODULE}.bin
        COMMAND ${CMAKE_OBJCOPY} -O ihex
            $<TARGET_FILE:${FPRIME_CURRENT_MODULE}>
            ${CMAKE_BINARY_DIR}/${FPRIME_CURRENT_MODULE}.hex
        COMMAND ${CMAKE_SIZE} --format=berkeley
            $<TARGET_FILE:${FPRIME_CURRENT_MODULE}>
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:${FPRIME_CURRENT_MODULE}>*"
            "${CMAKE_INSTALL_PREFIX}/${TOOLCHAIN_NAME}/${FPRIME_CURRENT_MODULE}/bin"
    )

    # If debugging is enabled, add additional information
    if (CMAKE_VERBOSE_MAKEFILE)
        add_custom_command(
            TARGET "${FPRIME_CURRENT_MODULE}"
            POST_BUILD
            COMMAND ${CMAKE_OBJDUMP} -h ${FPRIME_CURRENT_MODULE}${FPRIME_EXECUTABLE_SUFFIX}
        )
    endif()
endfunction(finalize_atmel_executable)

# Debug output
if (CMAKE_DEBUG_OUTPUT)
    message(STATUS "[atmel-support] Using toolchain: ${TOOLCHAIN}")
    message(STATUS "[atmel-support] MCU Model: ${MCU_MODEL}")
    message(STATUS "[atmel-support] MCU Compile Flags: ${MCU_COMPILE_FLAGS}")
    message(STATUS "[atmel-support] MCU Linker Flags: ${MCU_LINKER_FLAGS}")
endif()
