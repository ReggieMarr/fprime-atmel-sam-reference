####
# Teensyduino.cmake:
#
# Toolchain file setup for building F prime for the Teensy hardware platform. This must, by definition, include the
# Atmel framework in order to ensure that basic functions of the Teensy hardware platform are available. This
# toolchain will build the core Teensy Atmel libraries, and include the headers as part of the target include headers.
####
cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-std=c++17 -std=gnu++17) # Force C++17 standard

# System setup for Teensyduino
set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_SYSTEM_PROCESSOR "arm")
set(CMAKE_CROSSCOMPILING 1)
set(FPRIME_PLATFORM "AtmelFw")
set(FPRIME_USE_BAREMETAL_SCHEDULER ON)
set(ATMEL_BUILD_PROPERTIES)

# Teensy 40 is used to compile for the teensy 4.1 board
set(ATMEL_FQBN "teensy:avr:teensy41")
add_compile_options(-D_BOARD_TEENSY41 -DFPRIME_ARUDINO_SYNTHETIC_WRITE -DFPRIME_ARUDINO_SYNTHETIC_NEW_NOTHROW)
# Run the base atmel setup which should detect settings!
include("${CMAKE_CURRENT_LIST_DIR}/support/atmel-support.cmake")
