set(HARMONY_DIR ${PROJECT_ROOT}/Base/config/third_party/harmony/sam_v71_xult)

set(HARMONY_SOURCES
    # Core files
    ${HARMONY_DIR}/initialization.c
    ${HARMONY_DIR}/interrupts.c
    ${HARMONY_DIR}/exceptions.c
    ${HARMONY_DIR}/libc_syscalls.c
    # ${HARMONY_DIR}/freertos_hooks.c

    # OSAL
    ${HARMONY_DIR}/osal/osal_freertos.c

    # Peripheral implementations
    ${HARMONY_DIR}/peripheral/clk/plib_clk.c
    ${HARMONY_DIR}/peripheral/nvic/plib_nvic.c
    ${HARMONY_DIR}/peripheral/pio/plib_pio.c
    ${HARMONY_DIR}/peripheral/efc/plib_efc.c
)

# Add include directories
set(HARMONY_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
)
