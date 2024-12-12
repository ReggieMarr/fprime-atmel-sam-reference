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

# -Wl,--defsym=__STACK_SIZE=0x2000
# -Wl,--defsym=__HEAP_SIZE=0x1000
# -Wl,--defsym=__ROM_BASE=0x00400000
set(ARM_PACK_BOOT_LINKER_SCRIPT
    -T${ARM_PACK_DEV_DIR}/Source/GCC/startup_ARMC7.ld)

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

# This is the best explanation I could find from: https://github.com/PeakSat/minimal-dev-setup
# = Booting settings =-
#
# Background:
#
#   All the flags before this section merely configure the compiler to provide
#   the bare minimum defaults for compiling and linking objects for the SAMV71
#   platform. However, this is the primordial world of bare metal -- nothing is
#   a given here, the MCU has no idea of the C runtime environment or even where
#   main() is!
#
#   (ref: https://interrupt.memfault.com/blog/arm-cortex-m-exceptions-and-nvic)
#   What the MCU *does* know, though, is interrupts, because these can be
#   triggered by hardware. Each interrupt is assigned a unique number known as
#   'exception number'. Numbers from 1-15 are fixed and assigned by ARM, the rest
#   are up to the device's maker (Microchip). For example, the well-known HardFault
#   interrupt that gets triggered on fault conditions has exception number 3.
#
#   When an interrupt is triggered, its exception number is used as an index in
#   an array of function pointers known as the IVT (interrupt vector table). To
#   handle the interrupt, the MCU merely reads off the address that corresponds
#   to the exception number from the IVT and jumps to it. The IVT is always
#   located at offset 0 (the very beginning) of the executable's '.text' section.
#
#   Thus, when power is applied, the MCU follows this procedure:
#   - SP (stack pointer) = IVT[0] (index 0 reserved for initial SP value)
#   - PC (program counter) = IVT[1], (index 1: Reset_Handler)
#   - Start execution
#
#   Given all the above, in order for any C or C++ programs to be loadable at all
#   we need to provide a very minimal environment that satisfies the above
#   constraints. This environment is made from the following essential components:
#   - device startup file: This is a specially written, device-specific C file that:
#     i) describes the layout of the IVT (handlers sorted by exception number) for
#     the device in a special (.vectors) section of the executable, as well as the
#     signatures of all possible interrupt handlers
#     ii) provides default implementations of the interrupt handlers and, most
#     importantly, of the reset handler called on power-on. The reset handler
#     does chores like relocating the executable and IVT into RAM, clearing the
#     zero segment and initializing libc, before finally jumping into main().
#     iii) declares external "marker" symbols used by the IVT description, such as
#     the main() function or '_sstack'/'_estack' (start/end of stack), in order for
#     the linker to place each section at the correct addresses.
#
#     We use the default (startup_samv71q21b.c) provided by the DFP for our MCU.
#
#   - linker script: This describes:
#     i) locations of all basic sections (.text, .bss, .data) within the MCU
#     address space.
#     For example, depending on the configuration of GPNVM register (ref), the
#     .text section must be placed at:
#     - Bit 1 set: 0x00400000 (boot from internal flash)
#     - Bit 1 clr: 0x00000000 (boot from ROM, default).
#       (Note: the GPNVM1 bit *must* be set on every flash session for the MCU to
#        boot from the flash (ref). If using the SAMV71 Xplained Ultra devboard,
#        OpenOCD will do this for you but in the OBC EQM/FM where the bare chip is
#        used this might not be automatic, so be careful.)
#
#     ii) layout of the contents of each section. For example, .text must begin
#     with the .vectors subsection (the IVT) as described above.
#
#     We use the default (samv71q21b_flash.ld) script provided by the DFP. It is
#     assumed to be located in the project root. You may need to change it if
#     using features such as memory protection (MPU) or TCM, so make sure you
#     know the language, it's not that complicated if you spend some time to
#     learn about ELF files and get used to the symbols/terminology (ref
#     linker script tutorial)
set(MICROCHIP_PACK_BOOT_LINKER_SCRIPT
    -Wl,-T${MICROCHIP_PACK_DIR}/gcc/gcc/samv71q21b_flash.ld
    #  - --defsym=__bss_start__/__bss_end__: The newlib-nano port included with GCC requires definitions for the
    #    symbols denoting the start and end of .bss section, named __bss_start__ and __bss_end__ respectively.
    #    The corresponding symbols in the startup file and linker script are _sbss and _ebss, and the mismatch
    #    causes a linker error, which is fixed by defining the symbols to whatever the linker script is already using.
    #  - --defsym=end: The default implementation of _sbrk that comes with libnosys requires a linker-defined symbol
    #    named 'end'. By convention heap grows upwards thus its end is initially located at the *first* address of the heap space,
    #    which corresponds to the _sheap symbol in the linker script.
    -Wl,--defsym=__bss_start__=_sbss -Wl,--defsym=__bss_end__=_ebss -Wl,--defsym=end=_sheap
)

#   NOTE __DEBUG is a Microchip-specific define that adds some breakpoints,
#   and triggers sys_debug and more convience factors.
#   NOTE we may want to remove later but it's added by MPLAB X when in debug mode.
#   so kept for now for consistency.
set(MICROCHIP_PACK_DEBUG_DEFS __DEBUG)

set(FREERTOS_SOURCE_DIR ${DEPS_ROOT}/FreeRTOS/FreeRTOS/Source)
set(FREERTOS_PORT_DIR ${FREERTOS_SOURCE_DIR}/portable/GCC/ARM_CM7/r0p1)

set(FREERTOS_SOURCES
    ${FREERTOS_SOURCE_DIR}/tasks.c
    ${FREERTOS_SOURCE_DIR}/queue.c
    ${FREERTOS_SOURCE_DIR}/list.c
    ${FREERTOS_SOURCE_DIR}/timers.c
    # From https://www.FreeRTOS.org/a00111.html
    # + heap_4:
    #    + Can be used even when the application repeatedly deletes tasks,
    #      queues, semaphores, mutexes, etc..
    #    + Is much less likely than the heap_2 implementation to result in a heap
    #      space that is badly fragmented into multiple small blocks - even when
    #      the memory being allocated and freed is of random size.
    #    + Is not deterministic - but is much more efficient that most standard
    #      C library malloc implementations.
    # NOTE should only be used when configSUPPORT_DYNAMIC_ALLOCATION=1
    # TODO compare heap_5, use FreeRTOS-kernel and set port/heap using
    # recommended mechanisms like (e.g. set(FREERTOS_PORT "TEMPLATE" CACHE STRING "" FORCE))

    ${FREERTOS_SOURCE_DIR}/portable/MemMang/heap_4.c
    ${FREERTOS_PORT_DIR}/port.c
)

set(FREERTOS_INCLUDES
    ${FREERTOS_SOURCE_DIR}/include
    ${FREERTOS_PORT_DIR}
    # Sets our freeRTOS config
    ${THIRD_PARTY_ROOT}/peak/
)
