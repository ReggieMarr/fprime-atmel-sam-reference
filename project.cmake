# This CMake file is intended to register project-wide objects so they can be
# reused easily between deployments, but also by other projects.

add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/deps")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Base/")
