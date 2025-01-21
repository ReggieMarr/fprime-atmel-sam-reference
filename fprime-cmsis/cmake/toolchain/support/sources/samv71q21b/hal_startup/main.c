#include "Driver_GPIO.h"
#include "Driver_USART.h"
#include "os_tick.h"
#include "sam.h"
#include "hal_defs.h"
#include "cmsis_os2.h"
#include "peripheral/clk/plib_clk.h"
#include "definitions.h"
#include "Driver_I2C.h"

extern ARM_DRIVER_GPIO DRIVER_GPIO0;
static ARM_DRIVER_GPIO *GPIOdrv = &DRIVER_GPIO0;

extern ARM_DRIVER_I2C Driver_I2C0;
static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C0;

extern ARM_DRIVER_USART Driver_USART0;
static ARM_DRIVER_USART *usartDrv = &Driver_USART0;

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

// Pre-Main startup bootstrap
void _on_bootstrap(void) {
    /* Initialize the SAM system */
    /* Enable ICache (CMSIS-Core API) */
    SCB_EnableICache();

    /* Enable DCache (CMSIS-Core API)*/
    SCB_EnableDCache();

    EFC_Initialize();

    CLOCK_Initialize();
	PIO_Initialize();

	RSWDT_REGS->RSWDT_MR = RSWDT_MR_WDDIS_Msk;	// Disable RSWDT
	WDT_REGS->WDT_MR = WDT_MR_WDDIS_Msk; 		// Disable WDT

	TWIHS0_Initialize();
    USART1_Initialize();

    /* NOTE could pass a user defined signal event handler here */
    /* I2Cdrv->Initialize(NULL); */

    /* usartDrv->Initialize(NULL); */

    /* Nested vector interupt controller
     * Enable the interrupt sources and configure the priorities as configured
     * from within the "Interrupt Manager" of MHC. */
    NVIC_Initialize();

    /* NOTE should call GPIO_Setup for relevant io's here */
    setupLeds();

    SystemCoreClockUpdate();
}

typedef struct {
    size_t idx;
    size_t tickDelay;
} blinkArg_t;

void blink_led (void *argument) {
    blinkArg_t *args = (blinkArg_t *) argument;
    volatile pio_registers_t *ledReg = NULL;
    volatile uint32_t ledPin;
    size_t dfltTickDelay = 50000;
    // Off
    LED1_PIO->PIO_CODR = LED1_PIN;
    LED_PIO->PIO_CODR = LED0_PIN;
    osDelay(dfltTickDelay/2);
    LED1_PIO->PIO_SODR = LED1_PIN;
    LED_PIO->PIO_SODR = LED0_PIN;

    LED1_PIO->PIO_CODR = LED1_PIN;
    /* LED_PIO->PIO_CODR = LED0_PIN; */
    osDelay(dfltTickDelay);

    // Off
    LED1_PIO->PIO_SODR = LED1_PIN;
    /* LED_PIO->PIO_SODR = LED0_PIN; */

    osDelay(dfltTickDelay);

    if (args->idx == 0) {
        ledReg = LED_PIO;
        ledPin = LED0_PIN;
    }
    else if (args->idx == 1) {
        ledReg = LED1_PIO;
        ledPin = LED1_PIN;
    }
    else if (args->idx < 0) {
        LED_PIO->PIO_CODR = LED0_PIN;
    }

    while(ledReg != NULL) {
        // Set output (LED OFF)
        ledReg->PIO_SODR = ledPin;
        osDelay(args->tickDelay);
        // Clear output (LED ON)
        ledReg->PIO_CODR = ledPin;
        osDelay(args->tickDelay);
    }
}

int main(void) {
    static blinkArg_t led0 = {.idx = 0, .tickDelay = 5000};
    static blinkArg_t led1 = {.idx = 1, .tickDelay = 10000};
    osKernelInitialize();                 // Initialize CMSIS-RTOS
    // Create application main thread(s)
    osThreadNew(blink_led, &led0, NULL);
    osThreadNew(blink_led, &led1, NULL);
    osKernelStart();                      // Start thread execution

    /* Wait forever here other wise the owned threads will die */
    for (;;) {}

    return 0;
}
