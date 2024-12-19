# Function to verify paths
function(verify_path PATH_VAR DESCRIPTION)
    if(NOT EXISTS "${${PATH_VAR}}")
        message(FATAL_ERROR "${DESCRIPTION} not found at:{${PATH_VAR}, ${${PATH_VAR}}}")
    endif()
endfunction()

# NOTE this and PROJECT_ROOT must be defined
verify_path(PROJECT_ROOT "Project root")
verify_path(DEPS_ROOT "Dependency root")

set(THIRD_PARTY_ROOT "${PROJECT_ROOT}/Base/third_party/")

option(ENABLE_DEBUGGING "Build target with debug support" OFF)
option(ENABLE_OPTIMIZATION "Build target with optimizations turned on" OFF)
option(STARTUP_TYPE "Type of startup up used" MICROCHIP)
option(HOST_PATH "Host Path to map to " ${PROJECT_ROOT})

# Initialize the target sources and includes
set(TARGET_SOURCES "")
set(TARGET_INCLUDES ${THIRD_PARTY_ROOT})

# Language Settings
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MCU specific settings
set(MCU_FAMILY "SAMV71")
set(MCU_MODEL "SAMV71Q21B")
