restrict_platforms(AtmelFw)

# TEMPORARY FIX: Add ASSERT_RELATIVE_PATH definition for FW_ASSERT_LEVEL = FW_RELATIVE_PATH_ASSERT
if(NOT DEFINED ASSERT_RELATIVE_PATH)
    add_definitions(-DASSERT_RELATIVE_PATH="${FPRIME_PROJECT_ROOT}")
endif()

# Include subdirectories
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Atmel/Os")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Atmel/Drv/GpioDriver")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Atmel/Drv/StreamDriver")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Atmel/Drv/I2cDriver")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Atmel/Drv/I2cNodeDriver")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Atmel/Drv/SpiDriver")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Atmel/Drv/PwmDriver")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Atmel/Drv/TcpClient")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Atmel/Drv/HardwareRateDriver")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Atmel/Drv/AnalogDriver")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Atmel/Svc/LifeLed")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Atmel/Svc/AtmelTime")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Atmel/Svc/Ports")
