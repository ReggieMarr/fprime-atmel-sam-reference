# arm pack files
set(PACK_ROOT ${DEPS_ROOT}/mplab_dev_packs)
verify_path(PACK_ROOT "MPLAB device packs")

set(CMSIS_VERSION "5.8.0")

set(ARM_PACK_DIR ${PACK_ROOT}/arm/CMSIS/5.8.0)
verify_path(ARM_PACK_DIR "ARM CMSIS pack")

# since we're using an arm cortex m7 with double precision
set(ARM_PACK_DEFS
    -DARMCM7_DP
)

set(ARM_PACK_DEV_DIR ${ARM_PACK_DIR}/Device/ARM/ARMCM7)
set(ARM_PACK_SOURCES
    # Device implementations
    ${ARM_PACK_DEV_DIR}/Source/GCC/startup_ARMCM7.S
    ${ARM_PACK_DEV_DIR}/Source/startup_ARMCM7.c
    ${ARM_PACK_DEV_DIR}/Source/system_ARMCM7.c
)

set(ARM_PACK_INCLUDES
    ${ARM_PACK_DEV_DIR}/Include
    ${ARM_PACK_DIR}/CMSIS/Core/Include
)

set(ARM_PACK_STARTUP_LINKER_FLAGS
    -T${ARM_PACK_DEV_DIR}/Source/GCC/startup_ARMC7.ld
    -Wl,--print-memory-usage
    -Wl,--gc-sections
    # -Wl,--defsym=__STACK_SIZE=0x2000
    # -Wl,--defsym=__HEAP_SIZE=0x1000
    # -Wl,--defsym=__ROM_BASE=0x00400000
)

# Microchip pack files
set(MICROCHIP_PACK_DEFS
    -DUSE_CMSIS_INIT
)

set(DFP_VERSION "4.10.230")
set(MICROCHIP_PACK_DIR ${PACK_ROOT}/Microchip/SAMV71_DFP/${DFP_VERSION}/samv71b)
verify_path(MICROCHIP_PACK_DIR "Microchip SAM pack")

set(MICROCHIP_PACK_SOURCES
    ${MICROCHIP_PACK_DIR}/gcc/system_samv71q21b.c
    # ${MICROCHIP_PACK_DIR}/gcc/gcc/startup_samv71q21b.c
)

set(MICROCHIP_PACK_INCLUDES
    ${MICROCHIP_PACK_DIR}/include
)

set(MICROCHIP_PACK_FLASH_LINKER_FLAG "-T${MICROCHIP_PACK_DIR}/gcc/gcc/samv71q21b_flash.ld")
