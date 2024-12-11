set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(TOOLCHAIN arm-none-eabi)
set(CMAKE_C_COMPILER ${TOOLCHAIN}-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN}-g++)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN}-gcc)
set(CMAKE_AR ${TOOLCHAIN}-ar)
set(CMAKE_OBJCOPY ${TOOLCHAIN}-objcopy)
set(CMAKE_OBJDUMP ${TOOLCHAIN}-objdump)
set(CMAKE_SIZE ${TOOLCHAIN}-size)

# Architecture-specific flags
set(ARCH_FLAGS
    "-mcpu=cortex-m7"
    "-mthumb"
    "-mfloat-abi=hard"
    "-mfpu=fpv5-d16"
)
string(JOIN " " ARCH_FLAGS ${ARCH_FLAGS})

# Set initial flags for test compilation
set(CMAKE_C_FLAGS ${ARCH_FLAGS})
set(CMAKE_CXX_FLAGS ${ARCH_FLAGS})
set(CMAKE_ASM_FLAGS ${ARCH_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS ${ARCH_FLAGS})

# Since we're cross compiling we need to set it to library not an executable:
# https://stackoverflow.com/questions/53633705/cmake-the-c-compiler-is-not-able-to-compile-a-simple-test-program
# https://cmake.org/cmake/help/v3.26/variable/CMAKE_TRY_COMPILE_TARGET_TYPE.html
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Search behavior configuration for cross-compilation:
# NEVER - Only search host system (for tools like gcc, make)
# ONLY  - Only search cross-compile paths (for target-specific files)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)  # Host tools
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)   # Target libs
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)   # Target headers
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)   # Target packages
