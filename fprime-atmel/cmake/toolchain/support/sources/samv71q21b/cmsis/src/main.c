/* #include "sam.h" */

/* void delay(volatile uint32_t count) { */
/*     while (count--); */
/* } */

/* int main(void) { */
/*     // Initialize the SAMV71 system */
/*     SystemInit(); */

/*     // Enable the clock for Port A */
/*     PMC->PMC_PCER0 |= (1 << ID_PIOA); */

/*     // Configure PA10 (LED) as output */
/*     PIOA->PIO_PER = PIO_PA10; */
/*     PIOA->PIO_OER = PIO_PA10; */

/*     while (1) { */
/*         // Toggle the LED */
/*         PIOA->PIO_SODR = PIO_PA10;  // Set PA10 high */
/*         delay(1000000); */
/*         PIOA->PIO_CODR = PIO_PA10;  // Set PA10 low */
/*         delay(1000000); */
/*     } */

/*     return 0; */
/* } */
#include "sam.h"

void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < (ms * 1000); ++i) {
        __NOP(); // Simple delay loop
    }
}

int main(void) {
    // Enable clock for Port A (PIOA)
    PMC->PMC_PCER0 = (1 << ID_PIOA);

    // Configure PA0 as output
    PIOA->PIO_PER = PIO_PA0;
    PIOA->PIO_OER = PIO_PA0;

    while (1) {
        // Toggle PA0
        PIOA->PIO_SODR = PIO_PA0; // Set
        delay_ms(500);
        PIOA->PIO_CODR = PIO_PA0; // Clear
        delay_ms(500);
    }

    return 0;
}
