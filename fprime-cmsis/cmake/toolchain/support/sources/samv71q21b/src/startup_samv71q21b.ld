/*------------------------------------------------------------------------------
 * Linker script for starting up SAMV71Q21B with GCC
 * Derived from a combintation of the gcc and microchip specific flash linker
 * scripts found in the microchip software pack
 *----------------------------------------------------------------------------*/
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
SEARCH_DIR(.)

/* Set Reset_Handler as the entry point
 * This creates an entry point symbol in the ELF file that can be used by:
 * - Debuggers to identify the application start point
 * - objcopy when creating binary images
 * - gcc's startup code to initialize the vector table
 * The symbol must match the name of the reset handler function in startup code
 */
ENTRY(Reset_Handler)

/*************************************************************************
 * Memory Configuration and Error Checking
 *************************************************************************/
/*************************************************************************
 * Memory-Region Definitions
 * You may define these as options to the linker when calling
 * the linker via the arm-gcc shell.
 * ie. '-Wl,--defsym=<memory_type>_ORIGIN=<address>'
 * and/or '-Wl,--defsym=<memory_type>_LENGTH=<size>'
 *************************************************************************/

ROM_ORIGIN = DEFINED(ROM_ORIGIN) ? ROM_ORIGIN : 0x00400000;
ROM_LENGTH = DEFINED(ROM_LENGTH) ? ROM_LENGTH : 0x200000; /* 2MB Flash */
RAM_ORIGIN = DEFINED(RAM_ORIGIN) ? RAM_ORIGIN : 0x20400000;
RAM_LENGTH = DEFINED(RAM_LENGTH) ? RAM_LENGTH : 0x60000;  /* 384KB RAM */

/* Error checking for memory sizes */
ASSERT(ROM_LENGTH <= 0x200000, "ROM_LENGTH exceeds maximum size of 2MB")
ASSERT(RAM_LENGTH <= 0x60000, "RAM_LENGTH exceeds maximum size of 384KB")

/*************************************************************************
 * Memory-Region Definitions
 * The MEMORY command describes the location and size of blocks of memory
 * on the target device. The command below uses the macros defined above.
 *************************************************************************/
MEMORY
{
    rom (rx)    : ORIGIN = ROM_ORIGIN, LENGTH = ROM_LENGTH
    ram (rwx)   : ORIGIN = RAM_ORIGIN, LENGTH = RAM_LENGTH
}

/* The stack size used by the application. NOTE: you need to adjust according to your application. */
STACK_SIZE = DEFINED(STACK_SIZE) ? STACK_SIZE : DEFINED(__stack_size__) ? __stack_size__ : 0x0400;

/* The heapsize used by the application. NOTE: you need to adjust according to your application. */
HEAP_SIZE = DEFINED(HEAP_SIZE) ? HEAP_SIZE : DEFINED(__heap_size__) ? __heap_size__ : 0x0200;

/*************************************************************************
 * Section Definitions
 *************************************************************************/
SECTIONS
{
    /* Vector table and code */
    .text :
    {
        . = ALIGN(4);
        _sfixed = .;        /* Start of code section */

        /* Vector table must be first */
        KEEP(*(.vectors .vectors.*))

        /* Program code */
        *(.text .text.* .gnu.linkonce.t.*)
        *(.glue_7t) *(.glue_7)
        *(.rodata .rodata.* .gnu.linkonce.r.*)
        *(.ARM.extab* .gnu.linkonce.armextab.*)

        /* Support C constructors, and C destructors in both user code
           and the C library. This also provides support for C++ code. */
        . = ALIGN(4);
        KEEP(*(.init))
        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP(*(.preinit_array))
        __preinit_array_end = .;

        . = ALIGN(4);
        __init_array_start = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        __init_array_end = .;

        . = ALIGN(4);
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))

        /* Constructors/destructors */
        KEEP(*(.fini))
        . = ALIGN(4);
        __fini_array_start = .;
        KEEP(*(.fini_array))
        KEEP(*(SORT(.fini_array.*)))
        __fini_array_end = .;

        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))

        . = ALIGN(4);
        _efixed = .;        /* End of code section */
    } > rom

    /* .ARM.exidx is sorted, so has to go in its own output section.  */
    PROVIDE_HIDDEN (__exidx_start = .);
    .ARM.exidx :
    {
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > rom
    PROVIDE_HIDDEN (__exidx_end = .);

    . = ALIGN(4);
    _etext = .;            /* End of text and rodata sections */

    /* Data section */
    .relocate : AT (_etext)
    {
        . = ALIGN(4);
        _srelocate = .; /* Start of data section */
        *(.ramfunc .ramfunc.*);
        *(.data .data.*);
        . = ALIGN(4);
        _erelocate = .; /* End of data section */
    } > ram


    /* BSS section */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        _sbss = .;
        _szero = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
        _ebss = .;
        _ezero = .;
    } > ram

    /* Heap section */
    .heap (NOLOAD) :
    {
        . = ALIGN(8);
        _sheap = .;
        . = . + HEAP_SIZE;
        . = ALIGN(8);
        _eheap = .;
    } > ram

    /* Stack section */
    .stack (NOLOAD) :
    {
        . = ALIGN(8);
        _sstack = .;
        . = . + STACK_SIZE;
        . = ALIGN(8);
        _estack = .;
    } > ram

    PROVIDE(_stack = _estack);
    /* Provide symbols for newlib */
    end = _eheap;

    /* Check for overflow */
    ASSERT(_estack <= (ORIGIN(ram) + LENGTH(ram)), "RAM overflow!")

    . = ALIGN(4);
    _end = . ;
    _ram_end_ = ORIGIN(ram) + LENGTH(ram) - 1 ;
}
