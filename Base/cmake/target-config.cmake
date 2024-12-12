# Function to verify paths
function(verify_path PATH_VAR DESCRIPTION)
    if(NOT EXISTS "${${PATH_VAR}}")
        message(FATAL_ERROR "${DESCRIPTION} not found at:{${PATH_VAR}, ${${PATH_VAR}}}")
    endif()
endfunction()

option(ENABLE_DEBUGGING "Build target with debug support" OFF)
option(ENABLE_OPTIMIZATION "Build target with optimizations turned on" OFF)
