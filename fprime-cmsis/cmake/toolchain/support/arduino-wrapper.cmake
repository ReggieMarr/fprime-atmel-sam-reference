


####
# ends_with:
#
# Check if the string input ends with the given suffix. Sets OUTPUT_VAR to TRUE when it does and  sets OUTPUT_VAR to
# FALSE when it does not. OUTPUT_VAR is the name of the variable in PARENT_SCOPE that will be set.
#
# Note: regexs in CMake are known to be inefficient. Thus `starts_with` and `ends_with` are implemented without them
# in order to ensure speed.
#
# OUTPUT_VAR: variable to set
# STRING: string to check
# SUFFIX: expected ending
####
function(ends_with OUTPUT_VAR STRING SUFFIX)
    set("${OUTPUT_VAR}" FALSE PARENT_SCOPE)
    string(LENGTH "${STRING}" INPUT_LENGTH)
    string(LENGTH "${SUFFIX}" SUFFIX_LENGTH)
    if (INPUT_LENGTH GREATER_EQUAL SUFFIX_LENGTH)
        # Calculate the substring of suffix length at end of string
        math(EXPR START "${INPUT_LENGTH} - ${SUFFIX_LENGTH}")
        string(SUBSTRING "${STRING}" "${START}" "${SUFFIX_LENGTH}" FOUND_SUFFIX)
        # Check the substring
        if (FOUND_SUFFIX STREQUAL "${SUFFIX}")
            set("${OUTPUT_VAR}" TRUE PARENT_SCOPE)
        endif()
    endif()
endfunction(ends_with)

####
# Function `run_atmel_wrapper`:
#
# Runs `atmel-cli` and forwards all supplied arguments to it. Output text will be set into the variable
# `WRAPPER_OUTPUT` for use in the larger program.
#
# Args:
#   *: forwarded as CLI arguments to `atmel-cli`
#
# Returns: WRAPPER_OUTPUT containing standard-output of the `atmel-cli` run.
####
function(run_atmel_wrapper)
    find_program(ATMEL_WRAPPER NAMES atmel-cli-wrapper)
    if (NOT ATMEL_WRAPPER)
        message(FATAL_ERROR "atmel-cli-wrapper is required to be on PATH for atmel-support toolchain")
    elseif (NOT DEFINED ATMEL_FQBN)
        message(FATAL_ERROR "Variable ATMEL_FQDN must be set to use atmel-support")
    endif()
    set(EXECUTE_ARGS
            "${ATMEL_WRAPPER}"
            ${ARGN}
            --includes
            --detect-settings
            --post-link
            -o
            "${CMAKE_BINARY_DIR}/atmel-cli-sketch"
            )
    if (CMAKE_DEBUG_OUTPUT)
        list(APPEND EXECUTE_ARGS --debug)
        string(REPLACE ";" " " INVOCATION "${EXECUTE_ARGS}")
        message(STATUS "[atmel-cli] Invocation: ${INVOCATION}")
    endif()

    # Execute the python wrapper
    execute_process(COMMAND
            ${EXECUTE_ARGS}
            OUTPUT_VARIABLE WRAPPER_OUTPUT
            RESULT_VARIABLE RET_CODE
            )
    if (NOT RET_CODE EQUAL 0)
        message(FATAL_ERROR "Could not run 'atmel-cli' please install 'atmel-cli' and board '${ATMEL_FQBN}'.")
    endif()
    set(WRAPPER_OUTPUT "${WRAPPER_OUTPUT}" PARENT_SCOPE)
endfunction(run_atmel_wrapper)

###
# Function `read_json`:
#
# A wrapper to help read JSON data as native-cmake types.
#
# Args:
#  VARIABLE_NAME: variable to set
#  JSON_DATA: data to mine
#  *: field list (e.g. a.b.c[0] -> a b c 0)
###
function(read_json VARIABLE_NAME JSON_DATA)
    string(JSON FIELD_TYPE TYPE "${JSON_DATA}" ${ARGN})

    # Handle lists
    if (FIELD_TYPE STREQUAL ARRAY)
        set(READ_DATA)
        string(JSON FIELD_LENGTH LENGTH "${JSON_DATA}" ${ARGN})
        math(EXPR FIELD_LENGTH "${FIELD_LENGTH} - 1")
        if (FIELD_LENGTH LESS 0)
            set(READ_DATA "")
        else()
            foreach(INDEX RANGE ${FIELD_LENGTH})
                string(JSON TEMP_DATA GET "${JSON_DATA}" ${ARGN} ${INDEX})
                list(APPEND READ_DATA "${TEMP_DATA}")
            endforeach()
        endif()
    # Standard read
    else()
        string(JSON READ_DATA GET "${JSON_DATA}" ${ARGN} ${INDEX})
    endif()
    set(${VARIABLE_NAME} "${READ_DATA}" PARENT_SCOPE)
endfunction()
