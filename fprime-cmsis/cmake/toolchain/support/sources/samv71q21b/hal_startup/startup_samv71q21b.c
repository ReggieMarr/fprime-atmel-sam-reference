#include "startup_samv71q21b.h"
#include "FreeRTOS.h"
#include "device_vectors.h"
#include "interrupts.h"
#include "sam.h"

#ifdef __FPU_PRESENT
/* Enable FPU */
__STATIC_INLINE void FPU_Enable(void) {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    SCB->CPACR |= (((uint32_t)0xFU) << 20);
    __DSB();
    __ISB();

    if (primask == 0U) {
        __enable_irq();
    }
}
#endif

// Pre-Main startup bootstrap
void __attribute__((weak)) _on_bootstrap(void) {
    /* Initialize the SAM system */
    /* Enable ICache (CMSIS-Core API) */
    SCB_EnableICache();

    /* Enable DCache (CMSIS-Core API)*/
    SCB_EnableDCache();

    SYS_Initialize(NULL);
}

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void __attribute__((section(".text.Reset_Handler"), long_call, naked, externally_visible)) Reset_Handler(void) {
    uint32_t *pSrc, *pDest;

    /* Initialize the relocate segment */
    pSrc = &_etext;
    pDest = &_srelocate;

    if (pSrc != pDest) {
        for (; pDest < &_erelocate;) {
            *pDest++ = *pSrc++;
        }
    }

    /* Clear the zero segment */
    for (pDest = &_szero; pDest < &_ezero;) {
        *pDest++ = 0;
    }

    /* Set the vector table base address */
    pSrc = (uint32_t*)&_sfixed;
    SCB->VTOR = ((uint32_t)pSrc & SCB_VTOR_TBLOFF_Msk);

    /* Call the application-provided _on_reset() function. */
    _on_reset();

// TODO check this
#ifdef __FPU_PRESENT
    /* Enable the FPU if the application is built with -mfloat-abi=softfp or -mfloat-abi=hard */
    FPU_Enable();
#endif

    /* Initialize the C library */
    __libc_init_array();

    /* Call the application-provided _on_bootstrap() function. */
    _on_bootstrap();

    /* Branch to main function */
    int __launchStatus = main();
    // TODO do something to handle this being an error

    // TODO should have some sort of led flickering induced here
    // Should never reach here (since main should be calling a while loop)
    /* return (EXIT_FAILURE); */
}
