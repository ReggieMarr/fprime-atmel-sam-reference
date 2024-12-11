# Function to verify paths
function(verify_path PATH_VAR DESCRIPTION)
    if(NOT EXISTS "${${PATH_VAR}}")
        message(FATAL_ERROR "${DESCRIPTION} not found at:{${PATH_VAR}, ${${PATH_VAR}}}")
    endif()
endfunction()

get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../" ABSOLUTE)
set(DEPS_ROOT "${PROJECT_ROOT}/deps")
set(CONFIG_ROOT "${PROJECT_ROOT}/Base/config")

# Verify variables are defined
if(NOT DEFINED DEPS_ROOT)
    message(FATAL_ERROR "DEPS_ROOT not defined")
endif()

# Check if path exists
if(NOT EXISTS ${DEPS_ROOT})
    message(FATAL_ERROR "DEPS_ROOT path does not exist: ${DEPS_ROOT}")
endif()

verify_path(DEPS_ROOT "Dependency root")

# MCU specific settings
set(MCU_FAMILY "SAMV71")
set(MCU_MODEL "SAMV71Q21B")

# Common definitions
set(TARGET_DEFINITIONS
    __SAMV71Q21B__
    ARM_MATH_CM7
    ARM_MATH_MATRIX_CHECK
    ARM_MATH_ROUNDING
    __FPU_PRESENT=1
)
