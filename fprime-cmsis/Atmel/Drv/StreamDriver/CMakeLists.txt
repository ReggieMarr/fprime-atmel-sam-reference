####
# F prime CMakeLists.txt:
#
# SOURCE_FILES: combined list of source and autocoding diles
# MOD_DEPS: (optional) module dependencies
#
####
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/StreamDriver.fpp"
  "${CMAKE_CURRENT_LIST_DIR}/StreamDriver.cpp"
)
if(FPRIME_ATMEL)
    list(APPEND SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/StreamDriverAtmel.cpp")
endif()
register_fprime_module()
