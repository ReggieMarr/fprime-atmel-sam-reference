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

#  - --specs=nosys.specs: All implementations of C must supply a number of POSIX
#    syscall definitions (e.g. _exit, _open, _sbrk). These normally require an
#    operating system to be in place. Since we are on an embedded platform, we
#    don't really have a filesystem or the concept of processes or stuff
#    like this. Thus, nosys.specs is a 'spec file' included with GCC, which
#    reconfigures those implicit settings to link in 'libnosys', a set of stubs
#    for these syscalls that always return an error whenever called.
# Refs:
#  - https://web.archive.org/web/20230103002604/https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/LegacyCollaterals/Frequently-Asked-Questions-4.9.3.26.txt
#  - https://forum.microchip.com/s/topic/a5C3l000000Uib8EAC/t184545
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

# Search behavior configuration for cross-compilation:
# ONLY  - Only search cross-compile paths (for target-specific files)
# Combine flags for test compilation only
set(MCU_COMPILE_FLAGS ${MCU_ARCH_FLAGS} ${MCU_DEFINES})
string(JOIN " " TEST_COMPILE_FLAGS_STR ${MCU_COMPILE_FLAGS})

# Since we're cross compiling we need to set it to library not an executable:
# https://stackoverflow.com/questions/53633705/cmake-the-c-compiler-is-not-able-to-compile-a-simple-test-program
# https://cmake.org/cmake/help/v3.26/variable/CMAKE_TRY_COMPILE_TARGET_TYPE.html
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
# Set minimal flags for compiler test
set(CMAKE_C_FLAGS_INIT ${TEST_COMPILE_FLAGS_STR})
set(CMAKE_CXX_FLAGS_INIT ${TEST_COMPILE_FLAGS_STR})
set(CMAKE_ASM_FLAGS_INIT ${TEST_COMPILE_FLAGS_STR})

# Search behavior configuration
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Export variables for use in main CMakeLists.txt
set(MCU_COMPILE_FLAGS ${MCU_COMPILE_FLAGS} CACHE INTERNAL "")
set(MCU_LINKER_FLAGS ${MCU_LINKER_FLAGS} CACHE INTERNAL "")
