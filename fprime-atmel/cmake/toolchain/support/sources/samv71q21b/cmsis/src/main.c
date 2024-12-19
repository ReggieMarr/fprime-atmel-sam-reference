#include "sam.h"

#define LED0_PIN    PIO_PA23
#define LED_PIO     PIOA_REGS
#define LED1_PIN    PIO_PC9
#define LED1_PIO    PIOC_REGS

static void setupLeds(void)
{
    // Enable clocks for GPIO
    PMC_REGS->PMC_PCER0 = (1 << ID_PIOA) | (1 << ID_PIOC);

    // Configure LED0
    // Enable PIO control (as opposed to peripheral control)
    LED_PIO->PIO_PER = LED0_PIN;
    // Configure as output
    LED_PIO->PIO_OER = LED0_PIN;
    // Disable internal pull-up
    LED_PIO->PIO_PUDR = LED0_PIN;
    // Set high (LED off)
    LED_PIO->PIO_SODR = LED0_PIN;

    // Configure LED1
    // Enable PIO control
    LED1_PIO->PIO_PER = LED1_PIN;
    // Configure as output
    LED1_PIO->PIO_OER = LED1_PIN;
    // Disable pull-up
    LED1_PIO->PIO_PUDR = LED1_PIN;
    // Set high (LED off)
    LED1_PIO->PIO_SODR = LED1_PIN;
}

void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < (ms * 1000); ++i) {
        __NOP(); // Simple delay loop
    }
}

int main(void) {
    setupLeds();

    while(1) {
        // Set output (LED OFF)
        LED1_PIO->PIO_SODR = LED1_PIN;
        delay_ms(500);
        // Clear output (LED ON)
        LED1_PIO->PIO_CODR = LED1_PIN;
        delay_ms(500);
    }

    return 0;
}
