# -----------------------------------------------------------------------------
# F' Configuration
# -----------------------------------------------------------------------------

# Deployment is only compatible with the AtmelFw platform
# NOTE compatible with CMSIS platform might be more accurate
restrict_platforms(AtmelFw)

###
# Topology and Components
###
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/Top/")

set(SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/main.cpp")
set(MOD_DEPS ${FPRIME_CURRENT_MODULE}/Top)

register_fprime_deployment()

finalize_arduino_executable("${FPRIME_CURRENT_MODULE}")
