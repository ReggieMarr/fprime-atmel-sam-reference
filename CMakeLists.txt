####
# 
####
cmake_minimum_required(VERSION 3.26)
project(AtmelSam C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../" ABSOLUTE)
set(DEPS_ROOT "${PROJECT_ROOT}/deps")

###
# F' Core Setup
# This includes all of the F prime core components, and imports the make-system.
###
include("${CMAKE_CURRENT_LIST_DIR}/lib/fprime/cmake/FPrime.cmake")
# NOTE: register custom targets between these two lines
include("${FPRIME_FRAMEWORK_PATH}/cmake/FPrime-Code.cmake")

# This includes project-wide objects
include("${CMAKE_CURRENT_LIST_DIR}/project.cmake")
set_target_properties(Svc_FatalHandler PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(Svc_SystemResources PROPERTIES EXCLUDE_FROM_ALL TRUE) #U64
set_target_properties(Svc_Version PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(Svc_DpCatalog PROPERTIES EXCLUDE_FROM_ALL TRUE) # u64
set_target_properties(Svc_DpWriter PROPERTIES EXCLUDE_FROM_ALL TRUE) #u64
set_target_properties(Svc_DpManager PROPERTIES EXCLUDE_FROM_ALL TRUE) #64
set_target_properties(Svc_FileManager PROPERTIES EXCLUDE_FROM_ALL TRUE) #u64
set_target_properties(Svc_SeqDispatcher PROPERTIES EXCLUDE_FROM_ALL TRUE)
