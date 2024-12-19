if(NOT DEFINED CMAKE_SYSTEM_PROCESSOR)
    message(FATAL_ERROR "Must define CMAKE_SYSTEM_PROCESSOR")
elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "ARM")
    message("TODO check if custom type support is neccessary")
    set(TYPES_DIR "${CMAKE_CURRENT_LIST_DIR}/types/arm")
else()
    message(FATAL_ERROR "Unsupported processor type: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(CMAKE_EXECUTABLE_SUFFIX "${FPRIME_EXECUTABLE_SUFFIX}" CACHE INTERNAL "" FORCE)

# Add FPrime OSAL Implementations
choose_fprime_implementation(Os/File Os_File_Stub)
choose_fprime_implementation(Os/Queue Os_Generic_PriorityQueue)

# Add Baremetal OSAL Implementations
choose_fprime_implementation(Os/Cpu Os_Cpu_Baremetal)
choose_fprime_implementation(Os/Memory Os_Memory_Baremetal)
choose_fprime_implementation(Os/Mutex Os_Mutex_Baremetal)
choose_fprime_implementation(Os/Task Os_Task_Baremetal)
choose_fprime_implementation(Os/Task Os_Task_freeRTOS)

# Add Arduino OSAL Implementations
choose_fprime_implementation(Os/Console Os_Console_Arduino)
choose_fprime_implementation(Os/RawTime Os_RawTime_Arduino)

message(STATUS "[fprime-arduino] Including Types Directory: ${TYPES_DIR}")
include_directories("${TYPES_DIR}" "${CMAKE_CURRENT_LIST_DIR}")
