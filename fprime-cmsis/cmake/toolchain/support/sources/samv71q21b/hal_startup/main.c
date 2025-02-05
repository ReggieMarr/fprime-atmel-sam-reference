#include <stdint.h>
#include "Driver_GPIO.h"
#include "Driver_I2C.h"
#include "Driver_USART.h"
#include "cmsis_os2.h"
#include "definitions.h"
#include "hal_defs.h"
#include "os_tick.h"
#include "peripheral/clk/plib_clk.h"
#include "pioConfig.h"
#include "sam.h"
#include "samv71_bsp_cfg.h"
#include "samv71q21b.h"

extern ARM_DRIVER_I2C Driver_I2C0;
static ARM_DRIVER_I2C* I2Cdrv = &Driver_I2C0;

extern ARM_DRIVER_USART Driver_USART0;
static ARM_DRIVER_USART* usartDrv = &Driver_USART0;

extern ARM_DRIVER_GPIO Driver_GPIO;
static ARM_DRIVER_GPIO* gpioDrv = &Driver_GPIO;

typedef enum {
    GPIO_LED_0 = 0,
    GPIO_LED_1,
    GPIO_MAX_NUM,
} SAM_GPIOS_t;

static const uint32_t LED_PIN_0 = PIN_PA23;
static const uint32_t LED_PIN_1 = PIN_PC9;

static pioGPIOConfig_t gpioConfigs[GPIO_MAX_NUM] = {
    [GPIO_LED_0] =
        {
            .pinId = LED_PIN_0,
            .direction = ARM_GPIO_OUTPUT,
            .isOpenDrain = false,
            .isHighOnStart = true,
            .base =
                {
                    .pullResistor = ARM_GPIO_PULL_NONE,
                    .functionalType = PIO_FUNC_GPIO,
                    .driveStrength = 0,
                },
            .irqConfig =
                {
                    .trigger = ARM_GPIO_TRIGGER_NONE,
                },

        },
    [GPIO_LED_1] =
        {
            .pinId = LED_PIN_1,
            .direction = ARM_GPIO_OUTPUT,
            .isOpenDrain = false,
            .isHighOnStart = false,
            .base =
                {
                    .pullResistor = ARM_GPIO_PULL_NONE,
                    .functionalType = PIO_FUNC_GPIO,
                    .driveStrength = 0,
                },
            .irqConfig =
                {
                    .trigger = ARM_GPIO_TRIGGER_NONE,
                },

        },
};

// Pre-Main startup bootstrap
void _on_bootstrap(void) {
    /* Initialize the SAM system */

    /* Enable ICache (CMSIS-Core API) */
    SCB_EnableICache();

    /* Enable DCache (CMSIS-Core API) */
    SCB_EnableDCache();

    /* Initialize Embedded Flash Controller */
    EFC_Initialize();

    /* Initialize PIO with pin configurations */
    initPio(gpioConfigs, GPIO_MAX_NUM);

    /* Initialize Clocks */
    CLOCK_Initialize();

    /* Disable Watchdog Timers */
    RSWDT_REGS->RSWDT_MR = RSWDT_MR_WDDIS_Msk;  // Disable Reinforced Watchdog Timer
    WDT_REGS->WDT_MR = WDT_MR_WDDIS_Msk;        // Disable Standard Watchdog Timer

    /* Initialize Communication Peripherals */
    TWIHS0_Initialize();
    USART1_Initialize();

    /* Initialize Peripheral Drivers */
    I2Cdrv->Initialize(NULL);
    usartDrv->Initialize(NULL);

    /* Configure Nested Vector Interrupt Controller (NVIC) */
    NVIC_Initialize();

    /* System Clock Update */
    SystemCoreClockUpdate();
}

typedef struct {
    size_t idx;
    size_t tickDelay;
} blinkArg_t;

void blink_led(void* argument) {
    blinkArg_t* args = (blinkArg_t*)argument;
    size_t dfltTickDelay = 50000;

    // Set everything off
    gpioDrv->SetOutput(LED_PIN_0, GPIO_LOW);
    gpioDrv->SetOutput(LED_PIN_1, GPIO_LOW);
    osDelay(dfltTickDelay);

    // Set everything on
    gpioDrv->SetOutput(LED_PIN_0, GPIO_HIGH);
    gpioDrv->SetOutput(LED_PIN_1, GPIO_HIGH);

    osDelay(dfltTickDelay);

    gpioDrv->SetOutput(LED_PIN_0, GPIO_LOW);
    gpioDrv->SetOutput(LED_PIN_1, GPIO_HIGH);

    osDelay(dfltTickDelay);

    // Toggle LEDs
    while (true) {
        gpioDrv->SetOutput(LED_PIN_0, GPIO_HIGH);
        gpioDrv->SetOutput(LED_PIN_1, GPIO_LOW);
        osDelay(args->tickDelay);

        gpioDrv->SetOutput(LED_PIN_0, GPIO_LOW);
        gpioDrv->SetOutput(LED_PIN_1, GPIO_HIGH);
        osDelay(args->tickDelay);
    }
}

int main(void) {
    static blinkArg_t led0 = {.idx = 0, .tickDelay = 5000};
    static blinkArg_t led1 = {.idx = 1, .tickDelay = 10000};

    // Initialize CMSIS-RTOS
    osKernelInitialize();
    // Create application main thread(s)
    osThreadNew(blink_led, &led0, NULL);
    osThreadNew(blink_led, &led1, NULL);
    // Start thread execution
    osKernelStart();

    /* Wait forever here other wise the owned threads will die */
    for (;;) {
    }

    return 0;
}
