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

# Language Settings
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MCU specific settings
set(MCU_FAMILY "SAMV71")
set(MCU_MODEL "SAMV71Q21B")

set(MCU_DEFINES
    "__${MCU_MODEL}__"
    "ARM_MATH_CM7"
    "ARM_MATH_MATRIX_CHECK"
    "ARM_MATH_ROUNDING"
    "__FPU_PRESENT=1"
)

set(MCU_LINKER_FLAGS
    # Use newlib-nano (minimal C library)
    "-specs=nano.specs"
    # Minimal system calls implementation
    "-specs=nosys.specs"
)

# MCU-specific flags
# WARNING: to actually use the FPU, you must enable it first in software, before
# running any FPU instructions. FreeRTOS does enable it for you (ref vPortEnableVFP)
# but if you're not using FreeRTOS you have to write the relevant assembler
# stub yourself:
#  https://developer.arm.com/documentation/dui0646/c/Cortex-M7-Peripherals/Floating-Point-Unit/Enabling-the-FPU
set(MCU_ARCH_FLAGS
    "-mcpu=cortex-m7"
    "-mthumb"
    "-mfloat-abi=hard"
    "-mfpu=fpv5-d16"
)

# Combine all MCU-related flags
set(MCU_FLAGS
    ${MCU_ARCH_FLAGS}
    ${MCU_LINKER_FLAGS}
    ${MCU_DEFINES}
)
string(JOIN " " MCU_FLAGS_STR ${MCU_FLAGS})

# Set initial flags for test compilation
set(CMAKE_C_FLAGS ${MCU_FLAGS_STR})
set(CMAKE_CXX_FLAGS ${MCU_FLAGS_STR})
set(CMAKE_ASM_FLAGS ${MCU_FLAGS_STR})
set(CMAKE_EXE_LINKER_FLAGS ${MCU_FLAGS_STR})

# Since we're cross compiling we need to set it to library not an executable:
# https://stackoverflow.com/questions/53633705/cmake-the-c-compiler-is-not-able-to-compile-a-simple-test-program
# https://cmake.org/cmake/help/v3.26/variable/CMAKE_TRY_COMPILE_TARGET_TYPE.html
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Search behavior configuration for cross-compilation:
# ONLY  - Only search cross-compile paths (for target-specific files)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)   # Host tools
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)   # Target libs
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)   # Target headers
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)   # Target packages
