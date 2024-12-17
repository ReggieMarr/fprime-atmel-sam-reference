#include <Top/setup.hpp>

#ifdef __cplusplus
extern "C" {
#endif

#include "sam.h"
#include "FreeRTOS.h"
#include "task.h"
#include "startup_samv71q21b.h"

// LED definitions for SAMV71 Xplained board
// Pin 23 on Port A
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

// Pre-C runtime startup reset
void _on_reset(void) {
    // Set the HAL's state
    setupLeds();
}

// LED0 blink task
static void vLED0Task(void *pvParameters)
{
    (void) pvParameters;

    while(1) {
        // Set output (LED OFF)
        LED_PIO->PIO_SODR = LED0_PIN;
        vTaskDelay(pdMS_TO_TICKS(500));
        // Clear output (LED ON)
        LED_PIO->PIO_CODR = LED0_PIN;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// LED1 blink task (different pattern)
static void vLED1Task(void *pvParameters)
{
    (void) pvParameters;

    while(1) {
        // Set output (LED OFF)
        LED1_PIO->PIO_SODR = LED1_PIN;
        vTaskDelay(pdMS_TO_TICKS(800));
        // Clear output (LED ON)
        LED1_PIO->PIO_CODR = LED1_PIN;
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
static StaticTask_t xLED0TaskBuffer;
static StackType_t xLED0Stack[512];

int main(void)
{
    setupLeds();

    // Create the LED tasks
    TaskHandle_t xLED0Handle, xLED1Handle = NULL;

    // BaseType_t ret;
    unsigned long startingNumTasks = uxTaskGetNumberOfTasks();
    xLED0Handle = xTaskCreateStatic(vLED0Task,
                                    "LED0",
                                    512,
                                    NULL,
                                    2,
                                    xLED0Stack,
                                    &xLED0TaskBuffer);
    configASSERT(xLED0Handle);

    xTaskCreate(vLED1Task, "LED1", configMINIMAL_STACK_SIZE, NULL, 2, &xLED1Handle);
    configASSERT(xLED1Handle != NULL);
    if (xLED1Handle != NULL) {
        LED1_PIO->PIO_CODR = LED1_PIN;
    }

    configASSERT( uxTaskGetNumberOfTasks() - startingNumTasks == 2 );

    // Start the scheduler
    vTaskStartScheduler();

    /* Infinite loop */
    while(1);
}

#ifdef __cplusplus
}
#endif
