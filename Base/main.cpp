#include "sam.h"
#include "FreeRTOS.h"
#include "task.h"
#include "config.h"

// LED definitions for SAMV71 Xplained board
#define LED0_PIN    PIO_PA23  // LED0
#define LED_PIO     PIOA_REGS // LED0 is on PIOA
#define LED1_PIN    PIO_PC9   // LED1
#define LED1_PIO    PIOC_REGS // LED1 is on PIOC

static void prvSetupHardware(void)
{
    // Enable clocks for GPIO
    PMC_REGS->PMC_PCER0 = (1 << ID_PIOA) | (1 << ID_PIOC);

    // Configure LED0
    LED_PIO->PIO_PER = LED0_PIN;    // Enable PIO control
    LED_PIO->PIO_OER = LED0_PIN;    // Configure as output
    LED_PIO->PIO_PUDR = LED0_PIN;   // Disable pull-up
    LED_PIO->PIO_SODR = LED0_PIN;   // Set high (LED off)

    // Configure LED1
    LED1_PIO->PIO_PER = LED1_PIN;   // Enable PIO control
    LED1_PIO->PIO_OER = LED1_PIN;   // Configure as output
    LED1_PIO->PIO_PUDR = LED1_PIN;  // Disable pull-up
    LED1_PIO->PIO_SODR = LED1_PIN;  // Set high (LED off)
}

// LED0 blink task
static void vLED0Task(void *pvParameters)
{
    (void) pvParameters;

    while(1) {
        LED_PIO->PIO_CODR = LED0_PIN;    // Clear output (LED ON)
        vTaskDelay(pdMS_TO_TICKS(500));
        LED_PIO->PIO_SODR = LED0_PIN;    // Set output (LED OFF)
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// LED1 blink task (different pattern)
static void vLED1Task(void *pvParameters)
{
    (void) pvParameters;

    while(1) {
        LED1_PIO->PIO_CODR = LED1_PIN;   // Clear output (LED ON)
        vTaskDelay(pdMS_TO_TICKS(200));
        LED1_PIO->PIO_SODR = LED1_PIN;   // Set output (LED OFF)
        vTaskDelay(pdMS_TO_TICKS(800));
    }
}

int main(void)
{
    // Initialize the SAM system
    SystemInit();

    // Setup the hardware
    prvSetupHardware();

    // Create the LED tasks
    xTaskCreate(vLED0Task, "LED0", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(vLED1Task, "LED1", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

    // Start the scheduler
    vTaskStartScheduler();

    // Should never reach here
    while(1);
}
